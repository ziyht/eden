#include "etest.h"

#include "evec.h"
#include "eutils.h"

static int test_gen()
{
    typedef struct { etypev t; u16 size; } _IN_;

    evec v; int i;

    _IN_ map[] =
    {
        {E_I8 , 1},
        {E_I16, 2},
        {E_I32, 4},
        {E_I64, 8},

        {E_U8 , 1},
        {E_U16, 2},
        {E_U32, 4},
        {E_U64, 8},

        {E_F32, 4},
        {E_F64, 8},

        {E_PTR, 8},
        {E_STR, 8},
        {E_RAW, 8},

        {E_USER, 16},

        {E_NAV, 0},
    };

    for(i = 0; i < 100; i++)
    {
        if(map[i].t == E_NAV)
            break;

        v = evec_new2(map[i].t, map[i].size, 0);
        eexpect_num(evec_type (v), map[i].t);
        eexpect_num(evec_len  (v), 0);
        eexpect_num(evec_esize(v), map[i].size);
        evec_free(v);
    }

    return ETEST_OK;
}

static int test_appd()
{
    typedef struct { etypev t; u16 size; } _IN_;
    char __buf[256] = {0};

    evec v; int i;

    _IN_ map[] =
    {
        {E_I8 , 1},
        {E_I16, 2},
        {E_I32, 4},
        {E_I64, 8},

        {E_U8 , 1},
        {E_U16, 2},
        {E_U32, 4},
        {E_U64, 8},

        {E_F32, 4},
        {E_F64, 8},

        {E_PTR, 8},

        {E_NAV, 0},
    };

    _IN_* itr = map;

    while (itr->t != E_NAV) {

        v = evec_new(itr->t);

        for(i = 0; i < 128; i++)
        {
            evar ev = evar_gen(itr->t, 1, 0);
            ev.v.i8 = i;

            eexpect_num( evec_appdV(v, ev), true);
            eexpect_eq( evar_cmp((evar){ 0, itr->t, itr->size, 1, evec_i(v, i)}, ev), 0 );
        }
        eexpect_num( evec_free(v), i );

        itr++;
    }

    //! i64
    {
        v = evec_new(E_I64);

        for(i = 0; i < 128; i++)
        {
            eexpect_num( evec_appdI(v, i), true);
        }
        eexpect_num( evec_free(v), i );
    }

    //! f64
    {
        v = evec_new(E_F64);

        for(i = 0; i < 128; i++)
        {
            eexpect_num( evec_appdF(v, i), true);
        }
        eexpect_num( evec_free(v), i );
    }

    //! str
    {
        v = evec_new(E_STR);

        for(i = 0; i < 128; i++)
        {
            ll2str(i, __buf);
            eexpect_num( evec_appdS(v, __buf), true);

            cstr s = evec_i(v, i).s;

            eexpect_str( s, __buf);
        }
        eexpect_num( evec_free(v), i );
    }

    //! ptr
    {
        v = evec_new(E_PTR);

        for(i = 0; i < 128; i++)
        {
            eexpect_num( evec_appdP(v, &__buf[i]), true);

            eexpect_ptr( evec_i(v, i).p, &__buf[i]);
        }
        eexpect_num( evec_free(v), i );
    }

    //! raw
    {
        v = evec_new(E_RAW);

        char buf[128] = {0};

        for(i = 0; i < 128; i++)
        {
            //ll2str(i, __buf);
            eexpect_num( evec_appdR(v, i), true);

            cstr r = evec_i(v, i).s;

            eexpect_raw( r, buf, i );
        }
        eexpect_num( evec_free(v), i );
    }

    //! user
    {
        v = evec_new2(E_USER, 4, 0);

        for(i = 0; i < 128; i++)
        {
            eexpect_num( evec_appdV(v, EVAR_USER(__buf, ll2str(i, __buf))), true);

            cstr s = evec_i(v, i).p;

            eexpect_raw( s, __buf, ll2str(i, __buf));
        }
        eexpect_num( evec_free(v), i );
    }

    return ETEST_OK;
}

static int evec_valI_test()
{
    int i;

    evec v = evec_new(E_I64);

    for(i = 0; i < 100; i++)
    {
        evec_appdI(v, i);
    }
    eexpect_num(evec_len(v), 100);

    for(; i < 200; i++)
    {
        evec_appdV(v, EVAR_I64(i) );
    }
    eexpect_num(evec_len(v), 200);

    for(i = 0; i < 200; i++)
    {
        eval val = evec_i(v, i);
        eexpect_num(val.i, i);
        eexpect_num(evec_iValI(v, i), i);
    }

    evec_free(v);

    return ETEST_OK;
}

static int evec_valS_test()
{
    int i; char buf[16];

    ll2str(2, buf);

    evec v = evec_new(E_STR);

    for(i = 0; i < 100; i++)
    {
        evec_appdS(v, llstr(i));
    }
    eexpect_num(evec_len(v), 100);

    for(; i < 200; i++)
    {
        evec_appdV(v, EVAR_S(llstr(i)) );
    }
    eexpect_num(evec_len(v), 200);

    for(i = 0; i < 200; i++)
    {
        eval val = evec_i(v, i);
        eexpect_str(val.s, llstr(i));
        eexpect_str(evec_iValS(v, i), llstr(i));
    }

    evec_free(v);

    return ETEST_OK;
}

int test_basic(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_gen() );
    ETEST_RUN( test_appd() );

    ETEST_RUN( evec_valI_test() );
    ETEST_RUN( evec_valS_test() );

    return ETEST_OK;
}
