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

#define ECHAN_VERSION "echan 2.0.0"      // rebuild echan(95%), now can using it

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "ecompat.h"
#include "etype.h"
#include "evec.h"
#include "eutils.h"
#include "ethread.h"
#include "eerrno.h"

#include "echan.h"

#pragma pack(1)
typedef struct echan_s
{
    // Shared properties
    emutex_t    m_mu;
    emutex_t    r_mu;
    emutex_t    w_mu;
    econd_t     r_cond;
    econd_t     w_cond;
    int         r_waiting;
    int         w_waiting;
    int         status;

    // channal data
    int         type;
    uint        cap;
    union {
        evec        chan;
        u32         sigs;
    }           d;

}echan_t;
#pragma pack()

#define _c_sigs(c)  (c)->data.v.u64
#define _c_queue(c) (c)->data.v.p
#define _c_var(c)   (c)->data

enum {
    // -- echan status
    _OPENDED = 0,
    _CLOSING ,
    _CLOSED  ,
};

static __always_inline void __chan_uninit_locker(echan chan)
{
    emutex_free(chan->m_mu);
    emutex_free(chan->w_mu);
    emutex_free(chan->r_mu);
    econd_free(chan->r_cond);
    econd_free(chan->w_cond);
}

static __always_inline int __chan_init_locker(echan chan)
{
    memset(chan, 0, sizeof(*chan));

    is1_exe(emutex_init(chan->m_mu) , goto err);
    is1_exe(emutex_init(chan->w_mu) , goto err);
    is1_exe(emutex_init(chan->r_mu) , goto err);

    is1_exe(econd_init(chan->r_cond), goto err);
    is1_exe(econd_init(chan->w_cond), goto err);

    return 1;

err:
    __chan_uninit_locker(chan);

    return 0;
}

static __always_inline int __chan_init_channel(echan c, etypev type, uint cap)
{
    c->type = type;
    c->cap  = cap;

    if(cap == 0)
        cap = 1;

    if(type == E_SIG)
    {
        // do nothing, using chan->d.sigs
    }
    else if(type < E_USER)
    {
        c->d.chan = evec_new2(type, 0, cap);
    }
    else
    {
        c->d.chan = evec_new2(E_USER, sizeof(evar), cap);
    }

    return 1;
}

echan echan_new (etypev type, uint cap)
{
    echan c;

    c = (echan) malloc(sizeof(echan_t));
    is0_exeret(c, errno = E_ENOMEM, 0);

    is0_exeret(__chan_init_locker (c)           , free(c), 0);
    is0_exeret(__chan_init_channel(c, type, cap), free(c), 0);

    return c;
}

