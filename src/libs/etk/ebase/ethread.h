/// =====================================================================================
///
///       Filename:  ethread.h
///
///    Description:  a header file to compat thread for different platform, especially
///                  between linux and windows
///
///                  the pthread lib in libs are rebuild from mingwin
///
///        Version:  1.1
///        Created:  09/14/2017 14:47:25 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __ETHREAD_H__
#define __ETHREAD_H__

#define ETHREAD_VERSION "ethread 1.1.0"      // using pthread in all platform

#include "ethread_p.h"

typedef __ethread_t        ethread_t;
typedef __ethread_once_t   ethread_once_t;
typedef __ethread_key_t    ethread_key_t;
typedef __emutex_t         emutex_t;
typedef __econd_t          econd_t;
typedef __erwlock_t        erwlock_t;
typedef __ebarrier_t       ebarrier_t;

#define ethread_once(o, cb)     __ethread_once(&(o), cb)

#define ethread_init(t, cb, d)  __ethread_init(&(t), (cb), (d))
#define ethread_join(t)         __ethread_join((t))
#define ethread_join_ex(t, p)   __ethread_join_ex((t), &(p))
#define ethread_detach(t)       __ethread_detach((t))
#define ethread_quit(t)         __ethread_cancel(t)
#define ethread_self()          __ethread_self()
#define ethread_kill(t, s)      __ethread_kill(t, s)
#define ethread_equal(t1, t2)   __ethread_equal(t1, t2)

#define ethkey_init(k)          __ethread_key_create(&(k), 0)
#define ethkey_init_ex(k, rls)  __ethread_key_create(&(k), rls)
#define ethkey_get(k)           __ethread_key_get(k)
#define ethkey_set(k, ptr)      __ethread_key_set(k, ptr)
#define ethkey_free(k)          __ethread_key_free(k)

#define emutex_init(m)          __emutex_init(&(m))
#define emutex_init_ex(m, attr) __emutex_init(&(m))  // not support
#define emutex_lock(m)          __emutex_lock(&(m))
#define emutex_trylock(m)       __emutex_trylock(&(m))
#define emutex_ulck(m)          __emutex_ulck(&(m))
#define emutex_free(m)          __emutex_free(&(m))

#define econd_init(c)           __econd_init(&(c))
#define econd_wait(c, m)        __econd_wait(&(c), &(m))
#define econd_twait(c, m, t)    __econd_twait(&(c), &(m), (t));
#define econd_one(c)            __econd_one(&(c))
#define econd_all(c)            __econd_all(&(c))
#define econd_free(c)           __econd_free(&(c))

#define erwlock_init(l)         __erwlock_init(&(l))
#define erwlock_rlock(l)        __erwlock_rlock(&(l))
#define erwlock_wlock(l)        __erwlock_wlock(&(l))
#define erwlock_tryrlock(l)     __erwlock_tryrlock(&(l))
#define erwlock_trywlock(l)     __erwlock_trywlock(&(l))
#define erwlock_ulck(l)         __erwlock_ulck(&(l))
#define erwlock_free(l)         __erwlock_free(&(l))

#define ebarrier_init(b, c)     __ebarrier_init(&(b), NULL, c)
#define ebarrier_wait(b)        __ebarrier_wait(&(b))
#define ebarrier_free(b)        __ebarrier_destroy(&(b))

#endif  // __ETHREAD_H__
