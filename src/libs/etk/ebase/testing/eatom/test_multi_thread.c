/**
* this file is create by subsystem 'ETest' from EMake framework
*/



#include <etest.h>
#include "uv.h"
#include "eatom.h"

#include "ethread.h"

#define thread_cnt  4
#define scale       100000

static void start_threads_and_wait(void* (*cb)(void*d), void* d)
{
    ethread_t thrds[thread_cnt];

    for(int i = 0; i < thread_cnt; i++)
    {
        ethread_init(thrds[i], cb, d);
    }

    for(int i = 0; i < thread_cnt; i++)
    {
        ethread_join(thrds[i]);
    }
}


static void* _add8_cb (void* d) { for(int i = 0; i < scale; i++) eatom_add8 (*(i8* )d, 1);
                                  return 0; }
static void* _add16_cb(void* d) { for(int i = 0; i < scale; i++) eatom_add16(*(i16*)d, 1); return 0; }
static void* _add32_cb(void* d) { for(int i = 0; i < scale; i++) eatom_add32(*(i32*)d, 1); return 0; }
static void* _add64_cb(void* d) { for(int i = 0; i < scale; i++) eatom_add64(*(i64*)d, 1); return 0; }
static int test_multi_thread_add()
{
    {
        eval v = EVAL_0;
        eval e = EVAL_I8((i8)(scale * thread_cnt));
        start_threads_and_wait(_add8_cb, &v);
        eexpect_eq(v.i8, e.i8);
    }

    {
        eval v = EVAL_0;
        eval e = EVAL_I16((i16)(scale * thread_cnt));
        start_threads_and_wait(_add16_cb, &v);
        eexpect_eq(v.i16, e.i16);
    }

    {
        eval v = EVAL_0;
        eval e = EVAL_I32((i32)(scale * thread_cnt));
        start_threads_and_wait(_add32_cb, &v);
        eexpect_eq(v.i32, e.i32);
    }

    {
        eval v = EVAL_0;
        eval e = EVAL_I64((i64)(scale * thread_cnt));
        start_threads_and_wait(_add64_cb, &v);
        eexpect_eq(v.i64, e.i64);
    }

    return ETEST_OK;
}

static void* _sub8_cb (void* d) { for(int i = 0; i < scale; i++) eatom_sub8 (*(i8* )d, 1); return 0; }
static void* _sub16_cb(void* d) { for(int i = 0; i < scale; i++) eatom_sub16(*(i16*)d, 1); return 0; }
static void* _sub32_cb(void* d) { for(int i = 0; i < scale; i++) eatom_sub32(*(i32*)d, 1); return 0; }
static void* _sub64_cb(void* d) { for(int i = 0; i < scale; i++) eatom_sub64(*(i64*)d, 1); return 0; }
static int test_multi_thread_sub()
{
    {
        eval v = EVAL_0;
        eval e = EVAL_I8((i8)(-scale * thread_cnt));
        start_threads_and_wait(_sub8_cb, &v);
        eexpect_eq(v.i8, e.i8);
    }

    {
        eval v = EVAL_0;
        eval e = EVAL_I16((i16)(-scale * thread_cnt));
        start_threads_and_wait(_sub16_cb, &v);
        eexpect_eq(v.i16, e.i16);
    }

    {
        eval v = EVAL_0;
        eval e = EVAL_I32((i32)(-scale * thread_cnt));
        start_threads_and_wait(_sub32_cb, &v);
        eexpect_eq(v.i32, e.i32);
    }

    {
        eval v = EVAL_0;
        eval e = EVAL_I64((i64)(-scale * thread_cnt));
        start_threads_and_wait(_sub64_cb, &v);
        eexpect_eq(v.i64, e.i64);
    }

    return ETEST_OK;
}

int test_multi_thread(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_multi_thread_add() );
    ETEST_RUN( test_multi_thread_sub() );

    return ETEST_OK;
}

