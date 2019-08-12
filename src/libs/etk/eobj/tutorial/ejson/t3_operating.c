/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eutils.h"
#include "ejson.h"

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

int t3_operating(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( t3_operating_new_ejson() );
    ETEST_RUN( t3_operating_add() );
    ETEST_RUN( t3_operating_del() );
    ETEST_RUN( t3_operating_set() );
    ETEST_RUN( t3_operating_iterating() );

    return ETEST_OK;
}

