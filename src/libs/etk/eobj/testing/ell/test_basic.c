#include "ell.h"
#include "etest.h"

static ell l;

typedef struct obj_s{
    int  key;
    char val[16];
}obj_t, * obj;



static int __alloc_cnt;
static int __free_cnt;

static void __resetCnt()    { __alloc_cnt = __free_cnt = 0;}
static int  __checkCnt()    { eexpect_num(__alloc_cnt, __free_cnt); return ETEST_OK; }
static cptr __new()         { __alloc_cnt++; return emalloc(4); }
static void __free(eobj o)  { __free_cnt++ ; efree(o->p);   }

static int ell_new_test();
static int ell_push_test();
static int ell_appd_test();
static int ell_i_test();
static int ell_val_test();
static int ell_take_test();
static int ell_clear_test();
static int ell_free_test();

int ell_etest_basic()
{
    ETEST_RUN(ell_new_test());
    ETEST_RUN(ell_push_test());
    ETEST_RUN(ell_appd_test());
    ETEST_RUN(ell_i_test());
    ETEST_RUN(ell_val_test());
    ETEST_RUN(ell_take_test());
    ETEST_RUN(ell_clear_test());
    ETEST_RUN(ell_free_test());

    return ETEST_OK;
}

static int ell_new_test()
{
    l = ell_new();

    eunexpc_ptr(l, 0);
    eexpect_num(ell_size(l), 0);

    return ETEST_OK;
}

static int ell_push_test()
{
    int i;

    for(i = 0; i < 1000; i++)
    {
        ell_pushI(l, i);
    }
    eexpect_num(ell_size(l), 1000);
    ell_clear(l);


    // push 1 -> [1]
    eobj oi = ell_pushI(l, 1);
    eunexpc_ptr(oi, 0);
    eexpect_num(ell_size(l), 1);

    oi = ell_i(l, 0);
    eunexpc_ptr(oi, 0);
    eexpect_num(EOBJ_VALI(oi), 1);

    eexpect_num(ell_ivalI(l, 0), 1);
    eexpect_num(ell_ivalF(l, 0), 1.0);

    // push 2.0 -> [2.0, 1]
    oi = ell_pushF(l, 2);
    eunexpc_ptr(oi, 0);
    eexpect_num(ell_size(l), 2);

    oi = ell_i(l, 0);
    eunexpc_ptr(oi, 0);
    eexpect_num(EOBJ_VALF(oi), 2);

    eexpect_num(ell_ivalI(l, 0), 2);
    eexpect_num(ell_ivalF(l, 0), 2.0);

    ell_clear(l);

    return ETEST_OK;
}

static int ell_appd_test()
{
    int i;

    for(i = 0; i < 1000; i++)
    {
        ell_appdI(l, i);
    }
    eexpect_num(ell_size(l), 1000);
    ell_clear(l);

    // appd 1 -> [1]
    eobj oi = ell_appdI(l, 1);
    eunexpc_ptr(oi, 0);
    eexpect_num(ell_size(l), 1);

    oi = ell_i(l, 0);
    eunexpc_ptr(oi, 0);
    eexpect_num(EOBJ_VALI(oi), 1);

    eexpect_num(ell_ivalI(l, 0), 1);
    eexpect_num(ell_ivalF(l, 0), 1.0);

    // appd 2.0 -> [1, 2.0]
    eobj of = ell_appdF(l, 2);
    eunexpc_ptr(of, 0);
    eexpect_num(ell_size(l), 2);

    of = ell_i(l, 1);
    eunexpc_ptr(of, 0);
    eexpect_num(EOBJ_VALF(of), 2.0);

    eexpect_num(ell_ivalI(l, 1), 2);
    eexpect_num(ell_ivalF(l, 1), 2.0);

    ell_clear(l);

    return ETEST_OK;
}

