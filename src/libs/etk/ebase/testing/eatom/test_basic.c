/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eatom.h"

static int test_get()
{
    eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };

    eval v8  = init;
    eval v16 = init;
    eval v32 = init;
    eval v64 = init;

    eexpect_eq(eatom_get8( v8.i8  ), init.i8);
    eexpect_eq(eatom_get16(v16.i16), init.i16);
    eexpect_eq(eatom_get32(v32.i32), init.i32);
    eexpect_eq(eatom_get64(v64.i16), init.i64);

    return ETEST_OK;
}

static int test_set()
{
    eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
    eval desi = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

    eval v8  = init, v8_2;
    eval v16 = init, v16_2;
    eval v32 = init, v32_2;
    eval v64 = init, v64_2;

    v8_2.i8   = eatom_set8( v8 .i8 , desi.i8);
    v16_2.i16 = eatom_set16(v16.i16, desi.i16);
    v32_2.i32 = eatom_set32(v32.i32, desi.i32);
    v64_2.i64 = eatom_set64(v64.i64, desi.i64);

    eexpect_eq(eatom_get8(  v8_2.i8 ), init.i8);
    eexpect_eq(eatom_get16(v16_2.i16), init.i16);
    eexpect_eq(eatom_get32(v32_2.i32), init.i32);
    eexpect_eq(eatom_get64(v64_2.i64), init.i64);

    eexpect_eq(eatom_get8(  v8.i8 ), desi.i8);
    eexpect_eq(eatom_get16(v16.i16), desi.i16);
    eexpect_eq(eatom_get32(v32.i32), desi.i32);
    eexpect_eq(eatom_get64(v64.i64), desi.i64);

    return ETEST_OK;
}

static int test_add()
{
    eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
    eval add  = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

    eval v8  = init, v8_2;
    eval v16 = init, v16_2;
    eval v32 = init, v32_2;
    eval v64 = init, v64_2;

    v8_2.i8   = eatom_add8( v8 .i8 , add.i8);
    v16_2.i16 = eatom_add16(v16.i16, add.i16);
    v32_2.i32 = eatom_add32(v32.i32, add.i32);
    v64_2.i64 = eatom_add64(v64.i64, add.i64);

    eexpect_eq(eatom_get8(  v8_2.i8 ), init.i8  + add.i8);
    eexpect_eq(eatom_get16(v16_2.i16), init.i16 + add.i16);
    eexpect_eq(eatom_get32(v32_2.i32), init.i32 + add.i32);
    eexpect_eq(eatom_get64(v64_2.i64), init.i64 + add.i64);

    eexpect_eq(eatom_get8(  v8.i8 ), init.i8  + add.i8);
    eexpect_eq(eatom_get16(v16.i16), init.i16 + add.i16);
    eexpect_eq(eatom_get32(v32.i32), init.i32 + add.i32);
    eexpect_eq(eatom_get64(v64.i64), init.i64 + add.i64);

    return ETEST_OK;
}

static int test_fadd()
{
    eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
    eval add  = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

    eval v8  = init, v8_2;
    eval v16 = init, v16_2;
    eval v32 = init, v32_2;
    eval v64 = init, v64_2;

    v8_2.i8   = eatom_fadd8( v8 .i8 , add.i8);
    v16_2.i16 = eatom_fadd16(v16.i16, add.i16);
    v32_2.i32 = eatom_fadd32(v32.i32, add.i32);
    v64_2.i64 = eatom_fadd64(v64.i64, add.i64);

    eexpect_eq(eatom_get8(  v8_2.i8 ), init.i8 );
    eexpect_eq(eatom_get16(v16_2.i16), init.i16);
    eexpect_eq(eatom_get32(v32_2.i32), init.i32);
    eexpect_eq(eatom_get64(v64_2.i64), init.i64);

    eexpect_eq(eatom_get8(  v8.i8 ), init.i8  + add.i8);
    eexpect_eq(eatom_get16(v16.i16), init.i16 + add.i16);
    eexpect_eq(eatom_get32(v32.i32), init.i32 + add.i32);
    eexpect_eq(eatom_get64(v64.i64), init.i64 + add.i64);

    return ETEST_OK;
}

