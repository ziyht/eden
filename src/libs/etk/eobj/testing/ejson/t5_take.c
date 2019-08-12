/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eutils.h"
#include "ejson.h"

static int test_takeH()
{
    ejson r, e;

    cstr str =  "{"
                        "\"false\"   : false, "
                        "\"true\"    : true ,"
                        "\"null\"    : null, "
                        "\"int\"     : 100, "
                        "\"double\"  : 100.123, "
                        "\"str\"     : \"this is a str obj\","
                        "\"obj\"     : {}, "
                        "\"arr\"     : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []]"
                "}";

    r = ejson_parseS(str);
    eunexpc_ptr(r, 0);

    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EFALSE); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ETRUE ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENULL ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR  );

    eexpect_num(ejson_free(r), 1);
    r = e;

    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EFALSE); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ETRUE ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENULL ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeH(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR  ); eexpect_num(ejson_free(e), 1);

    eexpect_num(ejson_free(r), 1);

    return ETEST_OK;
}

static int test_takeT()
{
    ejson r, e;

    cstr str =  "{"
                        "\"false\"   : false, "
                        "\"true\"    : true ,"
                        "\"null\"    : null, "
                        "\"int\"     : 100, "
                        "\"double\"  : 100.123, "
                        "\"str\"     : \"this is a str obj\","
                        "\"obj\"     : {}, "
                        "\"arr\"     : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []]"
                "}";

    r = ejson_parseS(str);
    eunexpc_ptr(r, 0);

    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR);
    {
        ejson _r = r;

        r = e;

        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EARR  ); eexpect_num(ejson_free(e), 1);
        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ  ); eexpect_num(ejson_free(e), 1);
        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR  ); eexpect_num(ejson_free(e), 1);
        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENULL ); eexpect_num(ejson_free(e), 1);
        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ETRUE ); eexpect_num(ejson_free(e), 1);
        e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EFALSE); eexpect_num(ejson_free(e), 1);

        eexpect_num(ejson_free(r), 1);

        r = _r;
    }

    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EOBJ  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ESTR  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENUM  ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ENULL ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), ETRUE ); eexpect_num(ejson_free(e), 1);
    e = ejson_takeT(r); eunexpc_ptr(e, 0); eexpect_num(ejson_type(e), EFALSE); eexpect_num(ejson_free(e), 1);

    eexpect_num(ejson_free(r), 1);

    return ETEST_OK;
}