static int ell_i_test()
{
    char data[] = "1234567890";

    eobj o, ao = ell_newO(EOBJ, 6);

    //memcpy(ao->r, data, 6);

    ell_appdF(l, 1.0);
    ell_appdI(l, 2);
    ell_appdP(l, 0);
    ell_appdS(l, "4");
    o =
    ell_appdR(l, 5);  memcpy(o->r, data, 5);
    ell_appdO(l, ao);

    eexpect_num(ell_size(l), 6);

    o = ell_i(l, 0);
    eexpect_num(eobj_typeo(o), ENUM);
    eexpect_num(eobj_valI(o), 1);
    eexpect_num(eobj_valF(o), 1.0);

    o = ell_i(l, 1);
    eexpect_num(eobj_typeo(o), ENUM);
    eexpect_num(eobj_valI(o), 2);
    eexpect_num(eobj_valF(o), 2.0);

    o = ell_i(l, 2);
    eexpect_num(eobj_typeo(o), EPTR);
    eexpect_ptr(eobj_valP(o), 0);

    o = ell_i(l, 3);
    eexpect_num(eobj_typeo(o), ESTR);
    eexpect_str(eobj_valS(o), "4");

    o = ell_i(l, 4);
    eexpect_num(eobj_typeo(o), ERAW);
    eexpect_num(eobj_len( o), 5);
    eexpect_raw(o->r, data, eobj_len( o));

    o = ell_i(l, 5);
    eexpect_num(eobj_typeo(o), EOBJ);
    eexpect_num(eobj_len( o), 0);

    ell_clear(l);

    {
        for(int i = 0; i < 1000; i++)
        {
            obj_t* o = (obj_t*)ell_appdR(l, sizeof(obj_t));

            o->key = i;
        }

        srand(0);
        int i = 0;
        while(i < 1000)
        {
            int idx = rand() % 1000;

            obj_t* o = (obj_t*)ell_i(l, idx);

            eexpect_num(o->key, idx);

            if(!o->val[0])
            {
                o->val[0] = 1;
                i++;
            }
        }

        ell_clear(l);
    }

    return ETEST_OK;
}

static int ell_val_test()
{
    int i; char data[] = "1234567890";

    eobj o, ao = ell_newO(EOBJ, 6);

    ell_appdF(l, 1.0);
    ell_appdI(l, 2);
    ell_appdP(l, 0);
    ell_appdS(l, "4");
    o =
    ell_appdR(l,  5 );    memcpy(o->r, data, 5);
    ell_appdO(l, ao );

    eexpect_num(ell_size(l), 6);

    i = 0;
    o = ell_i(l, i);
    eexpect_num(eobj_typeo(o), ENUM);
    eexpect_num(eobj_valI(o), 1);
    eexpect_num(eobj_valF(o), 1.0);

    i = 1;
    o = ell_i(l, i);
    eexpect_num(eobj_typeo(o), ENUM);
    eexpect_num(eobj_valI(o), 2);
    eexpect_num(eobj_valF(o), 2.0);

    i = 2;
    o = ell_i(l, i);
    eexpect_num(eobj_typeo(o), EPTR);
    eexpect_ptr(eobj_valP(o), 0);

    i = 3;
    o = ell_i(l, i);
    eexpect_num(eobj_typeo(o), ESTR);
    eexpect_str(eobj_valS(o), "4");

    i = 4;
    o = ell_i(l, i);
    eexpect_num(eobj_typeo(o), ERAW);
    eexpect_num(eobj_len( o), 5);

    i = 5;
    o = ell_i(l, i);
    eexpect_num(eobj_typeo(o), EOBJ);

    /// -- test 2
    i = 0;
    eexpect_num(eobj_typeo(ell_i(l, i)), ENUM);
    eexpect_num(ell_ivalI(l, i), 1);
    eexpect_num(ell_ivalF(l, i), 1.0);

    i = 1;
    eexpect_num(eobj_typeo(ell_i(l, i)), ENUM);
    eexpect_num(ell_ivalI(l, i), 2);
    eexpect_num(ell_ivalF(l, i), 2.0);

    i = 2;
    eexpect_num(eobj_typeo(ell_i(l, i)), EPTR);
    eexpect_ptr(ell_ivalP(l, i), 0);

    i = 3;
    eexpect_num(eobj_typeo(ell_i(l, i)), ESTR);
    eexpect_str(ell_ivalS(l, i), "4");

    i = 4;
    eexpect_num(eobj_typeo(ell_i(l, i)), ERAW);
    eexpect_num(eobj_len  (ell_i(l, i)), 5);
    eexpect_raw(ell_ivalR(l, i), data, eobj_len (ell_i(l, i)));

    i = 5;
    eexpect_num(eobj_typeo(ell_i(l, i)), EOBJ);

    ell_clear(l);

    {
        for(int i = 0; i < 1000; i++)
        {
            obj_t* o = (obj_t*)ell_appdR(l, sizeof(obj_t));

            o->key = i;
        }

        srand(0);
        int i = 0;
        while(i < 1000)
        {
            int idx = rand() % 1000;

            obj_t* o = (obj_t*)ell_i(l, idx);

            eexpect_num(o->key, idx);

            if(!o->val[0])
            {
                o->val[0] = 1;
                i++;
            }
        }

        ell_clear(l);
    }

    return ETEST_OK;
}

