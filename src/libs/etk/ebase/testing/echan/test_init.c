/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "echan.h"

int test_echan_new_buffered()
{
    echan chan = echan_new(E_I32, 4);
    eexpect_1(chan);
    echan_free(chan);

    return ETEST_OK;
}

int test_echan_new_unbuffered()
{
    echan chan = echan_new(E_I32, 0);
    eexpect_1(chan);
    echan_free(chan);

    return ETEST_OK;

}

int test_init(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_echan_new_buffered() );
    ETEST_RUN( test_echan_new_unbuffered() );

    return ETEST_OK;
}

