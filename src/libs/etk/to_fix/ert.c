/// =====================================================================================
///
///       Filename:  ert.c
///
///    Description:  easy routine on threadpool, do not like routine, this is a threadpool
///                  actually
///
///        Version:  2.0
///        Created:  03/09/2017 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ecompat.h"
#include "ethread.h"
#include "ert.h"
#include "ejson.h"
#include "ell.h"
#include "echan.h"

// #define EUTILS_LLOG 0
#include "eutils.h"

/// ----------------------- thread_pool ----------------

#define TAG_LEN 32

typedef struct task_s* TASK;

typedef struct thread_handle_s{
    char         id[16];
    ert          tp;
    ethread_t    th;
    int          status;
    int          quit;
}thread_handle_t, * TH;

typedef struct task_s{
    char      tag[TAG_LEN];
    ert_cb    oprt;
    ert_cb    after_oprt;
    void*     data;
}task_t;

typedef struct thread_pool_s{
    volatile int status;

    // -- threads manager
    int          thrds_cap;     // the upper limit of threads
    int          thrds_idx;     // threads idx
    int          thrds_num;     // threads num
    volatile int thrds_idle;
    emutex_t     thrds_mu;

    // -- tasks manager
    echan        tasks;         // all tasks
    ejson        tasks_tags;    // task tags
    ell          tasks_cache;   // task cache
    emutex_t     tasks_mu;

    // -- quit manager
    echan        quit_sigs;
    int          quit_join_ths;

    // --
    emutex_t     wait_mu;
    econd_t      wait_co;
    int          wait_num;
}thread_pool_t;

static inline int  __ert_thrd_idle(ert tp) {int num; emutex_lock(tp->thrds_mu); num = tp->thrds_idle; emutex_ulck(tp->thrds_mu); return num;}
static inline int  __ert_thrd_num (ert tp) {int num; emutex_lock(tp->thrds_mu); num = tp->thrds_num ; emutex_ulck(tp->thrds_mu); return num;}

#define __ert_thrd_idle_PP(tp) { emutex_lock(tp->thrds_mu); tp->thrds_idle++; emutex_ulck(tp->thrds_mu); }
#define __ert_thrd_idle_MM(tp) { emutex_lock(tp->thrds_mu); tp->thrds_idle--; emutex_ulck(tp->thrds_mu); }
#define __ert_thrd_num_PP(tp)  { emutex_lock(tp->thrds_mu); tp->thrds_num++;  emutex_ulck(tp->thrds_mu); }
#define __ert_thrd_num_MM(tp)  { emutex_lock(tp->thrds_mu); tp->thrds_num--;  emutex_ulck(tp->thrds_mu); }

enum {
    // -- ert status
    _RELEASED         = 0,
    _INITIALING,
    _INITED,
    _RELEASE_REQED,
    _RELEASING,
};

#define _DFT_THREAD_NUM  4
#define _DFT_TASKS_BUFF  256

static ert  _df_tp;
static int  _df_thread_num = _DFT_THREAD_NUM;
static uint _cpu_num;

static void _etp_quit_cb(void* _tp);

#define USING_TASKMU 1

#if USING_TASKMU
#define tasks_lock() emutex_lock(tp->tasks_mu);
#define tasks_ulck() emutex_ulck(tp->tasks_mu);
#else
#define tasks_lock() emutex_lock(tp->thrds_mu);
#define tasks_ulck() emutex_ulck(tp->thrds_mu);
#endif

/// -------------------------- ert internal -----------------------

static void __ert_self_init(ert tp)
{
    emutex_init(tp->thrds_mu);
    emutex_init(tp->tasks_mu);
    emutex_init(tp->wait_mu);
}

static void __ert_self_release(ert tp)
{
    emutex_free(tp->thrds_mu);
    emutex_free(tp->tasks_mu);
    emutex_free(tp->wait_mu);

    free(tp);
}

static ert __ert_new(int max_thread_num)
{
    ert tp;

    is0_ret(tp = calloc(1, sizeof(*tp)), 0);

    tp->status        = _INITIALING;

    tp->tasks         = echan_new(ECHAN_LIST, INT_MAX);
    tp->tasks_tags    = ejson_new(EOBJ, EVAL_0);
    tp->tasks_cache   = ell_new();

    if(!tp->tasks || !tp->tasks_tags || !tp->tasks_cache)
        goto err_ret;

    __ert_self_init(tp);

    tp->thrds_cap = max_thread_num == 0 ? _DFT_THREAD_NUM : max_thread_num;

    tp->status    = _INITED;

    return tp;

err_ret:
    echan_free(tp->tasks);
    ejson_free(tp->tasks_tags);
    ell_free(tp->tasks_cache);

    __ert_self_release(tp);

    return 0;
}

static inline void __ert_exeWait(ert tp)
{
    while(tp->status != _RELEASED)
    {
        emutex_lock(tp->wait_mu);
        tp->wait_num++;
        econd_wait(tp->wait_co, tp->wait_mu);
        tp->wait_num--;
        emutex_ulck(tp->wait_mu);
    }
}