static int ell_take_test()
{
    int scale = 100;

    {
        for(int i = 0; i < scale; i++)
        {
            eobj o = ell_appdR(l, sizeof(eobj_t));

            o->v.i32a[0] = i;
        }

        int i, idx;
        while(ell_size(l))
        {
            idx = rand() % ell_size(l);

            eobj o = ell_i(l, idx);

            i = 0;
            ell_foreach(l, itr)
            {
                if(itr == o)
                {
                    eexpect_num(i, idx);
                    break;
                }

                i++;
            }

            o->v.i32a[1] = 1;

            o = ell_first(l);
            while(o && o->v.i32a[1])
            {
                o = ell_takeH(l);
                ell_freeO(0, o);

                o = ell_first(l);
            }
        }
        ell_clear(l);
    }

    {
        for(int i = 0; i < scale; i++)
        {
            eobj o = ell_appdR(l, sizeof(eobj_t));

            o->v.i32a[0] = i;
        }

        int i, idx;
        while(ell_size(l))
        {
            idx = rand() % ell_size(l);

            eobj o = ell_i(l, idx);

            i = 0;
            ell_foreach(l, itr)
            {
                if(itr == o)
                {
                    eexpect_num(i, idx);
                    break;
                }

                i++;
            }

            o->v.i32a[1] = 1;

            o = ell_last(l);
            while(o && o->v.i32a[1])
            {
                o = ell_takeT(l);
                ell_freeO(0, o);

                o = ell_last(l);
            }
        }
        ell_clear(l);
    }

    {
        for(int i = 0; i < scale; i++)
        {
            eobj o = ell_appdR(l, sizeof(eobj_t));

            o->v.i32a[0] = i;
        }

        int i, idx;
        while(ell_size(l))
        {
            idx = rand() % ell_size(l);

            eobj o = ell_i(l, idx);

            i = 0;
            ell_foreach(l, itr)
            {
                if(itr == o)
                {
                    eexpect_num(i, idx);
                    break;
                }

                i++;
            }

            o->v.i32a[1] = 1;

            if(ell_size(l) % 2 == 0)
            {
                o = ell_first(l);
                while(o && o->v.i32a[1])
                {
                    o = ell_takeH(l);
                    ell_freeO(0, o);

                    o = ell_first(l);
                }
            }
            else
            {
                o = ell_last(l);
                while(o && o->v.i32a[1])
                {
                    o = ell_takeT(l);
                    ell_freeO(0, o);

                    o = ell_last(l);
                }
            }
        }
        ell_clear(l);
    }

    return ETEST_OK;
}

static int ell_clear_test()
{
    int scale = 100;

    /// -- ell_clear()
    {
        for(int i = 0; i < scale; i++)
        {
            ell_appdP(l, 0);
        }

        ell_clear(l);

        eexpect_num(ell_size(l), 0);
    }

    /// -- ell_clearEx()
    {
        for(int i = 0; i < scale; i++)
        {
            ell_appdP(l, __new());
        }

        ell_clearEx(l, __free);
        eexpect_num(ell_size(l), 0);
        ETEST_RUN(__checkCnt());

        __resetCnt();
    }

    return ETEST_OK;
}

