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
#if defined(_AIX) || \
    defined(__OpenBSD__) || \
    !defined(PTHREAD_BARRIER_SERIAL_THREAD)
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