static int test_sub()
{
    eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
    eval sub  = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

    eval v8  = init, v8_2;
    eval v16 = init, v16_2;
    eval v32 = init, v32_2;
    eval v64 = init, v64_2;

    v8_2.i8   = eatom_sub8( v8 .i8 , sub.i8);
    v16_2.i16 = eatom_sub16(v16.i16, sub.i16);
    v32_2.i32 = eatom_sub32(v32.i32, sub.i32);
    v64_2.i64 = eatom_sub64(v64.i64, sub.i64);

    eexpect_eq(eatom_get8(  v8_2.i8 ), init.i8  - sub.i8);
    eexpect_eq(eatom_get16(v16_2.i16), init.i16 - sub.i16);
    eexpect_eq(eatom_get32(v32_2.i32), init.i32 - sub.i32);
    eexpect_eq(eatom_get64(v64_2.i64), init.i64 - sub.i64);

    eexpect_eq(eatom_get8(  v8.i8 ), init.i8  - sub.i8);
    eexpect_eq(eatom_get16(v16.i16), init.i16 - sub.i16);
    eexpect_eq(eatom_get32(v32.i32), init.i32 - sub.i32);
    eexpect_eq(eatom_get64(v64.i64), init.i64 - sub.i64);

    return ETEST_OK;
}

static int test_fsub()
{
    eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
    eval sub  = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

    eval v8  = init, v8_2;
    eval v16 = init, v16_2;
    eval v32 = init, v32_2;
    eval v64 = init, v64_2;

    v8_2.i8   = eatom_fsub8( v8 .i8 , sub.i8);
    v16_2.i16 = eatom_fsub16(v16.i16, sub.i16);
    v32_2.i32 = eatom_fsub32(v32.i32, sub.i32);
    v64_2.i64 = eatom_fsub64(v64.i64, sub.i64);

    eexpect_eq(eatom_get8(  v8_2.i8 ), init.i8 );
    eexpect_eq(eatom_get16(v16_2.i16), init.i16);
    eexpect_eq(eatom_get32(v32_2.i32), init.i32);
    eexpect_eq(eatom_get64(v64_2.i64), init.i64);

    eexpect_eq(eatom_get8(  v8.i8 ), init.i8  - sub.i8);
    eexpect_eq(eatom_get16(v16.i16), init.i16 - sub.i16);
    eexpect_eq(eatom_get32(v32.i32), init.i32 - sub.i32);
    eexpect_eq(eatom_get64(v64.i64), init.i64 - sub.i64);

    return ETEST_OK;
}

static int test_cas()
{
    {
        eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
        eval expe = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
        eval disi = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

        eval v8  = init, v8_2;
        eval v16 = init, v16_2;
        eval v32 = init, v32_2;
        eval v64 = init, v64_2;

        v8_2.i8   = eatom_cas8( v8 .i8 , expe.i8 , disi.i8);
        v16_2.i16 = eatom_cas16(v16.i16, expe.i16, disi.i16);
        v32_2.i32 = eatom_cas32(v32.i32, expe.i32, disi.i32);
        v64_2.i64 = eatom_cas64(v64.i64, expe.i64, disi.i64);

        eexpect_eq(eatom_get8(  v8_2.i8 ), init.i8 );
        eexpect_eq(eatom_get16(v16_2.i16), init.i16);
        eexpect_eq(eatom_get32(v32_2.i32), init.i32);
        eexpect_eq(eatom_get64(v64_2.i64), init.i64);

        eexpect_eq(eatom_get8(  v8.i8 ), disi.i8 );
        eexpect_eq(eatom_get16(v16.i16), disi.i16);
        eexpect_eq(eatom_get32(v32.i32), disi.i32);
        eexpect_eq(eatom_get64(v64.i64), disi.i64);
    }

    {
        eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
        eval expe = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };
        eval disi = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

        eval v8  = init, v8_2;
        eval v16 = init, v16_2;
        eval v32 = init, v32_2;
        eval v64 = init, v64_2;

        v8_2.i8   = eatom_cas8( v8 .i8 , expe.i8 , disi.i8);
        v16_2.i16 = eatom_cas16(v16.i16, expe.i16, disi.i16);
        v32_2.i32 = eatom_cas32(v32.i32, expe.i32, disi.i32);
        v64_2.i64 = eatom_cas64(v64.i64, expe.i64, disi.i64);

        eexpect_eq(eatom_get8(  v8_2.i8 ), init.i8 );
        eexpect_eq(eatom_get16(v16_2.i16), init.i16);
        eexpect_eq(eatom_get32(v32_2.i32), init.i32);
        eexpect_eq(eatom_get64(v64_2.i64), init.i64);

        eexpect_eq(eatom_get8(  v8.i8 ), init.i8 );
        eexpect_eq(eatom_get16(v16.i16), init.i16);
        eexpect_eq(eatom_get32(v32.i32), init.i32);
        eexpect_eq(eatom_get64(v64.i64), init.i64);
    }

    return ETEST_OK;
}

