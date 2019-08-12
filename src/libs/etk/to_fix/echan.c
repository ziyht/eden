/// =====================================================================================
///
///       Filename:  echan.c
///
///    Description:  A Pure C implementation of Go channels.
///
///        Version:  1.1
///        Created:  06/17/2017 11:31:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifdef __APPLE__
#define _XOPEN_SOURCE
#endif

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "ecompat.h"
#include "etype.h"
#include "ell.h"
#include "eutils.h"
#include "ethread.h"

#include "echan.h"


#pragma pack(1)
typedef struct _header_s{
    uint type     :  4;
    uint reserved :  7;
    uint alloc    :  1;
    uint multi    :  1;
    uint str_key  :  1;
    uint is_array :  1;
    uint is_ref   :  1;
    uint _ref     : 16;
    uint _len     : 32;
}_header_t, * _header;

typedef struct _qnode_s{
    _header_t header;
    eval_t    val;
}_qnode_t, * _qnode;

typedef struct _queue_s
{
    uint        size;
    uint        next;
    uint        capacity;
    _qnode_t*   arr;
}_queue_t, *_queue;
#pragma pack()

#define _n_o(n)         &(n)->val
#define _n_new(l)       malloc(sizeof(_qnode_t) - sizeof(eval_t) + 1 + l)
#define _n_init(n)      memset(n, 0, sizeof(_qnode_t) - sizeof(eval_t))
#define _n_free(n)      free(n)
#define _n_type(n)      (n)->header.type
#define _n_alloc(n)     (n)->header.alloc
#define _n_valR(n)      (n)->val.r
#define _n_valF(n)      (n)->val.f[1]
#define _n_valI(n)      (n)->val.i
#define _n_valP(n)      (cptr)(n)->val.p
#define _n_valS(n)      (n)->val.r
#define _n_lenR(n)      (n)->header._len

#define _o_n(o)         container_of(o, _qnode_t, val)
#define _o_free(o)      _n_free(_o_n(o))
#define _o_type(o)      _n_type(_o_n(o))
#define _o_valI(o)      ((s64*)(o))[0]
#define _o_valF(o)      ((f64*)(o))[1]
#define _o_valS(o)      (cstr)o
#define _o_valR(o)      (o)
#define _o_valP(o)      ((cptr*)(o))[0]

#define _queue_is_full(q) (q)->size >= (q)->capacity

static inline _queue _queue_new(uint capacity)
{
    if (capacity > INT_MAX / sizeof(_qnode_t))
    {
        errno = EINVAL;
        return 0;
    }

    _queue    q   = (_queue)malloc(sizeof(_queue_t));
    _qnode_t* arr = (_qnode)malloc(capacity * sizeof(_qnode_t));
    if (!q || !arr)
    {
        // In case of free(NULL), no operation is performed.
        free(q);
        free(arr);
        errno = ENOMEM;
        return 0;
    }

    q->size     = 0;
    q->next     = 0;
    q->capacity = capacity;
    q->arr      = arr;
    return q;
}

static inline void _queue_free(_queue q)
{
    uint i, pos = q->next;

    for(i = 0; i < q->size; i++, pos++)
    {
        if(pos >= q->capacity)
            pos -= q->capacity;

        switch (_n_type(&q->arr[pos])) {
            case ENUM:
            case EPTR:  if(!_n_alloc(&q->arr[pos]))
                                break;
            default      :  _n_free(_n_valP(&q->arr[pos]));
        }
    }

    free(q->arr);
    free(q);
}

static inline int _queue_addNode(_queue q, _qnode node)
{
    if (q->size >= q->capacity)
    {
        errno = ENOBUFS;
        return 0;
    }

    uint pos = q->next + q->size;
    if (pos >= q->capacity)
    {
       pos -= q->capacity;
    }

    q->arr[pos] = *node;
    q->size++;

    return 1;
}

static inline _qnode _queue_popH(_queue q)    // must be mutexed with _queue_add APIs
{
    _qnode value = NULL;

    if (q->size > 0)
    {
        value = &q->arr[q->next];
        q->next++;
        q->size--;
        if (q->next >= q->capacity)
        {
            q->next -= q->capacity;
        }
    }

    return value;
}

static inline _qnode _queue_peek(_queue q)
{
    return q->size ? &q->arr[q->next] : NULL;
}


typedef void (*__echan_free_cb)(echan chan);

typedef int  (*__echan_send_cb   )(echan chan, eval_t val);
typedef int  (*__echan_sendBin_cb)(echan chan, conptr in, uint inlen);
typedef int  (*__echan_sendI64_cb)(echan chan, i64    val);
typedef int  (*__echan_sendF64_cb)(echan chan, double val);
typedef int  (*__echan_sendStr_cb)(echan chan, constr str);
typedef int  (*__echan_sendPtr_cb)(echan chan, conptr ptr);
typedef int  (*__echan_sendObj_cb)(echan chan, eobj   obj);
typedef int  (*__echan_sendSig_cb)(echan chan, uint   sig);

typedef int  (*__echan_recv_cb   )(echan chan, uint type, eobj* _obj);
typedef eobj (*__echan_recvBin_cb)(echan chan);
typedef i64  (*__echan_recvI64_cb)(echan chan);
typedef f64  (*__echan_recvF64_cb)(echan chan);
typedef eobj (*__echan_recvStr_cb)(echan chan);
typedef cptr (*__echan_recvPtr_cb)(echan chan);
typedef eobj (*__echan_recvObj_cb)(echan chan);
typedef int  (*__echan_recvSig_cb)(echan chan, uint sig);
typedef eval (*__echan_recvALL_cb)(echan chan);

