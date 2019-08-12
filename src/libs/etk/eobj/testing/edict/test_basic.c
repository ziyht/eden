#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "etype.h"
#include "etest.h"
#include "ecompat.h"

#include "edict.h"

edict di;
edict ds;

int edict_new_test()
{
    di = edict_new(EKEY_I);
    eunexpc_ptr(di, 0);
    eexpect_num(eobj_typec((eobj)di), EDICT);
    eexpect_num(eobj_typeo((eobj)di), EOBJ);
    eexpect_num(edict_size(di), 0);

    ds = edict_new(EKEY_S);
    eunexpc_ptr(ds, 0);
    eexpect_num(eobj_typec((eobj)ds), EDICT);
    eexpect_num(eobj_typeo((eobj)ds), EOBJ);
    eexpect_num(edict_size(ds), 0);

    return ETEST_OK;
}

int edict_add_test()
{
    char key[] = "012345678901234567890";

    {
        edict d = di;

        for(int i = 0; i < 10; i++)
            edict_addI(d, ekey_i(i), i);
        eexpect_num(edict_size(d), 10);

        for(int i = 0; i < 10; i++)
            edict_addI(d, ekey_i(i), i);
        eexpect_num(edict_size(d), 10);

    }

    {
        edict d = ds;

        for(int i = 0; i < 10; i++)
            edict_addI(d, ekey_s(&key[i]), i);
        eexpect_num(edict_size(d), 10);

        for(int i = 0; i < 10; i++)
            edict_addI(d, ekey_s(&key[i]), i);
        eexpect_num(edict_size(d), 10);
    }

    return ETEST_OK;
}

int edict_clear_test()
{
    edict_clear(di);
    eexpect_num(edict_size(di), 0);

    edict_clear(ds);
    eexpect_num(edict_size(ds), 0);

    return ETEST_OK;
}

