/// =====================================================================================
///
///       Filename:  ethread.c
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

#include <assert.h>

#include "ethread.h"
#include "eerr.h"
#include "ealloc.h"

/* Note: guard clauses should match uv_barrier_init's in src/unix/thread.c. */
#if !defined(_WIN32) && (defined(_AIX) || defined(__OpenBSD__) || !defined(PTHREAD_BARRIER_SERIAL_THREAD))
/* TODO(bnoordhuis) Merge into uv_barrier_t in v2. */
struct _barrier {
    emutex_t  mutex;
    econd_t   cond;
    unsigned  threshold;
    unsigned  in;
    unsigned  out;
};

struct pthread_barrier_s{
    struct _barrier* b;
# if defined(PTHREAD_BARRIER_SERIAL_THREAD)
    /* TODO(bnoordhuis) Remove padding in v2. */
    char pad[sizeof(pthread_barrier_t) - sizeof(struct _barrier*)];
# endif
};

int pthread_barrier_init(pthread_barrier_t* barrier, unsigned int count)
{
    struct _barrier* b;
    int rc;

    if (barrier == NULL || count == 0)
        return E_EINVAL;

    b = emalloc(sizeof(*b));
    if (b == NULL)
        return E_ENOMEM;

    b->in = 0;
    b->out = 0;
    b->threshold = count;

    rc = emutex_init(b->mutex);
    if (rc != 0)
        goto error2;

    rc = econd_init(b->cond);
    if (rc != 0)
        goto error;

    barrier->b = b;

    return 0;

error:
    emutex_free(b->mutex);
error2:
    efree(b);

    return rc;
}

int pthread_barrier_wait(pthread_barrier_t* barrier)
{
    struct _barrier* b;
    int last;

    if (barrier == NULL || barrier->b == NULL)
        return E_EINVAL;

    b = barrier->b;
    emutex_lock(b->mutex);

    if (++b->in == b->threshold) {
        b->in = 0;
        b->out = b->threshold;
        econd_one(b->cond);
    } else {
        do
            econd_wait(b->cond, b->mutex);
        while (b->in != 0);
    }

    last = (--b->out == 0);
    if (!last)
        econd_one(b->cond);  /* Not needed for last thread. */

    emutex_ulck(b->mutex);

    return last;
}


void pthread_barrier_destroy(pthread_barrier_t* barrier)
{
    struct _barrier* b;

    b = barrier->b;
    emutex_lock(b->mutex);

    assert(b->in == 0);
    assert(b->out == 0);

    if (b->in != 0 || b->out != 0)
        abort();

    emutex_ulck(b->mutex);

    emutex_free(b->mutex);
    econd_free(b->cond);

    efree(barrier->b);
    barrier->b = NULL;
}

#endif

#if !__USING_PTHREAD

#define EUTILS_LLOG 1

#include "ealloc.h"
#include "eerrno.h"
#include "eutils.h"

#if defined(__MINGW64_VERSION_MAJOR)
/* MemoryBarrier expands to __mm_mfence in some cases (x86+sse2), which may
 * require this header in some versions of mingw64. */
#include <intrin.h>
#endif

struct __ethread_once_s{
    unsigned char ran;
    HANDLE event;
};

struct thread_ctx {
  void* (*entry)(void* arg);
  void* arg;
  __ethread_t self;
};

static __ethread_key_t  __current_thread_key;
static __ethread_once_t __current_thread_init_guard = E_ONCE_INIT;

static void __eonce_inner(__ethread_once_t* guard, void (*callback)(void))
{
    DWORD result;
    HANDLE existing_event, created_event;

    created_event = CreateEvent(NULL, 1, 0, NULL);
    if (created_event == 0) {
        /* Could fail in a low-memory situation? */
        llog_err("CreateEvent");
    }

    existing_event = InterlockedCompareExchangePointer(&guard->event,
                                                        created_event,
                                                        NULL);

    if (existing_event == NULL)
    {
        /* We won the race */
        callback();

        result = SetEvent(created_event);
        assert(result);
        guard->ran = 1;
    }
    else
    {
        /* We lost the race. Destroy the event we created and wait for the existing
         * one to become signaled. */
        CloseHandle(created_event);
        result = WaitForSingleObject(existing_event, INFINITE);
        assert(result == WAIT_OBJECT_0);
    }
}

