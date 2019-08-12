/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eutils.h"
#include "ejson.h"
#include "estr.h"

static int t4_valk_case1()
{
    ejson r, e;

    cstr str =  "{"
                    "\"1\": {"
                        "\"2\": {"
                            "\"3\": [[[\"val1\"], {"
                                                    "\"4\":\"val2\""
                                                  "}"
                                    "]"
                                   "]"
                        "}"
                    "}"
                "}";


    r = ejson_parseS(str);

    e = ejson_p(r, "1"             ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "1.2"           ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "1.2.3"         ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "1.2.3[0]"      ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "1.2.3[0][0]"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "1.2.3[0][0][0]"); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);
    e = ejson_p(r, "1.2.3[0][1]"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "1.2.3[0][1].4" ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);

    ejson_free(r);

    return ETEST_OK;
}

static int t4_valk_case2()
{
    ejson r, e;

    cstr str =  "["
                    "{"
                        "\"2\": {"
                            "\"3\": [[[\"val1\"], {"
                                                    "\"4\":\"val2\""
                                                 "}"
                                    "]"
                                   "]"
                        "}"
                    "}"
                "]";

    r = ejson_parseS(str);

    e = ejson_p(r, "[0]"             ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "[0].2"           ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "[0].2.3"         ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "[0].2.3[0]"      ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "[0].2.3[0][0]"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "[0].2.3[0][0][0]"); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);
    e = ejson_p(r, "[0].2.3[0][1]"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "[0].2.3[0][1].4" ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);

    e = ejson_p(r, "0"             ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "0.2"           ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "0.2.3"         ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "0.2.3.0"       ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "0.2.3.0.0"     ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "0.2.3.0.0.0"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);
    e = ejson_p(r, "0.2.3.0.1"     ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "0.2.3.0.1.4"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);

    e = ejson_p(r, "[0]"               ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "[0][2]"            ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "[0][2][3]"         ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "[0][2][3][0]"      ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "[0][2][3][0][0]"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    e = ejson_p(r, "[0][2][3][0][0][0]"); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);
    e = ejson_p(r, "[0][2][3][0][1]"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);
    e = ejson_p(r, "[0][2][3][0][1][4]"); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);

    ejson_free(r);

    return ETEST_OK;
}

int t4_valk(int argc, char* argv[])
{
    E_UNUSED(argc);  E_UNUSED(argv);

    ETEST_RUN( t4_valk_case1() );
    ETEST_RUN( t4_valk_case2() );

    return ETEST_OK;
}

