#include <stdio.h>
#include <inttypes.h>

#include "etype.h"
#include "etest.h"
#include "ecompat.h"

#include "test_main.h"

erb ti;
erb ts;

int erb_new_test()
{
    ti = erb_new(EKEY_I);
    eunexpc_ptr(ti, 0);
    eexpect_num(eobj_typec((eobj)ti), ERB);
    eexpect_num(eobj_typeo((eobj)ti), EOBJ);
    eexpect_num(erb_size(ti), 0);

    ts = erb_new(EKEY_S);
    eunexpc_ptr(ts, 0);
    eexpect_num(eobj_typec((eobj)ts), ERB);
    eexpect_num(eobj_typeo((eobj)ts), EOBJ);
    eexpect_num(erb_size(ts), 0);

    return ETEST_OK;
}

int erb_add_test()
{
    char key[] = "012345678901234567890";

    {
        erb h = ti;

        for(int i = 0; i < 10; i++)
            erb_addI(h, ekey_i(i), i);
        eexpect_num(erb_size(h), 10);

        for(int i = 0; i < 10; i++)
            erb_addI(h, ekey_i(i), i);
        eexpect_num(erb_size(h), 10);
    }

    {
        erb h = ts;

        for(int i = 0; i < 10; i++)
            erb_addI(h, ekey_s(&key[i]), i);
        eexpect_num(erb_size(h), 10);

        for(int i = 0; i < 10; i++)
            erb_addI(h, ekey_s(&key[i]), i);
        eexpect_num(erb_size(h), 10);
    }

    return ETEST_OK;
}

int erb_clear_test()
{
    erb_clear(ti);
    eexpect_num(erb_size(ti), 0);

    erb_clear(ts);
    eexpect_num(erb_size(ts), 0);

    return ETEST_OK;
}

