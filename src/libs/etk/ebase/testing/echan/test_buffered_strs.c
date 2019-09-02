/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eutils.h"
#include "echan.h"
#include "ethread.h"
#include "etime.h"

static int wait_ms;
static int recv_cnt;

static void* _recv_num(void*d)
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

static int test_buffered_strs_send()
{
    echan c; int ret; ethread_t th; void* thread_ret; char num[24];

    {
        c = echan_new(E_STR, 1);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_num, c);

        ret = echan_sendS(c, llstr_r(0, num));
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

        ret = echan_sendS(c, llstr_r(0, num));
        ret = echan_sendS(c, llstr_r(1, num));
        ret = echan_sendS(c, llstr_r(2, num));
        ret = echan_sendV(c, EVAR_S(llstr_r(3, num)));
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

        ret = echan_sendS(c, llstr_r(0, num));
        ret = echan_sendS(c, llstr_r(1, num));
        ret = echan_sendS(c, llstr_r(2, num));
        ret = echan_sendV(c, EVAR_S(llstr_r(3, num)));
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

        ret = echan_sendS(c, llstr_r(0, num));
        ret = echan_sendS(c, llstr_r(1, num));
        ret = echan_sendS(c, llstr_r(2, num));
        ret = echan_sendV(c, EVAR_S(llstr_r(3, num)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_strs_trysend()
{
    echan c; int ret; char num[24];

    {
        c = echan_new(E_STR, 1);

        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, true);


        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, false);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_strs_timesend()
{
    echan c; int ret; char num[24]; i64 ticker;

    {
        c = echan_new(E_STR, 1);

        e_ticker_ms(&ticker);
        ret = echan_timeSendS(c, llstr_r(0, num), 500);
        eexpect_eq(ret, true);
        eexpect_lt(e_ticker_ms(&ticker), 10);


        ret = echan_timeSendS(c, llstr_r(0, num), 500);
        eexpect_eq(ret, false);
        eexpect_gt(e_ticker_ms(&ticker), 400);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_strs_tryrecv()
{
    echan c; int ret; char num[24]; estr s;

    {
        c = echan_new(E_STR, 1);

        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, true);

        s = echan_tryRecvS(c);
        eexpect_str(s, "0");
        estr_free(s);

        s = echan_tryRecvS(c);
        eexpect_ptr(s, 0);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_strs_timerecv()
{
    echan c; int ret; char num[24]; estr s; i64 ticker;

    {
        c = echan_new(E_STR, 1);

        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, true);

        e_ticker_ms(&ticker);
        s = echan_timeRecvS(c, 500);
        eexpect_str(s, "0");
        eexpect_lt(e_ticker_ms(&ticker), 10);
        estr_free(s);

        s = echan_timeRecvS(c, 500);
        eexpect_ptr(s, 0);
        eexpect_gt(e_ticker_ms(&ticker), 400);

        echan_free(c);
    }

    return ETEST_OK;
}

int test_buffered_strs(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_buffered_strs_send() );
    ETEST_RUN( test_buffered_strs_trysend() );
    ETEST_RUN( test_buffered_strs_timesend() );

    ETEST_RUN( test_buffered_strs_tryrecv() );
    ETEST_RUN( test_buffered_strs_timerecv() );

    return ETEST_OK;
}

