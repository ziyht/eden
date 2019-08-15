#include <stdio.h>

#include "eutils.h"
#include "etime.h"
#include "estr.h"
#include "etest.h"

static int estr_split_perf_test_fun(int cnt)
{
    char s[] = "|bcs|b|bdfh|sdjflkjkls|dfjkl|sdjkfl|sdf||||sd|sdfjks|df|||";

    cstr* split1 = esplt_new(12, 1, 0);
    cstr* split2 = esplt_new(12, 1, 0);

    esplt_splitS(split1, s, "||");
    esplt_splitS(split2, s, "|");

    esplt_show(split1, -1);
    esplt_show(split2, -1);

    i64 t = e_nowms();
    for(int i = 0; i < cnt; i++)
    {
        esplt_splitS(split1, s, "||");
        esplt_splitS(split2, s, "|");

    }
    esplt_free(split1);
    esplt_free(split2);
    printf("split2 cost: %9"PRIi64" ms \n", e_nowms() - t);

    return ETEST_OK;
}


int estr_split_perf_test()
{
    ETEST_RUN(estr_split_perf_test_fun(1000000)); // 1000 000 => 360ms

    return ETEST_OK;
}

int test_perf(int argc, char* argv[])
{
    ETEST_RUN(estr_split_perf_test());

    return ETEST_OK;
}
