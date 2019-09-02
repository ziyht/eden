/// =====================================================================================
///
///       Filename:  etimer.c
///
///    Description:  a easier timer to run task
///
///        Version:  1.0
///        Created:  03/13/2017 11:00:34 AM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#define _GNU_SOURCE
#define __USE_XOPEN

#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>

#include <etime.h>
#include "etimer.h"
#include "ejson.h"
#include "ert.h"
#include "echan.h"
#include "esl.h"
#include "ecompat.h"
#include "ethread.h"

#define ETIMER_VERSION "etimer 1.1.6"       // try perfect __elapsesecffmt(), not over

#define  ELOG_LEVEL ELOG_DBG
//#include "elog.h"
#define EUTILS_LLOG 0
#include "eutils.h"

#if ETOOLS_HAVE_EPOLL_H

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/syscall.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define __NR_epoll_wait 232

#if defined(__alpha__)
# define UV__O_CLOEXEC        0x200000
#elif defined(__hppa__)
# define UV__O_CLOEXEC        0x200000
#elif defined(__sparc__)
# define UV__O_CLOEXEC        0x400000
#else
# define UV__O_CLOEXEC        0x80000
#endif

#define UV__EPOLL_CLOEXEC     UV__O_CLOEXEC
#define UV__EPOLL_CTL_ADD     1
#define UV__EPOLL_CTL_DEL     2
#define UV__EPOLL_CTL_MOD     3

#define __NR_epoll_create 213
#define __NR_epoll_create1 291
#endif

//#define ETOOLS_HAVE_EPOLL_H 0            // for testing

/// -- struct -------------------------------------------

struct etloop_s
{
    ert             tp;         // threadpool for running tasks

    volatile int    quit;
    volatile int    running;

//    cond_t          co_wait;
//    emutex_t         mu_wait;
    echan           sigs;

    emutex_t        mu;
    ethread_t       tl_th;
    esl             tl;         // timeline of tasks

    i64             wait_ms;

    int             epoll_fd;         // for epoll
    int             timer_fd;
volatile    int         sem;
};

typedef struct _etimer_s{
    cptr    __;                 // userdata, must in first place

    etloop  loop;

    u64     nextdl;
    u64     repeat;

    etm_cb  cb;

    uint    active   : 1;
    uint    running  : 1;
    uint    free     : 1;
    uint    cancle   : 1;
    uint    fresh    : 1;

    emutex_t mu;

}_etimer_t, * _etimer;


static etloop _df_loop;
static int    _df_thrdsnum = 4;
static ejson  _all_loops;
static ert    _inner_rt;

static emutex_t _wakeup_mu;

static inline void __wakeup_inner_loop(etloop loop);

/// -- etloop -------------------------------------------

static inline u64 __nextdl(i64 ms)
{
    return ms < 1 ? e_nowms()
                  : e_nowms() + ms;
}

static void __task_after_cb(_etimer e)
{
    emutex_lock(e->mu);

    e->running = 0;

    llog("after_cb nextdl: %ld", e->nextdl);

    if(e->cancle)
    {
        if(e->free)
        {
            emutex_ulck(e->mu);
            free(e);

            return ;
        }
    }

    if(e->fresh) e->active = 1;

    emutex_ulck(e->mu);


    etloop loop = e->loop;
    emutex_lock(loop->mu);

    if(loop->quit == 2)
    {
        if(e->cancle && e->free) free(e);

        if(loop->running <= 1)
        {
            emutex_ulck(loop->mu);
            free(loop);
            return;
        }

        loop->running--;
        emutex_ulck(loop->mu);
        return ;
    }

    if(e->fresh)
    {
        esl_addP(loop->tl, ekey_i(e->nextdl), e);
        __wakeup_inner_loop(loop);
    }
    emutex_ulck(loop->mu);
}

struct uv__epoll_event {
  uint32_t events;
  uint64_t data;
};

/**
 * @brief __etimer_run
 * @param loop
 * @param e
 * @return 0 : once running timer or canceled timer, cann't add to loop again
 *         1 : can add to loop again
 */
