/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

static int test_basic_case1()
{
    eexpect_num(1, 1);      // passed

    return ETEST_OK;
}



int test_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_basic_case1() );

    return ETEST_OK;
}