typedef int  (*__echan_tryRecv_cb   )(echan chan, uint type, eobj* _obj);
typedef eobj (*__echan_tryRecvObj_cb)(echan chan);
typedef int  (*__echan_tryRecvSig_cb)(echan chan, uint sig);
typedef eval (*__echan_tryRecvAll_cb)(echan chan);

typedef int  (*__echan_timeRecv_cb   )(echan chan, uint type, eobj* _obj, int timeout);
typedef eobj (*__echan_timeRecvObj_cb)(echan chan,                        int timeout);
typedef int  (*__echan_timeRecvSig_cb)(echan chan, uint sig,              int timeout);

typedef struct _oprt_s{
    __echan_send_cb    send;
    __echan_sendBin_cb sendBin;
    __echan_sendI64_cb sendI64;
    __echan_sendF64_cb sendF64;
    __echan_sendStr_cb sendStr;
    __echan_sendPtr_cb sendPtr;
    __echan_sendObj_cb sendObj;
    __echan_sendSig_cb sendSig;

    __echan_recv_cb    recv;
    __echan_recvBin_cb recvBin;
    __echan_recvI64_cb recvI64;
    __echan_recvF64_cb recvF64;
    __echan_recvStr_cb recvStr;
    __echan_recvPtr_cb recvPtr;
    __echan_recvObj_cb recvObj;
    __echan_recvSig_cb recvSig;
    __echan_recvALL_cb recvAll;

    __echan_tryRecv_cb    tryRecv;
    __echan_tryRecvObj_cb tryRecvObj;
    __echan_tryRecvSig_cb tryRecvSig;
    __echan_tryRecvAll_cb tryRecvAll;

    __echan_timeRecv_cb    timeRecv;
    __echan_timeRecvObj_cb timeRecvObj;
    __echan_timeRecvSig_cb timeRecvSig;
}_oprt_t, * _oprt;

#pragma pack(push, 1)
typedef struct echan_s
{
    _oprt       _oprt;

    // channal data
    uint        cap;
    union{
        ell         list;
        uint        sigs;
        _qnode_t    node;
    }           channal;

    // Shared properties
    emutex_t    r_mu;
    emutex_t    w_mu;
    emutex_t    m_mu;
    econd_t     r_cond;
    econd_t     w_cond;
    int         r_waiting;
    int         w_waiting;
    int         status;
}echan_t;
#pragma pack(pop)

enum {
    // -- echan status
    _OPENDED = 0,
    _CLOSING ,
    _CLOSED  ,
};

static int __chan_init_unbuffered(echan chan)
{
    memset(chan, 0, sizeof(*chan));

    is1_ret(emutex_init(chan->w_mu), 0);

    is1_exeret(emutex_init(chan->r_mu),
               emutex_free(chan->w_mu);,
               0);

    is1_exeret(emutex_init(chan->m_mu),
               emutex_free(chan->w_mu); emutex_free(chan->r_mu);,
               0);

    is1_exeret(econd_init(chan->r_cond),
               emutex_free(chan->w_mu); emutex_free(chan->r_mu); emutex_free(chan->m_mu);,
               0);

    is1_exeret(econd_init(chan->w_cond),
               emutex_free(chan->w_mu); emutex_free(chan->r_mu); emutex_free(chan->m_mu); econd_free(chan->r_cond);,
               0);

    return 1;
}

static int __chan_init_buffered(echan chan, int opts, uint capacity)
{
    ell  l = 0;

    is0_ret(__chan_init_unbuffered(chan), 0);

    if((opts & 0x01) == 0)    // list
    {
        l = ell_new();
        is0_ret(l, 0);

        chan->channal.list = l;
    }

    chan->cap = capacity;

    return 1;
}

static int __echan_send_buffered_list(echan chan, eval_t obj);
void echan_free(echan chan)
{
    is0_ret(chan, );

    echan_close(chan);

    if(chan->_oprt->send == __echan_send_buffered_list)
    {
        ell_free(chan->channal.list);
    }

    emutex_free(chan->w_mu);
    emutex_free(chan->r_mu);

    emutex_free(chan->m_mu);
    econd_free (chan->r_cond);
    econd_free (chan->w_cond);
    free(chan);
}

// Once a channel is closed, data cannot be sent into it. If the channel is
// buffered, data can be read from it until it is empty, after which reads will
// return an error code. Reading from a closed channel that is unbuffered will
// return an error code. Closing a channel does not release its resources. This
// must be done with a call to chan_dispose. Returns 0 if the channel was
// successfully closed, -1 otherwise. If -1 is returned, errno will be set.
int echan_close(echan chan)
{
    int success;

    is0_ret(chan, 0);

    success = 1;
    emutex_lock(chan->m_mu);
    if (chan->status != _OPENDED)
    {
        // Channel already closed.
        success = 0;
        errno = EPIPE;
    }
    else
    {
        // Otherwise close it.
        chan->status = _CLOSING;
        while(chan->r_waiting || chan->w_waiting)
        {
            emutex_ulck(chan->m_mu);
            econd_all(chan->r_cond);
            econd_all(chan->w_cond);
            emutex_lock(chan->m_mu);
            sleep(0);
        }
    }
    chan->status = _CLOSED;
    emutex_ulck(chan->m_mu);

    return success;
}

