/// =====================================================================================
///
///       Filename:  echan.h
///
///    Description:  A Pure C implementation of Go channals.
///
///        Version:  1.2
///        Created:  06/17/2017 11:31:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __ECHAN_H__
#define __ECHAN_H__

#define ECHAN_VERSION "echan 1.2.0"      // add time recv APIs, note: can using but not perfect

#include "etype.h"
#include "evar.h"
#include "estr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETHREAD_NOT_USING_PTHREAD_MINGEWIN_IN_ECHAN

typedef struct echan_s* echan;

/// ---------------------- creator -------------------------
///
///     echan creator and basic apis
///
/// note:
///     1. if the capacity is 0, this is a unbuffered channel,
/// you must recving the chan first then you can send a msg
/// successfully. else you'll create a buffered channel, in
/// this case, you must send a msg first then you can recv a
/// msg successfully.
///

#define E_ALL   E_UNKOWN

echan echan_new (etypev type, uint cap);     // create a echan
void  echan_free(echan  chan);               // close a echan and release all the resources

int   echan_close (echan chan);              // close a echan, note: only close it and not release it
int   echan_closed(echan chan);              // echan is closed or not

int   echan_open  (echan chan);             // Open a echan, so it can transfer data again
int   echan_opened(echan chan);             // Returns true

int   echan_type (echan chan);
uint  echan_wwait(echan chan);              // Returns the cnt of current write client
uint  echan_rwait(echan chan);              // Returns the cnt of current read  client
uint  echan_len  (echan chan);              // Returns the cnt of elements in channal
uint  echan_sigs (echan chan);              // Returns the cnt of signals  in channal
uint  echan_cap  (echan chan);              // Returns the cap of elements in channal

/// ---------------------- sender -------------------------
///
///     to send a value into the echan
///
/// note:
///     1. If the channel is unbuffered, this will block
/// until a receiver receives the value.
///     2. If the channel is buffered and at full, this
/// will block until a receiver receives a value.
///
/// returns:
///     0 if failed, and errno will be set.
///     1 if succeed.
///

#define EVAR_SIG(_cnt)            (evar){.type = E_SIG, .cnt = _cnt }

bool  echan_sendI  (echan chan, i64    val);
bool  echan_sendF  (echan chan, f64    val);
bool  echan_sendS  (echan chan, constr str);
bool  echan_sendP  (echan chan, conptr ptr);
bool  echan_sendV  (echan chan, evar   var);
bool  echan_sendSig(echan chan, uint  sigs);

bool  echan_trySendI  (echan chan, i64    val);
bool  echan_trySendF  (echan chan, f64    val);
bool  echan_trySendS  (echan chan, constr str);
bool  echan_trySendP  (echan chan, conptr ptr);
bool  echan_trySendV  (echan chan, evar   var);
bool  echan_trySendSig(echan chan, uint sigs);

bool  echan_timeSendI  (echan chan, i64    val, int timeout);
bool  echan_timeSendF  (echan chan, f64    val, int timeout);
bool  echan_timeSendS  (echan chan, constr str, int timeout);
bool  echan_timeSendP  (echan chan, conptr ptr, int timeout);
bool  echan_timeSendV  (echan chan, evar   var, int timeout);
bool  echan_timeSendSig(echan chan, uint  sigs, int timeout);

/// ---------------------- recver -------------------------
///
///     to recv a value from the echan
///
/// note:
///     1. This will block until there is right type data to
/// receive
///     2. echan_recvO() recv all type values
///     3. the msg recved by those api needs to be freed by
/// echan_freeO():
///         echan_recvB()
///         echan_recvS()
///         echan_recvO()
///
/// returns:
///     [0] if failed, and errno will be set.
///     [real value] or [ptr of eobj]
///

#define EVAR_ALL                  (evar){.type = E_ALL }

i64  echan_recvI  (echan chan);     // recving a i64 data
f64  echan_recvF  (echan chan);     // recving a f64 data
estr echan_recvS  (echan chan);     // recving a str data
cptr echan_recvP  (echan chan);     // recving a ptr data
evar echan_recvV  (echan chan);     // recving a     data
uint echan_recvSig(echan chan);     // recving a     sig , returns 0 if failed, else returns 1

i64  echan_tryRecvI  (echan chan);
f64  echan_tryRecvF  (echan chan);
estr echan_tryRecvS  (echan chan);
cptr echan_tryRecvP  (echan chan);
evar echan_tryRecvV  (echan chan);
uint echan_tryRecvSig(echan chan);

i64  echan_timeRecvI  (echan chan, int timeout);
f64  echan_timeRecvF  (echan chan, int timeout);
estr echan_timeRecvS  (echan chan, int timeout);
cptr echan_timeRecvP  (echan chan, int timeout);
evar echan_timeRecvV  (echan chan, int timeout);
uint echan_timeRecvSig(echan chan, int timeout);

evar echan_recvIs  (echan chan, uint cnt);   // recving a series of i64 data
evar echan_recvFs  (echan chan, uint cnt);   // recving a series of f64 data
evar echan_recvSs  (echan chan, uint cnt);   // recving a series of str data
evar echan_recvPs  (echan chan, uint cnt);   // recving a series of ptr data
evar echan_recvVs  (echan chan, uint cnt);   // recving a series of     data
uint echan_recvSigs(echan chan, uint sigs);  // recving   needed sigs

evar echan_tryRecvIs  (echan chan, uint cnt);
evar echan_tryRecvFs  (echan chan, uint cnt);
evar echan_tryRecvSs  (echan chan, uint cnt);
evar echan_tryRecvPs  (echan chan, uint cnt);
evar echan_tryRecvVs  (echan chan, uint cnt);
uint echan_tryRecvSigs(echan chan, uint cnt);

evar echan_timeRecvIs  (echan chan, uint cnt, int timeout);
evar echan_timeRecvFs  (echan chan, uint cnt, int timeout);
evar echan_timeRecvSs  (echan chan, uint cnt, int timeout);
evar echan_timeRecvPs  (echan chan, uint cnt, int timeout);
evar echan_timeRecvVs  (echan chan, uint cnt, int timeout);
uint echan_timeRecvSigs(echan chan, uint cnt, int timeout);

evar echan_recvAll    (echan chan);                 // try recv all the msgs in chan, if not have, blocking
evar echan_tryRecvAll (echan chan);
evar echan_timeRecvAll(echan chan, int timeout);

//! todo
int echan_select(echan recv_chans[], int recv_count, cptr* recv_out,
    echan send_chans[], int send_count, cptr send_msgs[]);


#ifdef __cplusplus
}
#endif

#endif
