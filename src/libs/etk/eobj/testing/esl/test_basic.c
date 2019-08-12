#include <stdio.h>
#include <inttypes.h>

#include "etest.h"
#include "ecompat.h"

#include "esl.h"

esl li;
esl ls;

int esl_new_test()
{
    li = esl_new(EKEY_I);
    eunexpc_ptr(li, 0);
    eexpect_num(eobj_typec((eobj)li), ESL);
    eexpect_num(eobj_typeo((eobj)li), EOBJ);
    eexpect_num(esl_size(li), 0);

    ls = esl_new(EKEY_S);
    eunexpc_ptr(ls, 0);
    eexpect_num(eobj_typec((eobj)ls), ESL);
    eexpect_num(eobj_typeo((eobj)ls), EOBJ);
    eexpect_num(esl_size(ls), 0);

    return ETEST_OK;
}

int esl_add_test()
{
    char key[] = "012345678901234567890";

    {
        esl l = li;

        for(int i = 0; i < 10; i++)
        {
            esl_addI(l, ekey_i(i), i);
        }
        eexpect_num(esl_size(l), 10);

        for(int i = 0; i < 10; i++)
            esl_addI(l, ekey_i(i), i);
        eexpect_num(esl_size(l), 10);
    }

    {
        esl l = ls;

        for(int i = 0; i < 10; i++)
            esl_addI(l, ekey_s(&key[i]), i);
        eexpect_num(esl_size(l), 10);

        for(int i = 0; i < 10; i++)
            esl_addI(l, ekey_s(&key[i]), i);
        eexpect_num(esl_size(l), 10);
    }

    return ETEST_OK;
}

int esl_clear_test()
{
    esl_clear(li);
    eexpect_num(esl_size(li), 0);

    esl_clear(ls);
    eexpect_num(esl_size(ls), 0);

    return ETEST_OK;
}

