#include "test_main.h"

#include "uthash.h"

typedef struct node_s
{
    UT_hash_handle hh;
    int i;
    char s[16];
}node_t;


void uthash_i_test(int scale)
{
    node_t * node;
    node_t * table = 0; u64 t, tn; int i; uint j;

    t = eutils_nowms();
    tn = eutils_nowns(); i = 0;
    node = calloc(1, sizeof(*node));
    node->i = i;
    HASH_ADD(hh, table, i, sizeof(i), node);
    printf("add   1   \telem: %6"PRIu64"ns\n", eutils_nowns() - tn); fflush(stdout);
    for(i = 1; i < scale; i++)
    {
        node = calloc(1, sizeof(*node));
        node->i = i;
        HASH_ADD(hh, table, i, sizeof(i), node);
    }
    if(HASH_COUNT(table) != (uint)scale)
    {
        printf("error: add %d elements, but only have %d elements in e\n", scale, HASH_COUNT(table)); fflush(stdout);
        exit(1);
    }
    printf("add   %d \telem: %6"PRIi64"ms\n", scale, eutils_nowms() - t); fflush(stdout);

    t = eutils_nowms();
    j = 0; scale ++;
    tn = eutils_nowns(); i = 0;
    HASH_FIND(hh, table, &i, sizeof(i), node);
    if(node) j++;
    printf("found 1   \telem: %6"PRIu64"ns\n", eutils_nowns() - tn); fflush(stdout);
    for(int i = 1; i < scale; i++)
    {
        HASH_FIND(hh, table, &i, sizeof(i), node);
        if(node) j++;
    }
    if(HASH_COUNT(table) != j)
    {
        printf("error: e have %d elements, but only found %d elements in e\n", HASH_COUNT(table), j); fflush(stdout);
        exit(1);
    }
    printf("found %d \telem: %6"PRIu64"ms\n", j, eutils_nowms() - t); fflush(stdout);

    t = eutils_nowms();
    int len = HASH_COUNT(table); j = 0;
    tn = eutils_nowns(); i = 0;
    HASH_FIND(hh, table, &i, sizeof(i), node);
    if(node)
    {
        HASH_DEL(table, node);
        j++;
        free(node);
    }
    printf("del   1   \telem: %6"PRIu64"ns\n", eutils_nowns() - tn); fflush(stdout);
    for(int i = 0; i < scale; i++)
    {
        HASH_FIND(hh, table, &i, sizeof(i), node);
        if(node)
        {
            HASH_DEL(table, node);
            j++;
            free(node);
        }
    }
    if(len != (int)j)
    {
        printf("error: e have %d elements, but only del %d elements in e\n", len, j); fflush(stdout);
        exit(1);
    }
    printf("del   %d \telem: %6"PRIu64"ms\n\n", j, eutils_nowms() - t); fflush(stdout);

}

void uthash_s_test(int scale)
{
    node_t * node;
    node_t * table = 0; u64 t; char keyS[16];

    t = eutils_nowms();
    for(int i = 0; i < scale; i++)
    {
        node = calloc(1, sizeof(*node));
        sprintf(node->s, "%d", i);
        HASH_ADD_STR(table, s, node);
    }
    if(HASH_COUNT(table) != (uint)scale)
    {
        printf("error: add %d elements, but only have %d elements in e\n", scale, HASH_COUNT(table)); fflush(stdout);
        exit(1);
    }
    printf("add   %d \telem: %6"PRIi64"ms\n", scale, eutils_nowms() - t); fflush(stdout);

    t = eutils_nowms();
    uint j = 0; scale ++;
    for(int i = 0; i < scale; i++)
    {
        sprintf(keyS, "%d", i);
        HASH_FIND_STR(table, keyS, node);
        if(node) j++;
    }
    if(HASH_COUNT(table) != j)
    {
        printf("error: e have %d elements, but only found %d elements in e\n", HASH_COUNT(table), j); fflush(stdout);
        exit(1);
    }
    printf("found %d \telem: %6"PRIu64"ms\n", j, eutils_nowms() - t); fflush(stdout);

    t = eutils_nowms();
    int len = HASH_COUNT(table); j = 0;
    for(int i = 0; i < scale; i++)
    {
        sprintf(keyS, "%d", i);
        HASH_FIND_STR(table, keyS, node);
        if(node)
        {
            HASH_DEL(table, node);
            j++;
            free(node);
        }
    }
    if(len != (int)j)
    {
        printf("error: e have %d elements, but only del %d elements in e\n", len, j); fflush(stdout);
        exit(1);
    }
    printf("del   %d \telem: %6"PRIu64"ms\n\n", j, eutils_nowms() - t); fflush(stdout);

}

void uthash_i_rand_test(int scale)
{
    node_t * node;
    node_t * table = 0; u64 tn1, tn, tn_total, tn_min, tn_max; int i; uint j; int key, key_min, key_max;

    for(i = 0; i < scale; i++)
    {
        node = calloc(1, sizeof(*node));
        node->i = i;
        HASH_ADD(hh, table, i, sizeof(i), node);
    }

    j = 0; tn_total = 0; tn_min = ULLONG_MAX; tn_max = 0;
    for(int i = 0; i < 10; i++)
    {
        key = eutils_rand() % scale;
        tn1 = eutils_nowns();
        HASH_FIND(hh, table, &key, sizeof(key), node);
        tn  = eutils_nowns() - tn1;

        tn_total += tn;

        if(tn >= tn_max)
        {
            tn_max = tn;
            key_max = key;
        }
        if(tn < tn_min)
        {
            tn_min = tn;
            key_min = key;
        }

        if(node) j++;

    }

    if(10 != j)
    {
        printf("error: e have %d elements, but only found %d elements in e\n", HASH_COUNT(table), j); fflush(stdout);
        exit(1);
    }
    printf("found %d  \telem: %6"PRIu64"ns\n", j, tn_total); fflush(stdout);
    printf("found %d \t    : %6"PRIu64"ns\n", key_min, tn_min); fflush(stdout);
    printf("found %d \t    : %6"PRIu64"ns\n", key_max, tn_max); fflush(stdout);
}

void uthash_performance_test()
{
#if 1
    printf("-- uthash_s_test --\n"); fflush(stdout);
    uthash_s_test(10000);
    uthash_s_test(20000);
    uthash_s_test(50000);
    uthash_s_test(100000);
    uthash_s_test(200000);
    uthash_s_test(500000);
    uthash_s_test(1000000);
    uthash_s_test(2000000);
#endif

#if 1
    printf("-- uthash_i_test --\n"); fflush(stdout);
    uthash_i_test(10000);
    uthash_i_test(20000);
    uthash_i_test(50000);
    uthash_i_test(100000);
    uthash_i_test(200000);
    uthash_i_test(500000);
    uthash_i_test(1000000);
    uthash_i_test(2000000);
#endif

    printf("-- uthash_i_rand_test --\n"); fflush(stdout);
    uthash_i_rand_test(10000);
}

int test_performance_uthash(int argc, char* argv[])
{
    uthash_performance_test();

    return ETEST_OK;
}