int __ethread_create(__ethread_t *tid, void* (*entry)(void *arg), void *arg)
{
  __ethread_options_t params;
  params.flags = ETHREAD_NO_FLAGS;
  return __ethread_create_ex(tid, &params, entry, arg);
}

static UINT __stdcall __ethread_start(void* arg);
int __ethread_create_ex(__ethread_t* tid,
                               const __ethread_options_t* params,
                               void* (*entry)(void* arg),
                               void* arg)
{
    struct thread_ctx* ctx;
    int err;
    HANDLE thread;
    SYSTEM_INFO sysinfo;
    size_t stack_size;
    size_t pagesize;

    stack_size =
        params->flags & ETHREAD_HAS_STACK_SIZE ? params->stack_size : 0;

    if (stack_size != 0) {
        GetNativeSystemInfo(&sysinfo);
        pagesize = (size_t)sysinfo.dwPageSize;
        /* Round up to the nearest page boundary. */
        stack_size = (stack_size + pagesize - 1) &~ (pagesize - 1);

        if ((unsigned)stack_size != stack_size)
            return E_EINVAL;
    }

    ctx = emalloc(sizeof(*ctx));
    if (ctx == NULL)
        return E_ENOMEM;

    ctx->entry = entry;
    ctx->arg   = arg;

    /* Create the thread in suspended state so we have a chance to pass
     * its own creation handle to it */
    thread = (HANDLE) _beginthreadex(NULL,
                                     (unsigned)stack_size,
                                     __ethread_start,
                                     ctx,
                                     CREATE_SUSPENDED,
                                     NULL);
    if (thread == NULL) {
        err = errno;
        efree(ctx);
    }
    else
    {
        err = 0;
        *tid = thread;
        ctx->self = thread;
        ResumeThread(thread);
    }

    switch (err)
    {
        case 0:
            return 0;
        case EACCES:
            return E_EACCES;
        case EAGAIN:
            return E_EAGAIN;
        case EINVAL:
            return E_EINVAL;
    }

    return E_EIO;
}

void __ethread_once(__ethread_once_t* guard, void (*callback)(void)) {
    /* Fast case - avoid WaitForSingleObject. */
    if (guard->ran) {
        return;
    }

    __eonce_inner(guard, callback);
}

static void __init_current_thread_key(void) {
  if (__ethread_key_create(&__current_thread_key))
    abort();
}

static UINT __stdcall __ethread_start(void* arg)
{
  struct thread_ctx *ctx_p;
  struct thread_ctx ctx;

  ctx_p = arg;
  ctx = *ctx_p;
  efree(ctx_p);

  __ethread_once(&__current_thread_init_guard, __init_current_thread_key);
  __ethread_key_set(&__current_thread_key, (void*) ctx.self);

  ctx.entry(ctx.arg);

  return 0;
}

int   __emutex_init   (__emutex_t* mutex) { InitializeCriticalSection(mutex); return 0; }
void  __emutex_destroy(__emutex_t* mutex) { DeleteCriticalSection(mutex); }
void  __emutex_lock   (__emutex_t* mutex) { EnterCriticalSection(mutex); }
int   __emutex_trylock(__emutex_t* mutex) { return TryEnterCriticalSection(mutex) ? 0 : E_EBUSY; }
void  __emutex_unlock (__emutex_t* mutex) { LeaveCriticalSection(mutex); }