int echan_closed(echan chan)
{
    int closed;

    is0_ret(chan, 1);

    emutex_lock(chan->m_mu);
    closed = chan->status == _CLOSED;
    emutex_ulck(chan->m_mu);
    return closed;
}

static int __echan_send_buffered_sigs(echan chan, eval_t sigs)
{
    is0_ret(sigs.i32, 0);

    emutex_lock(chan->m_mu);
    while( chan->channal.sigs + sigs.i32 > chan->cap )
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is removed.
        chan->w_waiting++;
        econd_wait(chan->w_cond, chan->m_mu);
        chan->w_waiting--;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    chan->channal.sigs += sigs.i32;

    if (chan->r_waiting > 0)
    {
        // Signal waiting reader.
        econd_one(chan->r_cond);
    }

    emutex_ulck(chan->m_mu);

    return 1;
}

static int __echan_send_unbuffered_sigs(echan chan, eval_t sigs)
{
    is0_ret(sigs.i32, 0);

    emutex_lock(chan->w_mu);
    emutex_lock(chan->m_mu);

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        emutex_ulck(chan->w_mu);
        errno = EPIPE;
        return 0;
    }

    chan->channal.sigs = sigs.i32;
    chan->w_waiting++;

    if (chan->r_waiting > 0)
    {
        // Signal waiting reader.
        econd_one(chan->r_cond);
    }

    // Block until reader consumed chan->data.
    econd_wait(chan->w_cond, chan->m_mu);

    emutex_ulck(chan->m_mu);
    emutex_ulck(chan->w_mu);

    return 1;
}

static int __echan_send_buffered_list(echan chan, eval_t obj)
{
    emutex_lock(chan->m_mu);
    while( chan->cap == ell_size(chan->channal.list))
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is removed.
        chan->w_waiting++;
        econd_wait(chan->w_cond, chan->m_mu);
        chan->w_waiting--;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    int success = ell_appdO(chan->channal.list, obj.p) != 0;

    if (chan->r_waiting > 0)
    {
        // Signal waiting reader.
        econd_one(chan->r_cond);
    }

    emutex_ulck(chan->m_mu);
    return success;
}

static int __echan_send_unbuffered_list(echan chan, eval_t obj)
{
    emutex_lock(chan->w_mu);
    emutex_lock(chan->m_mu);

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        emutex_ulck(chan->w_mu);
        errno = EPIPE;
        return 0;
    }

    chan->channal.node.val.p = obj.p;
    chan->w_waiting++;

    if (chan->r_waiting > 0)
    {
        // Signal waiting reader.
        econd_one(chan->r_cond);
    }

    // Block until reader consumed chan->data.
    econd_wait(chan->w_cond, chan->m_mu);

    emutex_ulck(chan->m_mu);
    emutex_ulck(chan->w_mu);

    return 1;
}

static int __echan_recv_buffered_sigs(echan chan, uint sigs, eobj* _obj)
{
    is0_ret(sigs, 0);

    emutex_lock(chan->m_mu);

recv_again:
    while (chan->channal.sigs == 0)
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is added.
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    if(chan->channal.sigs < sigs)
    {
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;

        goto recv_again;
    }

    chan->channal.sigs -= sigs;
    if (chan->w_waiting > 0)
    {
        // Signal waiting writer.
        econd_one(chan->w_cond);
    }

    emutex_ulck(chan->m_mu);
    return 1;
}

static int __echan_recv_unbuffered_sigs(echan chan, uint sigs, eobj* _obj)
{
    is0_ret(sigs, 0);

    emutex_lock(chan->r_mu);
    emutex_lock(chan->m_mu);

    while (chan->status == _OPENDED && (!chan->w_waiting || sigs > chan->channal.sigs))
    {
        // Block until writer has set chan->data.
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;

        if(chan->channal.sigs >= sigs)
            break;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        emutex_ulck(chan->r_mu);
        errno = EPIPE;
        return 0;
    }

    chan->channal.sigs = 0;
    chan->w_waiting--;

    // Signal waiting writer.
    econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    emutex_ulck(chan->r_mu);
    return 1;
}

static int __echan_time_recv_buffered_sigs(echan chan, uint sigs, eobj* _obj, int timeout)
{
    int ret = 0;

    is0_ret(sigs, 0);

    emutex_lock(chan->m_mu);

    if (chan->channal.sigs == 0)
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is added.
        chan->r_waiting++;
        econd_twait(chan->r_cond, chan->m_mu, timeout);
        chan->r_waiting--;

        // todo: using the ret val of cond_twait to check status
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    if(chan->channal.sigs < sigs)
        goto skip_recv_ret;

    chan->channal.sigs -= sigs;
    ret = sigs;

skip_recv_ret:
    if (chan->w_waiting > 0)
    {
        // Signal waiting writer.
        econd_one(chan->w_cond);
    }

    emutex_ulck(chan->m_mu);
    return ret;
}

