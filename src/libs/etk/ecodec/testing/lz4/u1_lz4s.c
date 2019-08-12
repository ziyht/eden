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

static int lz4s_handle()
{
    elz4s h = elz4s_encNew(0, 0);

    eunexpc_ptr(h, 0);

    //! do encode
    constr* sp = srcs;
    i64 ret;
    for(; *sp; sp++)
    {
        uint slen = strlen(*sp);

        ret = elz4s_encNext(h, *sp, slen);
        eexpect_ge(ret, 0);
    }

    //! do decode
    estr enc = elz4s_encData(h);
    eunexpc_ptr(enc, 0);
    eexpect_gt(estr_len(enc), 0);

    eexpect_gt(elz4s_decBegin(h, enc, (uint)estr_len(enc)), 0);

    int i = 0;
    do{
        ret = elz4s_decNext(h);

        if(!ret)
            break;

        estr dec = elz4s_decData(h);

        eexpect_raw(dec, srcs[i++], estr_len(dec));

        elz4s_decClear(h);
    }while(ret);

    elz4s_free(h);

    return ETEST_OK;
}


int u1_lz4s(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( lz4s_handle() );

    return ETEST_OK;
}