static int   __etimer_run(etloop loop, _etimer e, u64 now)
{
    int readd = 0;

    llog("----- timer: %d %p", esl_len(loop->tl), (void*)e);

    if(e->cancle)
    {
        e->active = 0;

        if(e->free)
        {
            free(e);

            llog("free  timer: %d %p", esl_len(loop->tl), (void*)e);
        }

        return 0;
    }

    e->active = e->repeat > 0;
    readd     = e->repeat > 0;

    if(e->running)
    {
        llog("run4");
        llog("skip  timer: %d %p", esl_len(loop->tl), (void*)e);
    }
    else if(loop->tp)        // running in tp
    {
        e->fresh = 0;
        e->nextdl = e->repeat ? e->nextdl + (((now - e->nextdl) / e->repeat) + 1) * e->repeat : 0;
        llog("run5");
        e->running = 1;
        ert_run(loop->tp, 0, (ert_cb)e->cb, (ert_cb)__task_after_cb, e);
    }
    else
    {
        e->fresh = 0;
        e->cb((etimer)e);
        e->nextdl = e->repeat ? e->nextdl + (((now - e->nextdl) / e->repeat) + 1) * e->repeat : 0;
    }

    return readd;
}

static void __polling_wakeup_all_loops(void* arg)
{
    int loop_cnt; ejson itr, all_loops; etloop loop;

    all_loops = (ejson)arg;

    do{
        sleep(10);

        emutex_lock(_wakeup_mu);

        ejson_foreach(all_loops, itr)
        {
            loop = EOBJ_VALP(itr);

            __wakeup_inner_loop(loop);
        }

        loop_cnt = ejson_len(all_loops);
        emutex_ulck(_wakeup_mu);

    }while(loop_cnt);

    ejson_free(all_loops);
}

static int __register_wakeup(etloop loop)
{
    char key[64];

    static int _init;

    if(!_init)
    {
        _init = 1;
        emutex_init(_wakeup_mu);
    }

    emutex_lock(_wakeup_mu);
    if(!_inner_rt)
    {
        _inner_rt  = ert_new(1);
        _all_loops = ejson_new(EOBJ, EVAL_0);
    }

    snprintf(key, 64, "%p", (void*)loop);

    ejson_addP(_all_loops, key, loop);
    ert_run(_inner_rt, "wakeup_guard", __polling_wakeup_all_loops, 0, _all_loops);

    emutex_ulck(_wakeup_mu);

    return 1;
}

static int __unregister_wakeup(etloop loop)
{
    char key[64];

    if(!_all_loops)
        return 1;

    snprintf(key, 64, "%p", (void*)loop);

    emutex_lock(_wakeup_mu);
    ejson_freeR(_all_loops, key);

    if(0 == ejson_len(_all_loops))
    {
        ert_destroy(_inner_rt, 0);

        _all_loops = 0;
        _inner_rt  = 0;
    }

    emutex_ulck(_wakeup_mu);

    return 1;
}

static inline void __wakeup_inner_loop(etloop loop)
{
#if ETOOLS_HAVE_EPOLL_H

    int res;
    struct itimerspec  timer = {{0, 0}, {0, 0}};
    struct epoll_event epe   = {EPOLLIN | EPOLLET, {0}};

    if(loop->wait_ms)
    {
        esln first = esl_first(loop->tl);

        if(first)
        {
            i64 need_wait = first->score - eutils_nowms();
            if(need_wait > 0 && (loop->wait_ms > (i32)need_wait))
                loop->wait_ms = need_wait;
            else if(need_wait <= 0)
                 loop->wait_ms = 0;
        }

        if(loop->wait_ms > 60000)
            loop->wait_ms = 60000;
    }

    llog("epoll wait set: %ld", loop->wait_ms);

    timer.it_value.tv_nsec = loop->wait_ms % 1000 * 1000000;
    timer.it_value.tv_sec  = loop->wait_ms / 1000;
    if(0 == loop->wait_ms) timer.it_value.tv_nsec = 1;

    res = timerfd_settime(loop->timer_fd, 0, &timer, 0);
    if(res == -1) {
        perror("timerfd_settime");
        return;
    }

    epe.data.fd = loop->timer_fd;
    res = epoll_ctl(loop->epoll_fd, EPOLL_CTL_MOD, loop->timer_fd, &epe);
    if(res == -1) {
        perror("epoll_ctl");
        return;
    }
#else
    //cond_all(loop->co_wait);
#endif

    //mutex_ulck(loop->mu_wait);
    echan_sendSig(loop->sigs, 1);
}