static int __echan_time_recv_unbuffered_sigs(echan chan, uint sigs, eobj* _obj, int timeout)
{
    int ret = 0;

    is0_ret(sigs, 0);

    emutex_lock(chan->r_mu);
    emutex_lock(chan->m_mu);

    if (chan->status == _OPENDED && (!chan->w_waiting || sigs > chan->channal.sigs))
    {
        // Block until writer has set chan->data.
        chan->r_waiting++;
        econd_twait(chan->r_cond, chan->m_mu, timeout);
        chan->r_waiting--;

        // todo: using the ret val of cond_twait to check status
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        emutex_ulck(chan->r_mu);
        errno = EPIPE;
        return 0;
    }

    if(chan->channal.sigs >= sigs)
    {
        chan->channal.sigs = 0;
        chan->w_waiting--;
        ret = sigs;
    }

    // Signal waiting writer.
    econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    emutex_ulck(chan->r_mu);
    return ret;
}

static int __echan_try_recv_sigs(echan chan, uint sigs, eobj* _obj)
{
    is0_ret(sigs, 0);

    emutex_lock(chan->m_mu);

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    if(chan->channal.sigs < sigs)
    {
        emutex_ulck(chan->m_mu);
        return 0;
    }

    chan->channal.sigs -= sigs;

    // Signal waiting writer.
    if (chan->w_waiting > 0)
        econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    return sigs;
}

static uint __echan_recv_all_sigs(echan chan)
{
    uint sigs;
    emutex_lock(chan->m_mu);

recv_again:
    while (chan->channal.sigs == 0)
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is added.
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    if(chan->channal.sigs == 0)
    {
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;

        goto recv_again;
    }

    sigs = chan->channal.sigs;
    chan->channal.sigs = 0;
    if (chan->w_waiting > 0)
    {
        // Signal waiting writer.
        econd_one(chan->w_cond);
    }

    emutex_ulck(chan->m_mu);
    return sigs;
}

static uint __echan_try_recv_all_sigs(echan chan)
{
    uint sigs;
    emutex_lock(chan->m_mu);

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    sigs = chan->channal.sigs;
    chan->channal.sigs = 0;

    // Signal waiting writer.
    if (chan->w_waiting > 0)
        econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    return sigs;
}

static int __echan_recv_buffered_list(echan chan, uint type, eobj* _obj)
{
    eobj obj;

    emutex_lock(chan->m_mu);

recv_again:

    while (ell_size(chan->channal.list) == 0)
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is added.
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    obj = ell_first(chan->channal.list);
    if(eobj_typeo(obj) != type)
    {
        if(type == EOBJ)
            goto recv_always;

        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;

        goto recv_again;
    }

recv_always:

    ell_takeH(chan->channal.list);
    *_obj = obj;

    if (chan->w_waiting > 0)
    {
        // Signal waiting writer.
        econd_one(chan->w_cond);
    }

    emutex_ulck(chan->m_mu);
    return 1;
}

static int __echan_recv_unbuffered_list(echan chan, uint type, eobj* _obj)
{
    emutex_lock(chan->r_mu);
    emutex_lock(chan->m_mu);

    while (chan->status == _OPENDED && !chan->w_waiting)
    {
        // Block until writer has set chan->data.
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;

        if(eobj_typeo(chan->channal.node.val.p) == type || EOBJ == type) // todo: eobj_type -> EOBJ_TYPE
            break;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        emutex_ulck(chan->r_mu);
        errno = EPIPE;
        return 0;
    }

    *_obj = (cptr)chan->channal.node.val.p;
    chan->w_waiting--;

    // Signal waiting writer.
    econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    emutex_ulck(chan->r_mu);
    return 1;
}

static int __echan_time_recv_buffered_list(echan chan, uint type, eobj* _obj, int timeout)
{
    eobj obj; int ret = 0;

    emutex_lock(chan->m_mu);

    if(ell_size(chan->channal.list) == 0)
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is added or timeout
        chan->r_waiting++;
        econd_twait(chan->r_cond, chan->m_mu, timeout);
        chan->r_waiting--;

        // todo: using the ret val of cond_twait to check status
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    if(ell_size(chan->channal.list) == 0)
    {
        //errno = EPIPE;
        goto skip_recv_ret;
    }

    obj = ell_first(chan->channal.list);
    if(eobj_typeo(obj) != type)
    {
        if(type == EOBJ)
            goto recv_always;

        //errno = EPIPE;
        goto skip_recv_ret;
    }

recv_always:

    ell_takeH(chan->channal.list);
    *_obj = obj;
    ret   = 1;

skip_recv_ret:
    if (chan->w_waiting > 0)
    {
        // Signal waiting writer.
        econd_one(chan->w_cond);
    }

    emutex_ulck(chan->m_mu);
    return ret;
}

static int __echan_time_recv_unbuffered_list(echan chan, uint type, eobj* _obj, int timeout)
{
    int ret = 0;

    emutex_lock(chan->r_mu);
    emutex_lock(chan->m_mu);

    if (chan->status == _OPENDED && !chan->w_waiting)
    {
        // Block until writer has set chan->data.
        chan->r_waiting++;
        econd_twait(chan->r_cond, chan->m_mu, timeout);
        chan->r_waiting--;

        // todo: using the ret val of cond_twait to check status
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        emutex_ulck(chan->r_mu);
        errno = EPIPE;
        return 0;
    }

    if(eobj_typeo(chan->channal.node.val.p) == type || EOBJ == type)
    {
        chan->w_waiting--;
        *_obj = (cptr)chan->channal.node.val.p;
        ret   = 1;
    }

    // Signal waiting writer.
    if (chan->w_waiting > 0)
        econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    emutex_ulck(chan->r_mu);
    return ret;
}

