/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "evec.h"

static int test_limit_i8()
{
    bool ok;

    evec v = evec_new2(E_I8, 0, 10);

    for(int i = 0; i < 10; i++)
    {
        ok = evec_appdV(v, EVAR_I8(i));

        eexpect_eq(ok, true);
    }

    ok = evec_appdV(v, EVAR_I8(0));
    eexpect_eq(ok, false);

    evec_free(v);

    return ETEST_OK;
}

static int test_limit_i32()
{
    bool ok;

    evec v = evec_new2(E_I32, 0, 10);

    for(int i = 0; i < 10; i++)
    {
        ok = evec_appdV(v, EVAR_I32(i));

        eexpect_eq(ok, true);
    }

    ok = evec_appdV(v, EVAR_I32(0));
    eexpect_eq(ok, false);

    evec_free(v);

    return ETEST_OK;
}

static int test_limit_i64()
{
    bool ok;

    evec v = evec_new2(E_I64, 0, 10);

    for(int i = 0; i < 10; i++)
    {
        ok = evec_appdV(v, EVAR_I64(i));

        eexpect_eq(ok, true);
    }

    ok = evec_appdV(v, EVAR_I64(0));
    eexpect_eq(ok, false);

    evec_free(v);

    return ETEST_OK;
}

int test_limit(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_limit_i8() );
    ETEST_RUN( test_limit_i32() );
    ETEST_RUN( test_limit_i64() );

    return ETEST_OK;
}

