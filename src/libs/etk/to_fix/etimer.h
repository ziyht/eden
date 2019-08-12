/// =====================================================================================
///
///       Filename:  etimer.h
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

#ifndef __ETIMER_H__
#define __ETIMER_H__

#include "etype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct etloop_s* etloop;
typedef struct etimer_s
{
    void* data;
}etimer_t, * etimer;

typedef void (*etm_cb)(etimer t);

/// ------------------- etimer loop ---------------------
///
///     to get a new loop or default loop
///

etloop etloop_new(int maxthread);
etloop etloop_df (int maxthread);

void   etloop_stop(etloop loop);

/// ------------------- etimer --------------------------
///
///     etimer creator ...
///
etimer etimer_new(etloop loop);
void   etimer_destroy(etimer e);

int    etimer_start(etimer e, etm_cb cb, u64 timeout, u64 repeat);
int    etimer_stop (etimer e);

int    etimer_runing(etimer _e);


/// ------------------- etimer sync tool ----------------
///
///     this is a tool for external using, to get a sync
/// information between local machine to the target server,
/// using ntp protocol.
///

typedef struct etimer_sync_stat_s{
    int    status;          // 1: ok | 0: faild

    i64    offusec;         // offset of usec, 1 sec = 1000000 usec
    i64    nowusec;         // now of usec

    constr err;             // if status is 0, we show err info here, but do not free it
}esync_t, esync;

esync etimer_esyncGet(constr server, int timeout);   // server can have port(lg:"1.1.1.1:123"), default port is 123(ntp); the unit of timeout is ms

#ifdef __cplusplus
}
#endif

#endif
