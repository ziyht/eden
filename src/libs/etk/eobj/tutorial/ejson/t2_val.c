/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ejson.h"

static int t2_val_get()
{
    char* s =   "{"
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

    ejson e = ejson_parseS(s);

    // 获取值 顶层 obj 的值
    {
        puts("------ .");
        etypeo v_false  = ejson_rType(e, "false");  printf("%s\n"  , ejson_rTypeS(e, "false"));
        etypeo v_true   = ejson_rType(e, "true");   printf("%s\n"  , ejson_rTypeS(e, "true"));
        etypeo v_null   = ejson_rType(e, "null");   printf("%s\n"  , ejson_rTypeS(e, "null"));
        i64    v_int    = ejson_rValI(e, "int");    printf("%"PRIi64"\n", v_int);
        f64    v_double = ejson_rValF(e, "double"); printf("%f\n"  , v_double);
        constr v_str    = ejson_rValS(e, "str");    printf("%s\n"  , v_str);
    }

    // 获取值 多层 obj 的值
    {
        puts("------ .[]");
        etypeo v_false  = ejson_pType(e, "arr[0]"); printf("%s\n"  , ejson_pTypeS(e, "arr[0]"));
        etypeo v_true   = ejson_pType(e, "arr[1]"); printf("%s\n"  , ejson_pTypeS(e, "arr[1]"));
        etypeo v_null   = ejson_pType(e, "arr[2]"); printf("%s\n"  , ejson_pTypeS(e, "arr[2]"));
        i64    v_int    = ejson_pValI(e, "arr[3]"); printf("%"PRIi64"\n", v_int);
        f64    v_double = ejson_pValF(e, "arr[4]"); printf("%f\n"  , v_double);
        constr v_str    = ejson_pValS(e, "arr[5]"); printf("%s\n"  , v_str);
    }
    {
        puts("------ ..");
        etypeo v_false  = ejson_pType(e, "obj.false");  printf("%s\n"  , ejson_pTypeS(e, "obj.false"));
        etypeo v_true   = ejson_pType(e, "obj.true");   printf("%s\n"  , ejson_pTypeS(e, "obj.true"));
        etypeo v_null   = ejson_pType(e, "obj.null");   printf("%s\n"  , ejson_pTypeS(e, "obj.null"));
        i64    v_int    = ejson_pValI(e, "obj.int");    printf("%"PRIi64"\n", v_int);
        f64    v_double = ejson_pValF(e, "obj.double"); printf("%f\n"  , v_double);
        constr v_str    = ejson_pValS(e, "obj.str");    printf("%s\n"  , v_str);
    }
    {
        puts("------ ..[]");
        etypeo v_false  = ejson_pType(e, "obj.arr[0]"); printf("%s\n"  , ejson_pTypeS(e, "obj.arr[0]"));
        etypeo v_true   = ejson_pType(e, "obj.arr[1]"); printf("%s\n"  , ejson_pTypeS(e, "obj.arr[1]"));
        etypeo v_null   = ejson_pType(e, "obj.arr[2]"); printf("%s\n"  , ejson_pTypeS(e, "obj.arr[2]"));
        i64    v_int    = ejson_pValI(e, "obj.arr[3]"); printf("%"PRIi64"\n", v_int);
        f64    v_double = ejson_pValF(e, "obj.arr[4]"); printf("%f\n"  , v_double);
        constr v_str    = ejson_pValS(e, "obj.arr[5]"); printf("%s\n"  , v_str);
    }

    ejson_free(e);

    return ETEST_OK;
}

static int t2_val_get2()
{
    char* s =  "{"
                    "\"fruit.name\" :\"apple\","
                    "\"fruit.color\":\"red\","
                    "\"fruit\":{"
                    "   \"name\": \"orange\","
                    "   \"color\": \"orange\","
                    "}"
                "}";

    ejson e = ejson_parseS(s);

    puts("Get val from OBJ by using rawk or path: ");
    ejson e1 = ejson_r(e, "fruit.name");    // r 表示 rawk，不解析 key，直接查找
    ejson e2 = ejson_r(e, "fruit.color");
    ejson e3 = ejson_p(e, "fruit.name");    // p 表示 path，会进行内部解析，支持 . or []
    ejson e4 = ejson_p(e, "fruit.name");
    ejson e5 = ejson_p(e, "fruit[name]");   // p 表示 path，会进行内部解析，支持 . or []
    ejson e6 = ejson_p(e, "fruit[name]");

    printf("e1: %s\n", eobj_valS(e1));
    printf("e2: %s\n", eobj_valS(e2));
    printf("e3: %s\n", eobj_valS(e3));
    printf("e4: %s\n", eobj_valS(e4));
    printf("e5: %s\n", eobj_valS(e5));
    printf("e6: %s\n", eobj_valS(e6));

    printf("%s\n", ejson_rValS(e, "fruit.name"));
    printf("%s\n", ejson_rValS(e, "fruit.color"));
    printf("%s\n", ejson_pValS(e, "fruit.name"));
    printf("%s\n", ejson_pValS(e, "fruit.color"));
    printf("%s\n", ejson_pValS(e, "fruit[name]"));
    printf("%s\n", ejson_pValS(e, "fruit[color]"));

    ejson_free(e);

    puts("Get val from ARR by using index or path: ");
    s = "[4, 3, 2, 1]";
    e = ejson_parseS(s);

    printf("i: %" PRId64 "\n", ejson_iValI(e, 0));  // 对于数组，可以直接 使用 i族 API 获取值
    printf("i: %" PRId64 "\n", ejson_iValI(e, 1));
    printf("i: %" PRId64 "\n", ejson_iValI(e, 2));
    printf("i: %" PRId64 "\n", ejson_iValI(e, 3));

    printf("p: %" PRId64 "\n", ejson_pValI(e, "0"));  // 对于数组，也可以使用 p族 API 取值，但是效率没有 i族高
    printf("p: %" PRId64 "\n", ejson_pValI(e, "1"));
    printf("p: %" PRId64 "\n", ejson_pValI(e, ".2"));
    printf("p: %" PRId64 "\n", ejson_pValI(e, "[3]"));

    ejson_free(e);

    return ETEST_OK;
}

int t2_val(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( t2_val_get() );
    ETEST_RUN( t2_val_get2() );

    return ETEST_OK;
}