int   __erwlock_init     (__erwlock_t* rwlock)
{
    /* Initialize the semaphore that acts as the write lock. */
    HANDLE handle = CreateSemaphoreW(NULL, 1, 1, NULL);
    if (handle == NULL)
        return uv_translate_sys_error(GetLastError());

    rwlock->state_.write_semaphore_ = handle;

    /* Initialize the critical section protecting the reader count. */
    InitializeCriticalSection(&rwlock->state_.num_readers_lock_);

    /* Initialize the reader count. */
    rwlock->state_.num_readers_ = 0;

    return 0;
}
void  __erwlock_destroy  (__erwlock_t* rwlock)
{
    DeleteCriticalSection(&rwlock->state_.num_readers_lock_);
    CloseHandle(rwlock->state_.write_semaphore_);
}
void  __erwlock_rdlock   (__erwlock_t* rwlock)
{
    /* Acquire the lock that protects the reader count. */
    EnterCriticalSection(&rwlock->state_.num_readers_lock_);

    /* Increase the reader count, and lock for write if this is the first
     * reader.
     */
    if (++rwlock->state_.num_readers_ == 1) {
      DWORD r = WaitForSingleObject(rwlock->state_.write_semaphore_, INFINITE);
      if (r != WAIT_OBJECT_0)
        eerr_fatal(GetLastError(), "WaitForSingleObject");
    }

    /* Release the lock that protects the reader count. */
    LeaveCriticalSection(&rwlock->state_.num_readers_lock_);
}
int   __erwlock_tryrdlock(__erwlock_t* rwlock)
{
    int err;

    if (!TryEnterCriticalSection(&rwlock->state_.num_readers_lock_))
        return E_EBUSY;

    err = 0;

    if (rwlock->state_.num_readers_ == 0)
    {
        /* Currently there are no other readers, which means that the write lock
         * needs to be acquired.
        */
        DWORD r = WaitForSingleObject(rwlock->state_.write_semaphore_, 0);
        if (r == WAIT_OBJECT_0)
            rwlock->state_.num_readers_++;
        else if (r == WAIT_TIMEOUT)
            err = UV_EBUSY;
        else if (r == WAIT_FAILED)
            eerr_fatal(GetLastError(), "WaitForSingleObject");

    } else {
        /* The write lock has already been acquired because there are other
         * active readers.
        */
        rwlock->state_.num_readers_++;
    }

    LeaveCriticalSection(&rwlock->state_.num_readers_lock_);
    return err;
}
void  __erwlock_rdunlock (__erwlock_t* rwlock)
{
    EnterCriticalSection(&rwlock->state_.num_readers_lock_);

    if (--rwlock->state_.num_readers_ == 0) {
        if (!ReleaseSemaphore(rwlock->state_.write_semaphore_, 1, NULL))
            eerr_fatal(GetLastError(), "ReleaseSemaphore");
    }

    LeaveCriticalSection(&rwlock->state_.num_readers_lock_);
}
void  __erwlock_wrlock   (__erwlock_t* rwlock)
{
    DWORD r = WaitForSingleObject(rwlock->state_.write_semaphore_, INFINITE);
    if (r != WAIT_OBJECT_0)
        eerr_fatal(GetLastError(), "WaitForSingleObject");
}
int   __erwlock_trywrlock(__erwlock_t* rwlock)
{
    DWORD r = WaitForSingleObject(rwlock->state_.write_semaphore_, 0);

    if (r == WAIT_OBJECT_0)
        return 0;
    else if (r == WAIT_TIMEOUT)
        return E_EBUSY;
    else
        eerr_fatal(GetLastError(), "WaitForSingleObject");

    return E_UNKNOWN;
}
void  __erwlock_wrunlock (__erwlock_t* rwlock)
{
    if (!ReleaseSemaphore(rwlock->state_.write_semaphore_, 1, NULL))
        eerr_fatal(GetLastError(), "ReleaseSemaphore");
}

