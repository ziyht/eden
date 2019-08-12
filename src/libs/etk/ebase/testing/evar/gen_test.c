/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "evar.h"

static int test_stack()
{
    {
        evar var = EVAR_NAV;

        eexpect_eq( evar_cnt  (var), 0 );
        eexpect_eq( evar_esize(var), 0 );
        eexpect_eq( evar_type (var), E_NAV );
    }

    {
        evar var = EVAR_I8(123);

        eexpect_eq( evar_cnt  (var), 1 );
        eexpect_eq( evar_esize(var), 1 );
        eexpect_eq( evar_type (var), E_I8 );

        eexpect_eq( evar_cnt  (evar_i    (var, 0)), 1);
        eexpect_eq( evar_esize(evar_i    (var, 0)), 1);
        eexpect_eq( evar_type (evar_i    (var, 0)), E_I8);
        eexpect_eq( evar_iVal(var, 0).i8, 123);

        eexpect_eq( evar_cnt  (evar_i    (var, 1)), 0);
        eexpect_eq( evar_esize(evar_i    (var, 1)), 0);
        eexpect_eq( evar_type (evar_i    (var, 1)), E_NAV);
        eexpect_eq( evar_iVal(var, 1).i8, 0);
    }

    {
        evar var = EVAR_I16(123);

        eexpect_eq( evar_cnt  (var), 1 );
        eexpect_eq( evar_esize(var), 2 );
        eexpect_eq( evar_type (var), E_I16 );

        eexpect_eq( evar_cnt  (evar_i    (var, 0)), 1);
        eexpect_eq( evar_esize(evar_i    (var, 0)), 2);
        eexpect_eq( evar_type (evar_i    (var, 0)), E_I16);
        eexpect_eq( evar_iVal(var, 0).i16, 123);

        eexpect_eq( evar_cnt  (evar_i    (var, 1)), 0);
        eexpect_eq( evar_esize(evar_i    (var, 1)), 0);
        eexpect_eq( evar_type (evar_i    (var, 1)), E_NAV);
        eexpect_eq( evar_iVal(var, 1).i16, 0);
    }

    {
        evar var = EVAR_I32(123);

        eexpect_eq( evar_cnt  (var), 1 );
        eexpect_eq( evar_esize(var), 4 );
        eexpect_eq( evar_type (var), E_I32 );

        eexpect_eq( evar_cnt  (evar_i    (var, 0)), 1);
        eexpect_eq( evar_esize(evar_i    (var, 0)), 4);
        eexpect_eq( evar_type (evar_i    (var, 0)), E_I32);
        eexpect_eq( evar_iVal(var, 0).i32, 123);

        eexpect_eq( evar_cnt  (evar_i    (var, 1)), 0);
        eexpect_eq( evar_esize(evar_i    (var, 1)), 0);
        eexpect_eq( evar_type (evar_i    (var, 1)), E_NAV);
        eexpect_eq( evar_iVal(var, 1).i32, 0);
    }

    {
        evar var = EVAR_RAW("user", 4);

        eexpect_eq( evar_cnt  (var), 1 );
        eexpect_eq( evar_esize(var), 4 );
        eexpect_eq( evar_type (var), E_RAW );

        eexpect_eq( evar_cnt  (evar_i    (var, 0)), 1);
        eexpect_eq( evar_esize(evar_i    (var, 0)), 4);
        eexpect_eq( evar_type (evar_i    (var, 0)), E_RAW);
        eexpect_raw( evar_iVal(var, 0).p, "user", 4);

        eexpect_eq( evar_cnt  (evar_i    (var, 1)), 0);
        eexpect_eq( evar_esize(evar_i    (var, 1)), 0);
        eexpect_eq( evar_type (evar_i    (var, 1)), E_NAV);
        eexpect_ptr( evar_iVal(var, 1).p, 0);

        var = evar_gen(E_RAW, 1, 0);

        evar_iSetV(var, 0, EVAR_RAW("user", 4));

        eexpect_eq( evar_cnt  (var), 1 );
        eexpect_eq( evar_esize(var), 8 );
        eexpect_eq( evar_type (var), E_RAW );

        eexpect_eq( evar_cnt  (evar_i    (var, 0)), 1);
        eexpect_eq( evar_esize(evar_i    (var, 0)), 8);
        eexpect_eq( evar_type (evar_i    (var, 0)), E_RAW);
        eexpect_raw( evar_iVal(var, 0).p, "user", 4);

        eexpect_eq( evar_cnt  (evar_i    (var, 1)), 0);
        eexpect_eq( evar_esize(evar_i    (var, 1)), 0);
        eexpect_eq( evar_type (evar_i    (var, 1)), E_NAV);
        eexpect_ptr( evar_iVal(var, 1).p, 0);

        evar_free(var);
    }

    {
        evar var = EVAR_USER("user", 4);

        eexpect_eq( evar_cnt  (var), 1 );
        eexpect_eq( evar_esize(var), 4 );
        eexpect_eq( evar_type (var), E_USER );

        eexpect_eq( evar_cnt  (evar_i    (var, 0)), 1);
        eexpect_eq( evar_esize(evar_i    (var, 0)), 4);
        eexpect_eq( evar_type (evar_i    (var, 0)), E_USER);
        eexpect_raw( evar_iVal(var, 0).p, "user", 4);

        eexpect_eq( evar_cnt  (evar_i    (var, 1)), 0);
        eexpect_eq( evar_esize(evar_i    (var, 1)), 0);
        eexpect_eq( evar_type (evar_i    (var, 1)), E_NAV);
        eexpect_ptr( evar_iVal(var, 1).p, 0);

        var = evar_gen(E_USER, 1, 4);

        evar_iSetV(var, 0, EVAR_USER("user", 4));

        eexpect_eq( evar_cnt  (var), 1 );
        eexpect_eq( evar_esize(var), 4 );
        eexpect_eq( evar_type (var), E_USER );

        eexpect_eq( evar_cnt  (evar_i    (var, 0)), 1);
        eexpect_eq( evar_esize(evar_i    (var, 0)), 4);
        eexpect_eq( evar_type (evar_i    (var, 0)), E_USER);
        eexpect_raw( evar_iVal(var, 0).p, "user", 4);

        eexpect_eq( evar_cnt  (evar_i    (var, 1)), 0);
        eexpect_eq( evar_esize(evar_i    (var, 1)), 0);
        eexpect_eq( evar_type (evar_i    (var, 1)), E_NAV);
        eexpect_ptr( evar_iVal(var, 1).p, 0);

        evar_free(var);
    }

    return ETEST_OK;
}

