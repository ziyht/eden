/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"
#include "echan.h"

int count = 100000;

void* recver(void* d)
{
    echan c = d;

    while (1) {
        evar v = echan_timeRecvV(c, 1000);

        if(v.type == E_NAV)
            return (void*)0;

        if(v.v.i32 == count)
            break;
    }

    return (void*)1;
}

static int t1_basic_case1()
{

    ethread_t th; void* ret;
    echan c = echan_new(E_I32, 128);    // type, cap

    ethread_init(th, recver, c);

    for(int i = 1; i <= count; i ++)
    {
        if(false == echan_sendV(c, EVAR_I32(i)))
        {
            puts("send failed");
            break;
        }
    }

    ethread_join_ex(th, ret);
    echan_free(c);

    if(ret != (void*)1)
    {
        puts("recver failed");
        return ETEST_ERR;
    }

    puts("ok");

    return ETEST_OK;
}



int t1_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( t1_basic_case1() );

    return ETEST_OK;
}

