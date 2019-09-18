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

static void* _recv_all(void*d)
{
    echan c = d; int recved = 0; char num[24];

    usleep(wait_ms * 1000);

    for(int i = 0; recved < recv_cnt; i++)
    {
        evar strs = echan_recvAll(c);

        if(strs.type == E_STR)
        {
            for(uint j = 0; j < strs.cnt; j++)
            {
                if(0 == strcmp(evar_iValS(strs, j), llstr_r(recved, num)))
                    recved++;
                else
                    goto ret;
            }
        }

        evar_free(strs);
    }

ret:
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

        wait_ms  = 100;
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

        usleep(100 * 1000);

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

        wait_ms  = 100;
        recv_cnt = 4;

        ethread_init(th, _recv_all, c);

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

        ethread_init(th, _recv_all, c);

        usleep(100 * 1000);

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

static int test_unbuffered_strs_trySend()
{
    echan c; int ret; ethread_t th; void* thread_ret; char num[24];

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_str, c);

        usleep(100 * 1000);

        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, true);

        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, false);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_all, c);

        usleep(100 * 1000);

        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, true);

        ret = echan_trySendS(c, llstr_r(0, num));
        eexpect_eq(ret, false);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_unbuffered_strs_timeSend()
{
    echan c; int ret; ethread_t th; void* thread_ret; char num[24]; i64 ticker;

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_str, c);

        usleep(100 * 1000);

        e_ticker_ms(&ticker);
        ret = echan_timeSendS(c, llstr_r(0, num), 100);
        eexpect_eq(ret, true);
        eexpect_le(e_ticker_ms(&ticker), 10);

        ret = echan_timeSendS(c, llstr_r(0, num), 100);
        eexpect_eq(ret, false);
        eexpect_ge(e_ticker_ms(&ticker), 50);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _recv_all, c);

        usleep(100 * 1000);

        e_ticker_ms(&ticker);
        ret = echan_timeSendS(c, llstr_r(0, num), 100);
        eexpect_eq(ret, true);
        eexpect_le(e_ticker_ms(&ticker), 10);

        ret = echan_timeSendS(c, llstr_r(0, num), 100);
        eexpect_eq(ret, false);
        eexpect_ge(e_ticker_ms(&ticker), 50);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}

static void* _send_sig(void* d)
{
    echan c = d; char num[24];

    usleep(wait_ms * 1000);

    bool ret = echan_sendS(c, llstr_r(0, num));

    return ret ? (void*)1 : 0;
}

static int test_unbuffered_strs_tryRecv()
{
    echan c; ethread_t th; void* thread_ret; estr s; evar strs;

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _send_sig, c);

        usleep(100 * 1000);

        s = echan_tryRecvS(c);
        eexpect_str(s, "0");
        estr_free(s);

        s = echan_tryRecvS(c);
        eexpect_ptr(s, 0);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _send_sig, c);

        usleep(100 * 1000);

        strs = echan_tryRecvAll(c);
        eexpect_eq(strs.type, E_STR);
        eexpect_eq(strs.cnt , 1);
        eexpect_str(evar_iValS(strs, 0), "0");
        evar_free(strs);

        strs = echan_tryRecvAll(c);
        eexpect_eq(strs.type, E_NAV);
        eexpect_eq(strs.cnt , 0);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    return ETEST_OK;
}

static int test_unbuffered_strs_timeRecv()
{
    echan c; ethread_t th; void* thread_ret; estr s; evar strs; i64 ticker;

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _send_sig, c);

        usleep(100 * 1000);

        e_ticker_ms(&ticker);
        s = echan_timeRecvS(c, 100);
        eexpect_str(s, "0");
        eexpect_le(e_ticker_ms(&ticker), 10);
        estr_free(s);

        s = echan_timeRecvS(c, 100);
        eexpect_ptr(s, 0);
        eexpect_ge(e_ticker_ms(&ticker), 40);

        ethread_join_ex(th, thread_ret);
        eexpect_ptr(thread_ret, (void*)1);

        echan_free(c);
    }

    {
        c = echan_new(E_STR, 0);

        wait_ms  = 0;
        recv_cnt = 1;

        ethread_init(th, _send_sig, c);

        usleep(100 * 1000);

        e_ticker_ms(&ticker);
        strs = echan_timeRecvAll(c, 100);
        eexpect_eq(strs.type, E_STR);
        eexpect_eq(strs.cnt , 1);
        eexpect_str(evar_iValS(strs, 0), "0");
        eexpect_le(e_ticker_ms(&ticker), 10);
        evar_free(strs);

        strs = echan_timeRecvAll(c, 100);
        eexpect_eq(strs.type, E_NAV);
        eexpect_eq(strs.cnt , 0);
        eexpect_ge(e_ticker_ms(&ticker), 40);

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
    ETEST_RUN( test_unbuffered_strs_trySend() );
    ETEST_RUN( test_unbuffered_strs_timeSend() );

    ETEST_RUN( test_unbuffered_strs_tryRecv() );
    ETEST_RUN( test_unbuffered_strs_timeRecv() );


    return ETEST_OK;
}