static inline void __ert_quitWait(ert tp)
{
    while(tp->wait_num)
    {
        econd_all(tp->wait_co);
        usleep(10000);
    }
}

static inline void __ert_task_cache(ert tp, TASK t)
{
    tasks_lock();

    if(t->tag[0])
    {
#if _TP_DEBUG_
        if(ejso_len(tp->tasks_tag) == 0)   // this should not happen
        {
            llog("--------------- err ----------------");   // make a breakpoint here
        }

        ejso_freeR(tp->tasks_tag, tag);

        cstr s;
        if(ejsr(tp->tasks_tag, tag))
        {    llog("[thread%s]: rm %s failed, %d, %s", th->id, tag, ejso_len(tp->tasks_tag), s = ejso_toUStr(tp->tasks_tag));ejss_free(s);}
        else
        {    llog("[thread%s]: rm %s ok, %d, %s", th->id, tag, ejso_len(tp->tasks_tag), s = ejso_toUStr(tp->tasks_tag));ejss_free(s);}
#else
        ejson_freeR(tp->tasks_tags, t->tag);
#endif
    }

    if(ell_size(tp->tasks_cache) >= _DFT_TASKS_BUFF)
        ell_freeO(0, (eobj)t);
    else
    {
        memset(t, 0, sizeof(*t));
        ell_appdO(tp->tasks_cache, (eobj)t);
    }

    tasks_ulck();
}

static inline int __ert_task_add(ert tp, constr _tag, ert_cb oprt, ert_cb after_oprt, cptr arg)
{
    ejson rete; TASK t; char tag[TAG_LEN];

    tasks_lock();
    if(_tag && *_tag)
    {
        strncpy(tag, _tag, TAG_LEN);
        rete = ejson_addT(tp->tasks_tags, tag, ETRUE);
        if(0 == rete)
        {
            llog("[threadpool]: have a task named \"%s\" already, %s, %d", tag, ejson_err(), ejso_len(tp->tasks_tags));
            tasks_ulck();
            return 0;
        }
    }
    else
        tag[0] = 0;

    t = (TASK)ell_takeH(tp->tasks_cache);
    tasks_ulck();

    if(!t) t = (TASK)ell_newO(ERAW, sizeof(*t));
    t->oprt       = oprt;
    t->after_oprt = after_oprt;
    t->data       = arg;

    if(*tag) strncpy(t->tag, tag, TAG_LEN);

    if(!echan_sendO(tp->tasks, (eobj)t))
    {
        llog("[threadpool]: add new task [%s] faild, %d tasks now", (tag && *tag) ? tag : "", echan_size(tp->tasks));
        ell_freeO(0, (eobj)t);
        return 0;
    }

    llog("[threadpool]: add new task [%s] ok, %d tasks now", (tag && *tag) ? tag : "", echan_size(tp->tasks));
    return 1;
}

static inline void __ert_task_release(ert tp)
{
    llog("[threadpool]: %s", "releasing tasks");

    tasks_lock();

    echan_free(tp->tasks);          tp->tasks       = 0;
    ell_free  (tp->tasks_cache);    tp->tasks_cache = 0;
    ejson_free(tp->tasks_tags);     tp->tasks_tags  = 0;

    tasks_ulck();
}

static void* __task_thread(void* _th)
{
    TH th; ert tp; TASK t; char id[16];

#ifdef _WIN32_THREAD
    thread_t _th_backup = th;
#endif

    th = _th;
    tp = th->tp;

    __ert_thrd_idle_PP(tp);

    while((t = (TASK)echan_recvO(tp->tasks)))
    {
        __ert_thrd_idle_MM(tp);

        // -- this will happen when the chan is closed (ert is quited and releasing)
        if(!t)
            break;

        llog("[thread%s]: run %s.oprt", th->id, t->tag);
        t->oprt(t->data);
        if(t->after_oprt)
        {
            llog("[thread%s]: run %s.after_oprt", th->id, t->tag);
            t->after_oprt(t->data);
        }
        llog("[thread%s]: run %s over", th->id, t->tag);

        __ert_task_cache(tp, t);

        // -- we do not want too many waiting thread, it will affect performance seriously
        if(echan_rwait(tp->tasks) > _cpu_num)
        {
            _llog("[thread%s]: too many waiting thread, free self and quited", th->id);
            free(th);
            __ert_thrd_num_MM(tp);
            return 0;
        }

        __ert_thrd_idle_PP(tp);
    }

    memccpy(id, th->id, '\0', 16);
    free(th);

    llog("[thread%s]: free self and quited", id);

    while(!echan_closed(tp->tasks))
        usleep(100);

    emutex_lock(tp->thrds_mu);

    if(tp->thrds_num > 1)
    {
        tp->thrds_num--;
        emutex_ulck(tp->thrds_mu);
    }
    else
    {
        emutex_ulck(tp->thrds_mu);
        if(tp->quit_join_ths)
        {
            assert(echan_sendSig(tp->quit_sigs, 1));
        }
        else
        {
            __ert_task_release(tp);
            __ert_quitWait(tp);
            __ert_self_release(tp);

            _llog("[threadpool] free tp in [thread%s] \n", id);
        }
    }

#ifdef _WIN32_THREAD
    if(_th_backup) thread_quit(_th_backup);
#endif

    return 0;
}