int   __esem_init   (__esem_t* sem, unsigned int value)
{
    *sem = CreateSemaphore(NULL, value, INT_MAX, NULL);
    if (*sem == NULL)
        return uv_translate_sys_error(GetLastError());
    else
        return 0;
}
void  __esem_destroy(__esem_t* sem)
{
    if (!CloseHandle(*sem))
        abort();
}
void  __esem_post   (__esem_t* sem)
{
    if (!ReleaseSemaphore(*sem, 1, NULL))
        abort();
}
void  __esem_wait   (__esem_t* sem)
{
    if (WaitForSingleObject(*sem, INFINITE) != WAIT_OBJECT_0)
         abort();
}
int   __esem_trywait(__esem_t* sem)
{
    DWORD r = WaitForSingleObject(*sem, 0);

    if (r == WAIT_OBJECT_0)
        return 0;

    if (r == WAIT_TIMEOUT)
        return E_EAGAIN;

    abort();
    return -1; /* Satisfy the compiler. */
}

int   __econd_init     (__econd_t* cond)
{
    InitializeConditionVariable(&cond->cond_var);
    return 0;
}
void  __econd_destroy  (__econd_t* cond)
{
    /* nothing to do */
    (void) &cond;
}
void  __econd_one      (__econd_t* cond)
{
    WakeConditionVariable(&cond->cond_var);
}
void  __econd_all      (__econd_t* cond)
{
    WakeAllConditionVariable(&cond->cond_var);
}
void  __econd_wait     (__econd_t* cond, __emutex_t* mutex)
{
    if (!SleepConditionVariableCS(&cond->cond_var, mutex, INFINITE))
      abort();
}
int  __econd_timedwait(__econd_t* cond, __emutex_t* mutex, uint64_t timeout)
{
    if (SleepConditionVariableCS(&cond->cond_var, mutex, (DWORD)(timeout / 1e6)))
        return 0;
    if (GetLastError() != ERROR_TIMEOUT)
        abort();
    return E_ETIMEDOUT;
}

int   __ebarrier_init   (__ebarrier_t* barrier, unsigned int count)
{
    int err;

    barrier->n = count;
    barrier->count = 0;

    err = __emutex_init(&barrier->mutex);
    if (err)
      return err;

    err = __esem_init(&barrier->turnstile1, 0);
    if (err)
      goto error2;

    err = __esem_init(&barrier->turnstile2, 1);
    if (err)
      goto error;

    return 0;

  error:
    __esem_destroy(&barrier->turnstile1);
  error2:
    __emutex_destroy(&barrier->mutex);
    return err;
}
void  __ebarrier_destroy(__ebarrier_t* barrier)
{
    __esem_destroy(&barrier->turnstile2);
    __esem_destroy(&barrier->turnstile1);
    __emutex_destroy(&barrier->mutex);
}
int   __ebarrier_wait   (__ebarrier_t* barrier)
{
    int serial_thread;

    __emutex_lock(&barrier->mutex);
    if (++barrier->count == barrier->n) {
      __esem_wait(&barrier->turnstile2);
      __esem_post(&barrier->turnstile1);
    }
    __emutex_unlock(&barrier->mutex);

    __esem_wait(&barrier->turnstile1);
    __esem_post(&barrier->turnstile1);

    __emutex_lock(&barrier->mutex);
    serial_thread = (--barrier->count == 0);
    if (serial_thread) {
      __esem_wait(&barrier->turnstile1);
      __esem_post(&barrier->turnstile2);
    }
    __emutex_unlock(&barrier->mutex);

    __esem_wait(&barrier->turnstile2);
    __esem_post(&barrier->turnstile2);
    return serial_thread;
}

int   __ethread_key_create(__ethread_key_t* key)
{
    key->tls_index = TlsAlloc();
    if (key->tls_index == TLS_OUT_OF_INDEXES)
        return E_ENOMEM;
    return 0;
}
void  __ethread_key_delete(__ethread_key_t* key)
{
    if (TlsFree(key->tls_index) == FALSE)
        abort();
    key->tls_index = TLS_OUT_OF_INDEXES;
}
void* __ethread_key_get(__ethread_key_t* key)
{
    void* value;

    value = TlsGetValue(key->tls_index);
    if (value == NULL)
        if (GetLastError() != ERROR_SUCCESS)
            abort();

    return value;
}
void  __ethread_key_set(__ethread_key_t* key, void* value)
{
    if (TlsSetValue(key->tls_index, value) == FALSE)
        abort();
}

#endif