int erb_k_test()
{
    eobj o; int i; char key[32] = "012345678901234567890";

    {
        erb h = ti;

        erb_addI(h, ekey_i(1), 1);
        erb_addF(h, ekey_i(2), 2);
        erb_addP(h, ekey_i(3), (cptr)3);
        erb_addS(h, ekey_i(4), "4");
        erb_addR(h, ekey_i(5), 5);
        eexpect_num(erb_size(h), 5);

        /// -- test 1
        i = 1;
        o = erb_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 1);
        eexpect_num(eobj_valF(o), 1.0);

        i = 2;
        o = erb_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 2);
        eexpect_num(eobj_valF(o), 2.0);

        i = 3;
        o = erb_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), EPTR);
        eexpect_ptr(eobj_valP(o), (cptr)3);

        i = 4;
        o = erb_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ESTR);
        eexpect_str(eobj_valS(o), "4");
        eexpect_num(eobj_len(o), 1);

        i = 5;
        o = erb_k(h, ekey_i(i));
        eexpect_num(eobj_typeo(o), ERAW);
        eexpect_raw(eobj_valR(o), "\0\0\0\0\0", 5);
        eexpect_num(eobj_len(o), 5);

        i = 1;
        eexpect_num(eobj_typeo(erb_k(h, ekey_i(i))), ENUM);
        eexpect_num(erb_kValI(h, ekey_i(i)), 1);
        eexpect_num(erb_kValF(h, ekey_i(i)), 1.0);

        i = 2;
        eexpect_num(eobj_typeo(erb_k(h, ekey_i(i))), ENUM);
        eexpect_num(erb_kValI(h, ekey_i(i)), 2);
        eexpect_num(erb_kValF(h, ekey_i(i)), 2.0);

        i = 3;
        eexpect_num(eobj_typeo(erb_k(h, ekey_i(i))), EPTR);
        eexpect_ptr(erb_kValP(h, ekey_i(i)), (cptr)3);

        i = 4;
        eexpect_num(eobj_typeo(erb_k(h, ekey_i(i))), ESTR);
        eexpect_str(erb_kValS(h, ekey_i(i)), "4");

        i = 5;
        eexpect_num(eobj_typeo(erb_k(h, ekey_i(i))), ERAW);
        eexpect_raw(erb_kValR(h, ekey_i(i)), "\0\0\0\0\0", 5);

        ETEST_RUN(erb_clear_test());

        /// -- test 3
        for(i = 0; i < 100; i++)
        {
            erb_addI(h, ekey_i(i), i);
        }
        for(i = 0; i < 100; i++)
        {
            eexpect_num(erb_kValI(h, ekey_i(i)), i);
        }
    }

    {
        erb h = ts;

        erb_addI(h, ekey_s(&key[1]), 1);
        erb_addF(h, ekey_s(&key[2]), 2);
        erb_addP(h, ekey_s(&key[3]), (cptr)3);
        erb_addS(h, ekey_s(&key[4]), "4");
        erb_addR(h, ekey_s(&key[5]), 5);
        eexpect_num(erb_size(h), 5);

        /// -- test 1
        i = 1;
        o = erb_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 1);
        eexpect_num(eobj_valF(o), 1.0);

        i = 2;
        o = erb_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ENUM);
        eexpect_num(eobj_valI(o), 2);
        eexpect_num(eobj_valF(o), 2.0);

        i = 3;
        o = erb_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), EPTR);
        eexpect_ptr(eobj_valP(o), (cptr)3);

        i = 4;
        o = erb_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ESTR);
        eexpect_str(eobj_valS(o), "4");
        eexpect_num(eobj_len(o), 1);

        i = 5;
        o = erb_k(h, ekey_s(&key[i]));
        eexpect_num(eobj_typeo(o), ERAW);
        eexpect_raw(eobj_valR(o), "\0\0\0\0\0", 5);
        eexpect_num(eobj_len(o), 5);

        /// -- test 2
        i = 1;
        eexpect_num(eobj_typeo(erb_k(h, ekey_s(&key[i]))), ENUM);
        eexpect_num(erb_kValI(h, ekey_s(&key[i])), 1);
        eexpect_num(erb_kValF(h, ekey_s(&key[i])), 1.0);

        i = 2;
        eexpect_num(eobj_typeo(erb_k(h, ekey_s(&key[i]))), ENUM);
        eexpect_num(erb_kValI(h, ekey_s(&key[i])), 2);
        eexpect_num(erb_kValF(h, ekey_s(&key[i])), 2.0);

        i = 3;
        eexpect_num(eobj_typeo(erb_k(h, ekey_s(&key[i]))), EPTR);
        eexpect_ptr(erb_kValP(h, ekey_s(&key[i])), (cptr)3);
        i = 4;
        eexpect_num(eobj_typeo(erb_k(h, ekey_s(&key[i]))), ESTR);
        eexpect_str(erb_kValS(h, ekey_s(&key[i])), "4");

        i = 5;
        eexpect_num(eobj_typeo(erb_k(h, ekey_s(&key[i]))), ERAW);
        eexpect_raw(erb_kValR(h, ekey_s(&key[i])), "\0\0\0\0\0", 5);

        ETEST_RUN(erb_clear_test());

        /// -- test 3
        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            erb_addI(h, ekey_s(key), i);
        }
        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            eexpect_num(erb_kValI(h, ekey_s(key)), i);
        }
    }

    ETEST_RUN(erb_clear_test());

    return ETEST_OK;
}

int erb_free_test()
{
    if(ti)
    {
        eexpect_num(erb_free(ti) > 0, 1);
        ti = 0;
    }

    if(ts)
    {
        eexpect_num(erb_free(ts) > 0, 1);
        ts = 0;
    }

    return ETEST_OK;
}

int erb_addo_test()
{
    erb h = ti;

    erb_addO(h, ekey_i(1), erb_newO(EFALSE, 0));
    erb_addO(h, ekey_i(2), erb_newO(ETRUE,  0));
    erb_addO(h, ekey_i(3), erb_newO(ENULL,  0));
    erb_addO(h, ekey_i(4), erb_newO(ENUM,   0));
    erb_addO(h, ekey_i(5), erb_newO(EPTR,   0));
    erb_addO(h, ekey_i(6), erb_newO(ESTR,   0));
    erb_addO(h, ekey_i(7), erb_newO(EOBJ,   0));

    ETEST_RUN(erb_free_test());

    return ETEST_OK;
}