static void* __inner_loop(void* arg)
{
    eobj first, itr; _etimer e; i64 now; esl sl; etloop loop;

#if ETOOLS_HAVE_EPOLL_H
    struct uv__epoll_event events[1];
#endif

    loop = (etloop)arg;
    sl   = esl_new(0);

    while(!loop->quit)
    {
        llog("timers: %d", esl_len(loop->tl));
        if(!(first = esl_first(loop->tl)))
        {
            llog("loop: no timer lock");
            //cond_twait(loop->co_wait, loop->mu_wait, 1000);
            //mutex_lock(loop->mu_wait);
            echan_recvAll(loop->sigs);
            llog("loop: no timer lock over");

            continue;
        }
        else
            echan_tryRecvSigs(loop->sigs, 1000);


        now    = e_nowms();

        do{
            loop->wait_ms = eobj_keyI(first) - now;

            if     (loop->wait_ms > 60000)   loop->wait_ms = 60000;
            else if(loop->wait_ms < 0)
            {
                llog("loop: first: %"PRIi64" now: %"PRIi64", break!", first->score, now);
                break;
            }

            llog("loop: wait %d ms needed", loop->wait_ms);
#if ETOOLS_HAVE_EPOLL_H
            syscall(__NR_epoll_wait, loop->epoll_fd, events, ARRAY_SIZE(events), loop->wait_ms);
#else
            //cond_twait(loop->co_wait, loop->mu_wait, loop->wait_ms);
            echan_timeRecvSigs(loop->sigs, 1, (int)loop->wait_ms);
#endif
            llog("loop: wait %"PRIi64" ms actrually", eutils_nowms() - now);

            now   = e_nowms();
            first = esl_first(loop->tl);
        }while(first);

        emutex_lock(loop->mu);

        if(loop->quit)
        {
            emutex_ulck(loop->mu);
            goto quit_loop;
        }

        do{
            if(eobj_keyI(first) > now)
            {
                llog("first: %"PRIi64" now: %"PRIi64", break!", first->score, now);
                break;
            }

            e = eobj_valP(first);

            emutex_lock(e->mu);
            if(__etimer_run(loop, e, now)) esl_addO(sl, ekey_i(e->nextdl), esl_takeH(loop->tl));
            else                           esl_freeH(loop->tl);
            emutex_ulck(e->mu);

        }while((first = esl_first(loop->tl)));

        while((itr = esl_takeH(sl)))
        {
            esl_addO(loop->tl, ekey_i(eobj_keyI(itr)), itr);
            llog("readd: %"PRIi64" %p %d timers", e->nextdl, (void*)e, esl_len(loop->tl));
        }

        emutex_ulck(loop->mu);
    }

quit_loop:
    esl_free(sl);

    return 0;
}