int edict_val_test()
{
    eobj o; int i; char key[] = "012345678901234567890";

    {
        edict d = di;

        i = 1; edict_addI(d, ekey_i(i), 1);
        i = 2; edict_addF(d, ekey_i(i), 2);
        i = 3; edict_addP(d, ekey_i(i), (cptr)3);
        i = 4; edict_addS(d, ekey_i(i), "4");
        i = 5; edict_addR(d, ekey_i(i), 5);
        eexpect_num(edict_size(d), 5);

        i = 1;
        o = edict_k(d, ekey_i(i));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 1);
        eexpect_num(eobj_valF(o), 1.0);

        i = 2;
        o = edict_k(d, ekey_i(i));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 2);
        eexpect_num(eobj_valF(o), 2.0);

        i = 3;
        o = edict_k(d, ekey_i(i));
        eexpect_num(eobj_typeo(o), EPTR);
        eexpect_ptr(eobj_valP(o), (cptr)3);

        i = 4;
        o = edict_k(d, ekey_i(i));
        eexpect_num(eobj_typeo(o), ESTR);
        eexpect_str(eobj_valS(o), "4");
        eexpect_num(eobj_len(o), 1);

        i = 5;
        o = edict_k(d, ekey_i(i));
        eexpect_num(eobj_typeo(o), ERAW);
        eexpect_raw(eobj_valR(o), "\0\0\0\0\0", 5);
        eexpect_num(eobj_len(o), 5);

        i = 1;
        eexpect_num(eobj_typeo(edict_k(d, ekey_i(i))), ENUM);
        eexpect_num(edict_kValI(d, ekey_i(i)), 1);
        eexpect_num(edict_kValF(d, ekey_i(i)), 1.0);

        i = 2;
        eexpect_num(eobj_typeo(edict_k(d, ekey_i(i))), ENUM);
        eexpect_num(edict_kValI(d, ekey_i(i)), 2);
        eexpect_num(edict_kValF(d, ekey_i(i)), 2.0);

        i = 3;
        eexpect_num(eobj_typeo(edict_k(d, ekey_i(i))), EPTR);
        eexpect_ptr(edict_kValP(d, ekey_i(i)), (cptr)3);

        i = 4;
        eexpect_num(eobj_typeo(edict_k(d, ekey_i(i))), ESTR);
        eexpect_str(edict_kValS(d, ekey_i(i)), "4");

        i = 5;
        eexpect_num(eobj_typeo(edict_k(d, ekey_i(i))), ERAW);
        eexpect_raw(edict_kValR(d, ekey_i(i)), "\0\0\0\0\0", 5);

        ETEST_RUN(edict_clear_test());

        /// -- test 3
        for(i = 0; i < 100; i++)
        {
            edict_addI(d, ekey_i(i), i);
        }
        for(i = 0; i < 100; i++)
        {
            eexpect_num(edict_kValI(d, ekey_i(i)), i);
        }
    }

    {
        edict d = ds;

        i = 1; edict_addI(d, ekey_s(&key[i]), 1);
        i = 2; edict_addF(d, ekey_s(&key[i]), 2);
        i = 3; edict_addP(d, ekey_s(&key[i]), (cptr)3);
        i = 4; edict_addS(d, ekey_s(&key[i]), "4");
        i = 5; edict_addR(d, ekey_s(&key[i]), 5);
        eexpect_num(edict_size(d), 5);

        /// -- test 1
        i = 1;
        o = edict_k(d, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 1);
        eexpect_num(eobj_valF(o), 1.0);

        i = 2;
        o = edict_k(d, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 2);
        eexpect_num(eobj_valF(o), 2.0);

        i = 3;
        o = edict_k(d, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), EPTR);
        eexpect_ptr(eobj_valP(o), (cptr)3);

        i = 4;
        o = edict_k(d, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ESTR);
        eexpect_str(eobj_valS(o), "4");
        eexpect_num(eobj_len(o), 1);

        i = 5;
        o = edict_k(d, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ERAW);
        eexpect_raw(eobj_valR(o), "\0\0\0\0\0", 5);
        eexpect_num(eobj_len(o), 5);

        /// -- test 2
        i = 1;
        eexpect_num(eobj_typeo(edict_k(d, ekey_s(&key[i]))), ENUM);
        eexpect_num(edict_kValI(d, ekey_s(&key[i])), 1);
        eexpect_num(edict_kValF(d, ekey_s(&key[i])), 1.0);

        i = 2;

        eexpect_num(eobj_typeo(edict_k(d, ekey_s(&key[i]))), ENUM);
        eexpect_num(edict_kValI(d, ekey_s(&key[i])), 2);
        eexpect_num(edict_kValF(d, ekey_s(&key[i])), 2.0);

        i = 3;
        eexpect_num(eobj_typeo(edict_k(d, ekey_s(&key[i]))), EPTR);
        eexpect_ptr(edict_kValP(d, ekey_s(&key[i])), (cptr)3);

        i = 4;
        eexpect_num(eobj_typeo(edict_k(d, ekey_s(&key[i]))), ESTR);
        eexpect_str(edict_kValS(d, ekey_s(&key[i])), "4");

        i = 5;
        eexpect_num(eobj_typeo(edict_k(d, ekey_s(&key[i]))), ERAW);
        eexpect_raw(edict_kValR(d, ekey_s(&key[i])), "\0\0\0\0\0", 5);

        ETEST_RUN(edict_clear_test());

        /// -- test 3
        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            edict_addI(d, ekey_s(key), i);
        }
        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            eexpect_num(edict_kValI(d, ekey_s(key)), i);
        }
    }

    ETEST_RUN(edict_clear_test());

    return ETEST_OK;
}

int edict_addo_test()
{
//    edict_addO(di, ekey_i(1), edict_newO(EFALSE, 0));
//    edict_addO(di, ekey_i(2), edict_newO(ETRUE,  0));
//    edict_addO(di, ekey_i(3), edict_newO(ENULL,  0));
//    edict_addO(di, ekey_i(4), edict_newO(ENUM,   0));
//    edict_addO(di, ekey_i(5), edict_newO(EPTR,   0));
//    edict_addO(di, ekey_i(6), edict_newO(ESTR,   0));
//    edict_addO(di, ekey_i(7), edict_newO(EOBJ,   0));

    return ETEST_OK;
}

int edict_free_test()
{
    if(di)
    {
        eexpect_num(edict_free(di) > 0, 1);
        di = 0;
    }

    if(ds)
    {
        eexpect_num(edict_free(ds) > 0, 1);
        ds = 0;
    }

    return ETEST_OK;
}