static inline void __ert_thread_create_if_need(ert tp)
{
    TH th;

    if(tp->thrds_idle)
        return;

    if(tp->thrds_num >= tp->thrds_cap)
    {
        llog("[threadpool]: create new thread failed, reach max threads cnt");
        return ;
    }

    if((th = calloc(1, sizeof(*th))))
    {
        th->tp = tp;
        snprintf(th->id, 16, "%2d", tp->thrds_idx);

        if(ethread_init(th->th, __task_thread, th) != 0)
        {
            free(th);
            llog("[threadpool]: create new thread failed, %s", strerror(errno));
            return;
        }

        ethread_detach(th->th);

        emutex_lock(tp->thrds_mu);
        tp->thrds_num++;
        tp->thrds_idx++;
        emutex_ulck(tp->thrds_mu);

        llog("[threadpool]: create new thread%s", th->id);
    }
}

static inline void __ert_releasing(ert tp)
{
    if(__ert_thrd_num(tp) == 0)
    {
        __ert_task_release(tp);
        __ert_quitWait(tp);
        __ert_self_release(tp);

        llog("[threadpool] free tp in __ert_releasing() \n");

        return ;
    }

    if(tp->quit_join_ths)
    {
        tp->quit_sigs = echan_new(ECHAN_SIGS, INT_MAX);
    }

    echan_close(tp->tasks);     // close the chan, so the task thread will not recieve any tasks and quit automaticly

    if(tp->quit_join_ths)
    {
        // -- wait all the threads to quit
        echan_recvSig(tp->quit_sigs, 1);
        echan_free(tp->quit_sigs);

        __ert_task_release(tp);
        __ert_quitWait(tp);
        __ert_self_release(tp);

        _llog("[threadpool]: free tp in __ert_releasing() \n");
    }
}

/// ---------------------- creator -------------------------

ert  ert_new(int max_thread_num)
{
    if(_cpu_num == 0) _cpu_num = e_get_nprocs();

    return __ert_new(max_thread_num);
}

void ert_join(ert tp)
{
    is1_exe(tp == DEFAULT_ERT, tp = _df_tp);
    is0_ret(tp, );
    is0_ret(tp->status == _INITED, );

    __ert_exeWait(tp);
}


int ert_destroy(ert tp, int opt)
{
    int running = 0;

    is1_exe(tp == DEFAULT_ERT, tp = _df_tp);
    is0_ret(tp, 0);
    is0_ret(tp->status == _INITED, 0);

    tp->status        = _RELEASING;
    tp->quit_join_ths = opt & ERT_WAITING_RUNNING_TASKS;

    if(!tp->quit_join_ths)
    {
        emutex_lock(tp->thrds_mu);
        running = tp->thrds_num - tp->thrds_idle;
        emutex_ulck(tp->thrds_mu);
    }

    llog("[threadpool]: _RELEASING");
    __ert_releasing(tp);

    if(tp == _df_tp) _df_tp = 0;

    return running;
}

void ert_maxThread(ert tp, int num)
{
    is0_ret(tp && num > 0, );
    is1_exe(tp == DEFAULT_ERT, tp = _df_tp);
    is0_exeret(tp, _df_thread_num = num;, );

    emutex_lock(tp->thrds_mu);
    tp->thrds_cap = num ;
    emutex_ulck(tp->thrds_mu);
}

int ert_run(ert tp, constr tag, ert_cb oprt, ert_cb after_oprt, cptr arg)
{
    is1_ret(!tp || !oprt, 0);

    if(tp == DEFAULT_ERT)
    {
        is0_exe(_df_tp, _df_tp = ert_new(_df_thread_num));
        is0_ret(_df_tp, 0);

        tp = _df_tp;
    }

    is0_ret(tp, 0);
    emutex_lock(tp->thrds_mu);
    is0_exeret(tp->status == _INITED, emutex_ulck(tp->thrds_mu);, 0);
    emutex_ulck(tp->thrds_mu);

    is0_ret(__ert_task_add(tp, tag, oprt, after_oprt, arg), 0);
    __ert_thread_create_if_need(tp);

    return 1;
}

/// @brief ert_query - to query a task whether is in the threadpool or not
///
/// @param tp  - the pointor to a threadpool returned by TP_New()
/// @param tag - the tag of the task you want to query
/// @return [0] - this task is running over or haven't been added to the threadpool
///         [1] - this task is running or in the waiting list
///
int  ert_query(ert tp, constr tag)
{
    int ret;

    is1_exe(tp == DEFAULT_ERT, tp = _df_tp);
    is0_ret(tp, 0);
    is0_ret(tp->status == _INITED, 0);

    tasks_lock();
    ret = ejson_r(tp->tasks_tags, tag) ? 1 : 0;
    tasks_ulck();

    return ret;
}
