/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"
#include "echan.h"

static int wait_ms;
static int recv_sigs;
static int send_sigs;

static void* _recv_sig(void*d)
{
    echan c = d;

    usleep(wait_ms * 1000);

    int s = echan_recvSigs(c, recv_sigs);

    return s == recv_sigs ? (void*)1 : 0;
}

static int test_buffered_sendSigs()
{
    echan c; int ret; ethread_t th; void* thread_ret;

    {
        c = echan_new(E_SIG, 1);

        wait_ms   = 0;
        recv_sigs = 1;

        ethread_init(th, _recv_sig, c);

        ret = echan_sendSig(c, 1);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 10);

        wait_ms   = 0;
        recv_sigs = 10;

        ethread_init(th, _recv_sig, c);

        ret = echan_sendSig(c, 10);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 5);

        wait_ms   = 500;
        recv_sigs = 10;

        ethread_init(th, _recv_sig, c);

        ret = echan_sendSig(c, 10);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 5);

        wait_ms   = 0;
        recv_sigs = 10;

        ethread_init(th, _recv_sig, c);

        usleep(500 * 1000);

        ret = echan_sendSig(c, 10);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_trySendSigs()
{
    echan c; int ret;

    {
        c = echan_new(E_SIG, 1);

        ret = echan_trySendSig(c, 1);
        eexpect_eq(ret, true);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 1);

        ret = echan_trySendSig(c, 2);
        eexpect_eq(ret, false);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_timeSendSigs()
{
    echan c; int ret; i64 ticker;

    {
        c = echan_new(E_SIG, 1);

        e_ticker_ms(&ticker);
        ret = echan_timeSendSig(c, 1, 500);
        eexpect_eq(ret, true);
        eexpect_lt(e_ticker_ms(&ticker), 10);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 1);

        e_ticker_ms(&ticker);
        ret = echan_timeSendSig(c, 2, 500);
        eexpect_eq(ret, false);
        eexpect_ge(e_ticker_ms(&ticker), 400);

        echan_free(c);
    }

    return ETEST_OK;
}

static void* _send_sig(void* d)
{
    echan c = d;

    usleep(wait_ms * 1000);

    bool ret = echan_sendSig(c, send_sigs);

    return ret ? (void*)1 : 0;
}

static int test_buffered_recvSigs()
{
    echan c; int ret; ethread_t th; void* thread_ret;

    {
        c = echan_new(E_SIG, 1);

        wait_ms   = 0;
        send_sigs = 1;

        ethread_init(th, _send_sig, c);

        ret = echan_recvSig(c);
        eexpect_eq(ret, 1);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 10);

        wait_ms   = 0;
        send_sigs = 10;

        ethread_init(th, _send_sig, c);

        ret = echan_recvSigs(c, 10);
        eexpect_eq(ret, 10);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 5);

        wait_ms   = 500;
        send_sigs = 10;

        ethread_init(th, _send_sig, c);

        ret = echan_recvSigs(c, 10);
        eexpect_eq(ret, 10);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 5);

        wait_ms   = 0;
        send_sigs = 10;

        ethread_init(th, _send_sig, c);

        usleep(500 * 1000);

        ret = echan_recvSigs(c, 10);
        eexpect_eq(ret, 10);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_tryRecvSigs()
{
    echan c; int ret;

    {
        c = echan_new(E_SIG, 1);

        ret = echan_tryRecvSigs(c, 1);
        eexpect_eq(ret, false);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 1);

        ret = echan_tryRecvSigs(c, 2);
        eexpect_eq(ret, false);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_buffered_timeRecvSigs()
{
    echan c; int ret; i64 ticker;

    {
        c = echan_new(E_SIG, 1);

        e_ticker_ms(&ticker);
        ret = echan_timeRecvSigs(c, 1, 500);
        eexpect_eq(ret, false);
        eexpect_ge(e_ticker_ms(&ticker), 400);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 1);

        ret = echan_timeRecvSigs(c, 2, 500);
        eexpect_eq(ret, false);
        eexpect_ge(e_ticker_ms(&ticker), 400);

        echan_free(c);
    }

    return ETEST_OK;
}

int test_buffered_sigs(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_buffered_sendSigs() );
    ETEST_RUN( test_buffered_trySendSigs() );
    ETEST_RUN( test_buffered_timeSendSigs() );

    ETEST_RUN( test_buffered_recvSigs() );
    ETEST_RUN( test_buffered_tryRecvSigs() );
    ETEST_RUN( test_buffered_timeRecvSigs() );

    return ETEST_OK;
}

