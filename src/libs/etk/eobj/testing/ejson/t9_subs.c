/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ejson.h"

static int t9_subs_case1()
{
    cstr key;

    ejson r = ejson_parseS("{"
                           "    \"i\"  : 123,"
                           "    \"s\"  : \"abcdefg******abcdefg\","
                           "    \"obj\": {"
                           "                \"i\": 123,"
                           "                \"s\": \"abcdefg******abcdefg\","
                           "                \"f\": 123"
                           "             },"
                           "    \"arr\": [ 123,"
                           "               \"abcdefg******abcdefg\","
                           "               123,"
                           "             ]"
                           "}");

    eexpect_1(r != 0);

    //! 1
    key = "s";
    ejson_rReplaceS(r, key, "******", "123456");
    eexpect_str(ejson_rValS(r, key), "abcdefg123456abcdefg");

    ejson_rReplaceS(r, key, "123456", "**");
    eexpect_str(ejson_rValS(r, key), "abcdefg**abcdefg");

    ejson_rReplaceS(r, key, "a", "[i am a]");
    eexpect_str(ejson_rValS(r, key), "[i am a]bcdefg**[i am a]bcdefg");

    ejson_rReplaceS(r, key, "b", "");
    eexpect_str(ejson_rValS(r, key), "[i am a]cdefg**[i am a]cdefg");

    //! 2
    key = "obj.s";
    ejson_pReplaceS(r, key, "******", "123456");
    eexpect_str(ejson_pValS(r, key), "abcdefg123456abcdefg");

    ejson_pReplaceS(r, key, "123456", "**");
    eexpect_str(ejson_pValS(r, key), "abcdefg**abcdefg");

    ejson_pReplaceS(r, key, "a", "[i am a]");
    eexpect_str(ejson_pValS(r, key), "[i am a]bcdefg**[i am a]bcdefg");

    ejson_pReplaceS(r, key, "b", "");
    eexpect_str(ejson_pValS(r, key), "[i am a]cdefg**[i am a]cdefg");

    ejson_free(r);

    return ETEST_OK;
}

static int t9_subs_case2()
{
    cstr key;

    ejson r = ejson_parseS("["
                           "    123,"
                           "    \"abcdefg******abcdefg\","
                           "    {"
                           "                \"i\": 123,"
                           "                \"s\": \"abcdefg******abcdefg\","
                           "                \"f\": 123"
                           "             },"
                           "    [ 123,"
                           "               \"abcdefg******abcdefg\","
                           "               123,"
                           "             ]"
                           "]");

    eexpect_1(r != 0);

    //! 1
    key = "[1]";
    ejson_pReplaceS(r, key, "******", "123456");
    eexpect_str(ejson_pValS(r, key), "abcdefg123456abcdefg");

    ejson_pReplaceS(r, key, "123456", "**");
    eexpect_str(ejson_pValS(r, key), "abcdefg**abcdefg");

    ejson_pReplaceS(r, key, "a", "[i am a]");
    eexpect_str(ejson_pValS(r, key), "[i am a]bcdefg**[i am a]bcdefg");

    ejson_pReplaceS(r, key, "b", "");
    eexpect_str(ejson_pValS(r, key), "[i am a]cdefg**[i am a]cdefg");

    //! 3
    key = "[3][1]";
    ejson_pReplaceS(r, key, "******", "123456");
    eexpect_str(ejson_pValS(r, key), "abcdefg123456abcdefg");

    ejson_pReplaceS(r, key, "123456", "**");
    eexpect_str(ejson_pValS(r, key), "abcdefg**abcdefg");

    ejson_pReplaceS(r, key, "a", "[i am a]");
    eexpect_str(ejson_pValS(r, key), "[i am a]bcdefg**[i am a]bcdefg");

    ejson_pReplaceS(r, key, "b", "");
    eexpect_str(ejson_pValS(r, key), "[i am a]cdefg**[i am a]cdefg");

    ejson_free(r);

    return ETEST_OK;
}

int t9_subs(int argc, char* argv[])
{
    ETEST_RUN( t9_subs_case1() );
    ETEST_RUN( t9_subs_case2() );

    return ETEST_OK;
}