static ell __echan_recv_all_list(echan chan)
{
    ell out = 0, new_list;

    emutex_lock(chan->m_mu);

recv_again:

    // todo: if this is a unbuffered chan, this must raise coredump
    while (ell_size(chan->channal.list) == 0)
    {
        if (chan->status != _OPENDED)
        {
            emutex_ulck(chan->m_mu);
            errno = EPIPE;
            return 0;
        }

        // Block until something is added.
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;
    }

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    if(ell_size(chan->channal.list) == 0)
    {
        chan->r_waiting++;
        econd_wait(chan->r_cond, chan->m_mu);
        chan->r_waiting--;

        goto recv_again;
    }

    if((new_list = ell_new()) == 0)
    {
        emutex_ulck(chan->m_mu);
        errno = ENOMEM;
        return 0;
    }

    out = chan->channal.list;
    chan->channal.list = new_list;

    // Signal waiting writer.
    if (chan->w_waiting > 0)
        econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    return out;
}

static int __echan_try_recv_list(echan chan, uint type, eobj* _obj)
{
    eobj obj;

    emutex_lock(chan->m_mu);

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    obj = ell_first(chan->channal.list);
    if(0 == obj)
    {
        emutex_ulck(chan->m_mu);
        return 0;
    }

    if(eobj_typeo(obj) != type)
    {
        if(type == EOBJ)
            goto recv_always;

        emutex_ulck(chan->m_mu);
        return 0;
    }

recv_always:

    ell_takeH(chan->channal.list);
    *_obj = obj;

    if (chan->w_waiting > 0)
    {
        // Signal waiting writer.
        econd_one(chan->w_cond);
    }

    emutex_ulck(chan->m_mu);
    return 1;
}

static ell __echan_try_recv_all_list(echan chan)
{
    ell out = 0, new_list;

    emutex_lock(chan->m_mu);

    if (chan->status != _OPENDED)
    {
        emutex_ulck(chan->m_mu);
        errno = EPIPE;
        return 0;
    }

    if(ell_size(chan->channal.list) == 0)
    {
        emutex_ulck(chan->m_mu);
        return 0;
    }

    if((new_list = ell_new()) == 0)
    {
        emutex_ulck(chan->m_mu);
        errno = ENOMEM;
        return 0;
    }

    out = chan->channal.list;
    chan->channal.list = new_list;

    // Signal waiting writer.
    if (chan->w_waiting > 0)
        econd_one(chan->w_cond);

    emutex_ulck(chan->m_mu);
    return out;
}

static int __echan_sendBin_list(echan chan, conptr in , uint inlen) {                                   eobj obj = ell_newO(ERAW, inlen);              is0_ret(obj, 0); memcpy(obj, in, inlen);     return chan->_oprt->send(chan, (eval_t){.p=obj});}
static int __echan_sendI64_list(echan chan, i64    val            ) {                                   eobj obj = ell_newO(ENUM, sizeof(double) * 2); is0_ret(obj, 0); EOBJ_VALI(obj) = val;       return chan->_oprt->send(chan, (eval_t){.p=obj});}
static int __echan_sendF64_list(echan chan, f64    val            ) {                                   eobj obj = ell_newO(ENUM, sizeof(double) * 2); is0_ret(obj, 0); EOBJ_VALF(obj) = val;       return chan->_oprt->send(chan, (eval_t){.p=obj});}
static int __echan_sendStr_list(echan chan, constr str            ) { uint  len = str ? strlen(str) : 0;eobj obj = ell_newO(ESTR, len);                is0_ret(obj, 0); memcpy(obj, str, len);      return chan->_oprt->send(chan, (eval_t){.p=obj});}
static int __echan_sendPtr_list(echan chan, conptr ptr            ) {                                   eobj obj = ell_newO(EPTR, sizeof(conptr));     is0_ret(obj, 0); EOBJ_VALP(obj) = (cptr)ptr; return chan->_oprt->send(chan, (eval_t){.p=obj});}
static int __echan_sendObj_list(echan chan, eobj   obj            ) {                                                                                       is0_ret(obj, 0);                             return chan->_oprt->send(chan, (eval_t){.p=obj});}
static int __echan_sendSig_list(echan chan, uint   sig            ) { fprintf(stderr, "__echan_sendSig_list: wrong use of echan, you chan only using this in a SIGS chan\n"); abort(); }

static eobj __echan_recvBin_list(echan chan) { eobj obj; int ret; ret = chan->_oprt->recv(chan, ERAW, &obj); return ret ? obj : 0;}
static i64  __echan_recvI64_list(echan chan) { eobj obj; int ret; ret = chan->_oprt->recv(chan, ENUM, &obj); if(ret) { i64 val = EOBJ_VALI(obj); ell_freeO(0, obj); return val;} return 0;}
static f64  __echan_recvF64_list(echan chan) { eobj obj; int ret; ret = chan->_oprt->recv(chan, ENUM, &obj); if(ret) { f64 val = EOBJ_VALF(obj); ell_freeO(0, obj); return val;} return 0;}
static eobj __echan_recvStr_list(echan chan) { eobj obj; int ret; ret = chan->_oprt->recv(chan, ESTR, &obj); return ret ? obj : 0;}
static cptr __echan_recvPtr_list(echan chan) { eobj obj; int ret; ret = chan->_oprt->recv(chan, EPTR, &obj); if(ret) { cptr ptr= EOBJ_VALP(obj); ell_freeO(0, obj); return ptr;} return 0;}
static eobj __echan_recvObj_list(echan chan) { eobj obj; int ret; ret = chan->_oprt->recv(chan, EOBJ, &obj); return ret ? obj : 0;}
static int  __echan_recvSig_list(echan chan, uint sigs) { fprintf(stderr, "__echan_recvSig_list: wrong use of echan, you chan only using this in a SIGS chan\n"); abort(); }
static eval __echan_recvALL_list(echan chan           ) { ell l = __echan_recv_all_list(chan); return (eval){.p=l}; }

