/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"
#include "echan.h"

static int wait_ms;
static int recv_cnt;
static int send_cnt;

static void* _recv_num(void*d)
{
    echan c = d; int recved = 0;

    usleep(wait_ms * 1000);

    for(int i = 0; i < recv_cnt; i++)
    {
        evar var = echan_recvV(c);

        if(var.v.i64 == i)
            recved++;
    }

    return recved == recv_cnt ? (void*)1 : 0;
}

static int test_buffered_sendNums(etypev t)
{
    echan c; int ret; ethread_t th; void* thread_ret;

    {
        c = echan_new(t, 1);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_num, c);

        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(0)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(t, 10);

        wait_ms = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_num, c);

        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(0)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(1)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(3)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(t, 2);

        wait_ms  = 500;
        recv_cnt = 4;

        ethread_init(th, _recv_num, c);

        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(0)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(1)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(3)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(t, 2);

        wait_ms  = 0;
        recv_cnt = 4;

        ethread_init(th, _recv_num, c);

        usleep(500 * 1000);

        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(0)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(1)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(3)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}



static int test_buffered_trySendNums(etypev t)
{
    echan c; int ret;

    c = echan_new(t, 1);

    ret = echan_trySendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
    eexpect_eq(ret, true);

    ret = echan_trySendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
    eexpect_eq(ret, false);

    echan_free(c);

    return ETEST_OK;
}

static int test_buffered_timeSendNums(etypev t)
{
    echan c; int ret; i64 ticker;

    c = echan_new(t, 1);

    e_ticker_ms(&ticker);
    ret = echan_timeSendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)), 500);
    eexpect_eq(ret, true);
    eexpect_lt(e_ticker_ms(&ticker), 10);

    ret = echan_timeSendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)), 500);
    eexpect_eq(ret, false);
    eexpect_gt(e_ticker_ms(&ticker), 400);

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_tryRecvNums(etypev t)
{
    echan c; int ret; i64 ticker; evar var;

    c = echan_new(t, 1);

    ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(1)));
    eexpect_eq(ret, true);

    e_ticker_ms(&ticker);
    var = echan_tryRecvV(c);
    eexpect_ne(var.type, E_NAV);
    eexpect_eq(var.v.i64, 1);
    eexpect_lt(e_ticker_ms(&ticker) , 10);

    e_ticker_ms(&ticker);
    var = echan_tryRecvV(c);
    eexpect_eq(var.type, E_NAV);
    eexpect_eq(var.v.i64, 0);
    eexpect_lt(e_ticker_ms(&ticker) , 10);

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_timeRecvNums(etypev t)
{
    echan c; int ret; i64 ticker; evar var;

    c = echan_new(t, 1);

    ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(1)));
    eexpect_eq(ret, true);

    e_ticker_ms(&ticker);
    var = echan_timeRecvV(c, 500);
    eexpect_ne(var.type, E_NAV);
    eexpect_eq(var.v.i64, 1);
    eexpect_lt(e_ticker_ms(&ticker) , 10);

    e_ticker_ms(&ticker);
    var = echan_timeRecvV(c, 500);
    eexpect_eq(var.type, E_NAV);
    eexpect_eq(var.v.i64, 0);
    eexpect_gt(e_ticker_ms(&ticker) , 400);

    echan_free(c);

    return ETEST_OK;
}

static int test_buffered_nums_exec(etypev t)
{
    ETEST_RUN( test_buffered_sendNums(E_I8) );
    ETEST_RUN( test_buffered_trySendNums(E_I8) );
    ETEST_RUN( test_buffered_timeSendNums(E_I8) );

    ETEST_RUN( test_unbuffered_tryRecvNums(E_I8) );
    ETEST_RUN( test_unbuffered_timeRecvNums(E_I8) );

    return ETEST_OK;
}

int test_buffered_nums(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_buffered_nums_exec(E_I8) );
    ETEST_RUN( test_buffered_nums_exec(E_I16) );
    ETEST_RUN( test_buffered_nums_exec(E_I32) );
    ETEST_RUN( test_buffered_nums_exec(E_I64) );

    ETEST_RUN( test_buffered_nums_exec(E_U8) );
    ETEST_RUN( test_buffered_nums_exec(E_U16) );
    ETEST_RUN( test_buffered_nums_exec(E_U32) );
    ETEST_RUN( test_buffered_nums_exec(E_U64) );

    ETEST_RUN( test_buffered_nums_exec(E_PTR) );

    return ETEST_OK;
}

