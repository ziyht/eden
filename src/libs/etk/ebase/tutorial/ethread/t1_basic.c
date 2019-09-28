/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"

int counter = 0;

void* t1_cb(void* d)
{
    emutex_t* mu = d;

    int i = 100000;

    while(i--)
    {
        emutex_lock(*mu);
        counter++;
        emutex_ulck(*mu);
    }

    return (void*)1;
}

static int t1_basic_case1()
{

    ethread_t th1, th2; emutex_t mu;

    emutex_init(mu);

    ethread_init(th1, t1_cb, &mu);
    ethread_init(th2, t1_cb, &mu);

    ethread_join(th1);
    ethread_join(th2);

    printf("%d\n", counter);

    return ETEST_OK;
}



int t1_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( t1_basic_case1() );

    return ETEST_OK;
}

