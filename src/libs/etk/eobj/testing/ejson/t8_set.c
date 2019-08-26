/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eutils.h"

#include "ejson.h"

char key[32] = "01234567890";
char vals[32] = {0};

static constr itostr(int i)
{
    static char __[32];

    sprintf(__, "%d", i);

    return __;
}

#define CHILD_CNT 10

static ejson gen_test_ejson(etypeo type, int deep)
{
    ejson r; int i;

    if(deep <= 0)
    {
        return ejson_new(type, EVAL_ZORE);
    }
    else if(deep == 1)
    {
        r = ejson_new(type, EVAL_ZORE);

        i = -1;
        ++i; ejson_addT(r, &key[i], EFALSE);
        ++i; ejson_addT(r, &key[i], ETRUE );
        ++i; ejson_addT(r, &key[i], ENULL );
        ++i; ejson_addI(r, &key[i], 1);
        ++i; ejson_addF(r, &key[i], 2.0);
        ++i; ejson_addS(r, &key[i], &key[i]);
        ++i; ejson_addP(r, &key[i], &key[i]);
        ++i; ejson_addR(r, &key[i], 9);
        ++i; ejson_addT(r, &key[i], EOBJ);
        ++i; ejson_addT(r, &key[i], EARR);

        return r;
    }
    else if(deep > 1)
    {
        deep--;

        r = ejson_new(type, EVAL_ZORE);

        i = -1;
        ++i; ejson_addT(r, &key[i], EFALSE);
        ++i; ejson_addT(r, &key[i], ETRUE );
        ++i; ejson_addT(r, &key[i], ENULL );
        ++i; ejson_addI(r, &key[i], 1);
        ++i; ejson_addF(r, &key[i], 2.0);
        ++i; ejson_addS(r, &key[i], &key[i]);
        ++i; ejson_addP(r, &key[i], &key[i]);
        ++i; ejson_addR(r, &key[i], 9);
        ++i; ejson_addO(r, &key[i], gen_test_ejson(EOBJ, deep));
        ++i; ejson_addO(r, &key[i], gen_test_ejson(EARR, deep));

        return r;
    }

    return 0;
}

