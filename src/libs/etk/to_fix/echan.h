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
#include "eobj.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETHREAD_NOT_USING_PTHREAD_MINGEWIN_IN_ECHAN

typedef struct echan_s* echan;

/// ---------------------- creator -------------------------
///
///     to encode a echan handler;
///
/// note:
///     1. if the capacity is 0, this is a unbuffered channel,
/// you must recving the chan first then you can send a msg
/// successfully. else you'll create a buffered channel, in
/// this case, you must send a msg first then you can recv a
/// msg successfully.
///

#define ECHAN_LIST 0x00
#define ECHAN_SIGS 0x01

echan echan_new(int opts, uint capacity);   // create a echan
void  echan_free(echan chan);               // close a echan and release all the resources

int   echan_close(echan chan);              // close a echan
int   echan_closed(echan chan);             // echan is closed or not

/// ---------------------- sender -------------------------
///
///     to send a value into the echan
///
/// note:
///     1. If the channel is unbuffered, this will block
/// until a receiver receives the value.
///     2. If the channel is buffered and at capacity, this
/// will block until a receiver receives a value.
///
/// returns:
///     0 if failed, and errno will be set.
///     1 if succeed.
///

int  echan_sendB(echan chan, conptr in , uint inlen);
int  echan_sendI(echan chan, i64    val);
int  echan_sendF(echan chan, f64    val);
int  echan_sendS(echan chan, constr str);
int  echan_sendP(echan chan, conptr ptr);
int  echan_sendO(echan chan, eobj   obj);

int  echan_sendSig(echan chan, uint sigs);

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

eobj echan_recvB(echan chan);   // recving a bin    data, using echan_freeO() to release after using it
i64  echan_recvI(echan chan);   // recving a int    data
f64  echan_recvF(echan chan);   // recving a double data
eobj echan_recvS(echan chan);   // recving a str    data, using echan_freeO() to release after using it
cptr echan_recvP(echan chan);   // recving a ptr    data
eobj echan_recvO(echan chan);   // recving a obj    data, using echan_freeO() to release after using it
int  echan_recvSig(echan chan, uint sigs);
eval echan_recvALL(echan chan); // recving all objs or sigs, using .p to get the elist of all objs; using .u to get the recved all sigs

eobj echan_tryRecvObj(echan chan);
int  echan_tryRecvSig(echan chan, uint sigs);
eval echan_tryRecvAll(echan chan);

eobj echan_timeRecvObj(echan chan,            int timeout);
int  echan_timeRecvSig(echan chan, uint sigs, int timeout);
eval echan_timeRecvAll(echan chan,            int timeout);  // todo

int echan_select(echan recv_chans[], int recv_count, cptr* recv_out,
    echan send_chans[], int send_count, cptr send_msgs[]);

/// ---------------------- utils -------------------------
///
///     some utils tools
///
int  echan_type (echan chan);
uint echan_wwait(echan chan);
uint echan_rwait(echan chan);
uint echan_size (echan chan);
uint echan_sigs (echan chan);
uint echan_cap  (echan chan);
void echan_freeO(eobj  obj );

#ifdef __cplusplus
}
#endif

#endif
