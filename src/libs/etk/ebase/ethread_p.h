#ifndef __ETHREAD_P_H__
#define __ETHREAD_P_H__

#include <time.h>
#include "ecompat.h"

#include "etime.h"
#include "etype.h"
#include "ecompat.h"

#define __USING_PTHREAD 1

#if __USING_PTHREAD

#define _DARWIN_C_SOURCE

#include <pthread.h>

#if !defined(_WIN32) && (defined(_AIX) || defined(__OpenBSD__) || !defined(PTHREAD_BARRIER_SERIAL_THREAD))
typedef struct pthread_barrier_s pthread_barrier_t;
int  pthread_barrier_init(pthread_barrier_t* barrier, unsigned int count);
int  pthread_barrier_wait(pthread_barrier_t* barrier);
void pthread_barrier_destroy(pthread_barrier_t* barrier);
#endif

typedef pthread_t               __ethread_t;
typedef pthread_once_t          __ethread_once_t;
typedef pthread_key_t           __ethread_key_t;
typedef pthread_mutex_t         __emutex_t;
typedef pthread_cond_t          __econd_t;
typedef pthread_rwlock_t        __erwlock_t;
typedef pthread_barrier_t       __ebarrier_t;

#define __ethread_once(o, cb)     pthread_once((o), cb)

#ifdef _WIN32

static int __pthread_inited;
static void __pthread_init_np()
{
    if(!__pthread_inited)
    {
        __pthread_inited = 1;

        if(!pthread_win32_process_attach_np())
        {
            perror("pthread_win32_process_attach_np failed.");
            abort();
        }
        if(!pthread_win32_thread_attach_np())
        {
            perror("pthread_win32_thread_attach_np failed.");
            abort();
        }
    }
}

static __always_inline int
__pthread_key_create (pthread_key_t *key, void (* dest)(void *))
{
    __pthread_init_np();
    return pthread_key_create(key, dest);
}

static __always_inline int
__pthread_create (pthread_t *th, const pthread_attr_t *attr, void *(* func)(void *), void *arg)
{
    __pthread_init_np();
    return pthread_create(th, attr, func, arg);
}

static __always_inline int
__pthread_mutex_init (pthread_mutex_t *m, const pthread_mutexattr_t *a)
{
    __pthread_init_np();
    return pthread_mutex_init(m, a);
}

static __always_inline int
__pthread_cond_init (pthread_cond_t *c, const pthread_condattr_t *a)
{
    __pthread_init_np();
    return pthread_cond_init(c, a);
}

static __always_inline int
__pthread_rwlock_init(pthread_rwlock_t *lock, const pthread_rwlockattr_t *attr)
{
    __pthread_init_np();
    return pthread_rwlock_init(lock, attr);
}

static __always_inline int
__pthread_barrier_init (pthread_barrier_t * barrier,
                                  const pthread_barrierattr_t * attr,
                                  unsigned int count)
{
    __pthread_init_np();
    return  pthread_barrier_init(barrier, attr, count);
}

#define pthread_key_create   __pthread_key_create
#define pthread_create       __pthread_create
#define pthread_mutex_init   __pthread_mutex_init
#define pthread_cond_init    __pthread_cond_init
#define pthread_rwlock_init  __pthread_rwlock_init
#define pthread_barrier_init __pthread_barrier_init
#endif

#define __ethread_key_init(k)           pthread_key_create((k), 0)
#define __ethread_key_init_ex(k, rls)   pthread_key_create((k), rls)
#define __ethread_key_get(k)            pthread_getspecific(k)
#define __ethread_key_set(k, ptr)       pthread_setspecific(k, ptr)
#define __ethread_key_free(k)           pthread_key_delete(k)

#define __ethread_init(t, cb, d)  pthread_create((t), NULL, (cb), (d))
#define __ethread_join(t)         pthread_join((t), NULL)
#define __ethread_join_ex(t, p)   pthread_join((t), (p))
#define __ethread_detach(t)       pthread_detach((t))
#define __ethread_quit(t)         pthread_cancel(t)
#define __ethread_self()          pthread_self()
#define __ethread_kill(t, s)      pthread_kill(t, s)
#define __ethread_equal(t1, t2)   pthread_equal(t1, t2)

#define __emutex_init(m)          pthread_mutex_init((m), NULL)
#define __emutex_init_ex(m, attr) pthread_mutex_init((m), attr)
#define __emutex_lock(m)          pthread_mutex_lock((m))
#define __emutex_trylock(m)       pthread_mutex_trylock((m))
#define __emutex_ulck(m)          pthread_mutex_unlock((m))
#define __emutex_free(m)          pthread_mutex_destroy((m))

#define __econd_init(c)           pthread_cond_init((c), NULL)
#define __econd_wait(c, m)        pthread_cond_wait((c), (m))
#define __econd_twait(c, m, t)    __pthread_cond_timedwait((c), (m), (t));
#define __econd_one(c)            pthread_cond_signal((c))
#define __econd_all(c)            pthread_cond_broadcast((c))
#define __econd_free(c)           pthread_cond_destroy((c))

#define __erwlock_init(l)         pthread_rwlock_init((l))
#define __erwlock_rlock(l)        pthread_rwlock_rdlock((l))
#define __erwlock_wlock(l)        pthread_rwlock_wrlock((l))
#define __erwlock_tryrlock(l)     pthread_rwlock_tryrdlock((l))
#define __erwlock_trywlock(l)     pthread_rwlock_trywrlock((l))
#define __erwlock_ulck(l)         pthread_rwlock_unlock((l))
#define __erwlock_free(l)         pthread_rwlock_destroy((l))

