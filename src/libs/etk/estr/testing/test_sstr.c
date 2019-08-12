#include <stdio.h>

#include "etest.h"
#include "estr.h"

static int sstr_subc_test()
{
    estr e0, e1; cstr from, to; char buf0[100]; char buf1[100]; i64 cnt;

    e0 = sstr_init(buf0, 100);
    e1 = sstr_init(buf1, 100);

    sstr_wrtS(e0, "abcdcbd");
    sstr_wrtS(e1, "aascdasdabcsbcabbccabcdf");

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
    eexpect_num(estr_len(e0), strlen(e0));//! ---------------------------------
    eexpect_str(e0, "*d*d");

    cnt = estr_subc(e1, from, to);
    eexpect_num(cnt, 5);
    eexpect_num(estr_len(e1), strlen(e1));
    eexpect_str(e1, "*s*d*sd*s*df");

    return ETEST_OK;
}

static int sstr_subs_test()
{
    sstr e0 = 0, e1 = 0, e2 = 0, e3 = 0, e; cstr from, to; i64 cnt;

    char buf0[100]; char buf1[100]; char buf2[100]; char buf3[100];

    e0 = sstr_init(buf0, 11);
    e1 = sstr_init(buf1, 22);
    e2 = sstr_init(buf2, 29);
    e3 = sstr_init(buf3, 80);

    sstr_wrtS(e0, "abcdasd");
    sstr_wrtS(e1, "abcd${PATH}asdasdf");
    sstr_wrtS(e2, "abcd${PATH}asdasdf${PATH}");
    sstr_wrtS(e3, "abcd${PATH}asdasdf${PATH}.../sdf///${PATH}fd%asd");

    e = e0; eexpect_num(estr_len(e), strlen(e));
    e = e1; eexpect_num(estr_len(e), strlen(e));
    e = e2; eexpect_num(estr_len(e), strlen(e));
    e = e3; eexpect_num(estr_len(e), strlen(e));

    //! ---------------------------------
    from = "${PATH}"; to = "${}";

    e = e0;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${}asdasdf");

    e = e2;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${}asdasdf${}");

    e = e3;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${}asdasdf${}.../sdf///${}fd%asd");

    //! ---------------------------------
    from = "${}"; to = "${PATH}";

    e = e0;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${PATH}asdasdf");

    e = e2;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${PATH}asdasdf${PATH}");

    e = e3;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${PATH}asdasdf${PATH}.../sdf///${PATH}fd%asd");

    //! ---------------------------------
    from = "${PATH}"; to = "${abcd}";

    e = e0;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf");

    e = e2;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 2);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf${abcd}");

    e = e3;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf${abcd}.../sdf///${abcd}fd%asd");

    //! ---------------------------------
    from = "${abcd}"; to = "${abcde}";

    from = "${abcd}"; to = "${abcde}";
    e = e0;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = sstr_subs(e, from, to);       // no enough space
    eexpect_num(cnt < 0, 1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf");

    e = e2;
    cnt = sstr_subs(e, from, to);       // no enough space
    eexpect_num(cnt, -1);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf${abcd}");

    e = e3;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcde}asdasdf${abcde}.../sdf///${abcde}fd%asd");

    //! ---------------------------------
    from = "${abcde}"; to = "";

    e = e0;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasd");

    e = e1;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf");

    e = e2;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 0);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcd${abcd}asdasdf${abcd}");

    e = e3;
    cnt = sstr_subs(e, from, to);
    eexpect_num(cnt, 3);
    eexpect_num(estr_len(e), strlen(e));
    eexpect_str(e, "abcdasdasdf.../sdf///fd%asd");

    return ETEST_OK;
}

int test_sstr(int argc, char* argv[])
{
    ETEST_RUN(sstr_subc_test());
    ETEST_RUN(sstr_subs_test());

    return ETEST_OK;
}
