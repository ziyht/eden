//
// Created by ziyht on 17-2-15.
//


#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#define  EUTILS_LLOG

#include "eutils.h"
#include "ejson.h"

#include "ebench.h"

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

void obj_bench_init(ebench_p b)
{
    b->bprvt.p = ejson_new(EOBJ, EVAL_ZORE);
}

void obj_bench_add(ebench_p b)
{
    u64 oprts = b->oprts;

    ejson e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        ejson_addT(e, ullstr(i), ETRUE);
    }

    b->scale = ejson_len(e);
}

void obj_bench_add_rand(ebench_p b)
{
    u64 oprts = b->oprts;

    ejson e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        if(!ejson_addT(e, str_data[i], ETRUE))
            ejson_takeR(e, str_data[i]);
    }

    b->scale = ejson_len(e);
}

void obj_bench_find(ebench_p b)
{
    u64 oprts = b->oprts;

    ejson e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        if(!ejson_r(e, ullstr(i)))
        {
            llog_err("'%d' can not find in json");
            abort();
        }
    }

    b->scale = ejson_len(e);
}

void obj_bench_find_rand(ebench_p b)
{
    u64 oprts = b->oprts;

    ejson e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        ejson_r(e, str_data[i]);
    }

    b->scale = ejson_len(e);
}

void obj_bench_free(ebench_p b)
{
    b->oprts = b->scale = ejson_len(b->bprvt.p);
    ejson_free(b->bprvt.p);
}

void arr_bench_init(ebench_p b)
{
    b->bprvt.p = ejson_new(EARR, EVAL_ZORE);
}

void arr_bench_add(ebench_p b)
{
    u64 oprts = b->oprts;

    ejson e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        ejson_addT(e, 0, ETRUE);
    }

    b->scale = ejson_len(e);
}

void arr_bench_find(ebench_p b)
{
    u64 oprts = b->oprts;

    ejson e = b->bprvt.p;

    for(u64 i = 0; i < oprts; i++)
    {
        if(!ejson_i(e, (uint)i))
        {
            llog_err("%" PRId64 " can not find in json", );
            abort();
        }
    }

     b->scale = ejson_len(e);
}

void arr_bench_free(ebench_p b)
{
    b->oprts = b->scale = ejson_len(b->bprvt.p);
    ejson_free(b->bprvt.p);
}

#define CALLGRIND 0


int ejson_benchmark(int argc, char* argv[])
{
    ebench b;
    udb_init_data();

    b = ebench_new("ejson obj operating");
    ebench_addStep(b, "init", obj_bench_init, EVAL_0);
    ebench_addOprt(b, "add" , obj_bench_add , EVAL_0);
    ebench_addOprt(b, "find", obj_bench_find, EVAL_0);
    ebench_addOprt(b, "free", obj_bench_free, EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    b = ebench_new("ejson arr operating");
    ebench_addStep(b, "init", arr_bench_init, EVAL_0);
    ebench_addOprt(b, "add" , arr_bench_add , EVAL_0);
    ebench_addOprt(b, "find", arr_bench_find, EVAL_0);
    ebench_addOprt(b, "free", arr_bench_free, EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    b = ebench_new("ejson obj rand key S");
    ebench_addStep(b, "init", obj_bench_init     , EVAL_0);
    ebench_addOprt(b, "add" , obj_bench_add_rand , EVAL_0);
    ebench_addOprt(b, "find", obj_bench_find_rand, EVAL_0);
    ebench_addOprt(b, "free", obj_bench_free     , EVAL_0);
    ebench_addCase(b,   50000, EVAL_0);
    ebench_addCase(b,  500000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    ebench_exec();

    ebench_showResult();

    ebench_release();

    udb_destroy_data();

    return ETEST_OK;
}