etloop etloop_new(int maxthread)
{
    etloop loop;

    if(maxthread < 1)  maxthread = 1;

    is0_ret(loop = calloc(1, sizeof(*loop)), NULL);

    loop->tp  = ert_new(maxthread);
    loop->tl  = esl_new(0);

    emutex_init(loop->mu);
//    emutex_init(loop->mu_wait);
//    cond_init (loop->co_wait);
    loop->sigs = echan_new(E_SIG, UINT_MAX);

    if(!loop->tp || !loop->tl || ethread_init(loop->tl_th, __inner_loop, loop))
        goto err_ret;

#if ETOOLS_HAVE_EPOLL_H
    int epoll_fd, timer_fd, res; struct itimerspec itimer; struct epoll_event ep_ev;

    epoll_fd = syscall(__NR_epoll_create1, UV__EPOLL_CLOEXEC);
    if (epoll_fd == -1 && (errno == ENOSYS || errno == EINVAL)) {
        epoll_fd = syscall(__NR_epoll_create, 256);
        if (epoll_fd == -1)
          goto err_ret;
    }

    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if(timer_fd == -1) {
        perror("timerfd_create");
        close(epoll_fd);
        goto err_ret;
    }

    memset(&itimer, 0, sizeof(itimer));
    res = timerfd_settime(timer_fd, 0, &itimer, 0);
    if(res == -1) {
        perror("timerfd_settime");
        close(epoll_fd);close(timer_fd);
        goto err_ret;
    }

    memset(&ep_ev, 0, sizeof(ep_ev));
    ep_ev.data.fd = timer_fd;
    ep_ev.events  = EPOLLIN | EPOLLET;

    res = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &ep_ev);
    if(res == -1) {
        perror("epoll_ctl:EPOLL_CTL_ADD");
        close(epoll_fd);close(timer_fd);
        goto err_ret;
    }

    loop->epoll_fd = epoll_fd;
    loop->timer_fd = timer_fd;
#else
    __register_wakeup(loop);
#endif

    return loop;

err_ret:

    emutex_free(loop->mu);
//    emutex_free(loop->mu_wait);
//    cond_free (loop->co_wait);
    echan_free(loop->sigs);

    ert_destroy(loop->tp, 0);
    esl_free(loop->tl);

    free(loop);

    return 0;
}

etloop etloop_df(int maxthread)
{
    if(maxthread > 0) _df_thrdsnum = maxthread;

    if(!_df_loop) return _df_loop = etloop_new(_df_thrdsnum);
    else          ert_maxThread(_df_loop->tp, _df_thrdsnum);

    return _df_loop;
}

void   etloop_stop(etloop loop)
{
    int running;

    is0_ret(loop, );

    if(loop == _df_loop) _df_loop = 0;

#if !ETOOLS_HAVE_EPOLL_H
    __unregister_wakeup(loop);
#endif

    loop->quit    = 1;
    loop->wait_ms = 0;
    __wakeup_inner_loop(loop);
    ethread_join(loop->tl_th);

    emutex_lock(loop->mu);
    loop->quit    = 2;
    echan_free(loop->sigs);

    esl_foreach_s(loop->tl, itr)
    {
        llog("free etimer: %p %d", itr->p, esl_len(loop->tl));
        ((_etimer)EOBJ_VALP(itr))->active = 0;
    }

    esl_free(loop->tl);       loop->tl = 0;
    running = ert_destroy(loop->tp, 0); loop->tp = 0;

#if ETOOLS_HAVE_EPOLL_H
    close(loop->epoll_fd);
    close(loop->timer_fd);
#endif

    loop->running = running;

    emutex_ulck(loop->mu);

    if(!running)
        free(loop);
}

etimer etimer_new(etloop loop)
{
    _etimer e;

    is0_ret(loop, NULL);
    is1_ret(loop->quit, NULL);
    is0_ret(e = calloc(1, sizeof(*e)), NULL);

    e->loop = loop;
    emutex_init(e->mu);

    return (etimer)e;
}

void   etimer_destroy(etimer _e)
{
    _etimer e = (_etimer)_e;

    is0_exe(e, );

    emutex_lock(e->mu);
    if(e->active)
    {
        e->repeat = 0;
        e->cancle = 1;
        e->free   = 1;
        e->nextdl = 0;

        emutex_ulck(e->mu);
    }
    else
    {
        emutex_ulck(e->mu);
        emutex_free(e->mu);
        free(e);
    }

    //_llog("destroy timer");
}

