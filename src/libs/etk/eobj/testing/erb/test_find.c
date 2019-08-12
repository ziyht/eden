#include <stdio.h>
#include <assert.h>

#include "etype.h"
#include "eobj.h"
#include "etest.h"

#include "test_main.h"


int erb_find_test()
{
    erb root = erb_new(EKEY_I); eobj s; char buf[32];


    for(i64 i = 0; i < 200; i++)
    {
        sprintf(buf, "%"PRId64"", i+100000000000);
        s = erb_addS(root, ekey_i(i), buf);
    }

    for(i64 i = 0; i < 200; i++)
    {
        s = erb_k(root, (ekey){i});

        assert(s);

        printf("key: %"PRId64" \t val: %s\n", eobj_keyI(s), eobj_valS(s)); fflush(stdout);
    }

    for(i64 i = 0; i < 200; i++)
    {
        printf("key: %"PRId64" \t val: %s\n", i, erb_kValS(root, ekey_i(i))); fflush(stdout);
    }

    erb_free(root);

    return ETEST_OK;
}

int test_find(int argc, char* argv[])
{
    erb_find_test();

    return ETEST_OK;
}
