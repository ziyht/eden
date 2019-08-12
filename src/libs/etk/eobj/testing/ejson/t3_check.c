/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "etime.h"
#include "eutils.h"
#include "ejson.h"

static int t3_check_ok_1()
{
    uint ret; constr err;

    cstr str = "{"
                       "\"false\":false, #asdasdfdsasdf\n"
                       "\"true\":true ,//asdfasdfasdf\n"
                       "\"null\":null,/*sdfasdgdfsgsdfgsdfgsdfgsdfg*/"
                       "\"int\":100, "
                       "\"double\":100.123, "
                       "\"str\":\"this is a str obj\","
                       "\"arr\":[false, true, null, 100, 100.123, \"this is a str in arr\", {}],"
                       "\"obj\":{"
                           "\"false\":false, "
                           "\"true\":true ,"
                           "\"null\":null, "
                           "\"int\":100, "
                           "\"double\":100.123, "
                           "\"str\":\"this is a str obj\","
                           "\"arr\":[false, true, null, 100, 100.123, \"this is a str in arr\", {}],"
                           "\"obj\":{}"
                       "}"
                   "}";

    ret = ejson_checkSEx(str, &err, COMMENT);

    eexpect_num(ret, 31);

    return ETEST_OK;
}

static int t3_check_ok_2()
{
    uint ret; constr err; i64 t;

#define DIR MAIN_PROJECT_ROOT_DIR "src/libs/etk/eobj/testing/ejson/json/"

    t = e_nowms();
    ret = ejson_checkFEx(DIR "big.json", &err, ALL_OFF);
    printf("check  \t cost: %6"PRId64"ms\n", e_nowms() - t); fflush(stdout);

    eexpect_num(ret, 37778);

    return ETEST_OK;
}

static int t3_check_err_1()
{
    bool ret; constr err;

    cstr str =  "{"
                "   \"key\": true,"
                "   \"key\": false"     // <-- same key
                "}";

    ret = ejson_checkSEx(str, &err, COMMENT);

    eexpect_num(ret, false);

    return ETEST_OK;
}

static int t3_check_err_2()
{
    bool ret; constr err;

    cstr str =  "{"
                "   \"key1\": true,,"   // continues ','
                "   \"key2\": false"
                "}";

    ret = ejson_checkSEx(str, &err, COMMENT);

    eexpect_num(ret, false);

    return ETEST_OK;
}

int t3_check(int argc, char* argv[])
{
    E_UNUSED(argc); E_UNUSED(argv);

    ETEST_RUN( t3_check_ok_1() );
    ETEST_RUN( t3_check_ok_2() );

    ETEST_RUN( t3_check_err_1() );  // same key in obj
    ETEST_RUN( t3_check_err_2() );

    return ETEST_OK;
}

