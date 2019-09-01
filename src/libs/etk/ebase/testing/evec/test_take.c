/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "evec.h"

static int test_take_num(etypev t)
{
    evec v = evec_new(t);

    int left = 100;

    for(int i = 0; i < left; i++)
    {
        evec_appdV(v, __EVAR_MK(t, evec_esize(v), 0, EVAL_I8(i)));

        i64 val = evec_i(v, i).i64;

        eexpect_eq(val, i);
    }
    eexpect_eq(evec_len(v), 100);

    int s = 0, e = 99;

    {
        int cnt = 10;

        for(int i = 0; i < cnt; i++, s++)
        {
            evar t = evec_takeH(v);
            eexpect_eq(t.cnt , 1);
            eexpect_eq(t.v.i8, s);
            evar_free(t);
        }

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }


    {
        int cnt = 8;

        evar t = evec_takeHs(v, cnt);
        eexpect_eq(t.cnt , cnt);

        for(uint i = 0; i < t.cnt; i++, s++)
        {
            eexpect_eq(evar_iVal(t, i).i64, s);
        }

        evar_free(t);

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }

    {
        int cnt = 16;

        evar t = evec_takeHs(v, cnt);
        eexpect_eq(t.cnt , cnt);

        for(uint i = 0; i < t.cnt; i++, s++)
        {
            eexpect_eq(evar_iVal(t, i).i64, s);
        }

        evar_free(t);

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }

    {
        int cnt = 10;

        for(int i = 99; i > 99 - cnt; i--, e--)
        {
            evar t = evec_takeT(v);
            eexpect_eq(t.cnt , 1);
            eexpect_eq(t.v.i8, e);
            evar_free(t);
        }

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }


    {
        int cnt = 8;

        evar t = evec_takeTs(v, cnt);
        eexpect_eq(t.cnt , cnt);

        for(int i = t.cnt - 1; i >=0; i--, e--)
        {
            eexpect_eq(evar_iVal(t, i).i64, e);
        }

        evar_free(t);

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }

    {
        int cnt = 16;

        evar t = evec_takeTs(v, cnt);
        eexpect_eq(t.cnt , cnt);

        for(int i = t.cnt - 1; i >=0; i--, e--)
        {
            eexpect_eq(evar_iVal(t, i).i64, e);
        }

        evar_free(t);

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }

    {
        int idx = 1;
        int cnt = 1;

        s = evec_i(v, idx).i64;
        evar t = evec_takeIs(v, idx, cnt);

        eexpect_eq(t.cnt , cnt);

        for(int i = 0; i < cnt; i++, s++)
        {
            eexpect_eq(evar_iVal(t, i).i64, s);
        }

        evar_free(t);

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }

    {
        int idx = 1;
        int cnt = 4;

        s = evec_i(v, idx).i64;
        evar t = evec_takeIs(v, idx, cnt);

        eexpect_eq(t.cnt , cnt);

        for(int i = 0; i < cnt; i++, s++)
        {
            eexpect_eq(evar_iVal(t, i).i64, s);
        }

        evar_free(t);

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }

    {
        int idx = -10;
        int cnt = 10;

        s = evec_i(v, idx).i64;
        evar t = evec_takeIs(v, idx, cnt);

        eexpect_eq(t.cnt , cnt);

        for(int i = 0; i < cnt; i++, s++)
        {
            eexpect_eq(evar_iVal(t, i).i64, s);
        }

        evar_free(t);

        left -= cnt;
        eexpect_eq(evec_len(v), left);
    }

    evec_free(v);

    return ETEST_OK;
}

/**
 * @brief prepare_test_vec
 * @param t
 * @return
 *
 *   test vec is:
 *
 *       32          64          32           <-- cnt
 *   [********................********]
 *            r               f
 *    32 - 63                 0 - 31          <-- value
 */
static evec prepare_test_vec(etypev t)
{
    evec v = evec_new(t);

    for(int i = 0; i < 96; i++)
    {
        evec_appdV(v, __EVAR_MK(t, evec_esize(v), 0, EVAL_I8(i)));
    }

    for(int i = 0; i < 32; i++)
    {
        evec_appdV(v, __EVAR_MK(t, evec_esize(v), 0, EVAL_I8(i)));
    }

    evar takes = evec_takeHs(v, 96);
    evar_free(takes);

    for(int i = 0; i < 32; i++)
    {
        evec_appdV(v, __EVAR_MK(t, evec_esize(v), 0, EVAL_I8(i + 32)));
    }

    return v;
}

static int test_take_idxs_cnt(etypev t, int start, int cnt)
{
    for(int i = start; i < 64; i++)
    {
        evec v = prepare_test_vec(t);

        int pre_len  = evec_len(v);
        int can_take = evec_len(v) - i;

        evar t = evec_takeIs(v, i, cnt);

        int check = can_take > cnt ? cnt : can_take;

        eexpect_eq(t.cnt, check);

        eexpect_eq(evec_len(v), pre_len - check);

        {
            char checks_map[128] = {0};
            int  checks_cnt      = 0;

            for(uint i = 0; i < t.cnt; i++)
            {
                eexpect_eq(checks_map[evar_i(t, i).v.i64], 0);

                checks_map[evar_i(t, i).v.i64] = 1;
                checks_cnt ++;
            }

            for(uint i = 0; i < evec_len(v); i++)
            {
                eexpect_eq(checks_map[evec_i(v, i).i64], 0);

                checks_map[evec_i(v, i).i64] = 1;
                checks_cnt ++;
            }

            eexpect_eq(checks_cnt, pre_len);
        }

        evar_free(t);
        evec_free(v);
    }

    return ETEST_OK;
}

static int test_take_idxs(etypev t)
{
    //! check the value if is we wanted
    {
        evec v = prepare_test_vec(t);

        for(uint i = 0; i < evec_len(v); i++)
        {
            eexpect_eq(evec_i(v, i).i64, i);
        }

        evec_free(v);
    }

    ETEST_RUN( test_take_idxs_cnt(t, 0 , 1) );
    ETEST_RUN( test_take_idxs_cnt(t, 28, 5) );

    for(int i = 0; i <= 64; i++ )
    {
        ETEST_RUN( test_take_idxs_cnt(t, 0, i) );
    }

    return ETEST_OK;
}

int test_take(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_take_num(E_I8) );
    ETEST_RUN( test_take_num(E_I16) );
    ETEST_RUN( test_take_num(E_I32) );
    ETEST_RUN( test_take_num(E_I64) );

    ETEST_RUN( test_take_num(E_U8) );
    ETEST_RUN( test_take_num(E_U16) );
    ETEST_RUN( test_take_num(E_U32) );
    ETEST_RUN( test_take_num(E_U64) );

    ETEST_RUN( test_take_idxs(E_I8) );
    ETEST_RUN( test_take_idxs(E_I16) );
    ETEST_RUN( test_take_idxs(E_I32) );
    ETEST_RUN( test_take_idxs(E_I64) );

    ETEST_RUN( test_take_idxs(E_U8) );
    ETEST_RUN( test_take_idxs(E_U16) );
    ETEST_RUN( test_take_idxs(E_U32) );
    ETEST_RUN( test_take_idxs(E_U64) );

    return ETEST_OK;
}

