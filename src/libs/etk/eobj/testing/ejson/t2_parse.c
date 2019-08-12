/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include"etime.h"
#include "eutils.h"
#include "ejson.h"

static int __obj_check(ejson r)
{
    ejson e;

    eunexpc_ptr(r, 0);
    eexpect_num(ejson_type(r), EOBJ);
    eexpect_num(ejson_size(r), 8);

    e = ejson_r(r, "false" ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EFALSE);
    e = ejson_r(r, "true"  ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ETRUE);
    e = ejson_r(r, "null"  ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENULL);
    e = ejson_r(r, "int"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM);  eexpect_num(eobj_valI(e), 100);
    e = ejson_r(r, "double"); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM);  eexpect_num(eobj_valF(e), 100.123);
    e = ejson_r(r, "str"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR);  eexpect_str(eobj_valS(e), "str");
    e = ejson_r(r, "arr"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);  eexpect_num(eobj_len(e), 8);
    e = ejson_r(r, "obj"   ); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ);  eexpect_num(eobj_len(e), 8);

    return ETEST_OK;
}

static int parse_string()
{
    typedef struct { cstr json; cstr key; etypeo t;} _IN_;

    int i; _IN_* map; ejson r, e;

    //! simple json string
    _IN_ map1[] = {
                {"false"    , 0, EFALSE,    },
                {"true"     , 0, ETRUE ,    },
                {"null"     , 0, ENULL ,    },
                {"100"      , 0, ENUM  ,    },
                {"100.123"  , 0, ENUM  ,    },
                {"\"str\""  , 0, ESTR  ,    },
                {"{}"       , 0, EOBJ  ,    },
                {"[]"       , 0, EARR  ,    },
    };
    map = map1;

    i = -1;
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_ptr(eobj_keyS(e), 0); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);

    //! simple json string with a key
    _IN_ map2[] = {
                {"\"key0\":false"    , "key0", EFALSE,    },
                {"\"key1\":true"     , "key1", ETRUE ,    },
                {"\"key2\":null"     , "key2", ENULL ,    },
                {"\"key3\":100"      , "key3", ENUM  ,    },
                {"\"key4\":100.123"  , "key4", ENUM  ,    },
                {"\"key5\":\"str\""  , "key5", ESTR  ,    },
                {"\"key6\":{}"       , "key6", EOBJ  ,    },
                {"\"key7\":[]"       , "key7", EARR  ,    },
    };
    map = map2;

    i = -1;
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);
    i++; e = ejson_parseS(map[i].json); eunexpc_ptr(e, 0); eexpect_str(eobj_keyS(e), map[i].key); eexpect_num(ejson_type(e), map[i].t); ejson_free(e);

    //! a whole json obj with no comment
    cstr json_str1 = "{"
                       "\"false\":false,"
                       "\"true\":true ,"
                       "\"null\":null,"
                       "\"int\":100, "
                       "\"double\":100.123, "
                       "\"str\":\"str\","
                       "\"arr\":[false, true, null, 100, 100.123, \"this is a str in arr\", {}, []],"
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

    r = ejson_parseS(json_str1);
    ETEST_RUN( __obj_check(r) );
    ejson_free(r);

    //! a whole json obj with no comment
    cstr json_str2 = "{"
                       "\"false\":false,  #  comment1\n"
                       "\"true\":true ,   // comment2\n"
                       "\"null\":null,    /* comment3*/"
                       "\"int\":100, "
                       "\"double\":100.123, "
                       "\"str\":\"str\","
                       "\"arr\":[false, true, null, 100, 100.123, \"this is a str in arr\", {}, []],"
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

    r = ejson_parseSEx(json_str2, 0, COMMENT);
    ETEST_RUN( __obj_check(r) );
    ejson_free(r);


    return ETEST_OK;
}

static int parse_file()
{
    ejson e; i64 t;


#define DIR MAIN_PROJECT_ROOT_DIR "src/libs/etk/eobj/testing/ejson/json/"

#define FILE1 "test_comment.json"
#define FILE2 "test_nocomment.json"
#define FILE3 "test_comment2.json"

    e = ejson_parseFEx(DIR FILE1, 0, COMMENT);
    eunexpc_ptr(e, 0);
    eexpect_num(ejson_size(e), 8);
    eexpect_num(ejson_free(e), 31);

    e = ejson_parseFEx(DIR FILE2, 0, 0);
    eunexpc_ptr(e, 0);
    eexpect_num(ejson_size(e), 8);
    eexpect_num(ejson_free(e), 31);

    e = ejson_parseFEx(DIR FILE3, 0, COMMENT);
    eunexpc_ptr(e, 0);
    eexpect_num(ejson_size(e), 20);
    eexpect_num(ejson_free(e), 35);

    t = e_nowms();
    e = ejson_parseF(DIR "big.json");
    printf("parse  \t cost: %6"PRId64"ms\n", e_nowms() - t); fflush(stdout);

    eunexpc_ptr(e, 0);
    eexpect_num(ejson_size(e), 11);

    t = e_nowms();
    eexpect_num(ejson_free(e), 37778);
    printf("release\t cost: %6"PRId64"ms\n", e_nowms() - t); fflush(stdout);

    return ETEST_OK;
}

int t2_parse(int argc, char* argv[])
{
    E_UNUSED(argc);  E_UNUSED(argv);

    ETEST_RUN( parse_string() );
    ETEST_RUN( parse_file() );

    return ETEST_OK;
}