static int gen_auto_val()
{
    evar v; int i = 0;

    for(i = 1; i <= 2; i++)
    {
        v = evar_gen(E_I8, i, 0);
        eexpect_num(evar_type (v) , E_I8);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 1);
        evar_free(v);

        v = evar_gen(E_I16, i, 0);
        eexpect_num(evar_type (v), E_I16);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 2);
        evar_free(v);

        v = evar_gen(E_I32, i, 0);
        eexpect_num(evar_type (v), E_I32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        evar_free(v);

        v = evar_gen(E_I64, i, 0);
        eexpect_num(evar_type (v), E_I64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        evar_free(v);

        v = evar_gen(E_U8, i, 0);
        eexpect_num(evar_type (v), E_U8);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 1);
        evar_free(v);

        v = evar_gen(E_U16, i, 0);
        eexpect_num(evar_type (v), E_U16);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 2);
        evar_free(v);

        v = evar_gen(E_U32, i, 0);
        eexpect_num(evar_type (v), E_U32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        evar_free(v);

        v = evar_gen(E_U64, i, 0);
        eexpect_num(evar_type (v), E_U64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        evar_free(v);

        v = evar_gen(E_F32, i, 0);
        eexpect_num(evar_type (v), E_F32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        evar_free(v);

        v = evar_gen(E_F64, i, 0);
        eexpect_num(evar_type (v), E_F64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        evar_free(v);

    }

    return ETEST_OK;
}

static int gen_arr1()
{
    evar v;

    for(int i = 1; i < 8; i++)
    {
        v = evar_gen(E_I8, i, 0);
        eexpect_num(evar_type (v), E_I8);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 1);
        evar_free(v);
    }

    for(int i = 1; i <= 4; i++)
    {
        v = evar_gen(E_I16, i, 0);
        eexpect_num(evar_type (v), E_I16);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 2);
        evar_free(v);
    }

    for(int i = 1; i <= 2; i++)
    {
        v = evar_gen(E_I32, i, 0);
        eexpect_num(evar_type (v), E_I32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        evar_free(v);
    }

    for(int i = 1; i <= 1; i++)
    {
        v = evar_gen(E_I64, i, 0);
        eexpect_num(evar_type (v), E_I64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        evar_free(v);
    }

    for(int i = 1; i < 8; i++)
    {
        v = evar_gen(E_U8, i, 0);
        eexpect_num(evar_type (v), E_U8);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 1);
        evar_free(v);
    }

    for(int i = 1; i <= 4; i++)
    {
        v = evar_gen(E_U16, i, 0);
        eexpect_num(evar_type (v), E_U16);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 2);
        evar_free(v);
    }

    for(int i = 1; i <= 2; i++)
    {
        v = evar_gen(E_U32, i, 0);
        eexpect_num(evar_type (v), E_U32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        evar_free(v);
    }

    for(int i = 1; i <= 1; i++)
    {
        v = evar_gen(E_U64, i, 0);
        eexpect_num(evar_type (v), E_U64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        evar_free(v);
    }

    for(int i = 1; i <= 2; i++)
    {
        v = evar_gen(E_F32, i, 0);
        eexpect_num(evar_type (v), E_F32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        evar_free(v);
    }

    for(int i = 1; i <= 1; i++)
    {
        v = evar_gen(E_F64, i, 0);
        eexpect_num(evar_type (v), E_F64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        evar_free(v);
    }

    return ETEST_OK;
}

static int gen_arr2()
{
    evar v;

    for(int i = 1; i < 8; i++)
    {
        v = evar_gen(E_I8, i, 0);
        eexpect_num(evar_type (v), E_I8);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 1);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_I16, i, 0);
        eexpect_num(evar_type (v), E_I16);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 2);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_I32, i, 0);
        eexpect_num(evar_type (v), E_I32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_I64, i, 0);
        eexpect_num(evar_type (v), E_I64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_U8, i, 0);
        eexpect_num(evar_type (v), E_U8);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 1);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_U16, i, 0);
        eexpect_num(evar_type (v), E_U16);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 2);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_U32, i, 0);
        eexpect_num(evar_type (v), E_U32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_U64, i, 0);
        eexpect_num(evar_type (v), E_U64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_F32, i, 0);
        eexpect_num(evar_type (v), E_F32);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 4);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_F64, i, 0);
        eexpect_num(evar_type (v), E_F64);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 8);
        eexpect_num(evar_free (v), i);
        evar_free(v);

        v = evar_gen(E_USER, i, 16);
        eexpect_num(evar_type (v), E_USER);
        eexpect_num(evar_cnt  (v), i);
        eexpect_num(evar_esize(v), 16);
        eexpect_num(evar_free (v), i);
    }

    return ETEST_OK;
}


int gen_test(int argc, char* argv[])
{
    ETEST_RUN( test_stack() );

    ETEST_RUN( gen_auto_val() );
    ETEST_RUN( gen_arr1() );
    ETEST_RUN( gen_arr2() );

    return ETEST_OK;
}

