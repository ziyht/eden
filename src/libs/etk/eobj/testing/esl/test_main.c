#include <stdio.h>
#include <inttypes.h>

#include "test_main.h"

int main()
{
#if 1
    esl_basic_test();
#else
    esl_performance_test();
#endif
}

void esl_basic_test()
{
    esln itr, tmp;

    esl sl = esl_new(ESL_DES);

    esl_addP(sl, 1, "1");
    esl_addP(sl, 2, "2");
    esl_addP(sl, 8, "8");
    esl_addP(sl, 3, "3");
    esl_addP(sl, 7, "7");
    esl_addP(sl, 5, "5");
    esl_addP(sl, 1, "1 2");

    esl_itr2(sl, itr, tmp)
    {
        printf("%"PRIu64" : %s\n", itr->score, (cstr)itr->obj); fflush(stdout);
    }

    printf("\n------- free head --------\n");
    esl_freeH(sl);
    esl_itr2(sl, itr, tmp)
    {
        printf("%"PRIu64" : %s\n", itr->score, (cstr)itr->obj); fflush(stdout);
    }

    printf("\n------- free tail --------\n");
    esl_freeT(sl);
    esl_itr2(sl, itr, tmp)
    {
        printf("%"PRIu64" : %s\n", itr->score, (cstr)itr->obj); fflush(stdout);
    }

    esl_free(sl);
}