static int test_resetTk(ejson r, etypeo type)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetT(r, &key[i], type);

                        eexpect_num(ejson_rType(r, &key[i]),  type);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetT(r, itostr(i), type);

                        if(!ejson_len(r))
                            continue;

                        eexpect_num(ejson_rType(r, itostr(i)),  type);
                    }
                    break;

        default:    return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetTi(ejson r, etypeo type)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetT(r, &key[i], type);

                        eexpect_num(ejson_rType(r, &key[i]),  type);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_iSetT(r, i, type);

                        eexpect_num(ejson_iType(r, i),  type);
                    }
                    break;

        default:    return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetIk(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetI(r, &key[i], 123);

                        eexpect_num(ejson_rType(r, &key[i]),  ENUM);
                        eexpect_num(ejson_rValI(r, &key[i]),  123);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetI(r, itostr(i), 123);

                        if(!ejson_len(r))
                            continue;

                        eexpect_num(ejson_rType(r, itostr(i)),  ENUM);
                        eexpect_num(ejson_rValI(r, itostr(i)),  123);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetIi(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetI(r, &key[i], 123);

                        eexpect_num(ejson_rType(r, &key[i]),  ENUM);
                        eexpect_num(ejson_rValI(r, &key[i]),  123);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_iSetI(r, i, 123);

                        eexpect_num(ejson_iType(r, i),  ENUM);
                        eexpect_num(ejson_iValI(r, i),  123);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetFk(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetF(r, &key[i], 123.12);

                        eexpect_num(ejson_rType(r, &key[i]),  ENUM);
                        eexpect_num(ejson_rValF(r, &key[i]),  123.12);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetF(r, itostr(i), 123.12);

                        if(!ejson_len(r))
                            continue;

                        eexpect_num(ejson_rType(r, itostr(i)),  ENUM);
                        eexpect_num(ejson_rValF(r, itostr(i)),  123.12);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetFi(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetF(r, &key[i], 123.12);

                        eexpect_num(ejson_rType(r, &key[i]),  ENUM);
                        eexpect_num(ejson_rValF(r, &key[i]),  123.12);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_iSetF(r, i, 123.12);

                        eexpect_num(ejson_iType(r, i),  ENUM);
                        eexpect_num(ejson_iValF(r, i),  123.12);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetSk(ejson r)
{

#define STR      "123456"
#define STR_LEN  strlen(STR)

    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetS(r, &key[i], STR);

                        eexpect_num(ejson_rType(r, &key[i]),  ESTR);
                        eexpect_num(ejson_rLen (r, &key[i]),  STR_LEN);
                        eexpect_str(ejson_rValS(r, &key[i]),  STR );
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetS(r, itostr(i), STR);

                        if(!ejson_len(r))
                            continue;

                        eexpect_num(ejson_rType(r, itostr(i)),  ESTR);
                        eexpect_num(ejson_rLen (r, itostr(i)),  STR_LEN);
                        eexpect_str(ejson_rValS(r, itostr(i)),  STR );
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetSi(ejson r)
{

#define STR      "123456"
#define STR_LEN  strlen(STR)

    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetS(r, &key[i], STR);

                        eexpect_num(ejson_rType(r, &key[i]),  ESTR);
                        eexpect_num(ejson_rLen (r, &key[i]),  STR_LEN);
                        eexpect_str(ejson_rValS(r, &key[i]),  STR );
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_iSetS(r, i, STR);

                        eexpect_num(ejson_iType(r, i),  ESTR);
                        eexpect_num(ejson_iLen (r, i),  STR_LEN);
                        eexpect_str(ejson_iValS(r, i),  STR );
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetPk(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetP(r, &key[i], &key[i]);

                        eexpect_num(ejson_rType(r, &key[i]),  EPTR);
                        eexpect_ptr(ejson_rValP(r, &key[i]),  &key[i]);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetP(r, itostr(i), &key[i]);

                        if(!ejson_len(r))
                            continue;

                        eexpect_num(ejson_rType(r, itostr(i)),  EPTR);
                        eexpect_ptr(ejson_rValP(r, itostr(i)),  &key[i]);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetPi(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetP(r, &key[i], &key[i]);

                        eexpect_num(ejson_rType(r, &key[i]),  EPTR);
                        eexpect_ptr(ejson_rValP(r, &key[i]),  &key[i]);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_iSetP(r, i, &key[i]);

                        eexpect_num(ejson_iType(r, i),  EPTR);
                        eexpect_ptr(ejson_iValP(r, i),  &key[i]);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetRk(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetR(r, &key[i], i);

                        eexpect_num(ejson_rType(r, &key[i]),  ERAW);
                        eexpect_num(ejson_rLen (r, &key[i]),  i);
                        eexpect_raw(ejson_rValR(r, &key[i]),  vals, i);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetR(r, itostr(i), i);

                        if(!ejson_len(r))
                            continue;

                        eexpect_num(ejson_rType(r, itostr(i)),  ERAW);
                        eexpect_num(ejson_rLen (r, itostr(i)),  i);
                        eexpect_raw(ejson_rValR(r, itostr(i)),  vals, i);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_resetRi(ejson r)
{
    uint i;

    switch (ejson_type(r))
    {
        case EOBJ:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_rSetR(r, &key[i], i);

                        eexpect_num(ejson_rType(r, &key[i]),  ERAW);
                        eexpect_num(ejson_rLen (r, &key[i]),  i);
                        eexpect_raw(ejson_rValR(r, &key[i]),  vals, i);
                    }
                    break;

        case EARR:  for(i = 0; i < CHILD_CNT; i++)
                    {
                        ejson_iSetR(r, i, i);

                        eexpect_num(ejson_iType(r, i),  ERAW);
                        eexpect_num(ejson_iLen (r, i),  i);
                        eexpect_raw(ejson_iValR(r, i),  vals, i);
                    }
                    break;

        default :   return ETEST_ERR;
    }

    return ETEST_OK;
}

static int test_create_in_obj_k()
{
    ejson r; int deep; etypeo t = EOBJ;

    deep = 0;
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EFALSE) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ETRUE ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ENULL ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EOBJ  ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EARR  ) ); ejson_free(r);

    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetIk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetFk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetSk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetPk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetRk(r        ) ); ejson_free(r);

    return ETEST_OK;
}

static int test_create_in_arr_k()
{
    ejson r; int deep; etypeo t = EARR;

    deep = 0;
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EFALSE) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ETRUE ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ENULL ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EOBJ  ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EARR  ) ); ejson_free(r);

    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetIk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetFk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetSk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetPk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetRk(r        ) ); ejson_free(r);

    return ETEST_OK;
}