void echan_free(echan chan)
{
    is0_ret(chan, );

    echan_close(chan);

    if(chan->type != E_SIG)
    {
        evec_free(chan->d.chan);
    }

    econd_free (chan->r_cond);
    econd_free (chan->w_cond);

    emutex_free(chan->w_mu);
    emutex_free(chan->r_mu);

    emutex_free(chan->m_mu);

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
            sleep(0);
            emutex_lock(chan->m_mu);
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

int  echan_type (echan chan)
{
    return chan ? chan->type : E_NAV;
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
    emutex_lock(chan->m_mu);

    size = chan->cap ? chan->type == E_SIG ? chan->d.sigs
                                           : evec_len(chan->d.chan)
                     : 0;

    emutex_ulck(chan->m_mu);

    return size;
}

uint echan_sigs (echan chan)
{
    int sigs = 0;

    emutex_lock(chan->m_mu);

    sigs = chan->cap ? chan->type == E_SIG ? chan->d.sigs
                                           : 0
                     : 0;

    emutex_ulck(chan->m_mu);

    return sigs;
}

uint echan_cap  (echan chan)
{
    return chan ? chan->cap : 0;
}

#define _c_checkopened_m(c)             \
do{                                     \
    if (c->status != _OPENDED)          \
    {                                   \
        emutex_ulck(c->m_mu);           \
        errno = EPIPE;                  \
        return 0;                       \
    }                                   \
}while(0)

#define _c_checkopened_mw(c)            \
do{                                     \
    if (c->status != _OPENDED)          \
    {                                   \
        emutex_ulck(c->m_mu);           \
        emutex_ulck(c->w_mu);           \
        errno = EPIPE;                  \
        return 0;                       \
    }                                   \
}while(0)

#define _c_checkopened_mr(c)            \
do{                                     \
    if (c->status != _OPENDED)          \
    {                                   \
        emutex_ulck(c->m_mu);           \
        emutex_ulck(c->r_mu);           \
        errno = EPIPE;                  \
        return 0;                       \
    }                                   \
}while(0)


static __always_inline bool __echan_send_dispatch(echan c, int timeout, evar var);

bool echan_sendI  (echan chan, i64    val) { return __echan_send_dispatch(chan, -1, EVAR_I64(val)); }
bool echan_sendF  (echan chan, f64    val) { return __echan_send_dispatch(chan, -1, EVAR_F64(val)); }
bool echan_sendS  (echan chan, constr str) { return __echan_send_dispatch(chan, -1, EVAR_CS (str)); }
bool echan_sendP  (echan chan, conptr ptr) { return __echan_send_dispatch(chan, -1, EVAR_CP (ptr)); }
bool echan_sendV  (echan chan, evar   var) { return __echan_send_dispatch(chan, -1,          var ); }
bool echan_sendSig(echan chan, uint  sigs) { return __echan_send_dispatch(chan, -1, EVAR_SIG(sigs)); }

bool echan_trySendI  (echan chan, i64    val) { return __echan_send_dispatch(chan, 0, EVAR_I64(val)); }
bool echan_trySendF  (echan chan, f64    val) { return __echan_send_dispatch(chan, 0, EVAR_F64(val)); }
bool echan_trySendS  (echan chan, constr str) { return __echan_send_dispatch(chan, 0, EVAR_CS (str)); }
bool echan_trySendP  (echan chan, conptr ptr) { return __echan_send_dispatch(chan, 0, EVAR_CP (ptr)); }
bool echan_trySendV  (echan chan, evar   var) { return __echan_send_dispatch(chan, 0,          var ); }
bool echan_trySendSig(echan chan, uint  sigs) { return __echan_send_dispatch(chan, 0, EVAR_SIG(sigs)); }

bool echan_timeSendI  (echan chan, i64    val, int timeout) { return __echan_send_dispatch(chan, timeout, EVAR_I64(val)); }
bool echan_timeSendF  (echan chan, f64    val, int timeout) { return __echan_send_dispatch(chan, timeout, EVAR_F64(val)); }
bool echan_timeSendS  (echan chan, constr str, int timeout) { return __echan_send_dispatch(chan, timeout, EVAR_CS (str)); }
bool echan_timeSendP  (echan chan, conptr ptr, int timeout) { return __echan_send_dispatch(chan, timeout, EVAR_CP (ptr)); }
bool echan_timeSendV  (echan chan, evar   var, int timeout) { return __echan_send_dispatch(chan, timeout,          var ); }
bool echan_timeSendSig(echan chan, uint  sigs, int timeout) { return __echan_send_dispatch(chan, timeout, EVAR_SIG(sigs)); }

static bool __echan_time_send_sigs_buffered  (echan c, int timeout, evarp vp);
static bool __echan_time_send_sigs_unbuffered(echan c, int timeout, evarp vp);
static bool __echan_time_send_chan_buffered  (echan c, int timeout, evarp vp);
static bool __echan_time_send_chan_unbuffered(echan c, int timeout, evarp vp);
static __always_inline bool __echan_send_dispatch(echan c, int timeout, evar var)
{
    is0_ret(c, 0);

    if(c->type != E_ALL && c->type != var.type)
        return 0;

    if(c->cap)
    {
        if(c->type == E_SIG)  return __echan_time_send_sigs_buffered(c, timeout, &var);
        else                  return __echan_time_send_chan_buffered(c, timeout, &var);
    }
    else
    {
        if(c->type == E_SIG)  return __echan_time_send_sigs_unbuffered(c, timeout, &var);
        else                  return __echan_time_send_chan_unbuffered(c, timeout, &var);
    }

    return 0;
}

static bool __echan_time_send_sigs_buffered(echan c, int timeout, evarp vp)
{
    uint avail, need; int ret = 1;

    is1_ret(!vp->cnt, 0);

    emutex_lock(c->m_mu);

    _c_checkopened_m(c);

    need = vp->cnt;

    while( 1 )
    {
        if( (avail = c->cap - c->d.sigs) )
        {
            if(avail >= need)
            {
                c->d.sigs += need;
                need       = 0;
            }
            else
            {
                c->d.sigs += avail;
                need      -= avail;
            }

            if(c->r_waiting)
            {
                // Signal waiting reader.
                econd_one(c->r_cond);
            }

            if(!need)
            {
                ret = 0;
                break;
            }
        }

        else if(ret == 0 && c->r_waiting)
        {
            // Signal waiting reader.
            econd_one(c->r_cond);
        }

        // Block until something is removed.
        c->w_waiting++;
        ret = econd_twait(c->w_cond, c->m_mu, timeout);
        c->w_waiting--;

        _c_checkopened_m(c);

        if(ret == ETIMEDOUT)
        {
            if(need < vp->cnt)
            {
                uint recved = vp->cnt - need;

                if(c->d.sigs <= recved) c->d.sigs = 0;
                else                    c->d.sigs -= recved;
            }

            errno = ETIMEDOUT;
            break;
        }
    }

    emutex_ulck(c->m_mu);

    return !ret;
}

static bool __echan_time_send_sigs_unbuffered(echan c, int timeout, evarp vp)
{
    int ret = 1;

    is0_ret(vp->cnt, 0);

    emutex_lock(c->w_mu);
    emutex_lock(c->m_mu);

    _c_checkopened_mw(c);

    while(1)
    {
        if(vp->cnt)
        {
            if(!c->d.sigs)
            {
                c->d.sigs = vp->cnt;
                vp->cnt   = 0;

                ret = 0;
            }
        }
        else
        {
            ret = 0;
            break;
        }

        if (ret == 0 && c->r_waiting > 0)
        {
            // Signal waiting reader.
            econd_one(c->r_cond);
        }

        // Block until reader consumed chan->data.
        c->w_waiting++;
        ret = econd_twait(c->w_cond, c->m_mu, timeout);
        c->w_waiting--;

        _c_checkopened_mw(c);

        if(ret == ETIMEDOUT)
        {
            errno = ETIMEDOUT;
            break;
        }
    }

    emutex_ulck(c->m_mu);
    emutex_ulck(c->w_mu);

    return !ret;
}

static bool __echan_time_send_chan_buffered(echan c, int timeout, evarp vp)
{
    uint avail; int ret = 1;

    emutex_lock(c->m_mu);

    _c_checkopened_m(c);

    while( 1 )
    {
        if( (avail = c->cap - evec_len(c->d.chan)) )
        {
            ret = !evec_appdV(c->d.chan, *vp);

            if (c->r_waiting > 0)
            {
                // Signal waiting reader.
                econd_one(c->r_cond);
            }

            break;
        }

        if(ret == 0 && c->r_waiting > 0)
        {
            // Signal waiting reader.
            econd_one(c->r_cond);
        }

        // Block until something is removed.
        c->w_waiting++;
        ret = econd_twait(c->w_cond, c->m_mu, timeout);
        c->w_waiting--;

        _c_checkopened_m(c);

        if(ret == ETIMEDOUT)
        {
            errno = ETIMEDOUT;
            break;
        }
    }

    emutex_ulck(c->m_mu);

    return !ret;
}

static bool __echan_time_send_chan_unbuffered(echan c, int timeout, evarp vp)
{
    int ret = 1;

    emutex_lock(c->w_mu);
    emutex_lock(c->m_mu);

    _c_checkopened_mw(c);

    while(1)
    {
        if(vp->cnt)
        {
            if(!evec_len(c->d.chan))
            {
                evec_appdV(c->d.chan, *vp);
                vp->cnt = 0;

                ret = 0;
            }
        }
        else
        {
            ret = 0;
            break;
        }

        if (ret == 0 && c->r_waiting > 0)
        {
            // Signal waiting reader.
            econd_one(c->r_cond);

            break;
        }

        // Block until reader consumed chan->data.
        c->w_waiting++;
        ret = econd_twait(c->w_cond, c->m_mu, timeout);
        c->w_waiting--;

        _c_checkopened_mw(c);

        if(ret == ETIMEDOUT)
        {
            errno = ETIMEDOUT;
            break;
        }
    }

    emutex_ulck(c->m_mu);
    emutex_ulck(c->w_mu);

    return !ret;
}


static __always_inline int __echan_recv_dispatch(echan c, int timeout, evarp varp);

#define EVAR_I64_C(c)  __EVAR_MK(E_I64, 0, c, EVAL_0)
#define EVAR_F64_C(c)  __EVAR_MK(E_F64, 0, c, EVAL_0)
#define EVAR_STR_C(c)  __EVAR_MK(E_STR, 0, c, EVAL_0)
#define EVAR_PTR_C(c)  __EVAR_MK(E_PTR, 0, c, EVAL_0)
#define EVAR_SIG_C(c)  __EVAR_MK(E_SIG, 0, c, EVAL_0)
#define EVAR_ALL_C(c)  __EVAR_MK(E_ALL, 0, c, EVAL_0)

i64  echan_recvI  (echan chan) { evar var = EVAR_I64_C(1); return __echan_recv_dispatch(chan, -1, &var) ? var.v.i64 : 0; }
f64  echan_recvF  (echan chan) { evar var = EVAR_F64_C(1); return __echan_recv_dispatch(chan, -1, &var) ? var.v.f64 : 0; }
estr echan_recvS  (echan chan) { evar var = EVAR_STR_C(1); return __echan_recv_dispatch(chan, -1, &var) ? var.v.s   : 0; }
cptr echan_recvP  (echan chan) { evar var = EVAR_PTR_C(1); return __echan_recv_dispatch(chan, -1, &var) ? var.v.p   : 0; }
evar echan_recvV  (echan chan) { evar var = EVAR_ALL_C(1); return __echan_recv_dispatch(chan, -1, &var) ? var       : EVAR_NAV; }
uint echan_recvSig(echan chan) { evar var = EVAR_SIG_C(1); return __echan_recv_dispatch(chan, -1, &var) ? 1         : 0; }

i64  echan_tryRecvI  (echan chan) { evar var = EVAR_I64_C(1); return __echan_recv_dispatch(chan, 0, &var) ? var.v.i64 : 0; }
f64  echan_tryRecvF  (echan chan) { evar var = EVAR_F64_C(1); return __echan_recv_dispatch(chan, 0, &var) ? var.v.f64 : 0; }
estr echan_tryRecvS  (echan chan) { evar var = EVAR_STR_C(1); return __echan_recv_dispatch(chan, 0, &var) ? var.v.s   : 0; }
cptr echan_tryRecvP  (echan chan) { evar var = EVAR_PTR_C(1); return __echan_recv_dispatch(chan, 0, &var) ? var.v.p   : 0; }
evar echan_tryRecvV  (echan chan) { evar var = EVAR_ALL_C(1); return __echan_recv_dispatch(chan, 0, &var) ? var       : EVAR_NAV; }
uint echan_tryRecvSig(echan chan) { evar var = EVAR_SIG_C(1); return __echan_recv_dispatch(chan, 0, &var) ? 1         : 0; }

i64  echan_timeRecvI  (echan chan, int timeout) { evar var = EVAR_I64_C(1); return __echan_recv_dispatch(chan, timeout, &var) ? var.v.i64 : 0; }
f64  echan_timeRecvF  (echan chan, int timeout) { evar var = EVAR_F64_C(1); return __echan_recv_dispatch(chan, timeout, &var) ? var.v.f64 : 0; }
estr echan_timeRecvS  (echan chan, int timeout) { evar var = EVAR_STR_C(1); return __echan_recv_dispatch(chan, timeout, &var) ? var.v.s   : 0; }
cptr echan_timeRecvP  (echan chan, int timeout) { evar var = EVAR_PTR_C(1); return __echan_recv_dispatch(chan, timeout, &var) ? var.v.p   : 0; }
evar echan_timeRecvV  (echan chan, int timeout) { evar var = EVAR_ALL_C(1); return __echan_recv_dispatch(chan, timeout, &var) ? var       : EVAR_NAV; }
uint echan_timeRecvSig(echan chan, int timeout) { evar var = EVAR_SIG_C(1); return __echan_recv_dispatch(chan, timeout, &var) ? 1         : 0; }


evar echan_recvIs  (echan chan, uint cnt) { evar var = EVAR_I64_C(cnt); return __echan_recv_dispatch(chan, -1, &var) ? var : EVAR_NAV; }
evar echan_recvFs  (echan chan, uint cnt) { evar var = EVAR_F64_C(cnt); return __echan_recv_dispatch(chan, -1, &var) ? var : EVAR_NAV; }
evar echan_recvSs  (echan chan, uint cnt) { evar var = EVAR_STR_C(cnt); return __echan_recv_dispatch(chan, -1, &var) ? var : EVAR_NAV; }
evar echan_recvPs  (echan chan, uint cnt) { evar var = EVAR_PTR_C(cnt); return __echan_recv_dispatch(chan, -1, &var) ? var : EVAR_NAV; }
evar echan_recvVs  (echan chan, uint cnt) { evar var = EVAR_ALL_C(cnt); return __echan_recv_dispatch(chan, -1, &var) ? var : EVAR_NAV; }
uint echan_recvSigs(echan chan, uint cnt) { evar var = EVAR_SIG_C(cnt); return __echan_recv_dispatch(chan, -1, &var) ? cnt : 0; }

evar echan_tryRecvIs  (echan chan, uint cnt) { evar var = EVAR_I64_C(cnt); return __echan_recv_dispatch(chan, 0, &var) ? var : EVAR_NAV; }
evar echan_tryRecvFs  (echan chan, uint cnt) { evar var = EVAR_F64_C(cnt); return __echan_recv_dispatch(chan, 0, &var) ? var : EVAR_NAV; }
evar echan_tryRecvSs  (echan chan, uint cnt) { evar var = EVAR_STR_C(cnt); return __echan_recv_dispatch(chan, 0, &var) ? var : EVAR_NAV; }
evar echan_tryRecvPs  (echan chan, uint cnt) { evar var = EVAR_PTR_C(cnt); return __echan_recv_dispatch(chan, 0, &var) ? var : EVAR_NAV; }
evar echan_tryRecvVs  (echan chan, uint cnt) { evar var = EVAR_ALL_C(cnt); return __echan_recv_dispatch(chan, 0, &var) ? var : EVAR_NAV; }
uint echan_tryRecvSigs(echan chan, uint cnt) { evar var = EVAR_SIG_C(cnt); return __echan_recv_dispatch(chan, 0, &var) ? cnt : 0; }

evar echan_timeRecvIs  (echan chan, uint cnt, int timeout) { evar var = EVAR_I64_C(cnt); return __echan_recv_dispatch(chan, timeout, &var) ? var : EVAR_NAV; }
evar echan_timeRecvFs  (echan chan, uint cnt, int timeout) { evar var = EVAR_F64_C(cnt); return __echan_recv_dispatch(chan, timeout, &var) ? var : EVAR_NAV; }
evar echan_timeRecvSs  (echan chan, uint cnt, int timeout) { evar var = EVAR_STR_C(cnt); return __echan_recv_dispatch(chan, timeout, &var) ? var : EVAR_NAV; }
evar echan_timeRecvPs  (echan chan, uint cnt, int timeout) { evar var = EVAR_PTR_C(cnt); return __echan_recv_dispatch(chan, timeout, &var) ? var : EVAR_NAV; }
evar echan_timeRecvVs  (echan chan, uint cnt, int timeout) { evar var = EVAR_ALL_C(cnt); return __echan_recv_dispatch(chan, timeout, &var) ? var : EVAR_NAV; }
uint echan_timeRecvSigs(echan chan, uint cnt, int timeout) { evar var = EVAR_SIG_C(cnt); return __echan_recv_dispatch(chan, timeout, &var) ? cnt : 0; }

#define _RECV_ALL 0x10

evar echan_recvAll    (echan chan)              { evar var = EVAR_ALL_C(1); var.__ |= _RECV_ALL; if(__echan_recv_dispatch(chan,      -1, &var)) { return var; } return EVAR_NAV;}
evar echan_tryRecvAll (echan chan)              { evar var = EVAR_ALL_C(1); var.__ |= _RECV_ALL; if(__echan_recv_dispatch(chan,       0, &var)) { return var; } return EVAR_NAV;}
evar echan_timeRecvAll(echan chan, int timeout) { evar var = EVAR_ALL_C(1); var.__ |= _RECV_ALL; if(__echan_recv_dispatch(chan, timeout, &var)) { return var; } return EVAR_NAV;}

static int __echan_time_recv_sigs_buffered  (echan c, int timeout, evarp varp);
static int __echan_time_recv_sigs_unbuffered(echan c, int timeout, evarp varp);
static int __echan_time_recv_chan_buffered  (echan c, int timeout, evarp varp);
static int __echan_time_recv_chan_unbuffered(echan c, int timeout, evarp varp);
static __always_inline int __echan_recv_dispatch(echan c, int timeout, evarp varp)
{
    is0_ret(c, 0);

    if(varp->type != E_ALL && c->type != varp->type)
        return 0;

    if(c->cap)
    {
        if     (c->type == E_SIG)  return __echan_time_recv_sigs_buffered(c, timeout, varp);
        else                       return __echan_time_recv_chan_buffered(c, timeout, varp);
    }
    else
    {
        if     (c->type == E_SIG)  return __echan_time_recv_sigs_unbuffered(c, timeout, varp);
        else                       return __echan_time_recv_chan_unbuffered(c, timeout, varp);
    }

    return 0;
}


static int __echan_time_recv_sigs_buffered(echan c, int timeout, evarp varp)
{
    int ret = 1;

    is1_exeret(!varp->cnt, *varp = EVAR_NAV, 0);

    emutex_lock(c->m_mu);

    _c_checkopened_m(c);

    uint need = 0;

    while(1)
    {
        if(c->d.sigs)
        {
            if(need == 0)       // init need
            {
                if(varp->__ & _RECV_ALL) need = varp->cnt = c->d.sigs;
                else                     need = varp->cnt;

                if(need == 0)
                    need = 1;
            }

            if(c->d.sigs >= need)
            {
                c->d.sigs -= need;
                need      = 0;
            }
            else
            {
                need     -= c->d.sigs;
                c->d.sigs = 0;
            }

            if(c->w_waiting > 0)
            {
                // Signal waiting writer.
                econd_one(c->w_cond);
            }

            if(need == 0)
            {
                *varp = EVAR_SIG(varp->cnt);

                ret = 0;
                break;
            }
        }
        else if(ret == 0 && c->w_waiting > 0)
        {
            // Signal waiting writer.
            econd_one(c->w_cond);
        }

        // Block until something is added.
        c->r_waiting++;
        ret = econd_twait(c->r_cond, c->m_mu, timeout);
        c->r_waiting--;

        _c_checkopened_m(c);

        if(ret == ETIMEDOUT)
        {
            errno = ETIMEDOUT;
            break;
        }
    }

    emutex_ulck(c->m_mu);

    return !ret;
}

static int __echan_time_recv_sigs_unbuffered(echan c, int timeout, evarp varp)
{
    int ret = 1;

    is0_ret(varp->cnt, 0);

    emutex_lock(c->r_mu);
    emutex_lock(c->m_mu);

    _c_checkopened_mr(c);

    while( 1 )
    {
        if(c->w_waiting && c->d.sigs && (varp->__ & _RECV_ALL || varp->cnt == c->d.sigs))
        {
            *varp = EVAR_SIG(c->d.sigs);
            c->d.sigs = 0;

            if(varp->cnt)
            {
                // Signal waiting writer.
                econd_one(c->w_cond);
            }

            ret = 0;

            break;
        }

        if(ret == 0 && c->w_waiting)
        {
            // Signal waiting writer.
            econd_one(c->w_cond);
        }

        // Block until writer has set chan->data.
        c->r_waiting++;
        ret = econd_twait(c->r_cond, c->m_mu, timeout);
        c->r_waiting--;

        _c_checkopened_mr(c);

        if(ret == ETIMEDOUT)
        {
            errno = ETIMEDOUT;

            break;
        }
    }

    emutex_ulck(c->m_mu);
    emutex_ulck(c->r_mu);

    return !ret;
}

static int __echan_time_recv_chan_buffered(echan c, int timeout, evarp varp)
{
    uint need = 0; int ret = 1;

    is1_ret(varp->cnt > c->cap, 0);

    emutex_lock(c->m_mu);

    _c_checkopened_m(c);

    while(1)
    {
        if(need == 0 && evec_len(c->d.chan))
        {
            if(varp->__ & _RECV_ALL)
            {
                varp->cnt = evec_len(c->d.chan);

                if(varp->cnt == 0)
                    varp->cnt = 1;
            }

            need = varp->cnt;
        }

        if(evec_len(c->d.chan) >= varp->cnt)
        {
            *varp = evec_takeHs(c->d.chan, varp->cnt);

            if(c->w_waiting > 0)
            {
                // Signal waiting writer.
                econd_one(c->w_cond);
            }

            ret = 0;

            break;
        }

        if(ret == 0 && c->w_waiting)
        {
            // Signal waiting writer.
            econd_one(c->w_cond);
        }

        // Block until something is added.
        c->r_waiting++;
        ret = econd_twait(c->r_cond, c->m_mu, timeout);
        c->r_waiting--;

        _c_checkopened_m(c);

        if(ret == ETIMEDOUT)
        {
            errno = ETIMEDOUT;
            break;
        }
    }

    emutex_ulck(c->m_mu);

    return !ret;
}

static int __echan_time_recv_chan_unbuffered(echan c, int timeout, evarp varp)
{
    int ret = 1;

    emutex_lock(c->r_mu);
    emutex_lock(c->m_mu);

    _c_checkopened_mr(c);

    while (1)
    {
        if(evec_len(c->d.chan))
        {
            *varp = evec_takeH(c->d.chan);

            if(c->w_waiting)
            {
                // Signal waiting writer.
                econd_one(c->w_cond);
            }

            ret = 0;

            break;
        }

        if(ret == 0 && c->w_waiting)
        {
            // Signal waiting writer.
            econd_one(c->w_cond);
        }

        // Block until writer has set chan->data.
        c->r_waiting++;
        ret = econd_twait(c->r_cond, c->m_mu, timeout);
        c->r_waiting--;

        _c_checkopened_mr(c);

        if(ret == ETIMEDOUT)
        {
            errno = ETIMEDOUT;
            break;
        }
    }

    emutex_ulck(c->m_mu);
    emutex_ulck(c->r_mu);

    return !ret;
}

//bool   echan_sendSig(echan chan, uint sigs           ) { return chan ? chan->_oprt->sendSig(chan, sigs     ) : 0; }


//int  echan_recvSig(echan chan, uint sigs) { return chan ? chan->_oprt->recvSig(chan, sigs) : 0; }
//eval echan_recvALL(echan chan) { return chan ? chan->_oprt->recvAll(chan) : (eval){.u=0}; }

//eobj echan_tryRecvObj(echan chan           ) { return chan ? chan->_oprt->tryRecvObj(chan      ) : 0; }
//int  echan_tryRecvSig(echan chan, uint sigs) { return chan ? chan->_oprt->tryRecvSig(chan, sigs) : 0; }
//eval echan_tryRecvAll(echan chan           ) { return chan ? chan->_oprt->tryRecvAll(chan      ) : (eval){.u=0};}

//eobj echan_timeRecvObj(echan chan,            int timeout){ return chan ? chan->_oprt->timeRecvObj(chan,       timeout) : 0;}
//int  echan_timeRecvSig(echan chan, uint sigs, int timeout){ return chan ? chan->_oprt->timeRecvSig(chan, sigs, timeout) : 0;}


#if 0

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
