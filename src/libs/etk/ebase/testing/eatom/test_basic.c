/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eatom.h"
#include "ethread.h"

static int thread_cnt = 4;
static ethread_t* thread_arr;

void init_thread()
{
    thread_arr = ecalloc(1, sizeof(ethread_t*) * thread_cnt);

    for(int i = 0; i < thread_cnt; i++)
    {

    }

}


static int test_get()
{
    u8  v8  = 3;
    u16 v16 = 3;
    u32 v32 = 3;
    u64 v64 = 3;

    eexpect_eq(eatom_get8( v8 ), 3);
    eexpect_eq(eatom_get16(v16), 3);
    eexpect_eq(eatom_get32(v32), 3);
    eexpect_eq(eatom_get64(v64), 3);

    return ETEST_OK;
}

static int test_set()
{
    u8  v8  = 3;
    u16 v16 = 3;
    u32 v32 = 3;
    u64 v64 = 3;

    u8  v8_2  = eatom_set8( v8, 4);
    u16 v16_2 = eatom_set16(v16, 4);
    u32 v32_2 = eatom_set32(v32, 4);
    u64 v64_2 = eatom_set64(v64, 4);

    eexpect_eq(eatom_get8(  v8_2), 3);
    eexpect_eq(eatom_get16(v16_2), 3);
    eexpect_eq(eatom_get32(v32_2), 3);
    eexpect_eq(eatom_get64(v64_2), 3);

    eexpect_eq(eatom_get8(  v8), 4);
    eexpect_eq(eatom_get16(v16), 4);
    eexpect_eq(eatom_get32(v32), 4);
    eexpect_eq(eatom_get64(v64), 4);

    return ETEST_OK;
}

static int test_add()
{
    u32 v32 = 3;
    u64 v64 = 3;

    u32 v32_2 = eatom_add32(v32, 1);
    u64 v64_2 = eatom_add64(v64, 1);

    eexpect_eq(eatom_get32(v32_2), 4);
    eexpect_eq(eatom_get64(v64_2), 4);

    eexpect_eq(eatom_get32(v32), 4);
    eexpect_eq(eatom_get64(v64), 4);

    return ETEST_OK;
}

static int test_fadd()
{
    u32 v32 = 3;
    u64 v64 = 3;

    u32 v32_2 = eatom_fadd32(v32, 1);
    u64 v64_2 = eatom_fadd64(v64, 1);

    eexpect_eq(eatom_get32(v32_2), 3);
    eexpect_eq(eatom_get64(v64_2), 3);

    eexpect_eq(eatom_get32(v32), 4);
    eexpect_eq(eatom_get64(v64), 4);

    return ETEST_OK;
}

static int test_sub()
{
    u32 v32 = 3;
    u64 v64 = 3;

    u32 v32_2 = eatom_sub32(v32, 1);
    u64 v64_2 = eatom_sub64(v64, 1);

    eexpect_eq(eatom_get32(v32_2), 2);
    eexpect_eq(eatom_get64(v64_2), 2);

    eexpect_eq(eatom_get32(v32), 2);
    eexpect_eq(eatom_get64(v64), 2);

    return ETEST_OK;
}

static int test_cas()
{
    u16 v16 = 3;
    u32 v32 = 3;
    u64 v64 = 3;

    {
        u16 v16_2 = eatom_cas16(v16, 3, 4);
        u32 v32_2 = eatom_cas32(v32, 3, 4);
        u64 v64_2 = eatom_cas64(v64, 3, 4);

        eexpect_eq(eatom_get16(v16_2), 3);
        eexpect_eq(eatom_get32(v32_2), 3);
        eexpect_eq(eatom_get64(v64_2), 3);

        eexpect_eq(eatom_get16(v16), 4);
        eexpect_eq(eatom_get32(v32), 4);
        eexpect_eq(eatom_get64(v64), 4);
    }

    {
        u16 v16_2 = eatom_cas16(v16, 3, 5);
        u32 v32_2 = eatom_cas32(v32, 3, 5);
        u64 v64_2 = eatom_cas64(v64, 3, 5);

        eexpect_eq(eatom_get16(v16_2), 4);
        eexpect_eq(eatom_get32(v32_2), 4);
        eexpect_eq(eatom_get64(v64_2), 4);

        eexpect_eq(eatom_get16(v16), 4);
        eexpect_eq(eatom_get32(v32), 4);
        eexpect_eq(eatom_get64(v64), 4);
    }

    return ETEST_OK;
}

int test_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_get() );
    ETEST_RUN( test_set() );
    ETEST_RUN( test_add() );
    ETEST_RUN( test_fadd() );
    ETEST_RUN( test_sub() );
    ETEST_RUN( test_cas() );

    return ETEST_OK;
}

