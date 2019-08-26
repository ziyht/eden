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

#include <time.h>
#include <pthread.h>

#include "etime.h"
#include "etype.h"
#include "ecompat.h"

#if !defined(_WIN32) && (defined(_AIX) || defined(__OpenBSD__) || !defined(PTHREAD_BARRIER_SERIAL_THREAD))
typedef struct pthread_barrier_s pthread_barrier_t;
int pthread_barrier_init(pthread_barrier_t* barrier, unsigned int count);
int pthread_barrier_wait(pthread_barrier_t* barrier);
void pthread_barrier_destroy(pthread_barrier_t* barrier);
#endif

typedef pthread_t               ethread_t;
typedef pthread_once_t          eonce_t;
typedef pthread_key_t           ethkey_t;
typedef pthread_mutex_t         emutex_t;
//typedef pthread_spinlock_t      espin_t;
typedef pthread_cond_t          econd_t;
typedef pthread_rwlock_t        erwlock_t;
typedef pthread_barrier_t       ebarrier_t;

#define eonce(o, cb)            pthread_once(&(o), cb)

#define ethread_init(t, cb, d)  pthread_create(&(t), NULL, (cb), (d))
#define ethread_join(t)         pthread_join((t), NULL)
#define ethread_detach(t)       pthread_detach((t))
#define ethread_quit(t)         pthread_cancel(t)
#define ethread_self()          pthread_self()
#define ethread_kill(t, s)      pthread_kill(t, s)
#define ethread_equal(t1, t2)   pthread_equal(t1, t2)

#define ethkey_init(k)          pthread_key_create(&(k))
#define ethkey_init_ex(k, rls)  pthread_key_create(&(k), rls)
#define ethkey_get(k)           pthread_getspecific(k)
#define ethkey_set(k, ptr)      pthread_setspecific(k, ptr)
#define ethkey_free(k)          pthread_key_delete(k)

#define emutex_init(m)          pthread_mutex_init(&(m), NULL)
#define emutex_init_ex(m, attr) pthread_mutex_init(&(m), attr)
#define emutex_lock(m)          pthread_mutex_lock(&(m))
#define emutex_trylock(m)       pthread_mutex_trylock(&(m))
#define emutex_ulck(m)          pthread_mutex_unlock(&(m))
#define emutex_free(m)          pthread_mutex_destroy(&(m))

#define espin_init(l)           pthread_spin_init(&(l), 0)
#define espin_lock(l)           pthread_spin_lock(&(l))
#define espin_trylock(l)        pthread_spin_trylock(&(l))
#define espin_free(l)           pthread_spin_destroy(&(l))

#define econd_init(c)           pthread_cond_init(&(c), NULL)
#define econd_wait(c, m)        pthread_cond_wait(&(c), &(m))
#define econd_twait(c, m, t)    __pthread_cond_timedwait(&(c), &(m), (t));
#define econd_one(c)            pthread_cond_signal(&(c))
#define econd_all(c)            pthread_cond_broadcast(&(c))
#define econd_free(c)           pthread_cond_destroy(&(c))

#define erwlock_init(l)         pthread_rwlock_init(&(l))
#define erwlock_rlock(l)        pthread_rwlock_rdlock(&(l))
#define erwlock_wlock(l)        pthread_rwlock_wrlock(&(l))
#define erwlock_tryrlock(l)     pthread_rwlock_tryrdlock(&(l))
#define erwlock_trywlock(l)     pthread_rwlock_trywrlock(&(l))
#define erwlock_ulck(l)         pthread_rwlock_unlock(&(l))
#define erwlock_free(l)         pthread_rwlock_destroy(&(l))

#define ebarrier_init(b, c)     pthread_barrier_init(&(b), NULL, c)
#define ebarrier_wait(b)        pthread_barrier_wait(&(b))
#define ebarrier_free(b)        pthread_barrier_destroy(&(b))

static __always_inline int __pthread_cond_timedwait(econd_t* co, emutex_t* mu, int timeout)
{
    struct timespec ts;

#if defined(__APPLE__) && defined(__MACH__)
    ts.tv_sec  = timeout / ((uint64_t) 1e9);
    ts.tv_nsec = timeout % ((uint64_t) 1e9);
    return pthread_cond_timedwait_relative_np(co, mu, &ts);
#else
    i64 t =  timeout + e_nowms();
    ts.tv_sec  = t / (1000);
    ts.tv_nsec = t % (1000);
    ts.tv_nsec *= 1000000;
#if defined(__ANDROID__) && defined(HAVE_PTHREAD_COND_TIMEDWAIT_MONOTONIC)
    /*
     * The bionic pthread implementation doesn't support CLOCK_MONOTONIC,
     * but has this alternative function instead.
     */
    return pthread_cond_timedwait_monotonic_np(co, mu, &ts);
#else
    return pthread_cond_timedwait(co, mu, &ts);
#endif /* __ANDROID__ */
#endif
}

#endif  // __ETHREAD_H__
