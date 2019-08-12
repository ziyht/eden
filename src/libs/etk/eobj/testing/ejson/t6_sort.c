/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "eutils.h"

#include "ejson.h"

static int ejson_sort_obj_test(){

    int i ;

    ejson e = ejson_new(EOBJ, EVAL_ZORE);

    ejson_addI(e, "9", 0);
    ejson_addI(e, "8", 1);
    ejson_addI(e, "7", 2);
    ejson_addI(e, "6", 3);
    ejson_addI(e, "5", 4);
    ejson_addI(e, "4", 5);
    ejson_addI(e, "3", 6);
    ejson_addI(e, "2", 7);
    ejson_addI(e, "1", 8);
    ejson_addI(e, "0", 9);

    ejson_sort(e, __KEYS_ACS);
    i = 9;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i--);
    }

    ejson_sort(e, __KEYS_DES);
    i = 0;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i++);
    }

    ejson_sort(e, __VALI_ACS);
    i = 0;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i++);
    }

    ejson_sort(e, __VALI_DES);
    i = 9;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i--);
    }

    ejson_free(e);

    return ETEST_OK;
}

static int  ejson_sort_arr_test()
{
    int i;
    ejson e = ejson_new(EARR, EVAL_ZORE);

    ejson_addJ(e, 0, "\"9\":0");
    ejson_addJ(e, 0, "\"8\":1");
    ejson_addJ(e, 0, "\"7\":2");
    ejson_addJ(e, 0, "\"6\":3");
    ejson_addJ(e, 0, "\"5\":4");
    ejson_addJ(e, 0, "\"4\":5");
    ejson_addJ(e, 0, "\"3\":6");
    ejson_addJ(e, 0, "\"2\":7");
    ejson_addJ(e, 0, "\"1\":8");
    ejson_addJ(e, 0, "\"0\":9");

    ejson_sort(e, __KEYS_ACS);
    i = 9;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i--);
    }

    ejson_sort(e, __KEYS_DES);
    i = 0;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i++);
    }

    ejson_sort(e, __VALI_ACS);
    i = 0;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i++);
    }

    ejson_sort(e, __VALI_DES);
    i = 9;
    ejson_foreach_s(e, itr)
    {
        eexpect_num(eobj_valI(itr), i--);
    }

    ejson_free(e);

    return ETEST_OK;
}

int t6_sort(int argc, char* argv[])
{
    E_UNUSED(argc); E_UNUSED(argv);

    ETEST_RUN( ejson_sort_obj_test() );
    ETEST_RUN( ejson_sort_arr_test() );

    return ETEST_OK;
}

