#include <stdlib.h>
#include <string.h>
#include "test_main.h"



#define DICT_HASH_FUNCTION_SEED 5381;
static inline uint __murmurHash2(constr key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    static uint32_t seed = DICT_HASH_FUNCTION_SEED;
    const  uint32_t m = 0x5bd1e995;
    const  int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}

static inline uint __djbHash(constr buf, int len) {
    register uint hash = (uint)5381;

    while (len--)
        hash += (hash << 5) + *buf++;   // hash * 33 + c

    return hash & ~(1 << 31);
}

static inline size_t __BKDRHash(constr buf, int len)
{
    register size_t hash = 0;

    while (len--)
    {
        hash = hash * 131 + *buf++;
    }

    return hash;
}

static int seed = 31;

cstr _fisrtkey(char key[9])
{
    memcpy(key, "aaaaaaaaa", 9);
    return key;
}

cstr _nextkey(char key[9])
{
    for(int i = 0; i < 9; i++)
    {
        key[i]++;
        if(key[i] <= 'z') break;

        key[i] = 'a';
    }

    return key;
}

cstr hit_map1;
cstr hit_map2;
uint hit_len = 1024 * 512;

typedef size_t (*__hash_cb)(constr buf, int len);

static int test_algorithmI(constr name,  __hash_cb __hash)
{
    struct __{
        int conflict1;
        int conflict2;
        int test_len;
    }result[65];

    size_t hash; uint test_len, level;

    if(!hit_map1)
    {
        hit_map1 = calloc(1, hit_len * 2);
        hit_map2 = hit_map1 + hit_len;
    }

    memset(result, 0, sizeof(result));
    level = 0;
    for(test_len = 4; test_len <= hit_len; test_len *= 2)
    {
        result[level].test_len = test_len;

        memset(hit_map1, 0, hit_len * 2);
        for(u64 i = 0; i < test_len; i++)
        {
            hash = __hash((constr)&i, sizeof(u64)) % test_len;

            if(hit_map1[hash]) result[level].conflict1++;
            else               hit_map1[hash] = 1;

            hash = hash % (test_len / 2);
            if(hit_map2[hash]) result[level].conflict2++;
            else              hit_map2[hash] = 1;
        }
        level++;
    }

    printf("%-9s", name); for(uint i = 0; i < level; i++)printf("%8d", result[i].test_len); puts("");
    printf("%-9s", "1" ); for(uint i = 0; i < level; i++)printf("%8d", result[i].conflict1); puts("");
    printf("%-9s", "1/2"); for(uint i = 0; i < level; i++)printf("%8d", result[i].conflict2); puts("\n");

    return result[level-1].conflict1;
}

static int test_algorithmS(constr name,  __hash_cb __hash)
{
    struct __{
        int conflict1;
        int conflict2;
        int test_len;
    }result[65];

    size_t hash; char buf[9]; uint test_len, level;

    if(!hit_map1)
    {
        hit_map1 = calloc(1, hit_len * 2);
        hit_map2 = hit_map1 + hit_len;
    }

    memset(result, 0, sizeof(result));
    level = 0;
    for(test_len = 4; test_len <= hit_len; test_len *= 2)
    {
        result[level].test_len = test_len;

        memset(hit_map1, 0, hit_len * 2);
        _fisrtkey(buf);
        for(u64 i = 0; i < test_len; i++)
        {
            hash = __hash(_nextkey(buf), 9) % test_len;

            if(hit_map1[hash]) result[level].conflict1++;
            else               hit_map1[hash] = 1;

            hash = hash % (test_len / 2);
            if(hit_map2[hash]) result[level].conflict2++;
            else              hit_map2[hash] = 1;
        }
        level++;
    }

    printf("%-9s", name); for(uint i = 0; i < level; i++)printf("%8d", result[i].test_len); puts("");
    printf("%-9s", "1"); for(uint i = 0; i < level; i++)printf("%8d", result[i].conflict1); puts("");
    printf("%-9s", "1/2"); for(uint i = 0; i < level; i++)printf("%8d", result[i].conflict2); puts("\n");

    return result[level-1].conflict1;
}

void edict_hash_test()
{

    // test_ziyhtSeed();

    test_algorithmI("murmur2", (__hash_cb)__murmurHash2);
    test_algorithmI("djb    ", (__hash_cb)__djbHash);
    test_algorithmI("BKDR   ", (__hash_cb)__BKDRHash);

    test_algorithmS("murmur2", (__hash_cb)__murmurHash2);
    test_algorithmS("djb    ", (__hash_cb)__djbHash);
    test_algorithmS("BKDR   ", (__hash_cb)__BKDRHash);

}

int test_hash(int argc, char* argv[])
{
    edict_hash_test();

    return ETEST_OK;
}