static int test_reset_in_obj_k()
{
    ejson r; int deep; etypeo t = EOBJ;

    deep = 1;
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EFALSE) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ETRUE ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ENULL ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EOBJ  ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EARR  ) ); ejson_free(r);

    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetIk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetFk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetSk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetPk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetRk(r        ) ); ejson_free(r);

    return ETEST_OK;
}

static int test_reset_in_arr_k()
{
    ejson r; int deep; etypeo t = EARR;

    deep = 1;
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EFALSE) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ETRUE ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, ENULL ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EOBJ  ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTk(r, EARR  ) ); ejson_free(r);

    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetIk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetFk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetSk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetPk(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetRk(r        ) ); ejson_free(r);

    return ETEST_OK;
}

static int test_reset_in_obj_i()
{
    ejson r; int deep; etypeo t = EOBJ;

    deep = 1;
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, EFALSE) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, ETRUE ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, ENULL ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, EOBJ  ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, EARR  ) ); ejson_free(r);

    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetIi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetFi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetSi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetPi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetRi(r        ) ); ejson_free(r);

    return ETEST_OK;
}

static int test_reset_in_arr_i()
{
    ejson r; int deep; etypeo t = EARR;

    deep = 1;
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, EFALSE) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, ETRUE ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, ENULL ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, EOBJ  ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetTi(r, EARR  ) ); ejson_free(r);

    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetIi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetFi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetSi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetPi(r        ) ); ejson_free(r);
    r = gen_test_ejson(t, deep); ETEST_RUN( test_resetRi(r        ) ); ejson_free(r);

    return ETEST_OK;
}

static int test_create_in_obj_p()
{
    typedef struct {constr keys; etypeo t; } _IN_;

    ejson r; int i;

    _IN_ map[] = {
        {"obj.false",   EFALSE},
        {"obj.true" ,   ETRUE },
        {"obj.null" ,   ENULL },
        {"obj.i64"  ,   ENUM  },
        {"obj.f64"  ,   ENUM  },
        {"obj.str"  ,   ESTR  },
        {"obj.ptr"  ,   EPTR  },
        {"obj.raw"  ,   ERAW  },
        {"obj.obj"  ,   EOBJ  },
        {"obj.arr"  ,   EARR  },

    };

    r = ejson_new(EOBJ, EVAL_ZORE);

    ejson_addO(r, "obj", gen_test_ejson(EOBJ, 0));

    /**
      * {
      *     "obj": {}
      * }
      *
      */

    i = -1;

    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);

    ejson_free(r);

    return ETEST_OK;
}