int esl_val_test()
{
    eobj o; int i; char key[32] = "012345678901234567890";

    {
        esl h = li;

        esl_addI(h, ekey_i(1), 1);
        esl_addF(h, ekey_i(2), 2);
        esl_addP(h, ekey_i(3), (cptr)3);
        esl_addS(h, ekey_i(4), "4");
        esl_addR(h, ekey_i(5), 5);
        eexpect_num(esl_size(h), 5);

        /// -- test 1
        i = 1;
        o = esl_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 1);
        eexpect_num(eobj_valF(o), 1.0);

        i = 2;
        o = esl_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 2);
        eexpect_num(eobj_valF(o), 2.0);

        i = 3;
        o = esl_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), EPTR);
        eexpect_ptr(eobj_valP(o), (cptr)3);

        i = 4;
        o = esl_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ESTR);
        eexpect_str(eobj_valS(o), "4");
        eexpect_num(eobj_len(o), 1);

        i = 5;
        o = esl_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ERAW);
        eexpect_raw(eobj_valR(o), "\0\0\0\0\0", 5);
        eexpect_num(eobj_len(o), 5);

        i = 1;
        eexpect_num(eobj_typeo(esl_k(h, ekey_i(i))), ENUM);
        eexpect_num(esl_kValI(h, ekey_i(i)), 1);
        eexpect_num(esl_kValF(h, ekey_i(i)), 1.0);

        i = 2;
        eexpect_num(eobj_typeo(esl_k(h, ekey_i(i))), ENUM);
        eexpect_num(esl_kValI(h, ekey_i(i)), 2);
        eexpect_num(esl_kValF(h, ekey_i(i)), 2.0);

        i = 3;
        eexpect_num(eobj_typeo(esl_k(h, ekey_i(i))), EPTR);
        eexpect_ptr(esl_kValP(h, ekey_i(i)), (cptr)3);

        i = 4;
        eexpect_num(eobj_typeo(esl_k(h, ekey_i(i))), ESTR);
        eexpect_str(esl_kValS(h, ekey_i(i)), "4");

        i = 5;
        eexpect_num(eobj_typeo(esl_k(h, ekey_i(i))), ERAW);
        eexpect_raw(esl_kValR(h, ekey_i(i)), "\0\0\0\0\0", 5);

        ETEST_RUN(esl_clear_test());

        /// -- test 3
        for(i = 0; i < 100; i++)
        {
            esl_addI(h, ekey_i(i), i);
        }
        for(i = 0; i < 100; i++)
        {
            eexpect_num(esl_kValI(h, ekey_i(i)), i);
        }
    }

    {
        esl h = ls;

        esl_addI(h, ekey_s(&key[1]), 1);
        esl_addF(h, ekey_s(&key[2]), 2);
        esl_addP(h, ekey_s(&key[3]), (cptr)3);
        esl_addS(h, ekey_s(&key[4]), "4");
        esl_addR(h, ekey_s(&key[5]), 5);
        eexpect_num(esl_size(h), 5);

        /// -- test 1
        i = 1;
        o = esl_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 1);
        eexpect_num(eobj_valF(o), 1.0);

        i = 2;
        o = esl_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 2);
        eexpect_num(eobj_valF(o), 2.0);

        i = 3;
        o = esl_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), EPTR);
        eexpect_ptr(eobj_valP(o), (cptr)3);

        i = 4;
        o = esl_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ESTR);
        eexpect_str(eobj_valS(o), "4");
        eexpect_num(eobj_len(o), 1);

        i = 5;
        o = esl_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ERAW);
        eexpect_raw(eobj_valR(o), "\0\0\0\0\0", 5);
        eexpect_num(eobj_len(o), 5);

        /// -- test 2
        i = 1;
        eexpect_num(eobj_typeo(esl_k(h, ekey_s(&key[i]))), ENUM);
        eexpect_num(esl_kValI(h, ekey_s(&key[i])), 1);
        eexpect_num(esl_kValF(h, ekey_s(&key[i])), 1.0);

        i = 2;
        eexpect_num(eobj_typeo(esl_k(h, ekey_s(&key[i]))), ENUM);
        eexpect_num(esl_kValI(h, ekey_s(&key[i])), 2);
        eexpect_num(esl_kValF(h, ekey_s(&key[i])), 2.0);

        i = 3;
        eexpect_num(eobj_typeo(esl_k(h, ekey_s(&key[i]))), EPTR);
        eexpect_ptr(esl_kValP(h, ekey_s(&key[i])), (cptr)3);
        i = 4;
        eexpect_num(eobj_typeo(esl_k(h, ekey_s(&key[i]))), ESTR);
        eexpect_str(esl_kValS(h, ekey_s(&key[i])), "4");

        i = 5;
        eexpect_num(eobj_typeo(esl_k(h, ekey_s(&key[i]))), ERAW);
        eexpect_raw(esl_kValR(h, ekey_s(&key[i])), "\0\0\0\0\0", 5);

        ETEST_RUN(esl_clear_test());

        /// -- test 3
        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            esl_addI(h, ekey_s(key), i);
        }
        eexpect_num(esl_size(h), 100);
        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            eexpect_num(esl_kValI(h, ekey_s(key)), i);
        }
    }

    ETEST_RUN(esl_clear_test());

    return ETEST_OK;
}

static int esl_free_test()
{
    if(li)
    {
        eexpect_num(esl_free(li) > 0, 1);
        li = 0;
    }

    if(ls)
    {
        eexpect_num(esl_free(ls) > 0, 1);
        ls = 0;
    }

    return ETEST_OK;
}

static int esl_addo_test()
{
    esl h = li = esl_new(EKEY_I);

    esl_addO(h, ekey_i(1), esl_newO(EFALSE, 0));
    esl_addO(h, ekey_i(2), esl_newO(ETRUE,  0));
    esl_addO(h, ekey_i(3), esl_newO(ENULL,  0));
    esl_addO(h, ekey_i(4), esl_newO(ENUM,   0));
    esl_addO(h, ekey_i(5), esl_newO(EPTR,   0));
    esl_addO(h, ekey_i(6), esl_newO(ESTR,   0));
    esl_addO(h, ekey_i(7), esl_newO(EOBJ,   0));

    ETEST_RUN(esl_free_test());

    return ETEST_OK;
}