int erb_itr_test()
{
    char buf[32], key[32]; i64 i;

    {
        erb h = ti = erb_new(EKEY_I);

        for(i = 0; i < 200; i++)
        {
            sprintf(buf, "%"PRId64"", i+100000000000);
            erb_addS(h, ekey_i(i), buf);
        }
        eexpect_num(erb_size(h), 200);

        i = 0;
        erb_foreach(h, itr)
        {
            sprintf(buf, "%"PRId64"", i+100000000000);
            eexpect_str(erb_kValS(h, ekey_i(i)), buf);
            eexpect_str(eobj_valS(itr), buf);
            i++;
        }
    }

    {
        erb h = ts = erb_new(EKEY_S);

        for(i = 0; i < 200; i++)
        {
            sprintf(key, "%04"PRId64"", i);
            sprintf(buf, "%"PRId64"", i+100000000000);
            erb_addS(h, ekey_s(key), buf);
        }
        eexpect_num(erb_size(h), 200);

        i = 0;
        erb_foreach(h, itr)
        {
            sprintf(key, "%04"PRId64"", i);
            sprintf(buf, "%"PRId64"", i+100000000000);
            eexpect_str(erb_kValS(h, ekey_s(key)), buf);
            eexpect_str(eobj_valS(itr), buf);
            i++;
        }
    }

    ETEST_RUN(erb_free_test());

    return ETEST_OK;
}

int erb_take_test()
{
    int i;  char key[32];

    {
        erb h = ti = erb_new(EKEY_I);

        for(i = 0; i < 100; i++)
            erb_addI(h, ekey_i(i), i);

        while(erb_size(h))
        {
            i = rand() % 100;

            erb_freeOne(h, ekey_i(i));

            erb_foreach(h, itr)
            {
                eexpect_ptr(erb_k(h, eobj_key(itr)), itr);
            }
        }
    }

    {
        erb h = ts = erb_new(EKEY_S);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%d", i);
            erb_addI(h, ekey_s(key), i);
        }

        while(erb_size(h))
        {
            i = rand() % 100;

            sprintf(key, "%d", i);

            erb_freeOne(h, ekey_s(key));

            erb_foreach(h, itr)
            {
                eexpect_ptr(erb_k(h, eobj_key(itr)), itr);
            }
        }
    }

    ETEST_RUN(erb_free_test());

    return ETEST_OK;
}

int erb_addM_test()
{
    int i; char key[16];

    {
        erb h = ti = erb_new(EKEY_I);

        for(i = 0; i < 100; i++)
        {
            erb_addI(h, ekey_i(i), i);
        }

        eexpect_num(erb_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            erb_addI(h, ekey_i(i), i);
        }

        eexpect_num(erb_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            erb_addMI(h, ekey_i(i), i);
        }

        eexpect_num(erb_size(h), 200);

        i = 0;
        erb_foreach(h, itr)
        {
            eexpect_num(eobj_valI(itr), (int)(i / 2));

            i++;
        }
    }

    {
        erb h = ts = erb_new(EKEY_S);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%03d", i);
            erb_addI(h, ekey_s(key), i);
        }

        eexpect_num(erb_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%03d", i);
            erb_addI(h, ekey_s(key), i);
        }

        eexpect_num(erb_size(h), 100);

        for(i = 0; i < 100; i++)
        {
            sprintf(key, "%03d", i);
            erb_addMI(h, ekey_s(key), i);
        }

        eexpect_num(erb_size(h), 200);

        i = 0;
        erb_foreach(h, itr)
        {
            eexpect_num(eobj_valI(itr), (int)(i / 2));

            i++;
        }
    }

    ETEST_RUN(erb_free_test());

    return ETEST_OK;
}

int test_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN(erb_new_test());
    ETEST_RUN(erb_add_test());
    ETEST_RUN(erb_clear_test());
    ETEST_RUN(erb_k_test());
    ETEST_RUN(erb_free_test());
    ETEST_RUN(erb_itr_test());
    ETEST_RUN(erb_take_test());
    ETEST_RUN(erb_addM_test());

    return ETEST_OK;
}
