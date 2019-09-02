/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "evec.h"
#include "eutils.h"
#include "estr.h"

static int test_take_str_case1()
{
    bool ret; evar s;

    evec v = evec_new(E_STR);

    ret = evec_appdS(v, "0");
    eexpect_eq(ret, 1);

    ret = evec_appdV(v, EVAR_S("1"));
    eexpect_eq(ret, 1);

    ret = evec_appdV(v, EVAR_S("2"));
    eexpect_eq(ret, 1);

    s = evec_takeH(v);
    eexpect_eq(s.type, E_STR);
    eexpect_str(evar_iValS(s, 0), "0");
    evar_free(s);

    s = evec_takeT(v);
    eexpect_eq(s.type, E_STR);
    eexpect_str(evar_iValS(s, 0), "2");
    evar_free(s);

    evec_free(v);

    return ETEST_OK;
}

static int test_take_strs()
{
    bool ret; evar strs;

    evec v = evec_new(E_STR);

    ret = evec_appdS(v, "0");
    eexpect_eq(ret, 1);

    ret = evec_appdV(v, EVAR_S("1"));
    eexpect_eq(ret, 1);

    ret = evec_appdV(v, EVAR_S("0"));
    eexpect_eq(ret, 1);

    ret = evec_appdV(v, EVAR_S("1"));
    eexpect_eq(ret, 1);

    strs = evec_takeHs(v, 2);
    eexpect_eq(strs.type, E_STR);
    eexpect_eq(strs.cnt,  2);
    for(uint i = 0; i < strs.cnt; i++)
    {
        eexpect_str(evar_iValS(strs, i), llstr(i));
        evar_free(strs);
    }

    strs = evec_takeTs(v, 2);
    eexpect_eq(strs.type, E_STR);
    eexpect_eq(strs.cnt,  2);
    for(uint i = 0; i < strs.cnt; i++)
    {
        eexpect_str(evar_iValS(strs, i), llstr(i));
        evar_free(strs);
    }

    ret = evec_appdV(v, EVAR_S("0"));
    eexpect_eq(ret, 1);

    strs = evec_takeHs(v, 2);
    eexpect_eq(strs.type, E_STR);
    eexpect_eq(strs.cnt,  1);
    for(uint i = 0; i < strs.cnt; i++)
    {
        eexpect_str(evar_iValS(strs, i), llstr(i));
        evar_free(strs);
    }

    ret = evec_appdV(v, EVAR_S("0"));
    eexpect_eq(ret, 1);

    strs = evec_takeTs(v, 2);
    eexpect_eq(strs.type, E_STR);
    eexpect_eq(strs.cnt,  1);
    for(uint i = 0; i < strs.cnt; i++)
    {
        eexpect_str(evar_iValS(strs, i), llstr(i));
        evar_free(strs);
    }

    evec_free(v);

    return ETEST_OK;
}

int test_take_str(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_take_str_case1() );
    ETEST_RUN( test_take_strs() );

    return ETEST_OK;
}