#define __ebarrier_init(b, c)     pthread_barrier_init((b), NULL, c)
#define __ebarrier_wait(b)        pthread_barrier_wait((b))
#define __ebarrier_free(b)        pthread_barrier_destroy((b))


static __always_inline int __pthread_cond_timedwait(__econd_t* co, __emutex_t* mu, int timeout)
{
    struct timespec ts;

    if(timeout == -1)
        return __econd_wait(co, mu);

#if defined(__APPLE__) && defined(__MACH__)
    ts.tv_sec  = timeout / (1000);
    ts.tv_nsec = timeout % (1000) * 1000000;
    return pthread_cond_timedwait_relative_np(co, mu, &ts);
#else
    i64 t =  timeout + e_nowms();
    ts.tv_sec  = t / (1000);
    ts.tv_nsec = t % (1000) * 1000000;
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
#else

typedef HANDLE                   __ethread_t;
typedef HANDLE                   __esem_t;
typedef CRITICAL_SECTION         __emutex_t;
typedef union   __econd_s        __econd_t;
typedef union   __erwlock_s      __erwlock_t;
typedef struct  __ebarrier_s     __ebarrier_t;
typedef struct  __ethread_once_s __ethread_once_t;

#define E_ONCE_INIT { 0, NULL }

struct __ebarrier_s{
  unsigned int n;
  unsigned int count;
  __emutex_t mutex;
  __esem_t turnstile1;
  __esem_t turnstile2;
};

typedef struct {
  DWORD tls_index;
} __ethread_key_t;

union __econd_s{
    CONDITION_VARIABLE cond_var;
    struct {
        unsigned int waiters_count;
        CRITICAL_SECTION waiters_count_lock;
        HANDLE signal_event;
        HANDLE broadcast_event;
    } unused_; /* TODO: retained for ABI compatibility; remove me in v2.x. */
};

union __erwlock_s{
    struct {
        unsigned int num_readers_;
        CRITICAL_SECTION num_readers_lock_;
        HANDLE write_semaphore_;
    } state_;

    /* TODO: remove me in v2.x. */
    struct {
        SRWLOCK unused_;
    } unused1_;

    /* TODO: remove me in v2.x. */
    struct {
        __emutex_t unused1_;
        __emutex_t unused2_;
    } unused2_;
};

typedef enum {
  ETHREAD_NO_FLAGS          = 0x00,
  ETHREAD_HAS_STACK_SIZE    = 0x01
} __ethread_create_flags;

typedef struct __ethread_options_s {
  unsigned int flags;
  size_t stack_size;
  /* More fields may be added at any time. */
}__ethread_options_t;

void  __ethread_once(__ethread_once_t* guard, void (*callback)(void));

int   __ethread_key_create(__ethread_key_t* key);
void  __ethread_key_delete(__ethread_key_t* key);
void* __ethread_key_get(__ethread_key_t* key);
void  __ethread_key_set(__ethread_key_t* key, void* value);

int   __ethread_create(__ethread_t* tid, void* (*entry)(void* arg), void* arg);
int   __ethread_create_ex(__ethread_t* tid,
                               const __ethread_options_t* params,
                               void* (*entry)(void* arg),
                               void* arg);
int   __ethread_join (__ethread_t *tid, void** ret);
int   __ethread_equal(const __ethread_t* t1, const __ethread_t* t2);
__ethread_t __ethread_self(void);

int   __emutex_init   (__emutex_t* mutex);
void  __emutex_destroy(__emutex_t* mutex);
void  __emutex_lock   (__emutex_t* mutex);
int   __emutex_trylock(__emutex_t* mutex);
void  __emutex_unlock (__emutex_t* mutex);

int   __erwlock_init     (__erwlock_t* rwlock);
void  __erwlock_destroy  (__erwlock_t* rwlock);
void  __erwlock_rdlock   (__erwlock_t* rwlock);
int   __erwlock_tryrdlock(__erwlock_t* rwlock);
void  __erwlock_rdunlock (__erwlock_t* rwlock);
void  __erwlock_wrlock   (__erwlock_t* rwlock);
int   __erwlock_trywrlock(__erwlock_t* rwlock);
void  __erwlock_wrunlock (__erwlock_t* rwlock);

int   __esem_init   (__esem_t* sem, unsigned int value);
void  __esem_destroy(__esem_t* sem);
void  __esem_post   (__esem_t* sem);
void  __esem_wait   (__esem_t* sem);
int   __esem_trywait(__esem_t* sem);

int   __econd_init     (__econd_t* cond);
void  __econd_destroy  (__econd_t* cond);
void  __econd_one      (__econd_t* cond);
void  __econd_all      (__econd_t* cond);
void  __econd_wait     (__econd_t* cond, __emutex_t* mutex);
int   __econd_timedwait(__econd_t* cond, __emutex_t* mutex, uint64_t timeout);

int   __ebarrier_init   (__ebarrier_t* barrier, unsigned int count);
void  __ebarrier_destroy(__ebarrier_t* barrier);
int   __ebarrier_wait   (__ebarrier_t* barrier);

#endif

#endif