int edict_itr_test()
{
    char buf[32], key[32]; i64 i; editr ditr;

    {
        edict h = di = edict_new(EKEY_I);

        for(i = 0; i < 200; i++)
        {
            sprintf(buf, "%"PRId64"", i+100000000000);
            edict_addS(h, ekey_i(i), buf);
        }
        eexpect_num(edict_size(h), 200);

        i = 0; ditr = edict_getItr(h, 1);
        edict_foreach(ditr, itr)
        {
            sprintf(buf, "%"PRId64"", i+100000000000);
            eexpect_str(edict_kValS(h, ekey_i(i)), buf);
            i++;
        }
        edict_freeItr(ditr);
    }

    {
        edict h = ds = edict_new(EKEY_S);

        for(i = 0; i < 200; i++)
        {
            sprintf(key, "%"PRId64"", i);
            sprintf(buf, "%"PRId64"", i+100000000000);
            edict_addS(h, ekey_s(key), buf);
        }

        eexpect_num(edict_size(h), 200);

        i = 0; ditr = edict_getItr(h, 1);
        edict_foreach(ditr, obj)
        {
            sprintf(key, "%"PRId64"", i);
            sprintf(buf, "%"PRId64"", i+100000000000);
            eexpect_str(edict_kValS(h, ekey_s(key)), buf);
            i++;
        }
        edict_freeItr(ditr);
    }

    ETEST_RUN(edict_free_test());

    return ETEST_OK;
}

int edict_take_test()
{
    int i; char key[32];  editr ditr;

    {
        edict h = di = edict_new(EKEY_I);

        for(i = 0; i < 100; i++)
            edict_addI(h, ekey_i(i), i);

        ditr = edict_getItr(h, 1);
        while(edict_size(h))
        {
            while(1)
            {
                if(edict_freeOne(h, ekey_i(rand() % 100)))
                    break;
            }

            edict_resetItr(ditr, 0, 1);
            edict_foreach(ditr, itr)
            {
                eexpect_ptr(edict_k(h, eobj_key(itr)), itr);
            }
        }
        edict_freeItr(ditr);
    }

    {
        edict h = ds = edict_new(EKEY_S);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            edict_addI(h, ekey_s(key), i);
        }

        ditr = edict_getItr(h, 1);
        while(edict_size(h))
        {
            while(1)
            {
                sprintf(key, "%d", rand() % 100);
                if(edict_freeOne(h, ekey_s(key)))
                    break;
            }

            edict_resetItr(ditr, 0, 1);
            edict_foreach(ditr, itr)
            {
                eexpect_ptr(edict_k(h, eobj_key(itr)), itr);
            }
        }
        edict_freeItr(ditr);
    }

    ETEST_RUN(edict_free_test());

    return ETEST_OK;
}

int edict_addM_test()
{
    int i; editr ditr; char key[16];

    {
        char arr[100] = {0};

        edict h = di = edict_new(EKEY_I);

        for(i = 0; i < 100; i++)
        {
            edict_addI(h, ekey_i(i), i);
        }

        eexpect_num(edict_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            edict_addI(h, ekey_i(i), i);
        }

        eexpect_num(edict_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            edict_addMI(h, ekey_i(i), i);
        }

        eexpect_num(edict_size(h), 200);

        ditr = edict_getItr(h, 1);
        edict_foreach(ditr, itr)
        {
            arr[eobj_valI(itr)] ++;
        }
        edict_freeItr(ditr);

        for(int i = 0; i < 100; i++)
        {
            eexpect_num(arr[i], 2);
        }
    }

    {
        char arr[200] = {0};

        edict h = ds = edict_new(EKEY_S);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            edict_addI(h, ekey_s(key), i);
        }

        eexpect_num(edict_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            edict_addI(h, ekey_s(key), i);
        }

        eexpect_num(edict_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            edict_addMI(h, ekey_s(key), i);
        }

        eexpect_num(edict_size(h), 200);

        ditr = edict_getItr(h, 1);
        edict_foreach(ditr, itr)
        {
            arr[eobj_valI(itr)] ++;
        }
        edict_freeItr(ditr);

        for(int i = 0; i < 100; i++)
        {
            eexpect_num(arr[i], 2);
        }
    }

    ETEST_RUN(edict_free_test());

    return ETEST_OK;
}


int test_basic(int argc, char* argv[])
{
    ETEST_RUN(edict_new_test());
    ETEST_RUN(edict_add_test());
    ETEST_RUN(edict_clear_test());
    ETEST_RUN(edict_val_test());
    ETEST_RUN(edict_free_test());
    ETEST_RUN(edict_itr_test());
    ETEST_RUN(edict_take_test());
    ETEST_RUN(edict_addM_test());

    return ETEST_OK;
}