static int test_takeO()
{
    ejson r, e1, e2;

    cstr str =  "{"
                        "\"false\"   : false, "
                        "\"true\"    : true ,"
                        "\"null\"    : null, "
                        "\"int\"     : 100, "
                        "\"double\"  : 100.123, "
                        "\"str\"     : \"this is a str obj\","
                        "\"obj\"     : {}, "
                        "\"arr1\"    : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []],"
                        "\"arr2\"    : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []]"
                "}";

    r = ejson_parseS(str);
    eunexpc_ptr(r, 0);

    e1 = ejson_r(r, "false"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), EFALSE); eexpect_num(ejson_free(e1), 1);

    e1 = ejson_r(r, "true"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), ETRUE ); eexpect_num(ejson_free(e1), 1);

    e1 = ejson_r(r, "null"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), ENULL ); eexpect_num(ejson_free(e1), 1);

    e1 = ejson_r(r, "int"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), ENUM); eexpect_num(ejson_free(e1), 1);

    e1 = ejson_r(r, "double"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), ENUM); eexpect_num(ejson_free(e1), 1);

    e1 = ejson_r(r, "str"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), ESTR); eexpect_num(ejson_free(e1), 1);

    e1 = ejson_r(r, "obj"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), EOBJ); eexpect_num(ejson_free(e1), 1);

    e1 = ejson_r(r, "arr1"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), EARR);

    {
        ejson _r = r;

        r = e1;

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), EFALSE); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ETRUE ); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ENULL ); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ENUM); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ENUM); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ESTR); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), EOBJ); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), EARR); eexpect_num(ejson_free(e1), 1);

        eexpect_num(ejson_free(r), 1);

        r = _r;
    }

    e1 = ejson_r(r, "arr2"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
    eexpect_num(ejson_type(e1), EARR);

    {
        ejson _r = r;

        r = e1;

        e1 = ejson_p(r, "[7]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), EARR); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[6]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), EOBJ ); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[5]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ESTR ); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[4]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ENUM); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[3]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ENUM); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[2]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ENULL); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[1]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), ETRUE); eexpect_num(ejson_free(e1), 1);

        e1 = ejson_p(r, "[0]"); eunexpc_ptr(e1, 0); e2 = ejson_takeO(r, e1); eunexpc_ptr(e2, 0); eexpect_ptr(e1, e2);
        eexpect_num(ejson_type(e1), EFALSE); eexpect_num(ejson_free(e1), 1);

        eexpect_num(ejson_free(r), 1);

        r = _r;
    }

    eexpect_num(ejson_free(r), 1);

    return ETEST_OK;
}

static int test_takeP()
{
    ejson r, e; int i;

    typedef struct { constr keys; etypeo t;} _IN_;

    cstr str =  "{"
                        "\"false\"   : false, "
                        "\"true\"    : true ,"
                        "\"null\"    : null, "
                        "\"int\"     : 100, "
                        "\"double\"  : 100.123, "
                        "\"str\"     : \"this is a str obj\","
                        "\"obj\"     : {}, "
                        "\"arr1\"    : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []],"
                        "\"arr2\"    : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []]"
                "}";

    _IN_ map[] = {
        {"arr2[7]", EARR   },
        {"arr2[6]", EOBJ   },
        {"arr2[5]", ESTR   },
        {"arr2[4]", ENUM   },
        {"arr2[3]", ENUM   },
        {"arr2[2]", ENULL  },
        {"arr2[1]", ETRUE  },
        {"arr2[0]", EFALSE },

        {"arr1[0]", EFALSE },
        {"arr1[0]", ETRUE  },
        {"arr1[0]", ENULL  },
        {"arr1[0]", ENUM   },
        {"arr1[0]", ENUM   },
        {"arr1[0]", ESTR   },
        {"arr1[0]", EOBJ   },
        {"arr1[0]", EARR   },

        {"false" , EFALSE },
        {"true"  , ETRUE },
        {"null"  , ENULL },
        {"int"   , ENUM },
        {"double", ENUM },
        {"str"   , ESTR },
        {"obj"   , EOBJ },
        {"arr1"  , EARR },
        {"arr2"  , EARR },

        {0, EOBJ_UNKNOWN},
    };

    r = ejson_parseS(str);
    eunexpc_ptr(r, 0);

    for(i = 0; i < 100; i++)
    {
        if(map[i].keys == 0)
            break;

        e = ejson_takeP(r, map[i].keys);
        eunexpc_ptr(e, 0);
        eexpect_num(ejson_type(e), map[i].t);
        eexpect_num(ejson_free(e), 1);
    }

    eexpect_num(ejson_free(r), 1);

    return ETEST_OK;
}

static int test_takeK()
{
    ejson r, e; int i;

    typedef struct { constr keys; etypeo t; int cnt; } _IN_;

    cstr str =  "{"
                        "\"false\"   : false, "
                        "\"true\"    : true ,"
                        "\"null\"    : null, "
                        "\"int\"     : 100, "
                        "\"double\"  : 100.123, "
                        "\"str\"     : \"this is a str obj\","
                        "\"obj\"     : {}, "
                        "\"arr1\"    : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []],"
                        "\"arr2\"    : [false, true, null, 100, 100.123, \"this is a str in arr\", {}, []]"
                "}";

    _IN_ map[] = {

        {"false" , EFALSE, 1},
        {"true"  , ETRUE , 1},
        {"null"  , ENULL , 1},
        {"int"   , ENUM  , 1},
        {"double", ENUM  , 1},
        {"str"   , ESTR  , 1},
        {"obj"   , EOBJ  , 1},
        {"arr1"  , EARR  , 1 + 8},
        {"arr2"  , EARR  , 1 + 8},

        {0, EOBJ_UNKNOWN, 0},
    };

    r = ejson_parseS(str);
    eunexpc_ptr(r, 0);

    for(i = 0; i < 100; i++)
    {
        if(map[i].keys == 0)
            break;

        e = ejson_takeR(r, map[i].keys);
        eunexpc_ptr(e, 0);
        eexpect_num(ejson_type(e), map[i].t);
        eexpect_num(ejson_free(e), map[i].cnt);
    }

    eexpect_num(ejson_free(r), 1);

    return ETEST_OK;

}


int t5_take(int argc, char* argv[])
{
    E_UNUSED(argc); E_UNUSED(argv);

    ETEST_RUN( test_takeH() );
    ETEST_RUN( test_takeT() );
    ETEST_RUN( test_takeO() );
    ETEST_RUN( test_takeP() );
    ETEST_RUN( test_takeK() );

    return ETEST_OK;
}

