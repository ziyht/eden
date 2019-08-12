/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>
#include <ebench.h>

#include "eutils.h"

#include "evec.h"

/** ----------------------------
 *  val S
 */

void evec_val_S_bench_init(ebench_p b)
{
    b->bprvt.p = evec_new(E_STR, 0);
}

void evec_val_S_bench_add(ebench_p b)
{
    uint oprts = (uint)b->oprts;

    evec e = b->bprvt.p;

    for(uint i = 0; i < oprts; i++)
    {
        evec_addS(e, i, ullstr(i));
    }
}

void evec_val_S_bench_find(ebench_p b)
{
    uint oprts = (uint)b->oprts;

    evec e = b->bprvt.p;

    for(uint i = 0; i < oprts; i++)
    {
        evec_iValS(e, i);
    }
}

void evec_val_S_bench_free(ebench_p b)
{
    evec_free(b->bprvt.p);
}

/** ----------------------------
 *  val I
 */

void evec_val_i64_bench_init(ebench_p b)
{
    b->bprvt.p = evec_new(E_I64, 0);
}

void evec_val_i64_bench_add(ebench_p b)
{
    uint oprts = (uint)b->oprts;

    evec e = b->bprvt.p;

    for(uint i = 0; i < oprts; i++)
    {
        evec_addI(e, i, i);
    }
}

void evec_val_i64_bench_find(ebench_p b)
{
    uint oprts = (uint)b->oprts;

    evec e = b->bprvt.p;

    for(uint i = 0; i < oprts; i++)
    {
        evec_iValI(e, i);
    }
}

void evec_val_i64_bench_free(ebench_p b)
{
    evec_free(b->bprvt.p);
}

int evec_benchmark(int argc, char* argv[])
{
    ebench b;

    b = ebench_new("evec val S operating");
    ebench_addStep(b, "init", evec_val_S_bench_init, EVAL_0);
    ebench_addOprt(b, "add" , evec_val_S_bench_add , EVAL_0);
    ebench_addOprt(b, "find", evec_val_S_bench_find, EVAL_0);
    ebench_addOprt(b, "free", evec_val_S_bench_free, EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    b = ebench_new("evec val i64 operating");
    ebench_addStep(b, "init", evec_val_i64_bench_init, EVAL_0);
    ebench_addOprt(b, "add" , evec_val_i64_bench_add , EVAL_0);
    ebench_addOprt(b, "find", evec_val_i64_bench_find, EVAL_0);
    ebench_addOprt(b, "free", evec_val_i64_bench_free, EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    ebench_exec();

    ebench_showResult();

    ebench_release();

    return ETEST_OK;
}