static int test_create_in_arr_p()
{
    typedef struct {constr keys; etypeo t; } _IN_;

    ejson r; int i;

    _IN_ map[] = {
        {"obj.false",   EOBJ_UNKNOWN},
        {"obj.true" ,   EOBJ_UNKNOWN },
        {"obj.null" ,   EOBJ_UNKNOWN },
        {"obj.i64"  ,   EOBJ_UNKNOWN  },
        {"obj.f64"  ,   EOBJ_UNKNOWN  },
        {"obj.str"  ,   EOBJ_UNKNOWN  },
        {"obj.ptr"  ,   EOBJ_UNKNOWN  },
        {"obj.raw"  ,   EOBJ_UNKNOWN  },
        {"obj.obj"  ,   EOBJ_UNKNOWN  },
        {"obj.arr"  ,   EOBJ_UNKNOWN  },

    };

    r = ejson_new(EOBJ, EVAL_ZORE);

    ejson_addO(r, "obj", gen_test_ejson(EARR, 0));

    /**
      * {
      *     "obj": []
      * }
      *
      */

    i = -1;

    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);
    i++;  ejson_pSetT(r, map[i].keys, map[i].t); eexpect_num( ejson_pType(r, map[i].keys),  map[i].t);

    ejson_free(r);

    return ETEST_OK;
}

static int test_reset_particular_k()
{
    ejson r;

    r = ejson_new(EOBJ, EVAL_ZORE);

    // 1. create automatically when path not exsit, we only create EOBJ obj
    ejson_pSetT(r, "obj.obj", EOBJ);
    ejson_pSetT(r, "obj.arr", EARR);
    eexpect_num(ejson_pType(r, "obj"    ), EOBJ);
    eexpect_num(ejson_pType(r, "obj.obj"), EOBJ);
    eexpect_num(ejson_pType(r, "obj.arr"), EARR);

    // 2. if EOBJ obj not contains the specific obj, create automatically, but not for EARR obj
    ejson_pSetT(r, "obj.obj.false", EFALSE);        // create
    ejson_pSetT(r, "obj.arr[0]"   , EFALSE);        // failed, not create EFALSE in 'obj.arr'
    eexpect_num(ejson_pType(r, "obj.obj.false"), EFALSE);
    eexpect_num(ejson_pType(r, "obj.arr[0]"   ), EOBJ_UNKNOWN);

    // 3. can not create or reset when expect obj can not be found because of a val obj
    ejson_pSetT(r, "obj.obj.false.true", ETRUE);    // failed, 'obj.obj.false' is invalid to create path
    //ejson_show(r);
    eexpect_ptr(ejson_p(r, "obj.obj.false.true"), 0);

    // 4. you can reset any obj which can be found no matter what it is, so be careful
    ejson_pSetT(r, "obj.obj", EARR);    // ok, now 'obj.obj' is a EARR obj, objs in prev one will be delete automatically
    //ejson_show(r);
    eexpect_num(ejson_pType(r, "obj.obj"), EARR);

    // 5. the created obj in path is EOBJ type
    ejson_pSetT(r, "[1][2][3][4]", ETRUE);
    //ejson_show(r);
    eexpect_num(ejson_pType(r, "1"), EOBJ);
    eexpect_num(ejson_pType(r, "1.2"), EOBJ);
    eexpect_num(ejson_pType(r, "1.2.3"), EOBJ);
    eexpect_num(ejson_pType(r, "1.2.3.4"), ETRUE);

    ejson_free(r);

    return ETEST_OK;
}

int t8_set(int argc, char* argv[])
{
    E_UNUSED(argc); E_UNUSED(argv);

    ETEST_RUN( test_create_in_obj_k() );
    ETEST_RUN( test_create_in_arr_k() );

    ETEST_RUN( test_reset_in_obj_k() );
    ETEST_RUN( test_reset_in_arr_k() );

    ETEST_RUN( test_reset_in_obj_i() );
    ETEST_RUN( test_reset_in_arr_i() );

    ETEST_RUN( test_create_in_obj_p() );
    ETEST_RUN( test_create_in_arr_p() );

    ETEST_RUN( test_reset_particular_k() );

    return ETEST_OK;
}

