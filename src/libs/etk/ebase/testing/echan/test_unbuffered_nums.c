/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"
#include "echan.h"
#include "etime.h"

static int wait_ms;

static void* _recv_num(void*d)
{
    echan c = d;

    usleep(wait_ms * 1000);

    evar var = echan_recvV(c);

    return var.v.i64 == 2 ? (void*)1 : 0;
}

static int test_unbuffered_sendNums(etypev t)
{
    echan c; int ret; ethread_t th; void* thread_ret;

    c = echan_new(t, 0);

    {
        wait_ms = 500;

        ethread_init(th, _recv_num, c);

        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    {
        wait_ms = 0;

        ethread_init(th, _recv_num, c);

        usleep(500 * 1000);

        ret = echan_sendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_trySendNums(etypev t)
{
    echan c; int ret;

    c = echan_new(t, 0);

    ret = echan_trySendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)));
    eexpect_eq(ret, false);

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_timeSendNums(etypev t)
{
    echan c; int ret; i64 tick;

    c = echan_new(t, 0);

    e_ticker_ms(&tick);
    ret = echan_timeSendV(c, __EVAR_MK(t, 1, 1, EVAL_I8(2)), 500);
    eexpect_eq(ret, false);
    eexpect_ge(e_ticker_ms(&tick), 400);

    echan_free(c);

    return ETEST_OK;
}

static void* _send_num(void* d)
{
    echan c = d;

    usleep(wait_ms * 1000);

    int ret = echan_sendV(c, __EVAR_MK(echan_type(c), 1, 1, EVAL_I8(2)));

    return ret ? (void*)1 : 0;
}

static int test_unbuffered_recvNums(etypev t)
{
    echan c; ethread_t th; void* thread_ret;

    c = echan_new(t, 0);

    {
        wait_ms = 500;

        ethread_init(th, _send_num, c);

        evar var = echan_recvV(c);
        eexpect_eq(var.v.i64, 2);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    {
        wait_ms = 0;

        ethread_init(th, _send_num, c);

        usleep(500 * 1000);

        evar var = echan_recvV(c);
        eexpect_eq(var.v.i64, 2);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_tryRecvNums(etypev t)
{
    echan c;

    c = echan_new(t, 0);

    evar var = echan_tryRecvV(c);
    eexpect_eq(var.type, E_NAV);
    eexpect_eq(var.v.i64, 0);

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_timeRecvNums(etypev t)
{
    echan c; i64 ticker;

    c = echan_new(t, 0);

    e_ticker_ms(&ticker);
    evar var = echan_timeRecvV(c, 500);
    eexpect_eq(var.type, E_NAV);
    eexpect_eq(var.v.i64, 0);
    eexpect_ge(e_ticker_ms(&ticker), 400);

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_nums_exec(etypev t)
{
    ETEST_RUN( test_unbuffered_sendNums    (t) );
    ETEST_RUN( test_unbuffered_trySendNums (t) );
    ETEST_RUN( test_unbuffered_timeSendNums(t) );

    ETEST_RUN( test_unbuffered_recvNums    (t) );
    ETEST_RUN( test_unbuffered_tryRecvNums (t) );
    ETEST_RUN( test_unbuffered_timeRecvNums(t) );

    return ETEST_OK;
}

int test_unbuffered_nums(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_unbuffered_nums_exec(E_I8) );
    ETEST_RUN( test_unbuffered_nums_exec(E_I16) );
    ETEST_RUN( test_unbuffered_nums_exec(E_I32) );
    ETEST_RUN( test_unbuffered_nums_exec(E_I64) );

    ETEST_RUN( test_unbuffered_nums_exec(E_U8) );
    ETEST_RUN( test_unbuffered_nums_exec(E_U16) );
    ETEST_RUN( test_unbuffered_nums_exec(E_U32) );
    ETEST_RUN( test_unbuffered_nums_exec(E_U64) );

    ETEST_RUN( test_unbuffered_nums_exec(E_PTR) );

    return ETEST_OK;
}

