/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include <QString>

#include "estr.h"

#define SRC_STR "aa__aa__aa____aa__aa____aa___aa"

#define FROM "aa"
#define TO1  "b"
#define TO2  "bb"
#define TO3  "bbbb"


static int qstring_sub_to_smaller(int cnt)
{

    QString s;

    for(int i = 0; i < cnt; i++)
    {
        s = SRC_STR;

        s.replace(FROM, TO1);
    }

    return ETEST_OK;
}

static int qstring_sub_to_same(int cnt)
{

    QString s;

    for(int i = 0; i < cnt; i++)
    {
        s = SRC_STR;

        s.replace(FROM, TO2);
    }

    return ETEST_OK;
}

static int qstring_sub_to_bigger(int cnt)
{

    QString s;

    for(int i = 0; i < cnt; i++)
    {
        s = SRC_STR;

        s.replace(FROM, TO3);
    }

    return ETEST_OK;
}

static int estr_sub_to_smaller(int cnt)
{
    estr s = 0;

    for(int i = 0; i < cnt; i++)
    {
        estr_wrtS(s, SRC_STR);

        estr_subs(s, FROM, TO1);
    }

    estr_free(s);

    return ETEST_OK;
}

static int estr_sub_to_same(int cnt)
{
    estr s = 0;

    for(int i = 0; i < cnt; i++)
    {
        estr_wrtS(s, SRC_STR);

        estr_subs(s, FROM, TO2);
    }

    estr_free(s);

    return ETEST_OK;
}

static int estr_sub_to_bigger(int cnt)
{
    estr s = 0;

    for(int i = 0; i < cnt; i++)
    {
        estr_wrtS(s, SRC_STR);

        estr_subs(s, FROM, TO3);
    }

    estr_free(s);

    return ETEST_OK;
}

static int subs_cmp_case2()
{
    eexpect_num(1, 0);      // will failed

    return ETEST_OK;
}


extern "C"
int subs_cmp(int argc, char* argv[])
{
    int cnt = 1000;

    ETEST_RUN( qstring_sub_to_smaller(cnt) );
    ETEST_RUN( qstring_sub_to_same(cnt) );
    ETEST_RUN( qstring_sub_to_bigger(cnt) );

    ETEST_RUN( estr_sub_to_smaller(cnt) );
    ETEST_RUN( estr_sub_to_same(cnt) );
    ETEST_RUN( estr_sub_to_bigger(cnt) );

    return ETEST_OK;
}

