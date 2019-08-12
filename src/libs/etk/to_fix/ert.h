/// =====================================================================================
///
///       Filename:  ert.h
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

#ifndef __ERT_H__
#define __ERT_H__

#define ERT_VERSION     "ert 2.0.3"         // return the num of running tasks when call ert_destroy()

#include "etype.h"

typedef struct thread_pool_s* ert;

typedef void (*ert_cb) (cptr arg);

#define DEFAULT_ERT ((ert)0x1)

#ifdef __cplusplus
extern "C" {
#endif

/// ---------------------- creator -------------------------
///
///     create a new routine threadpooll
///
/// @note:
///     1. default in 4 thread, you can use ert_maxThread()
///        to set the max thread num
///     2. the thread will be created when adding a task
///

ert  ert_new(int max_thread_num);

/// ---------------------- quiting -------------------------
///
///     destroy and releasing routine thread_pool resources,
/// you can using ert_join to wait the threadpool to quit.
///
///     if you using ERT_WAITING_TASKS in ert_release(),
/// ert_join will waiting until all the added tasks run over.
///
/// @note:
///     1. ert_release() is non-bloking
///     2. the inner threads those who is running task will
///        not quit immediately, we ensure the task(including
///        after_oprt add by TP_AddTask()) to running over
///        if you not quit the whole process
///

#define ERT_WAITING_RUNNING_TASKS 0x01

void ert_join(ert tp);
int  ert_destroy(ert tp, int opt);

/// ---------------------- setting -------------------------
///
///     some set APIs of routine thread_pool
///

void ert_maxThread(ert tp, int num);

/// ---------------------- tasks -------------------------
///
///     to running or query a task in routine threadpool
///
/// note: the len of tag should be limited in 31 character
///

int  ert_run  (ert tp, constr tag, ert_cb oprt, ert_cb after_oprt, cptr arg);
int  ert_query(ert tp, constr tag);

#ifdef __cplusplus
}
#endif

#endif
