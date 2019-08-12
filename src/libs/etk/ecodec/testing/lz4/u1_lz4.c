/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "elz4.h"

static constr srcs[] =
{
    "1234567890",
    "1111111111",
    "1212121212",
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    0,
};

static int lz4_encb_decb()
{
    constr* sp = srcs;
    estr enc = 0;
    estr dec = 0;
    i64  ret;

    for(; *sp; sp++)
    {
        uint slen = strlen(*sp);

        ret = elz4_encb(*sp, slen, &enc);
        eexpect_1(ret > 0);

        ret = elz4_decb(enc, (uint)estr_len(enc), &dec);
        eexpect_1(ret > 0);

        eexpect_raw(*sp, dec, (uint)estr_len(dec));

        estr_clear(enc);
        estr_clear(dec);
    }

    estr_free(enc);
    estr_free(dec);

    return ETEST_OK;
}

static int lz4_encb2b_decb2b()
{
    constr* sp = srcs;

    char enc[1024];
    char dec[1024];
    int  ret;

    for(; *sp; sp++)
    {
        uint slen = strlen(*sp);

        ret = elz4_encb2b(*sp, slen, enc, 1024);
        eexpect_1(ret > 0);

        ret = elz4_decb2b(enc, ret , dec, 1024);
        eexpect_1(ret > 0);

        eexpect_raw(*sp, dec, ret);
    }

    return ETEST_OK;
}

int u1_lz4(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( lz4_encb_decb() );
    ETEST_RUN( lz4_encb2b_decb2b() );

    return ETEST_OK;
}