static int esl_itr_test()
{
    char buf[32], key[32]; i64 i;

    {
        esl h = li = esl_new(EKEY_I);

        for(i = 0; i < 200; i++)
        {
            sprintf(buf, "%"PRId64"", i+100000000000);
            esl_addS(h, ekey_i(i), buf);
        }
        eexpect_num(esl_size(h), 200);

        i = 0;
        esl_foreach(h, itr)
        {
            sprintf(buf, "%"PRId64"", i+100000000000);
            eexpect_str(esl_kValS(h, ekey_i(i)), buf);
            eexpect_str(eobj_valS(itr), buf);
            i++;
        }
    }

    {
        esl h = ls = esl_new(EKEY_S);

        for(i = 0; i < 200; i++)
        {
            sprintf(key, "%04"PRId64"", i);
            sprintf(buf, "%"PRId64"", i+100000000000);
            esl_addS(h, ekey_s(key), buf);
        }
        eexpect_num(esl_size(h), 200);

        i = 0;
        esl_foreach(h, itr)
        {
            sprintf(key, "%04"PRId64"", i);
            sprintf(buf, "%"PRId64"", i+100000000000);
            eexpect_str(esl_kValS(h, ekey_s(key)), buf);
            eexpect_str(eobj_valS(itr), buf);
            i++;
        }
    }

    ETEST_RUN( esl_free_test() );

    return ETEST_OK;
}

static int esl_take_test()
{
    int i; char key[32];

    {
        esl h = li = esl_new(EKEY_I);

        for(i = 0; i < 100; i++)
            esl_addI(h, ekey_i(i), i);

        while(esl_size(h))
        {
            i = rand() % 100;

            esl_freeOne(h, ekey_i(i));

            esl_foreach(h, itr)
            {
                eexpect_ptr(esl_k(h, eobj_key(itr)), itr);
            }
        }
    }

    {
        esl h = ls = esl_new(EKEY_S);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            esl_addI(h, ekey_s(key), i);
        }

        while(esl_size(h))
        {
            i = rand() % 100;

            sprintf(key, "%d", i);

            esl_freeOne(h, ekey_s(key));

            esl_foreach(h, itr)
            {
                eexpect_ptr(esl_k(h, eobj_key(itr)), itr);
            }
        }
    }

    ETEST_RUN(esl_free_test());

    return ETEST_OK;
}

static int esl_addM_test()
{
    int i; char key[16];

    {
        esl h = li = esl_new(EKEY_I);

        for(i = 0; i < 100; i++)
        {
            esl_addI(h, ekey_i(i), i);
        }

        eexpect_num(esl_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            esl_addI(h, ekey_i(i), i);
        }

        eexpect_num(esl_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            esl_addMI(h, ekey_i(i), i);
        }

        eexpect_num(esl_size(h), 200);

        i = 0;
        esl_foreach(h, itr)
        {
            eexpect_num(eobj_valI(itr), (int)(i / 2));

            i++;
        }
    }

    {
        esl h = ls = esl_new(EKEY_S);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%03d", i);
            esl_addI(h, ekey_s(key), i);
        }

        eexpect_num(esl_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%03d", i);
            esl_addI(h, ekey_s(key), i);
        }

        eexpect_num(esl_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%03d", i);
            esl_addMI(h, ekey_s(key), i);
        }

        eexpect_num(esl_size(h), 200);

        i = 0;
        esl_foreach(h, itr)
        {
            eexpect_num(eobj_valI(itr), (int)(i / 2));

            i++;
        }
    }

    ETEST_RUN(esl_free_test());

    return ETEST_OK;
}

int test_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( esl_new_test  () );
    ETEST_RUN( esl_add_test  () );
    ETEST_RUN( esl_clear_test() );
    ETEST_RUN( esl_val_test  () );
    ETEST_RUN( esl_free_test () );
    ETEST_RUN( esl_addo_test () );
    ETEST_RUN( esl_itr_test  () );
    ETEST_RUN( esl_take_test () );
    ETEST_RUN( esl_addM_test () );

    return ETEST_OK;
}
