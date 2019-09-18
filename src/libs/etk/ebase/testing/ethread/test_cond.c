/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"

typedef struct lock_s{

    econd_t  co;
    emutex_t mu;

}lock_t;

void* cond_one(void* d)
{
    lock_t * l = d;

    econd_one(l->co);

    return 0;
}

static int test_cond_one()
{
    ethread_t th; lock_t lock;

    emutex_init(lock.mu);
    econd_init(lock.co);

    emutex_lock(lock.mu);

    ethread_init(th, cond_one, &lock);

    econd_wait(lock.co, lock.mu);

    emutex_ulck(lock.mu);

    emutex_free(lock.mu);
    econd_free(lock.co);

    ethread_join(th);

    return ETEST_OK;
}


int test_cond(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_cond_one() );

    return ETEST_OK;
}

