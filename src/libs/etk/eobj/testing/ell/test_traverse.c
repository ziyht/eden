#include "etest.h"
#include "ell.h"

int traverse1(int scale)
{
    ell l; eobj itr; int i;

    l = ell_new();

    for(i = 0; i < scale; i++)
    {
        ell_appdI(l, i);
    }

    i = 0;
    ell_foreach(l, itr)
    {
        eexpect_num(EOBJ_VALI(itr), i);
        i++;
    }

    for(itr=ell_last(l); (itr); itr = ell_prev(itr))
    {
        i--;
        eexpect_num(EOBJ_VALI(itr), i);
    }

    ell_free(l);

    return ETEST_OK;
}



int test_traverse(int argc, char* argv[])
{
    ETEST_RUN(traverse1(1000));

    return ETEST_OK;
}