static eobj __echan_tryRecvObj_list(echan chan           ) { eobj obj; int ret; ret = chan->_oprt->tryRecv(chan, EOBJ, &obj); return ret ? obj : 0; }
static int  __echan_tryRecvSig_list(echan chan, uint sigs) { fprintf(stderr, "__echan_tryRecvSig_list: wrong use of echan, you chan only using this in a SIGS chan\n"); abort(); }
static eval __echan_tryRecvAll_list(echan chan           ) { ell l = __echan_try_recv_all_list(chan); return (eval){.p=l}; }

static eobj __echan_timeRecvObj_list(echan chan,            int timeout) { eobj obj = 0; chan->_oprt->timeRecv(chan, EOBJ, &obj, timeout); return obj; }
static int  __echan_timeRecvSig_list(echan chan, uint sigs, int timeout) { fprintf(stderr, "__echan_timeRecvSig_list: wrong use of echan, you chan only using this in a SIGS chan\n"); abort(); }

static int __echan_sendBin_sigs(echan chan, conptr in , uint inlen) { fprintf(stderr, "__echan_sendBin_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int __echan_sendI64_sigs(echan chan, i64    val            ) { fprintf(stderr, "__echan_sendI64_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int __echan_sendF64_sigs(echan chan, f64    val            ) { fprintf(stderr, "__echan_sendF64_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int __echan_sendStr_sigs(echan chan, constr str            ) { fprintf(stderr, "__echan_sendStr_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int __echan_sendPtr_sigs(echan chan, conptr ptr            ) { fprintf(stderr, "__echan_sendPtr_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int __echan_sendObj_sigs(echan chan, eobj   obj            ) { fprintf(stderr, "__echan_sendObj_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int __echan_sendSig_sigs(echan chan, uint   sig            ) { return chan->_oprt->send(chan, (eval_t){.i32=sig});}

static eobj __echan_recvBin_sigs(echan chan) { fprintf(stderr, "__echan_recvBin_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static i64  __echan_recvI64_sigs(echan chan) { fprintf(stderr, "__echan_recvI64_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static f64  __echan_recvF64_sigs(echan chan) { fprintf(stderr, "__echan_recvF64_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static eobj __echan_recvStr_sigs(echan chan) { fprintf(stderr, "__echan_recvStr_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static cptr __echan_recvPtr_sigs(echan chan) { fprintf(stderr, "__echan_recvPtr_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static eobj __echan_recvObj_sigs(echan chan) { fprintf(stderr, "__echan_recvObj_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int  __echan_recvSig_sigs(echan chan, uint sigs) { return chan->_oprt->recv(chan, sigs, 0); }
static eval __echan_recvAll_sigs(echan chan           ) { uint sigs = __echan_recv_all_sigs(chan); return (eval){.i32=sigs}; }

static eobj __echan_tryRecvObj_sigs(echan chan           ) { fprintf(stderr, "__echan_tryRecvObj_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int  __echan_tryRecvSig_sigs(echan chan, uint sigs) { return chan->_oprt->tryRecv(chan, sigs, 0); }
static eval __echan_tryRecvAll_sigs(echan chan           ) { uint sigs = __echan_try_recv_all_sigs(chan); return (eval){.u=sigs}; }

static eobj __echan_timeRecvObj_sigs(echan chan,            int timeout) { fprintf(stderr, "__echan_timeRecvObj_sigs: wrong use of echan, you chan only using this in a LIST chan\n"); abort(); }
static int  __echan_timeRecvSig_sigs(echan chan, uint sigs, int timeout) { return chan->_oprt->timeRecv(chan, sigs, 0, timeout); }

static _oprt_t _list_oprt_buffered = {
    __echan_send_buffered_list,
    __echan_sendBin_list,
    __echan_sendI64_list,
    __echan_sendF64_list,
    __echan_sendStr_list,
    __echan_sendPtr_list,
    __echan_sendObj_list,
    __echan_sendSig_list,

    __echan_recv_buffered_list,
    __echan_recvBin_list,
    __echan_recvI64_list,
    __echan_recvF64_list,
    __echan_recvStr_list,
    __echan_recvPtr_list,
    __echan_recvObj_list,
    __echan_recvSig_list,
    __echan_recvALL_list,

    __echan_try_recv_list,
    __echan_tryRecvObj_list,
    __echan_tryRecvSig_list,
    __echan_tryRecvAll_list,

    __echan_time_recv_buffered_list,
    __echan_timeRecvObj_list,
    __echan_timeRecvSig_list,
};

static _oprt_t _list_oprt_unbuffered = {
    __echan_send_unbuffered_list,
    __echan_sendBin_list,
    __echan_sendI64_list,
    __echan_sendF64_list,
    __echan_sendStr_list,
    __echan_sendPtr_list,
    __echan_sendObj_list,
    __echan_sendSig_list,

    __echan_recv_unbuffered_list,
    __echan_recvBin_list,
    __echan_recvI64_list,
    __echan_recvF64_list,
    __echan_recvStr_list,
    __echan_recvPtr_list,
    __echan_recvObj_list,
    __echan_recvSig_list,
    __echan_recvALL_list,

    __echan_try_recv_list,
    __echan_tryRecvObj_list,
    __echan_tryRecvSig_list,
    __echan_tryRecvAll_list,

    __echan_time_recv_unbuffered_list,
    __echan_timeRecvObj_list,
    __echan_timeRecvSig_list,
};

static _oprt_t _sigs_oprt_buffered = {
    __echan_send_buffered_sigs,
    __echan_sendBin_sigs,
    __echan_sendI64_sigs,
    __echan_sendF64_sigs,
    __echan_sendStr_sigs,
    __echan_sendPtr_sigs,
    __echan_sendObj_sigs,
    __echan_sendSig_sigs,

    __echan_recv_buffered_sigs,
    __echan_recvBin_sigs,
    __echan_recvI64_sigs,
    __echan_recvF64_sigs,
    __echan_recvStr_sigs,
    __echan_recvPtr_sigs,
    __echan_recvObj_sigs,
    __echan_recvSig_sigs,
    __echan_recvAll_sigs,

    __echan_try_recv_sigs,
    __echan_tryRecvObj_sigs,
    __echan_tryRecvSig_sigs,
    __echan_tryRecvAll_sigs,

    __echan_time_recv_buffered_sigs,
    __echan_timeRecvObj_sigs,
    __echan_timeRecvSig_sigs,
};

static _oprt_t _sigs_oprt_unbuffered = {
    __echan_send_unbuffered_sigs,
    __echan_sendBin_sigs,
    __echan_sendI64_sigs,
    __echan_sendF64_sigs,
    __echan_sendStr_sigs,
    __echan_sendPtr_sigs,
    __echan_sendObj_sigs,
    __echan_sendSig_sigs,

    __echan_recv_unbuffered_sigs,
    __echan_recvBin_sigs,
    __echan_recvI64_sigs,
    __echan_recvF64_sigs,
    __echan_recvStr_sigs,
    __echan_recvPtr_sigs,
    __echan_recvObj_sigs,
    __echan_recvSig_sigs,
    __echan_recvAll_sigs,

    __echan_try_recv_sigs,
    __echan_tryRecvObj_sigs,
    __echan_tryRecvSig_sigs,
    __echan_tryRecvAll_sigs,

    __echan_time_recv_unbuffered_sigs,
    __echan_timeRecvObj_sigs,
    __echan_timeRecvSig_sigs,
};

static int __echan_get_oprt(echan chan, int opts, uint capacity)
{
    if((opts & 0x01) == 0) chan->_oprt = capacity ? &_list_oprt_buffered : &_list_oprt_unbuffered;
    else                   chan->_oprt = capacity ? &_sigs_oprt_buffered : &_sigs_oprt_unbuffered;

    return 1;
}

echan echan_new(int opts, uint capacity)
{
    echan chan;

    chan = (echan) malloc(sizeof(echan_t));
    is0_exeret(chan, errno = ENOMEM, 0);

    if (capacity > 0)
    {
        if(!__chan_init_buffered(chan, opts, capacity))
        {
            free(chan);
            return NULL;
        }
    }
    else
    {
        if(!__chan_init_unbuffered(chan))
        {
            free(chan);
            return NULL;
        }
    }

    __echan_get_oprt(chan, opts, capacity);

    return chan;
}

int  echan_sendB(echan chan, conptr in , uint inlen) { return chan ? chan->_oprt->sendBin(chan, in, inlen) : 0; }
int  echan_sendI(echan chan, i64    val            ) { return chan ? chan->_oprt->sendI64(chan, val      ) : 0; }
int  echan_sendF(echan chan, f64    val            ) { return chan ? chan->_oprt->sendF64(chan, val      ) : 0; }
int  echan_sendS(echan chan, constr str            ) { return chan ? chan->_oprt->sendStr(chan, str      ) : 0; }
int  echan_sendP(echan chan, conptr ptr            ) { return chan ? chan->_oprt->sendPtr(chan, ptr      ) : 0; }
int  echan_sendO(echan chan, eobj   obj            ) { return chan ? chan->_oprt->sendObj(chan, obj      ) : 0; }

int  echan_sendSig(echan chan, uint sigs           ) { return chan ? chan->_oprt->sendSig(chan, sigs     ) : 0; }

eobj echan_recvB(echan chan) { return chan ? chan->_oprt->recvBin(chan) : 0; }
i64  echan_recvI(echan chan) { return chan ? chan->_oprt->recvI64(chan) : 0; }
f64  echan_recvF(echan chan) { return chan ? chan->_oprt->recvF64(chan) : 0; }
eobj echan_recvS(echan chan) { return chan ? chan->_oprt->recvStr(chan) : 0; }
cptr echan_recvP(echan chan) { return chan ? chan->_oprt->recvPtr(chan) : 0; }
eobj echan_recvO(echan chan) { return chan ? chan->_oprt->recvObj(chan) : 0; }
int  echan_recvSig(echan chan, uint sigs) { return chan ? chan->_oprt->recvSig(chan, sigs) : 0; }
eval echan_recvALL(echan chan) { return chan ? chan->_oprt->recvAll(chan) : (eval){.u=0}; }

eobj echan_tryRecvObj(echan chan           ) { return chan ? chan->_oprt->tryRecvObj(chan      ) : 0; }
int  echan_tryRecvSig(echan chan, uint sigs) { return chan ? chan->_oprt->tryRecvSig(chan, sigs) : 0; }
eval echan_tryRecvAll(echan chan           ) { return chan ? chan->_oprt->tryRecvAll(chan      ) : (eval){.u=0};}

eobj echan_timeRecvObj(echan chan,            int timeout){ return chan ? chan->_oprt->timeRecvObj(chan,       timeout) : 0;}
int  echan_timeRecvSig(echan chan, uint sigs, int timeout){ return chan ? chan->_oprt->timeRecvSig(chan, sigs, timeout) : 0;}

int  echan_type (echan chan)
{
    is0_ret(chan, -1);

    return chan->_oprt->tryRecv == __echan_try_recv_list ? ECHAN_LIST : ECHAN_SIGS;
}

uint echan_wwait(echan chan)
{
    int count;

    is0_ret(chan, 0);

    emutex_lock(chan->m_mu);
    count = chan->w_waiting;
    emutex_ulck(chan->m_mu);

    return count;
}

uint echan_rwait(echan chan)
{
    int count;

    is0_ret(chan, 0);

    emutex_lock(chan->m_mu);
    count = chan->r_waiting;
    emutex_ulck(chan->m_mu);

    return count;
}

// Returns the number of items in the channel buffer. If the channel is
// unbuffered, this will return 0.
uint echan_size(echan chan)
{
    int size = 0;
    if (chan->_oprt->send == __echan_send_buffered_list)
    {
        emutex_lock(chan->m_mu);
        size = ell_size(chan->channal.list);
        emutex_ulck(chan->m_mu);
    }
    return size;
}

uint echan_sigs (echan chan)
{
    int sigs = 0;
    if (chan->_oprt->send == __echan_send_buffered_sigs)
    {
        emutex_lock(chan->m_mu);
        sigs = chan->channal.sigs;
        emutex_ulck(chan->m_mu);
    }
    return sigs;
}

uint echan_cap  (echan chan)
{
    return chan ? chan->cap : 0;
}

void echan_freeO(eobj obj )
{
    if(obj) ell_freeO(0, obj);
}

static int __echan_can_recv(echan chan)
{
    if (chan->_oprt->send == __echan_send_buffered_list || chan->_oprt->send == __echan_send_buffered_sigs)
    {
        return echan_size(chan) > 0;
    }

    emutex_lock(chan->m_mu);
    int sender = chan->w_waiting > 0;
    emutex_ulck(chan->m_mu);
    return sender;
}

static int __echan_can_send(echan chan)
{
    int send;
    if (chan->_oprt->send == __echan_send_buffered_list || chan->_oprt->send == __echan_send_buffered_sigs)
    {
        // Can send if buffered channel is not full.
        emutex_lock(chan->m_mu);
        send = ell_size(chan->channal.list) < chan->cap;
        emutex_ulck(chan->m_mu);
    }
    else
    {
        // Can send if unbuffered channel has receiver.
        emutex_lock(chan->m_mu);
        send = chan->r_waiting > 0;
        emutex_ulck(chan->m_mu);
    }

    return send;
}

typedef struct
{
    int     recv;
    echan   chan;
    cptr    msg_in;
    int     index;
} select_op_t;

#if 0
// A select statement chooses which of a set of possible send or receive
// operations will proceed. The return value indicates which channel's
// operation has proceeded. If more than one operation can proceed, one is
// selected randomly. If none can proceed, -1 is returned. Select is intended
// to be used in conjunction with a switch statement. In the case of a receive
// operation, the received value will be pointed to by the provided pointer. In
// the case of a send, the value at the same index as the channel will be sent.
int echan_select(echan recv_chans[], int recv_count, cptr* recv_out,
    echan send_chans[], int send_count, cptr send_msgs[])
{
    // TODO: Add support for blocking selects.

    select_op_t* candidates = alloca(sizeof(select_op_t) * (recv_count + send_count));
    int count = 0;
    int i;

    // Determine receive candidates.
    for (i = 0; i < recv_count; i++)
    {
        echan chan = recv_chans[i];
        if (__echan_can_recv(chan))
        {
            select_op_t op;
            op.recv = 1;
            op.chan = chan;
            op.index = i;
            candidates[count++] = op;
        }
    }

    // Determine send candidates.
    for (i = 0; i < send_count; i++)
    {
        echan chan = send_chans[i];
        if (__echan_can_send(chan))
        {
            select_op_t op;
            op.recv = 0;
            op.chan = chan;
            op.msg_in = send_msgs[i];
            op.index = i + recv_count;
            candidates[count++] = op;
        }
    }

    if (count == 0)
    {
        return -1;
    }

    // Seed rand using current time in nanoseconds.
    static uint seed; uint val = (uint)time(0); seed += val;
    srand(seed);

    // Select candidate and perform operation.
    select_op_t select = candidates[rand() % count];
    if (select.recv)
    {
        cptr recv = echan_recvO(select.chan);
        if(recv_out) *recv_out = recv;
        is0_ret(recv, -1);
    }
    else if (!select.recv && !echan_send(select.chan, select.msg_in))
    {
        return -1;
    }

    return select.index;
}
#endif