static int ell_free_test()
{
    int scale = 100;

    /// -- ell_freeH()
    {
        for(int i = 0; i < scale; i++)
        {
            eobj o = ell_appdR(l, sizeof(eobj_t));

            o->v.i32a[0] = i;
        }

        int i, idx;
        while(ell_size(l))
        {
            idx = rand() % ell_size(l);

            eobj o = ell_i(l, idx);

            i = 0;
            ell_foreach(l, itr)
            {
                if(itr == o)
                {
                    eexpect_num(i, idx);
                    break;
                }

                i++;
            }

            o->v.i32a[1] = 1;

            o = ell_first(l);
            while(o && o->v.i32a[1])
            {
                ell_freeH(l);

                o = ell_first(l);
            }
        }
        ell_clear(l);
    }

    /// -- ell_freeT()
    {
        for(int i = 0; i < scale; i++)
        {
            eobj o = ell_appdR(l, sizeof(eobj_t));

            o->v.i32a[0] = i;
        }

        int i, idx;
        while(ell_size(l))
        {
            idx = rand() % ell_size(l);

            eobj o = ell_i(l, idx);

            i = 0;
            ell_foreach(l, itr)
            {
                if(itr == o)
                {
                    eexpect_num(i, idx);
                    break;
                }

                i++;
            }

            o->v.i32a[1] = 1;

            o = ell_last(l);
            while(o && o->v.i32a[1])
            {
                ell_freeT(l);

                o = ell_last(l);
            }
        }
        ell_clear(l);
    }

    /// -- ell_freeI() from last
    {
        for(int i = 0; i < scale; i++)
        {
            ell_appdI(l, i);
        }

        while(ell_size(l))
        {
            eexpect_num(ell_ivalI(l, ell_size(l) - 1), ell_size(l) - 1);

            ell_freeI(l, ell_size(l) - 1);
        }
    }

    /// -- ell_freeI() from first
    {
        for(int i = 0; i < scale; i++)
        {
            ell_appdI(l, i);
        }

        while(ell_size(l))
        {
            eexpect_num(ell_ivalI(l, 0), scale - ell_size(l));

            ell_freeI(l, 0);
        }
    }

    /// -- ell_freeO()
    {
        for(int i = 0; i < scale; i++)
        {
            ell_appdI(l, i);
        }

        while(ell_size(l))
        {
            eobj o = ell_i(l, rand() % ell_size(l));

            ell_freeO(l, o);
        }
    }

    /// -- ell_freeEx()
    {
        for(int i = 0; i < scale; i++)
        {
            ell_appdP(l, __new());
        }

        ell_freeEx(l, __free);
        ETEST_RUN(__checkCnt());

        __resetCnt();
    }

    return ETEST_OK;
}


void my_list_show(ell l)
{
    eobj o;

    printf("len: %d\n", ell_size(l));

    for(o = ell_first(l); o; o = ell_next(o))
    {
        printf("  %4d: %s\n", ((obj)o)->key, ((obj)o)->val);
    }

    printf("\n");
    fflush(stdout);
}

void my_list_showr(ell l)
{
    eobj o;

    printf("len: %d\n", ell_size(l));

    for(o = ell_last(l); o; o = ell_prev(o))
    {
        printf("  %4d: %s\n", ((obj)o)->key, ((obj)o)->val);
    }

    printf("\n");
    fflush(stdout);
}

int ell_basic_test()
{
    ell l; eobj o;

    l = ell_new();

    for(int i = 0; i < 10; i++)
    {
        o = ell_appdR(l, sizeof(obj_t));

        ((obj)o)->key = i + 1;

        snprintf(((obj)o)->val, 16, "%d", ((obj)o)->key + 10);
    }

    my_list_show(l);
    //my_list_showr(l);

    ell_freeH(l);
    ell_freeT(l);
    my_list_show(l);
    //my_list_showr(l);

    ell_freeI(l, 3);
    my_list_show(l);
    //my_list_showr(l);

    ell_free(l);

    return ETEST_OK;
}


int test_basic(int argc, char* argv[])
{
    ETEST_RUN( ell_etest_basic() );

    return ETEST_OK;
}