int    etimer_start(etimer _e, etm_cb cb, u64 timeout, u64 repeat)
{
    _etimer e; etloop loop;

    is1_ret(!_e || !cb, 0);

    e = (_etimer)_e;

    // -- check and update etimer
    emutex_lock(e->mu);

    e->cb     = cb;
    e->repeat = repeat;
    e->cancle = 0;
    e->fresh  = 1;

    is1_exeret(e->active, emutex_ulck(e->mu), 0);

    e->nextdl = __nextdl(timeout);
    e->active = 1;

    emutex_ulck(e->mu);

    // -- add to loop
    loop = e->loop;
    emutex_lock(loop->mu);

    esl_addP(loop->tl, ekey_i(e->nextdl), e);
    llog("added: %d", esl_len(loop->tl));

    loop->wait_ms = timeout;
    __wakeup_inner_loop(loop);
    llog("ulck and cond_all: %"PRIi64"", e->nextdl);

    emutex_ulck(loop->mu);

    return 1;
}

int    etimer_stop (etimer _e)
{
    _etimer e; //etloop loop;

    is0_ret(_e, 0);

    e    = (_etimer)_e;
    //loop = e->loop;

    emutex_lock(e->mu);
    llog("set cancle");
    e->cancle = 1;
    e->repeat = 0;
    emutex_ulck(e->mu);

    return 1;
}

int    etimer_runing(etimer _e)
{
    _etimer e = (_etimer)_e;

    return e ? e->active : 0;
}

#ifdef _WIN32
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
//#include <Winsock2.h>					// net and gethostname
//#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define __need_timeval
#ifndef __APPLE__
#include <bits/time.h>
#endif
#include <sys/time.h>
#endif

#pragma pack(push, 1)
typedef struct _ntp_time_s{
    uint coarse;
    uint fine;
}_ntp_time_t;

typedef struct _ntp_packet_s
{
    u8          li_vn_mode;          // log(2) + version(3) + mode(3)
    u8          stratum;
    u8          poll;
    u8          precision;
    uint        root_delay;
    uint        root_dispersion;
    i8          ref_id[4];

    _ntp_time_t reference;
    _ntp_time_t originate;
    _ntp_time_t receive;
    _ntp_time_t transmit;

}_ntp_packet_t, * _ntp_packet;
#pragma pack(pop)

#define JAN_1970        0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT        123
#define NTP_PACK_LEN    48

#define LI          0
#define VN          3
#define MODE        3
#define STRATUM     0
#define POLL        4
#define PREC       -6

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) ( 4294*(x) + ( (1981*(x))>>11 ) )

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr    sockaddr;

#ifdef _WIN32
/* For windows implementation of "gettimeofday" Thanks to "http://www.cpp-programming.net/c-tidbits/gettimeofday-function-for-windows" */
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

#ifdef _MSC_VER
struct timezone
{
    int  tz_minuteswest; // minutes W of Greenwich
    int  tz_dsttime;     // type of dst correction
};
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME   ft;
    uint64_t   tmpres = 0;
    static int tzflag = 0;

    if (tv)
    {
#ifdef _WIN32_WCE
        SYSTEMTIME st;
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);
#else
        GetSystemTimeAsFileTime(&ft);
#endif

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;


        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec  = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (tz)
    {
        if (!tzflag)
        {
#if !TSK_UNDER_WINDOWS_RT
            _tzset();
#endif
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime     = _daylight;
    }

    return 0;
}
#endif

static int __get_addr(constr server, sockaddr_in* addr)
{
    cstr sport; int iport; char buf[32];

    is1_ret(!server || !*server, 0);

    strncpy(buf, server, 32);

    sport = strchr(buf, ':');
    if(sport)
    {
        *sport = '\0';
        sport++;
    }
    iport = sport ? atol(sport) : 123;

    addr->sin_port        = htons(iport);
    addr->sin_family      = AF_INET;
    addr->sin_addr.s_addr = inet_addr(buf);

    is1_ret(addr->sin_addr.s_addr == (uint)-1, 0);

    memset(&(addr->sin_zero),0, 8);

    return 1;
}

