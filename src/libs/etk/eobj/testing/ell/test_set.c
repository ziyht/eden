#include "etest.h"

#include "ell.h"


int set_all_type_test(ell l, uint idx)
{
    char _buf_zore[128] = {0,};

    eexpect_num(ell_size(l) > idx , 1);

    ell_isetI(l, idx, 10);
    eexpect_num(ell_ilen(l, idx), 0);
    eexpect_num(ell_ivalI(l, idx), 10);
    eexpect_num(ell_itype(l, idx), ENUM);

    ell_isetF(l, idx, 23.4);
    eexpect_num(ell_ilen(l, idx), 0);
    eexpect_num(ell_ivalF(l, idx), 23.4);
    eexpect_num(ell_itype(l, idx), ENUM);

    ell_isetP(l, idx, (cptr)30);
    eexpect_num(ell_ilen(l, idx), 0);
    eexpect_ptr(ell_ivalP(l, idx), (cptr)30);
    eexpect_num(ell_itype(l, idx), EPTR);

    ell_isetS(l, idx, "40");
    eexpect_num(ell_ilen(l, idx), 2);
    eexpect_str(ell_ivalS(l, idx), "40");
    eexpect_num(ell_itype(l, idx), ESTR);

    ell_isetR(l, idx, 50);
    eexpect_num(ell_ilen(l, idx), 50);
    eexpect_raw(ell_ivalR(l, idx), _buf_zore, ell_ilen(l, idx));
    eexpect_num(ell_itype(l, idx), ERAW);

    return ETEST_OK;
}

int test_convert_i()
{
    ell l = ell_new();

    ell_appdI(l, 1);
    ell_appdF(l, 2.0);
    ell_appdP(l, (cptr)3);
    ell_appdS(l, "4");
    ell_appdR(l, 5);

    ETEST_RUN(set_all_type_test(l, 0));
    ETEST_RUN(set_all_type_test(l, 1));
    ETEST_RUN(set_all_type_test(l, 2));
    ETEST_RUN(set_all_type_test(l, 3));
    ETEST_RUN(set_all_type_test(l, 4));

    ell_free(l);

    return ETEST_OK;
}

int test_set(int argc, char* argv[])
{
    ETEST_RUN( test_convert_i() );

    return ETEST_OK;
}
