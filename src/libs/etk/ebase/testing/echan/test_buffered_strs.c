/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eutils.h"
#include "echan.h"
#include "ethread.h"

static int wait_ms;
static int recv_cnt;

static void* _recv_num(void*d)
{
    echan c = d; int recved = 0;

    usleep(wait_ms * 1000);

    for(int i = 0; i < recv_cnt; i++)
    {
        estr s = echan_recvS(c);

        if(0 == strcmp(s, llstr(i)))
            recved++;

        estr_free(s);
    }

    return recved == recv_cnt ? (void*)1 : 0;
}

static int test_buffered_strs_send()
{
    echan c; int ret; ethread_t th; void* thread_ret;

    {
        c = echan_new(E_STR, 1);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_num, c);

        ret = echan_sendS(c, llstr(0));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 10);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_num, c);

        ret = echan_sendS(c, llstr(0));
        ret = echan_sendS(c, llstr(1));
        ret = echan_sendS(c, llstr(2));
        ret = echan_sendV(c, EVAR_S(llstr(3)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 2);

        wait_ms  = 500;
        recv_cnt = 4;

        ethread_init(th, _recv_num, c);

        ret = echan_sendS(c, llstr(0));
        ret = echan_sendS(c, llstr(1));
        ret = echan_sendS(c, llstr(2));
        ret = echan_sendV(c, EVAR_S(llstr(3)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 2);

        wait_ms  = 0;
        recv_cnt = 4;

        ethread_init(th, _recv_num, c);

        usleep(500 * 1000);

        ret = echan_sendS(c, llstr(0));
        ret = echan_sendS(c, llstr(1));
        ret = echan_sendS(c, llstr(2));
        ret = echan_sendV(c, EVAR_S(llstr(3)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}




int test_buffered_strs(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_buffered_strs_send() );

    return ETEST_OK;
}