static void __pack_ntp_req(_ntp_packet req)
{
    struct timeval now;

    memset(req, 0, sizeof(*req));

    ((uint*)req)[0] = htonl (
        ( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
        ( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );

    gettimeofday(&now, NULL);
    req->transmit.coarse = htonl(now.tv_sec + JAN_1970);
    req->transmit.fine   = htonl(NTPFRAC(now.tv_usec));

}
static void __unpack_ntp_res(_ntp_packet res)
{
    res->root_delay       = ntohl(res->root_delay);
    res->root_dispersion  = ntohl(res->root_dispersion);
    res->reference.coarse = ntohl(res->reference.coarse);
    res->reference.fine   = USEC(ntohl(res->reference.fine));   // convert to usec
    res->originate.coarse = ntohl(res->originate.coarse);
    res->originate.fine   = USEC(ntohl(res->originate.fine));
    res->receive.coarse   = ntohl(res->receive.coarse);
    res->receive.fine     = USEC(ntohl(res->receive.fine));
    res->transmit.coarse  = ntohl(res->transmit.coarse);
    res->transmit.fine    = USEC(ntohl(res->transmit.fine));
}

esync etimer_esyncGet(constr server, int timeout)
{
    sockaddr_in addr;  struct timeval wait, end; fd_set inset; _ntp_packet_t req_pack, res_pack; int diff_sec, diff_usec;//, delay_sec, delay_usec;
    esync out = {0,0,0,0};

    assert(sizeof(_ntp_packet_t) == NTP_PACK_LEN);

#ifdef _WIN32
    static int _init;

    SOCKET sockfd;

    if(!_init)
    {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != NO_ERROR)
        {
            // printf("WSAStartup failed: %d\n", iResult);
            WSACleanup();
            return out;
        }

        _init = 1;
    }
#else
    int sockfd;
#endif

    is0_exeret(__get_addr(server, &addr), out.err = "invalid server", out);
    is1_exeret((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0,  out.err = "create socket faild", out);

    wait.tv_sec  = timeout / 1000;
    wait.tv_usec = (timeout % 1000) * 1000;

    FD_ZERO(&inset);
    FD_SET(sockfd, &inset);

    __pack_ntp_req(&req_pack);

    // -- request
    sendto(sockfd, (constr)&req_pack, NTP_PACK_LEN, 0, (sockaddr *)&addr, sizeof(sockaddr));

    // -- recieve response
    if(select((int)(sockfd + 1), &inset, NULL, NULL, &wait) < 0)
    {
        goto ret;
    }
    else
    {
        if(FD_ISSET(sockfd, &inset))
        {
            if(recv(sockfd, (cstr)&res_pack, NTP_PACK_LEN, 0) < 0)
            {
                out.err = "res recv err";
                goto ret;
            }
        }
        else
        {
            out.err = "timeout";
            goto ret;
        }
    }

    gettimeofday(&end, NULL);
    end.tv_sec += JAN_1970;   // fanal recieve timestamp
    __unpack_ntp_res(&res_pack);

    diff_sec   = (((int)res_pack.receive.coarse - (int)res_pack.originate.coarse) + ((int)res_pack.transmit.coarse - (int)end.tv_sec )) / 2;
    diff_usec  = (((int)res_pack.receive.fine   - (int)res_pack.originate.fine  ) + ((int)res_pack.transmit.fine   - (int)end.tv_usec)) / 2;
    // delay_sec  = (((int)end.tv_sec  - (int)res_pack.originate.coarse) - ((int)res_pack.transmit.coarse - (int)res_pack.receive.coarse)) / 2;
    // delay_usec = (((int)end.tv_usec - (int)res_pack.originate.fine  ) - ((int)res_pack.transmit.fine   - (int)res_pack.receive.fine  )) / 2;

    out.offusec = diff_sec * 1000000LL + diff_usec;
    out.nowusec = (end.tv_sec - JAN_1970) * 1000000LL + end.tv_usec;

    out.status = 1;

ret:
#if _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
    return out;
}
