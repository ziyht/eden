/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ell.h"

static int test_sort_case1()
{
    int i;

    ell l = ell_new();

    ell_appdI(l,  0);
    ell_appdI(l,  1);
    ell_appdI(l,  2);
    ell_appdI(l,  3);
    ell_appdI(l,  4);
    ell_appdI(l,  5);
    ell_appdI(l,  6);
    ell_appdI(l,  7);
    ell_appdI(l,  8);
    ell_appdI(l,  9);

    ell_sort(l, ELL_VALI_DES);
    i = 9;
    ell_foreach_s(l, itr)
    {
        eexpect_num(eobj_valI(itr), i--);
    }

    ell_sort(l, ELL_VALI_ACS);
    i = 0;
    ell_foreach_s(l, itr)
    {
        eexpect_num(eobj_valI(itr), i++);
    }

    return ETEST_OK;
}


int test_sort(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_sort_case1() );

    return ETEST_OK;
}

