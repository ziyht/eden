#include <stdio.h>
#include <etest.h>

#include "estr.h"

static int estr_wrt_cat_W_test()
{
    estr s = 0;

    typedef struct {
        constr src; constr expect;
    } test;
    test tests[] = {

        {"word"   , "word"},
        {"word0"  , "word"},
        {"word "  , "word"},
        {"word\n" , "word"},
        {"word\t" , "word"},
        {" word"  , "word"},
        {"\nword" , "word"},
        {"\tword" , "word"},
        {"w w w"  , "w"},

        {"1word"  , ""},
        {"\0word" ,  ""},

        {0   , 0},

    };

    test* t = tests;

    for(int i = 0; ;i++, t++)
    {
        if(!t->src)
            break;

        estr_clear(s);
        estr_wrtW(s, t->src);
        eexpect_str(s, t->expect);

        estr_clear(s);
        estr_catW(s, t->src);
        eexpect_str(s, t->expect);
    }

    return ETEST_OK;
}

static int estr_wrt_cat_L_test()
{
    estr s = 0;

    typedef struct {
        constr src; constr expect;
    } test;
    test tests[] = {

        {"line"        , "line"},
        {"line\n"      , "line"},
        {"line\nline"  , "line"},

        {"\nline"      , ""},
        {"\0line"      , ""},

        {0   , 0},

    };

    test* t = tests;

    for(int i = 0; ;i++, t++)
    {
        if(!t->src)
            break;

        estr_clear(s);
        estr_wrtL(s, t->src);
        eexpect_str(s, t->expect);

        estr_clear(s);
        estr_catL(s, t->src);
        eexpect_str(s, t->expect);
    }

    return ETEST_OK;
}

static int estr_subc_test()
{
    estr e0, e1; cstr from, to; i64 cnt;

    e0 = estr_dupS("abcdcbd");
    e1 = estr_dupS("aascdasdabcsbcabbccabcdf");

    eexpect_str(e0, "abcdcbd");
    eexpect_str(e1, "aascdasdabcsbcabbccabcdf");
    eexpect_num(estr_len(e0), strlen("abcdcbd"));
    eexpect_num(estr_len(e1), strlen("aascdasdabcsbcabbccabcdf"));

    //! ---------------------------------
    from = "abc"; to = "1234";

    cnt = estr_subc(e0, from, to);
    eexpect_num(cnt, 2);
    eexpect_str(e0, "123d12d");
    eexpect_num(estr_len(e0), strlen(e0));

    cnt = estr_subc(e1, from, to);
    eexpect_num(cnt, 5);
    eexpect_str(e1, "12s1d1sd123s1234df");
    eexpect_num(estr_len(e1), strlen(e1));

    //! ---------------------------------
    from = "1234"; to = "*";

    cnt = estr_subc(e0, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e0), strlen(e0));
    eexpect_str(e0, "*d*d");

    cnt = estr_subc(e1, from, to);
    eexpect_num(cnt, 5);
    eexpect_num(estr_len(e1), strlen(e1));
    eexpect_str(e1, "*s*d*sd*s*df");

    estr_free(e0);
    estr_free(e1);

    return ETEST_OK;
}

int estr_subs_test()
{
    estr e0 = 0, e1 = 0, e2 = 0, e3 = 0, e; cstr from, to; i64 cnt;

    e0 = estr_dupS("abcdasd");
    e1 = estr_dupS("abcd${PATH}asdasdf");
    e2 = estr_dupS("abcd${PATH}asdasdf${PATH}");
    e3 = estr_dupS("abcd${PATH}asdasdf${PATH}.../sdf///${PATH}fd%asd");

    e = e0; eexpect_num(estr_len(e), strlen(e));
    e = e1; eexpect_num(estr_len(e), strlen(e));
    e = e2; eexpect_num(estr_len(e), strlen(e));
    e = e3; eexpect_num(estr_len(e), strlen(e));

    //! ---------------------------------
    from = "${PATH}"; to = "${}";

    e = e0;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${}asdasdf");

    e = e2;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${}asdasdf${}");

    e = e3;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${}asdasdf${}.../sdf///${}fd%asd");

    //! ---------------------------------
    from = "${}"; to = "${PATH}";

    e = e0;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");
    e0 = e;

    e = e1;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${PATH}asdasdf");
    e1 = e;

    e = e2;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${PATH}asdasdf${PATH}");
    e2 = e;

    e = e3;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${PATH}asdasdf${PATH}.../sdf///${PATH}fd%asd");
    e3 = e;

    //! ---------------------------------
    from = "${PATH}"; to = "${abcd}";

    e = e0;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf");

    e = e2;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf${abcd}");

    e = e3;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf${abcd}.../sdf///${abcd}fd%asd");

    //! ---------------------------------
    from = "${abcd}"; to = "${abcde}";
    e = e0;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");
    e0 = e;

    e = e1;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcde}asdasdf");
    e1 = e;

    e = e2;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcde}asdasdf${abcde}");
    e2 = e;

    e = e3;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcde}asdasdf${abcde}.../sdf///${abcde}fd%asd");
    e3 = e;

    //! ---------------------------------
    from = "${abcde}"; to = "";
    e = e0;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasdasdf");

    e = e2;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasdasdf");

    e = e3;
    cnt = estr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasdasdf.../sdf///fd%asd");

    estr_free(e0);
    estr_free(e1);
    estr_free(e2);
    estr_free(e3);

    return ETEST_OK;
}

int test_estr(int argc, char* argv[])
{
    ETEST_RUN( estr_wrt_cat_W_test() );
    ETEST_RUN( estr_wrt_cat_L_test() );

    ETEST_RUN( estr_subc_test() );
    ETEST_RUN( estr_subs_test() );

    return ETEST_OK;
}
