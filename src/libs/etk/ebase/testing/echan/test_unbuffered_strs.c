/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"
#include "echan.h"
#include "etime.h"
#include "eutils.h"

static int wait_ms;
static int recv_cnt;

static void* _recv_str(void*d)
{
    echan c = d; int recved = 0; char num[24];

    usleep(wait_ms * 1000);

    for(int i = 0; i < recv_cnt; i++)
    {
        estr s = echan_recvS(c);

        if(0 == strcmp(s, llstr_r(i, num)))
            recved++;

        estr_free(s);
    }

    return recved == recv_cnt ? (void*)1 : 0;
}

static int test_unbuffered_strs_sendrecv()
{
    echan c; int ret; ethread_t th; void* thread_ret; char num[24];

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_str, c);

        ret = echan_sendS(c, llstr_r(0, num));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 0);

        wait_ms = 500;
        recv_cnt = 4;

        ethread_init(th, _recv_str, c);

        ret = echan_sendS(c, llstr_r(0, num));
        ret = echan_sendS(c, llstr_r(1, num));
        ret = echan_sendS(c, llstr_r(2, num));
        ret = echan_sendS(c, llstr_r(3, num));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 0);

        wait_ms = 0;
        recv_cnt = 4;

        ethread_init(th, _recv_str, c);

        usleep(500 * 1000);

        ret = echan_sendS(c, llstr_r(0, num));
        ret = echan_sendS(c, llstr_r(1, num));
        ret = echan_sendS(c, llstr_r(2, num));
        ret = echan_sendS(c, llstr_r(3, num));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}


int test_unbuffered_strs(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_unbuffered_strs_sendrecv() );

    return ETEST_OK;
}

