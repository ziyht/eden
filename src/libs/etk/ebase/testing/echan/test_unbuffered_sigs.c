/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ethread.h"
#include "echan.h"
#include "etime.h"

static int  wait_ms;
static uint recv_sig;

static void* _recv_sig(void*d)
{
    echan c = d;

    usleep(wait_ms * 1000);

    uint s = echan_recvSig(c);

    return s == recv_sig ? (void*)1 : 0;
}

static void* _recv_all(void*d)
{
    echan c = d;

    usleep(wait_ms * 1000);

    evar sigs = echan_recvAll(c);

    return (sigs.type == E_SIG && sigs.cnt == recv_sig) ? (void*)1 : 0;
}

static int test_unbuffered_sendSigs()
{
    echan c; int ret; ethread_t th; void* thread_ret;

    c = echan_new(E_SIG, 0);

    {
        wait_ms  = 100;
        recv_sig = 1;

        ethread_init(th, _recv_sig, c);

        ret = echan_sendSig(c, 1);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    {
        wait_ms  = 0;
        recv_sig = 1;

        ethread_init(th, _recv_sig, c);

        usleep(100 * 1000);

        ret = echan_sendSig(c, 1);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    {
        wait_ms  = 100;
        recv_sig = 1;

        ethread_init(th, _recv_all, c);

        ret = echan_sendSig(c, 1);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    {
        wait_ms  = 0;
        recv_sig = 1;

        ethread_init(th, _recv_all, c);

        usleep(100 * 1000);

        ret = echan_sendSig(c, 1);
        eexpect_eq(ret, true);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_trySendSigs()
{
    echan c; int ret;

    c = echan_new(E_SIG, 0);

    ret = echan_trySendSig(c, 1);
    eexpect_eq(ret, false);

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_timeSendSigs()
{
    echan c; int ret;

    c = echan_new(E_SIG, 0);

    ret = echan_timeSendSig(c, 1, 500);
    eexpect_eq(ret, false);

    echan_free(c);

    return ETEST_OK;
}


static void* _send_sig(void* d)
{
    echan c = d;

    usleep(wait_ms * 1000);

    bool ret = echan_sendSig(c, 1);

    return ret ? (void*)1 : 0;
}

static int test_unbuffered_recvSigs()
{
    echan c; int ret; ethread_t th; void* thread_ret;

    c = echan_new(E_SIG, 0);

    {
        wait_ms = 100;

        ethread_init(th, _send_sig, c);

        ret = echan_recvSig(c);
        eexpect_eq(ret, 1);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    {
        wait_ms = 0;

        ethread_init(th, _send_sig, c);

        usleep(100 * 1000);

        ret = echan_recvSig(c);
        eexpect_eq(ret, 1);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);
    }

    echan_free(c);

    return ETEST_OK;
}

static int test_unbuffered_tryRecvSigs()
{
    echan c; int ret; ethread_t th; void* thread_ret; evar var;

    {
        c = echan_new(E_SIG, 0);

        wait_ms = 0;
        ethread_init(th, _send_sig, c);

        usleep(100 * 1000);

        ret = echan_tryRecvSig(c);
        eexpect_eq(ret, 1);

        ret = echan_tryRecvSig(c);
        eexpect_eq(ret, false);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 0);

        wait_ms = 0;
        ethread_init(th, _send_sig, c);

        usleep(100 * 1000);

        var = echan_tryRecvAll(c);
        eexpect_eq(var.type, E_SIG);
        eexpect_eq(var.cnt , 1);

        var = echan_tryRecvAll(c);
        eexpect_eq(var.type, E_NAV);
        eexpect_eq(var.cnt , 0);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }


    return ETEST_OK;
}

static int test_unbuffered_timeRecvSigs()
{
    echan c; int ret; i64 ticker; ethread_t th; void* thread_ret; evar var;

    {
        c = echan_new(E_SIG, 0);

        wait_ms = 0;
        ethread_init(th, _send_sig, c);

        e_ticker_ms(&ticker);
        ret = echan_timeRecvSig(c, 100);
        eexpect_eq(ret , 1);
        eexpect_le(e_ticker_ms(&ticker), 10);

        ret = echan_timeRecvSig(c, 100);
        eexpect_eq(ret , 0);
        eexpect_ge(e_ticker_ms(&ticker), 50);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_SIG, 0);

        wait_ms = 0;
        ethread_init(th, _send_sig, c);

        e_ticker_ms(&ticker);
        var = echan_timeRecvAll(c, 100);
        eexpect_eq(var.type, E_SIG);
        eexpect_eq(var.cnt , 1);
        eexpect_le(e_ticker_ms(&ticker), 10);

        var = echan_timeRecvAll(c, 100);
        eexpect_eq(var.type, E_NAV);
        eexpect_eq(var.cnt , 0);
        eexpect_ge(e_ticker_ms(&ticker), 50);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}

int test_unbuffered_sigs(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_unbuffered_sendSigs() );
    ETEST_RUN( test_unbuffered_trySendSigs() );
    ETEST_RUN( test_unbuffered_timeSendSigs() );

    ETEST_RUN( test_unbuffered_recvSigs() );
    ETEST_RUN( test_unbuffered_tryRecvSigs() );
    ETEST_RUN( test_unbuffered_timeRecvSigs() );


    return ETEST_OK;
}