static int test_cae()
{
    {
        eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
        eval expe = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
        eval disi = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

        eval v8  = init, v8_e  = expe;
        eval v16 = init, v16_e = expe;
        eval v32 = init, v32_e = expe;
        eval v64 = init, v64_e = expe;

        bool ret_8  = eatom_cae8( v8 .i8 , v8_e.i8  , disi.i8);
        bool ret_16 = eatom_cae16(v16.i16, v16_e.i16, disi.i16);
        bool ret_32 = eatom_cae32(v32.i32, v32_e.i32, disi.i32);
        bool ret_64 = eatom_cae64(v64.i64, v64_e.i64, disi.i64);

        eexpect_1(ret_8);
        eexpect_1(ret_16);
        eexpect_1(ret_32);
        eexpect_1(ret_64);

        eexpect_eq(eatom_get8(  v8_e.i8 ), init.i8 );
        eexpect_eq(eatom_get16(v16_e.i16), init.i16);
        eexpect_eq(eatom_get32(v32_e.i32), init.i32);
        eexpect_eq(eatom_get64(v64_e.i64), init.i64);

        eexpect_eq(eatom_get8(  v8.i8 ), disi.i8 );
        eexpect_eq(eatom_get16(v16.i16), disi.i16);
        eexpect_eq(eatom_get32(v32.i32), disi.i32);
        eexpect_eq(eatom_get64(v64.i64), disi.i64);
    }

    {
        eval init = { .i8a = {3, 3, 3, 3, 3, 3, 3, 3} };
        eval expe = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };
        eval disi = { .i8a = {4, 4, 4, 4, 4, 4, 4, 4} };

        eval v8  = init, v8_e  = expe;
        eval v16 = init, v16_e = expe;
        eval v32 = init, v32_e = expe;
        eval v64 = init, v64_e = expe;

        bool ret_8  = eatom_cae8( v8 .i8 , v8_e.i8  , disi.i8);
        bool ret_16 = eatom_cae16(v16.i16, v16_e.i16, disi.i16);
        bool ret_32 = eatom_cae32(v32.i32, v32_e.i32, disi.i32);
        bool ret_64 = eatom_cae64(v64.i64, v64_e.i64, disi.i64);

        eexpect_0(ret_8);
        eexpect_0(ret_16);
        eexpect_0(ret_32);
        eexpect_0(ret_64);

        eexpect_eq(eatom_get8(  v8_e.i8 ), init.i8 );
        eexpect_eq(eatom_get16(v16_e.i16), init.i16);
        eexpect_eq(eatom_get32(v32_e.i32), init.i32);
        eexpect_eq(eatom_get64(v64_e.i64), init.i64);

        eexpect_eq(eatom_get8(  v8.i8 ), init.i8 );
        eexpect_eq(eatom_get16(v16.i16), init.i16);
        eexpect_eq(eatom_get32(v32.i32), init.i32);
        eexpect_eq(eatom_get64(v64.i64), init.i64);
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
    ETEST_RUN( test_fsub() );
    ETEST_RUN( test_cas() );
    ETEST_RUN( test_cae() );

    return ETEST_OK;
}

