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

static cstr big_src;
static int  big_cnt;

static void big_src_init(int cnt)
{
    if(big_src)
        efree(big_src);

    big_src = emalloc(cnt);
    big_cnt = cnt;

    for(int i = 0; i < cnt; i++)
    {
        big_src[i] = '0' + i % 37;
    }
}
static void big_src_free()
{
    if(big_src)
        efree(big_src);
}

static int lz4f_encb_decb()
{
    constr* sp = srcs;
    estr enc = 0;
    estr dec = 0;
    i64  ret;

    for(; *sp; sp++)
    {
        uint slen = strlen(*sp);

        ret = elz4f_encb(*sp, slen, &enc);
        eexpect_1(ret > 0);

        ret = elz4f_decb(enc, estr_len(enc), &dec);
        eexpect_1(ret > 0);

        eexpect_raw(*sp, dec, estr_len(dec));

        estr_clear(enc);
        estr_clear(dec);
    }

    estr_free(enc);
    estr_free(dec);

    return ETEST_OK;
}

static int lz4f_encb2b_decb2b()
{
    constr* sp = srcs;
    char enc[1024];
    char dec[1024];
    i64  ret;

    for(; *sp; sp++)
    {
        uint slen = strlen(*sp);

        ret = elz4f_encb2b(*sp, slen, enc, 1024);
        eexpect_1(ret > 0);

        ret = elz4f_decb2b(enc, ret, dec, 1024);
        eexpect_1(ret > 0);

        eexpect_raw(*sp, dec, ret);
    }

    //! big src
    {
        cstr enc = emalloc(elz4f_bound(big_cnt));
        estr dec = emalloc(big_cnt);

        ret = elz4f_encb2b(big_src, big_cnt, enc, elz4f_bound(big_cnt));
        eexpect_1(ret > 0);

        ret = elz4f_decb2b(enc, ret, dec, big_cnt);
        eexpect_1(ret > 0);

        eexpect_raw(big_src, dec, big_cnt);

        efree(enc);
        efree(dec);
    }

    return ETEST_OK;
}

static int lz4f_handle()
{
    elz4f h = elz4f_encNew(0, 0);

    eunexpc_ptr(h, 0);

    //! do encode
    estr src = 0;
    constr* sp = srcs;
    i64 ret;
    for(; *sp; sp++)
    {
        uint slen = strlen(*sp);

        ret = elz4f_encNext(h, *sp, slen);
        eexpect_ge(ret, 0);

        estr_catB(src, *sp, slen);
    }
    ret = elz4f_encEnd(h);
    eexpect_gt(ret, 0);

    //! do decode
    estr enc = elz4f_encData(h);
    eunexpc_ptr(enc, 0);
    eexpect_gt(estr_len(enc), 0);

    eexpect_gt(elz4f_decBegin(h, enc, estr_len(enc)), 0);

    do{
        ret = elz4f_decNext(h);
        eexpect_ge(ret, 0);
    }while(ret);

    estr dec = elz4f_decData(h);

    eexpect_raw(src, dec, estr_len(dec));

    estr_free(src);

    elz4f_free(h);

    return ETEST_OK;
}

int u1_lz4f(int argc, char* argv[])
{
    (void)argc; (void)argv;

    big_src_init(100 * 1024 * 1024);

    ETEST_RUN( lz4f_encb_decb() );
    ETEST_RUN( lz4f_encb2b_decb2b() );
    ETEST_RUN( lz4f_handle() );

    big_src_free();

    return ETEST_OK;
}

