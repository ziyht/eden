# ejson

简单易用的高性能 json 解析器，使用 字典 管理 obj 的 子项，所以在查询上具有极高的效率。基本特性：

 * 简单易用的 API
 * 对 obj 的查询具有极高的效率
 * 支持解析式的查询
 * 支持注释


# 来源

* 部分源码 来自 cjson
* 字典部分 来自 redis/dic
* 字符串部分 来自 redis/sds

# 目录

* [快速入门](#快速入门)
* [解析](#解析)
    * [解析简单字串](#解析简单字串)
    * [解析完整json](#解析一个完整的json)
    * [解析带注释的json](#解析带有注释的json)
    * [解析文件](#解析文件)
* [取值/查询](#取值)
* [操作](#操作)
    * [新的 ejson](#新的ejson)
    * [增](#增)
    * [删](#删)
    * [改](#改)
    * [查](#查)
    * [排序（文档待完善😳）](#)
    * [遍历](#遍历)

* [benchmark](#benckmark)


# 快速入门

``` c
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
```
```
================ output ================
Get val from OBJ by using rawk or path: 
e1: apple
e2: red
e3: orange
e4: orange
e5: orange
e6: orange
apple
red
orange
orange
orange
orange
Get val from ARR by using index or path: 
i: 4
i: 3
i: 2
i: 1
p: 4
p: 3
p: 2
p: 1
```

# 解析

### 解析简单字串

```c
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
```
```
output: 
ejson(0x7f987d4004c8): true
ejson(0x7f987d4004c8): false
ejson(0x7f987d4004c8): null
ejson(0x7f987d4004c8): 1
ejson(0x7f987d4004c8): 2.300000
ejson(0x7f987d4004c8): "str"
ejson(0x7f987d400598): []
ejson(0x7f987d400608): {}
```

### 解析一个完整的json

```c
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
```
### 解析带有注释的json

```c
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
```
### 解析文件
```c
static int parsing_file()
{
    ejson e;

#define DIR MAIN_PROJECT_ROOT_DIR "src/libs/etools/ejson/testing/json/"

#define FILE1 DIR "test_comment.json"
#define FILE2 DIR "test_nocomment.json"

    e = ejson_parseF(FILE1); ejson_show(e); ejson_free(e);
    e = ejson_parseF(FILE2); ejson_show(e); ejson_free(e);

    return ETEST_OK;
}
```

# 取值
```C
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
        i64    v_int    = ejson_rValI(e, "int");    printf("%lld\n", v_int);
        f64    v_double = ejson_rValF(e, "double"); printf("%f\n"  , v_double);
        constr v_str    = ejson_rValS(e, "str");    printf("%s\n"  , v_str);
    }

    // 获取值 多层 obj 的值
    {
        puts("------ .[]");
        etypeo v_false  = ejson_pType(e, "arr[0]"); printf("%s\n"  , ejson_pTypeS(e, "arr[0]"));
        etypeo v_true   = ejson_pType(e, "arr[1]"); printf("%s\n"  , ejson_pTypeS(e, "arr[1]"));
        etypeo v_null   = ejson_pType(e, "arr[2]"); printf("%s\n"  , ejson_pTypeS(e, "arr[2]"));
        i64    v_int    = ejson_pValI(e, "arr[3]"); printf("%lld\n", v_int);
        f64    v_double = ejson_pValF(e, "arr[4]"); printf("%f\n"  , v_double);
        constr v_str    = ejson_pValS(e, "arr[5]"); printf("%s\n"  , v_str);
    }
    {
        puts("------ ..");
        etypeo v_false  = ejson_pType(e, "obj.false");  printf("%s\n"  , ejson_pTypeS(e, "obj.false"));
        etypeo v_true   = ejson_pType(e, "obj.true");   printf("%s\n"  , ejson_pTypeS(e, "obj.true"));
        etypeo v_null   = ejson_pType(e, "obj.null");   printf("%s\n"  , ejson_pTypeS(e, "obj.null"));
        i64    v_int    = ejson_pValI(e, "obj.int");    printf("%lld\n", v_int);
        f64    v_double = ejson_pValF(e, "obj.double"); printf("%f\n"  , v_double);
        constr v_str    = ejson_pValS(e, "obj.str");    printf("%s\n"  , v_str);
    }
    {
        puts("------ ..[]");
        etypeo v_false  = ejson_pType(e, "obj.arr[0]"); printf("%s\n"  , ejson_pTypeS(e, "obj.arr[0]"));
        etypeo v_true   = ejson_pType(e, "obj.arr[1]"); printf("%s\n"  , ejson_pTypeS(e, "obj.arr[1]"));
        etypeo v_null   = ejson_pType(e, "obj.arr[2]"); printf("%s\n"  , ejson_pTypeS(e, "obj.arr[2]"));
        i64    v_int    = ejson_pValI(e, "obj.arr[3]"); printf("%lld\n", v_int);
        f64    v_double = ejson_pValF(e, "obj.arr[4]"); printf("%f\n"  , v_double);
        constr v_str    = ejson_pValS(e, "obj.arr[5]"); printf("%s\n"  , v_str);
    }

    ejson_free(e);

    return ETEST_OK;
}
```

# 操作
### 新的ejson
```c

static int t3_operating_new_ejson()
{
    ejson e;

    e = ejson_new(EFALSE, EVAL_ZORE);       ejson_show(e); ejson_free(e);
    e = ejson_new(ETRUE , EVAL_ZORE);       ejson_show(e); ejson_free(e);
    e = ejson_new(ENULL , EVAL_ZORE);       ejson_show(e); ejson_free(e);
    e = ejson_new(ENUM_I, EVAL_I64(123));   ejson_show(e); ejson_free(e);
    e = ejson_new(ENUM_F, EVAL_F64(1.23));  ejson_show(e); ejson_free(e);
    e = ejson_new(ESTR  , EVAL_S("str"));   ejson_show(e); ejson_free(e);
    e = ejson_new(EOBJ  , EVAL_ZORE);       ejson_show(e); ejson_free(e);
    e = ejson_new(EARR  , EVAL_ZORE);       ejson_show(e); ejson_free(e);

    return ETEST_OK;
}
```
```
================ output ================
ejson(0x7fd48b500028): false
ejson(0x7fd48b500028): true
ejson(0x7fd48b500028): null
ejson(0x7fd48b500028): 123
ejson(0x7fd48b500028): 1.230000
ejson(0x7fd48b500028): "str"
ejson(0x7fd48b500108): {}
ejson(0x7fd48b5001b8): []
```

### 增
```c
ejson obj;
ejson arr;
static int t3_operating_add()
{
    obj = ejson_new(EOBJ, EVAL_ZORE);
    arr = ejson_new(EARR, EVAL_ZORE);

    ejson_addI(obj, "i64", 123);
    ejson_addF(obj, "f64", 1.23);
    ejson_addS(obj, "str", "str");

    ejson_addI(arr, 0, 123);
    ejson_addF(arr, 0, 1.23);
    ejson_addS(arr, 0, "str");

    ejson_show(obj);
    ejson_show(arr);

    return ETEST_OK;
}
```
```
================ output ================
ejson(0x7f97c2500108):
{
	"i64": 123,
	"f64": 1.230000,
	"str": "str"
}
ejson(0x7f97c25001b8):
[123, 1.230000, "str"]
```

### 删
```c
static int t3_operating_del()
{
    ejson e1 = ejson_takeR(obj, "f64");
    ejson_show(e1); ejson_free(e1);
    ejson_show(obj);

    ejson e2 = ejson_takeI(arr, 1);
    ejson_show(e2); ejson_free(e2);
    ejson_show(arr);

    ejson_freeR(obj, "i64");
    ejson_freeI(arr, 0);
    ejson_show(obj);
    ejson_show(arr);

    ejson_clear(obj);
    ejson_clear(arr);
    ejson_show(obj);
    ejson_show(arr);

    return ETEST_OK;
}
```
```
================ output ================
ejson(0x7f97c2500258): 1.230000
ejson(0x7f97c2500108):
{
	"i64": 123,
	"str": "str"
}
ejson(0x7f97c2500308): 1.230000
ejson(0x7f97c25001b8):
[123, "str"]
ejson(0x7f97c2500108):
{
	"str": "str"
}
ejson(0x7f97c25001b8):
["str"]
ejson(0x7f97c2500108): {}
ejson(0x7f97c25001b8): []
```

### 改
```c
static int t3_operating_set()
{
    ejson_addI(obj, "i64", 123);
    ejson_addF(obj, "f64", 1.23);
    ejson_addS(obj, "str", "str");
    ejson_addT(obj, "obj", EOBJ);
    ejson_addJ(obj, "arr", "[]");
    ejson_show(obj);

    ejson_rSetF(obj, "i64", 3456.12);
    ejson_rSetI(obj, "f64", 321);
    ejson_rSetT(obj, "str", EFALSE);
    ejson_rSetT(obj, "obj", ETRUE);
    ejson_rSetT(obj, "arr", ENULL);
    ejson_rSetT(obj, "new", EOBJ);  // 对于 OBJ 不存在的自动添加
    ejson_show(obj);

    ejson_addI(arr, "i64", 123);
    ejson_addF(arr, "f64", 1.23);
    ejson_addS(arr, "str", "str");
    ejson_addT(arr, "obj", EOBJ);
    ejson_addJ(arr, "arr", "[]");
    ejson_show(arr);

    ejson_iSetF(arr, 0, 3456.12);
    ejson_iSetI(arr, 1, 321);
    ejson_iSetT(arr, 2, EFALSE);
    ejson_iSetT(arr, 3, ETRUE);
    ejson_iSetT(arr, 4, ENULL);
    ejson_iSetT(arr, 5, EOBJ);      // 对于 ARR 不存在的无法设置
    ejson_show(arr);

    ejson_clear(obj);
    ejson_clear(arr);

    return ETEST_OK;
}
```
```
================ output ================
ejson(0x7f97c2500108):
{
	"i64": 123,
	"f64": 1.230000,
	"str": "str",
	"obj": {},
	"arr": []
}
ejson(0x7f97c2500108):
{
	"i64": 3456.120000,
	"f64": 321,
	"str": false,
	"obj": true,
	"arr": null,
	"new": {}
}
ejson(0x7f97c25001b8):
[123, 1.230000, "str", {}, []]
ejson(0x7f97c25001b8):
[3456.120000, 321, false, true, null]
```

### 查

请查看 [取值/查询](#取值)

### 排序

### 字串替换

### 遍历
```c
static int t3_operating_iterating()
{
    for(int i = 0; i < 10; i++)
    {
        ejson_addI(obj, llstr(i), i);   // #include "eutils.h"
    }

    // 遍历 obj
    ejson_foreach_s(obj, itr)
    {
        printf("%s: %" PRId64 "\n", eobj_keyS(itr), EOBJ_VALI(itr));
    }

    for(int i = 0; i < 10; i++)
    {
        ejson_addI(arr, 0, i);
    }

    // 遍历 ARR 1
    ejson_foreach_s(arr, itr)
    {
        ejson_show(itr);
    }

    // 遍历 ARR 2
    // 内部做了缓存，这样的遍历效率也是极高的，不会重复查询
    // 但是多线程[同时]遍历时会破坏这个特性，内部缓存无法被多线程同时共用
    for(int i = 0; i < 10; i++)
    {
        ejson_show(ejson_i(arr, i));
    }

    return ETEST_OK;
}
```
```
================ output ================
0: 0
1: 1
2: 2
3: 3
4: 4
5: 5
6: 6
7: 7
8: 8
9: 9
ejson(0x7ffd5d500368): 0
ejson(0x7ffd5d500398): 1
ejson(0x7ffd5d5003c8): 2
ejson(0x7ffd5d5003f8): 3
ejson(0x7ffd5d500428): 4
ejson(0x7ffd5d500458): 5
ejson(0x7ffd5d500488): 6
ejson(0x7ffd5d500528): 7
ejson(0x7ffd5d500558): 8
ejson(0x7ffd5d500588): 9
ejson(0x7ffd5d500368): 0
ejson(0x7ffd5d500398): 1
ejson(0x7ffd5d5003c8): 2
ejson(0x7ffd5d5003f8): 3
ejson(0x7ffd5d500428): 4
ejson(0x7ffd5d500458): 5
ejson(0x7ffd5d500488): 6
ejson(0x7ffd5d500528): 7
ejson(0x7ffd5d500558): 8
ejson(0x7ffd5d500588): 9
```


# benchmark
```
================ EBENCHMARK ================
ejson obj operating
============================================
OPRT   SCALE      COST      TPO          OPS 
---- ------- --------- -------- ------------ 
add    50000   12.21ms 244.20ns   4095004.10
find   50000    1.80ms  36.10ns  27700831.02
free   50000    8.30ms 165.96ns   6025548.32

add   500000  101.28ms 202.56ns   4936711.36
find  500000   14.51ms  29.01ns  34466119.80
free  500000   72.92ms 145.84ns   6856923.44

add  5000000 1071.42ms 214.28ns   4666704.00
find 5000000  234.68ms  46.94ns  21305516.85
free 5000000  687.30ms 137.46ns   7274801.25


================ EBENCHMARK ================
ejson arr operating
============================================
OPRT   SCALE      COST      TPO          OPS 
---- ------- --------- -------- ------------ 
add    50000    3.30ms  66.00ns  15151515.15
find   50000  210.00us   4.20ns 238095238.10
free   50000    3.77ms  75.42ns  13259082.47

add   500000   37.68ms  75.37ns  13268582.65
find  500000    2.57ms   5.13ns 194779898.71
free  500000   37.32ms  74.63ns  13398719.08

add  5000000  392.39ms  78.48ns  12742327.21
find 5000000   24.21ms   4.84ns 206551823.85  # 顺序查找，其实是遍历
free 5000000  373.02ms  74.61ns  13403927.35
```




