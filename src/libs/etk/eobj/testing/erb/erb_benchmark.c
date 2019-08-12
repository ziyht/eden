/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>
#include <ebench.h>

#include "eutils.h"

#include "erb.h"

static int data_size = 5000000;
static unsigned *int_data;
static char **str_data;
void udb_init_data()
{
    int i;
    char buf[256];
    printf("[benchmark] generating data... ");
    e_srand48(11);
    int_data = (unsigned*)calloc(data_size, sizeof(unsigned));
    str_data = (char**)calloc(data_size, sizeof(char*));
    for (i = 0; i < data_size; ++i) {
        int_data[i] = (unsigned)(data_size * e_drand48() / 4) * 271828183u;
        sprintf(buf, "%x", int_data[i]);
        str_data[i] = strdup(buf);
    }
    printf("done!\n");
}

void udb_destroy_data()
{
    int i;
    for (i = 0; i < data_size; ++i) free(str_data[i]);
    free(str_data); free(int_data);
}

/** ------------------------------------
 *  key S
 */

void erb_key_S_bench_init(ebench_p b)
{
    b->bprvt.p = erb_new(EKEY_S);
}

void erb_key_S_bench_add(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        erb_addI(e, ekey_s(ullstr(i)), 0);
    }
}

void erb_key_S_bench_add_rand(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        if(!erb_addI(e, ekey_s(str_data[i]), 0))
            erb_freeOne(e, ekey_s(str_data[i]));
    }

    b->scale = erb_size(e);
}

void erb_key_S_bench_find(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        erb_k(e, ekey_s(ullstr(i)));
    }
}

void erb_key_S_bench_find_rand(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        erb_k(e, ekey_s(str_data[i]));
    }

    b->scale = erb_size(e);
}

void erb_key_S_bench_free(ebench_p b)
{
    b->oprts = b->scale = erb_size(b->bprvt.p);
    erb_free(b->bprvt.p);
}

/** ------------------------------------
 *  key I
 */

void erb_key_I_bench_init(ebench_p b)
{
    b->bprvt.p = erb_new(EKEY_I);
}

void erb_key_I_bench_add(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        erb_addI(e, ekey_i(i), 0);
    }
}

void erb_key_I_bench_add_rand(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        if(!erb_addI(e, ekey_i(int_data[i]), 0))
        {
            erb_freeOne(e, ekey_i(int_data[i]));
        }
    }

    b->scale = erb_size(e);
}

void erb_key_I_bench_find(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        erb_k(e, ekey_i(i));
    }
}

void erb_key_I_bench_find_rand(ebench_p b)
{
    u64 oprts = b->oprts;

    erb e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        erb_k(e, ekey_i(int_data[i]));
    }

    b->scale = erb_size(e);
}

void erb_key_I_bench_free(ebench_p b)
{
    b->oprts = b->scale = erb_size(b->bprvt.p);
    erb_free(b->bprvt.p);
}

int erb_benchmark(int argc, char* argv[])
{
    ebench b;

    udb_init_data();

    b = ebench_new("erb key S operating");
    ebench_addStep(b, "init"       , erb_key_S_bench_init, EVAL_0);
    ebench_addOprt(b, "add"        , erb_key_S_bench_add , EVAL_0);
    ebench_addOprt(b, "find"       , erb_key_S_bench_find, EVAL_0);
    ebench_addOprt(b, "free"       , erb_key_S_bench_free, EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    b = ebench_new("erb key I operating");
    ebench_addStep(b, "init"       , erb_key_I_bench_init, EVAL_0);
    ebench_addOprt(b, "add"        , erb_key_I_bench_add , EVAL_0);
    ebench_addOprt(b, "find"       , erb_key_I_bench_find, EVAL_0);
    ebench_addOprt(b, "free"       , erb_key_I_bench_free, EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    b = ebench_new("erb key S operating rand");
    ebench_addStep(b, "init"       , erb_key_S_bench_init     , EVAL_0);
    ebench_addOprt(b, "add"        , erb_key_S_bench_add_rand , EVAL_0);
    ebench_addOprt(b, "find(rand)" , erb_key_S_bench_find_rand, EVAL_0);
    ebench_addOprt(b, "free"       , erb_key_S_bench_free     , EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    b = ebench_new("erb key I operating rand");
    ebench_addStep(b, "init"       , erb_key_I_bench_init     , EVAL_0);
    ebench_addOprt(b, "add"        , erb_key_I_bench_add_rand , EVAL_0);
    ebench_addOprt(b, "find(rand)" , erb_key_I_bench_find_rand, EVAL_0);
    ebench_addOprt(b, "free"       , erb_key_I_bench_free     , EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    //ebench_skipCase(500000, 5000000);
    ebench_exec();

    ebench_showResult();

    ebench_release();

    udb_destroy_data();

    return ETEST_OK;
}

