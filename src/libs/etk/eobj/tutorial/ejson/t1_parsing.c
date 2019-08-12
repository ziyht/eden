/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ejson.h"

static int parsing_str()
{
    ejson e;

    e = ejson_parseS("true"     ); ejson_show(e); ejson_free(e);
    e = ejson_parseS("false"    ); ejson_show(e); ejson_free(e);
    e = ejson_parseS("null"     ); ejson_show(e); ejson_free(e);
    e = ejson_parseS("1"        ); ejson_show(e); ejson_free(e);
    e = ejson_parseS("2.3"      ); ejson_show(e); ejson_free(e);
    e = ejson_parseS("\"str\""  ); ejson_show(e); ejson_free(e);
    e = ejson_parseS("[]"       ); ejson_show(e); ejson_free(e);
    e = ejson_parseS("{}"       ); ejson_show(e); ejson_free(e);

    return ETEST_OK;
}

static int parsing_json()
{
    ejson e;

    cstr json =  "{"
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

    e = ejson_parseS(json); ejson_show(e); ejson_free(e);

    return ETEST_OK;
}

static int parsing_json_with_comments()
{
    ejson e;

    cstr json =  "{"
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

    // ejson_parseS() 不支持 comment
    e = ejson_parseSEx(json, 0, COMMENT); ejson_show(e); ejson_free(e);

    return ETEST_OK;
}

static int parsing_file()
{
    ejson e;

#define DIR MAIN_PROJECT_ROOT_DIR "src/libs/etk/eobj/testing/ejson/json/"

#define FILE1 DIR "test_comment.json"
#define FILE2 DIR "test_nocomment.json"

    e = ejson_parseF(FILE1); ejson_show(e); ejson_free(e);
    e = ejson_parseF(FILE2); ejson_show(e); ejson_free(e);

    return ETEST_OK;
}

int t1_parsing(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( parsing_str() );
    ETEST_RUN( parsing_json() );
    ETEST_RUN( parsing_json_with_comments() );

    ETEST_RUN( parsing_file() );

    return ETEST_OK;
}

