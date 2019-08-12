/// =====================================================================================
///
///       Filename:  edict.c
///
///    Description:  an easier dict
///
///        Version:  1.1
///        Created:  2017-05-03 10:00:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#undef  EDICT_VERSION
#define EDICT_VERSION "edict 1.1.2"   // fix bugs caused by typek in node

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ecompat.h"
#include "eutils.h"

#include "eobj_p.h"
#include "edict.h"

#pragma pack(1)
typedef struct _edict_node_s* _edictn;
typedef struct _edict_node_s{
    _edictn         next;
    ekey_t          key;
    _ehdr_t         hdr;
    eobj_t          obj;
}_edictn_t;

typedef struct _edictht_s{
    _edictn*        table;       // to save data
    uld             size;
    uld             sizemask;
    uld             used;
}_edictht_t, *_edictht;

typedef struct _edict_s{
    _edictht_t      ht[2];
    long            rehashidx;
    int             iterators;

    eobj_rls_ex_cb  rls;
    eval            prvt;
}_edict_t;

typedef struct _edict_root_s {

    _edictn         next;
    ekey_t          key;
    _ehdr_t         hdr;
    _edict_t        dict;

}_edictr_t, * _edictr;

typedef struct _editr_s {
    edict   d;
    long    index;
    int     table,
            safe;
    _edictn entry,
            nextEntry;
    u64     fingerprint;
}_editr_t;

#pragma pack()

// -- pre definitions
#define DICT_OK                   0
#define DICT_ERR                  1
#define DICT_HT_INITIAL_SIZE      4

#define _dict_can_resize          1
#define _dict_force_resize_ratio  5

// -- micros
#define _dict_link(l, o)     {(o)->next = *((l)._pos); *((l)._pos) = o; *((l)._used) += 1;}

#define _ht_reset(ht)        memset((ht), 0, sizeof(_edictht_t))

#define _CUR_C_TYPE         EDICT

#define _DNODE_TYPE         _edictn_t
#define _DNODE_KEY_FIELD    key
#define _DNODE_HDR_FIELD    hdr
#define _DNODE_OBJ_FIELD    obj

#define _RNODE_TYPE         _edictr_t
#define _RNODE_TYP_FIELD    type
#define _RNODE_OBJ_FIELD    dict

#define _c_r(d)             container_of(d, _edictr_t, dict)
#define _c_keys(d)          _r_keys(_c_r(d))
#define _c_typek(d)         _r_typek(_c_r(d))
#define _c_ht(d)            d->ht
#define _c_ht0(d)           _c_ht(d)[0]
#define _c_ht1(d)           _c_ht(d)[1]
#define _c_htreset(d)       memset(_c_ht(d), 0, sizeof(_edictht_t) * 2);
#define _c_len(d)           (_c_ht0(d).used + _c_ht1(d).used)
#define _c_rehashIdx(d)     d->rehashidx
#define _c_iterators(d)     d->iterators
#define _c_privdata(d)      d->privdata
#define _c_isRehashing(d)   (_c_rehashIdx(d) != -1)
#define _c_free(d)          _r_free(_c_r(d))

#define _c_rls(d)           (d)->rls
#define _c_prvt(d)          (d)->prvt

#define _dict_hashKeyS(key, len) __djbHashS(key)
#define _dict_hashKeyU(key, len) (uld)key

#define _n_lenI(n)          sizeof(long)
#define _n_lenU(n)          sizeof(long)
#define _n_lenS(n)          strlen(_n_keyS(n))

#define _cur_freekeyS       efree
#define _cur_cmpkeyS        strcmp

static inline int __edict_init(edict d)
{
    _c_htreset(d);
    _c_rehashIdx(d) = -1;
    _c_iterators(d) = 0;

    return DICT_OK;
}

#define DICT_HASH_FUNCTION_SEED 5381;
static inline uint __MurmurHash2(constr key, int len) {
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

static inline uint __djbHashB(constr buf, int len) {
    register uint hash = (uint)5381;

    while (len--)
        hash += (hash << 5) + *buf++;   // hash * 33 + c

    return hash & ~(1 << 31);
}

static inline uint __djbHashS(constr buf) {
    register uint hash = (uint)5381;

    while (*buf)
        hash += (hash << 5) + *buf++;   // hash * 33 + c

    return hash & ~(1 << 31);
}

static inline size_t __BKDRHashB(constr buf, int len)
{
    register size_t hash = 0;

    while (len--)
    {
        hash = hash * 131 + *buf++;
    }

    return hash;
}

static u64 _edict_fingerprint(edict d) {
    long long integers[6], hash = 0;
    int j;

    integers[0] = (long long) _c_ht0(d).table;
    integers[1] = _c_ht0(d).size;
    integers[2] = _c_ht0(d).used;
    integers[3] = (long long) _c_ht1(d).table;
    integers[4] = _c_ht1(d).size;
    integers[5] = _c_ht1(d).used;

    /* We hash N integers by summing every successive integer with the integer
     * hashing of the previous sum. Basically:
     *
     * Result = hash(hash(hash(int1)+int2)+int3) ...
     *
     * This way the same set of integers in a different order will (likely) hash
     * to a different number. */
    for (j = 0; j < 6; j++) {
        hash += integers[j];
        /* For the hashing step we use Tomas Wang's 64 bit integer hash. */
        hash = (~hash) + (hash << 21); // hash = (hash << 21) - hash - 1;
        hash = hash ^ (hash >> 24);
        hash = (hash + (hash << 3)) + (hash << 8); // hash * 265
        hash = hash ^ (hash >> 14);
        hash = (hash + (hash << 2)) + (hash << 4); // hash * 21
        hash = hash ^ (hash >> 28);
        hash = hash + (hash << 31);
    }
    return hash;
}

static inline int __edict_rehash(edict d, int n)
{
    int empty_visits = n * 10;

    is0_ret(_c_isRehashing(d), 0);

    if(_c_keys(d))
    {
        while(n-- && _c_ht0(d).used != 0)
        {
            _edictn de, nextde;

            while(_c_ht0(d).table[_c_rehashIdx(d)] == NULL) {
                _c_rehashIdx(d)++;
                if (--empty_visits == 0) return 1;
            }

            de = _c_ht0(d).table[_c_rehashIdx(d)];

             while(de) {
                 unsigned int h;
                 nextde = de->next;
                 h = _dict_hashKeyS(_n_keyS(de), _n_lenS(de)) & _c_ht1(d).sizemask;
                 de->next = _c_ht1(d).table[h];
                 _c_ht1(d).table[h] = de;
                 _c_ht0(d).used--;
                 _c_ht1(d).used++;
                 de = nextde;
             }
             _c_ht0(d).table[_c_rehashIdx(d)] = NULL;
             _c_rehashIdx(d)++;
        }
    }
    else
    {
        while(n-- && _c_ht0(d).used != 0)
        {
            _edictn de, nextde;

            while(_c_ht0(d).table[_c_rehashIdx(d)] == NULL) {
                _c_rehashIdx(d)++;
                if (--empty_visits == 0) return 1;
            }

            de = _c_ht0(d).table[_c_rehashIdx(d)];

             while(de) {
                 uld h;
                 nextde = de->next;
                 h = _dict_hashKeyU(_n_keyI(de), _n_lenI(de)) & _c_ht1(d).sizemask;
                 de->next = _c_ht1(d).table[h];
                 _c_ht1(d).table[h] = de;
                 _c_ht0(d).used--;
                 _c_ht1(d).used++;
                 de = nextde;
             }
             _c_ht0(d).table[_c_rehashIdx(d)] = NULL;
             _c_rehashIdx(d)++;
        }
    }

    if (_c_ht0(d).used == 0) {
        free(_c_ht0(d).table);
        _c_ht0(d) = _c_ht1(d);
        _ht_reset(&_c_ht1(d));
        _c_rehashIdx(d) = -1;
        return DICT_OK;
    }

    return DICT_ERR;
}

static inline void __edict_rehashPtrStep(edict d)
{
    if (_c_iterators(d) == 0) __edict_rehash(d, 1);
}

static inline uld __edict_nextPower(uld size)
{
    uld i = DICT_HT_INITIAL_SIZE;

    if (size >= LONG_MAX) return LONG_MAX;
    while(1) {
        if (i >= size)
            return i;
        i *= 2;
    }
}

static inline int __edict_expand(edict d, uld size)
{
    _edictht_t n;
    uld realsize = __edict_nextPower(size);

    is1_ret(_c_isRehashing(d) || _c_ht0(d).used > size, DICT_ERR);
    is1_ret(realsize == _c_ht0(d).size,                    DICT_ERR);

    n.size     = realsize;
    n.sizemask = realsize - 1;
    n.table    = calloc(realsize * sizeof(void*), 1);
    n.used     = 0;

    is1_exeret(_c_ht0(d).table == NULL, _c_ht0(d) = n, DICT_OK);

    _c_ht1(d) = n;
    _c_rehashIdx(d) = 0;
    return DICT_OK;
}

static inline int __edict_expandIfNeeded(edict d)
{
    if (_c_isRehashing(d)) return DICT_OK;

    if (_c_ht0(d).size == 0) return __edict_expand(d, DICT_HT_INITIAL_SIZE);

    /* If we reached the 1:1 ratio, and we are allowed to resize the hash
     * table (global setting) or we should avoid it but the ratio between
     * elements/buckets is over the "safe" threshold, we resize doubling
     * the number of buckets. */
    if (_c_ht0(d).used >= _c_ht0(d).size && (_dict_can_resize || _c_ht0(d).used / _c_ht0(d).size > _dict_force_resize_ratio))
    {
        return __edict_expand(d, _c_ht0(d).used * 2);
    }
    return DICT_OK;
}

static inline ild __edict_keyIndex(edict d, ekey_t key, int multi)
{
    uld h, idx, table; _edictn he;

    is1_ret(__edict_expandIfNeeded(d) == DICT_ERR, -1);

    if(_c_keys(d))
    {
        is0_ret(_k_keyS(key), -1);

        h = _dict_hashKeyS(_k_keyS(key), _k_lenS(key));

        if(multi)
            return h & _c_ht0(d).sizemask;

        for (table = 0; table <= 1; table++)
        {
            idx = h & _c_ht(d)[table].sizemask;

            he = _c_ht(d)[table].table[idx];
            while(he) {
                if ( !strcmp(_k_keyS(key), _n_keyS(he)) )
                    return -1;
                he = he->next;
            }
            if (!_c_isRehashing(d)) break;
        }
    }
    else
    {
        h = _dict_hashKeyU(_k_keyU(key), _k_lenU(key));

        if(multi)
            return h & _c_ht0(d).sizemask;

        for (table = 0; table <= 1; table++)
        {
            idx = h & _c_ht(d)[table].sizemask;

            he = _c_ht(d)[table].table[idx];
            while(he) {
                if ( _k_keyU(key) == _n_keyU(he) )
                    return -1;
                he = he->next;
            }
            if (!_c_isRehashing(d)) break;
        }
    }

    return idx;
}

edict edict_new(etypek type)
{
    _edictr r = _r_new();

    __edict_init(_r_o(r));

    _r_typeco_set(r);

    _r_typek(r) = (type & EKEY_S) ? _EO_KEYS : _EO_KEYI;

    return _r_o(r);
}

bool edict_setType(edict d, etypek type)
{
    is1_ret(!d || 0 == _c_len(d), false);

    _c_typek(d) = (type & EKEY_S) ? _EO_KEYS : _EO_KEYI;

    return true;
}

bool edict_setPrvt(edict d, eval          prvt) { is0_ret(d, false); _c_prvt(d) = prvt; return 1; }
bool edict_setRls (edict d, eobj_rls_ex_cb rls) { is0_ret(d, false); _c_rls (d) = rls ; return 1; }

static void __edict_clearHt(edict d, _edictht ht, eobj_rls_ex_cb rls, bool release)
{
    uld i; _edictn he, nextHe;

    if(_c_keys(d))
    {
        if(rls)
        {
            for (i = 0; i < ht->size && ht->used > 0; i++)
            {
                if ((he = ht->table[i]) == NULL) continue;
                while(he) {
                    nextHe = he->next;

                    rls(_n_o(he), _c_prvt(d));
                    _n_freeK(he);
                    _n_free(he);

                    ht->used--;
                    he = nextHe;
                }
            }
        }
        else
        {
            for (i = 0; i < ht->size && ht->used > 0; i++)
            {
                if ((he = ht->table[i]) == NULL) continue;
                while(he) {
                    nextHe = he->next;

                    _n_freeK(he);
                    _n_free(he);

                    ht->used--;
                    he = nextHe;
                }
            }
        }
    }
    else
    {
        if(rls)
        {
            for (i = 0; i < ht->size && ht->used > 0; i++)
            {
                if ((he = ht->table[i]) == NULL) continue;
                while(he) {
                    nextHe = he->next;

                    rls(_n_o(he), _c_prvt(d));
                    _n_free(he);

                    ht->used--;
                    he = nextHe;
                }
            }
        }
        else
        {
            for (i = 0; i < ht->size && ht->used > 0; i++)
            {
                if ((he = ht->table[i]) == NULL) continue;
                while(he) {
                    nextHe = he->next;

                    _n_free(he);

                    ht->used--;
                    he = nextHe;
                }
            }
        }
    }

    if(release)
    {
        efree(ht->table);
        _ht_reset(ht);
    }
    else
    {
        memset(ht->table, 0, ht->size * sizeof(_edictn*));
    }
}

int   edict_clear(edict d)
{
    return edict_clearEx(d, 0);
}

int   edict_clearEx(edict d, eobj_rls_ex_cb rls)
{
    is0_ret(d, 0);

    is0_exe(rls, rls = _c_rls(d));

    if(_c_ht1(d).size > 0)
    {
        __edict_clearHt(d, &_c_ht1(d), rls, true);    // relese ht1
    }

    __edict_clearHt(d, &_c_ht0(d), rls, false);    // clear ht0

    _c_rehashIdx(d) = -1;

    return 1;
}

int edict_free(edict d)
{
    return edict_freeEx(d, 0);
}

int edict_freeEx(edict d, eobj_rls_ex_cb rls)
{
    is0_ret(d, 0);

    is0_exe(rls, rls = _c_rls(d));

    __edict_clearHt(d, &_c_ht1(d), rls, true);    // relese ht1
    __edict_clearHt(d, &_c_ht0(d), rls, true);    // relese ht0

    _c_free(d);

    return 1;
}

bool   edict_isEmpty(edict d) { if(d) return _c_len(d) == 0; return true; }
uld    edict_size   (edict d) { if(d) return _c_len(d)     ; return    0; }

static eobj __edict_makeRoom(edict d, ekey key, uint len, int multi)
{
    ild idx; _edictht_t* ht; _edictn n;

    is0_ret(d, 0);

    if(_c_isRehashing(d)) __edict_rehashPtrStep(d);

    is1_ret((idx = __edict_keyIndex(d, key, multi)) == -1, NULL); // already exist

    n = _n_newm(len); _n_init(n); _n_typek(n) = _c_typek(d);

    if(_c_keys(d)) _n_keyS(n) = strdup(_k_keyS(key));
    else           _n_keyU(n) = _k_keyU(key);

    ht = _c_isRehashing(d) ? &_c_ht1(d) : &_c_ht0(d);
    n->next        = ht->table[idx];
    ht->table[idx] = n;
    ht->used++;

    return _n_o(n);
}

eobj  edict_addI(edict d, ekey key, i64    val) { eobj o = __edict_makeRoom(d, key, sizeof(i64) , 0); if(o){ _eo_setI (o, val); _eo_typecoe(o) = _EDICT_COE_NUM_I; } return o; }
eobj  edict_addF(edict d, ekey key, f64    val) { eobj o = __edict_makeRoom(d, key, sizeof(f64) , 0); if(o){ _eo_setF (o, val); _eo_typecoe(o) = _EDICT_COE_NUM_F; } return o; }
eobj  edict_addP(edict d, ekey key, conptr ptr) { eobj o = __edict_makeRoom(d, key, sizeof(cptr), 0); if(o){ _eo_setP (o, ptr); _eo_typeco(o)  = _EDICT_CO_PTR   ; } return o; }
eobj  edict_addR(edict d, ekey key, uint   len) { eobj o = __edict_makeRoom(d, key, len + 1     , 0); if(o){ _eo_wipeR(o, len); _eo_typeco(o)  = _EDICT_CO_RAW   ; } return o; }
eobj  edict_addS(edict d, ekey key, constr str)
{
    eobj o; uint len;

    len = str ? strlen(str) : 0;

    o = __edict_makeRoom(d, key, len + 1, 0);

    if(o) { _eo_setS(o, str, len); _eo_typeco(o) = _EDICT_CO_STR; }

    return o;
}

eobj  edict_addMI(edict d, ekey key, i64    val) { eobj o = __edict_makeRoom(d, key, sizeof(i64) , 1); if(o){ _eo_setI (o, val); _eo_typecoe(o) = _EDICT_COE_NUM_I; } return o; }
eobj  edict_addMF(edict d, ekey key, f64    val) { eobj o = __edict_makeRoom(d, key, sizeof(f64) , 1); if(o){ _eo_setF (o, val); _eo_typecoe(o) = _EDICT_COE_NUM_F; } return o; }
eobj  edict_addMP(edict d, ekey key, conptr ptr) { eobj o = __edict_makeRoom(d, key, sizeof(cptr), 1); if(o){ _eo_setP (o, ptr); _eo_typeco(o)  = _EDICT_CO_PTR   ; } return o; }
eobj  edict_addMR(edict d, ekey key, uint   len) { eobj o = __edict_makeRoom(d, key, len + 1     , 1); if(o){ _eo_wipeR(o, len); _eo_typeco(o)  = _EDICT_CO_RAW   ; } return o; }
eobj  edict_addMS(edict d, ekey key, constr str)
{
    eobj o; uint len;

    len = str ? strlen(str) : 0;

    o = __edict_makeRoom(d, key, len + 1, 1);

    if(o) { _eo_setS(o, str, len); _eo_typeco(o) = _EDICT_CO_STR; }

    return o;
}

eobj edict_find(edict d, ekey key)
{
    uld h, idx, table; _edictn he;

    is0_ret(d, 0);

    if(_c_ht0(d).size == 0) return NULL;

    if(_c_isRehashing(d)) __edict_rehashPtrStep(d);

    if(_c_keys(d))
    {
        is0_ret(_k_keyS(key), 0);

        h = _dict_hashKeyS(_k_keyS(key), strlen(key.s));
        for (table = 0; table <= 1; table++) {
            idx = h & _c_ht(d)[table].sizemask;
            he = _c_ht(d)[table].table[idx];
            while(he) {
                if ( !strcmp(_k_keyS(key), _n_keyS(he)) )
                    return _n_o(he);
                he = he->next;
            }
            if (!_c_isRehashing(d)) return NULL;
        }
    }
    else
    {
        h = _dict_hashKeyU(_k_keyU(key), _n_lenI(key));
        for (table = 0; table <= 1; table++) {
            idx = h & _c_ht(d)[table].sizemask;
            he = _c_ht(d)[table].table[idx];
            while(he) {
                if ( _k_keyU(key) == _n_keyU(he) )
                    return _n_o(he);
                he = he->next;
            }
            if (!_c_isRehashing(d)) return NULL;
        }
    }

    return NULL;
}

eobj   edict_k      (edict d, ekey key) { return   edict_find(d, key); }
i64    edict_kValI  (edict d, ekey key) { eobj o = edict_find(d, key); _eo_retI(o); }
f64    edict_kValF  (edict d, ekey key) { eobj o = edict_find(d, key); _eo_retF(o); }
cptr   edict_kValR  (edict d, ekey key) { eobj o = edict_find(d, key); _eo_retR(o); }
cptr   edict_kValP  (edict d, ekey key) { eobj o = edict_find(d, key); _eo_retP(o); }
cstr   edict_kValS  (edict d, ekey key) { eobj o = edict_find(d, key); _eo_retS(o); }
etypeo edict_kType  (edict d, ekey key) { eobj o = edict_find(d, key); _eo_retT(o); }
uint   edict_kLen   (edict d, ekey key) { eobj o = edict_find(d, key); _eo_retL(o); }
bool   edict_kIsTrue(edict d, ekey key) { return __eobj_isTrue(edict_find(d, key)); }



#if 0
int edict_getL(edict d, conptr key, L l)
{
    int idx; _edictht ht;

    if(_c_isRehashing(d)) __edict_rehashPtrStep(d);

    is1_ret((idx = __edict_keyIndex(d, key)) == -1, 0); // already exist

    ht = _c_isRehashing(d) ? &_c_ht1(d) : &_c_ht0(d);
    l->_pos  = &ht->table[idx];
    l->_used = &ht->used;

    return 1;
}
#endif

eobj  edict_takeOne(edict d, ekey key)
{
    uld h, idx; int table; _edictn he, prevHe;

    is0_ret(d, 0);

    if (_c_ht0(d).size == 0) return NULL;

    if (_c_isRehashing(d)) __edict_rehashPtrStep(d);

    if(_c_keys(d))
    {
        is0_ret(_k_keyS(key), 0);

        h = _dict_hashKeyS(_k_keyS(key), _k_lenS(key));

        for (table = 0; table <= 1; table++) {
            idx = h & _c_ht(d)[table].sizemask;
            he = _c_ht(d)[table].table[idx];
            prevHe = NULL;
            while(he) {
                if ( !strcmp(_k_keyS(key), _n_keyS(he)) ) {
                    /* Unlink the element from the list */
                    if (prevHe) prevHe->next            = he->next;
                    else        _c_ht(d)[table].table[idx] = he->next;
                    _c_ht(d)[table].used--;
                    return _n_o(he);
                }
                prevHe = he;
                he = he->next;
            }
            if (!_c_isRehashing(d)) break;
        }
    }
    else
    {
        h = _dict_hashKeyU(_k_keyU(key), _k_lenU(key));

        for (table = 0; table <= 1; table++) {
            idx = h & _c_ht(d)[table].sizemask;
            he = _c_ht(d)[table].table[idx];
            prevHe = NULL;
            while(he) {
                if ( _k_keyU(key) == _n_keyU(he) ) {
                    /* Unlink the element from the list */
                    if (prevHe) prevHe->next            = he->next;
                    else        _c_ht(d)[table].table[idx] = he->next;
                    _c_ht(d)[table].used--;
                    return _n_o(he);
                }
                prevHe = he;
                he = he->next;
            }
            if (!_c_isRehashing(d)) break;
        }
    }

    return NULL; /* not found */
}

eobj  edict_takeO(edict d, eobj obj)
{
    uld h, idx; int table; _edictn he, prevHe, n;

    is0_ret(d, 0); is0_ret(obj, 0);

    if (_c_ht0(d).size == 0) return NULL;

    if (_c_isRehashing(d)) __edict_rehashPtrStep(d);

    n = _eo_dn(obj);
    if(_c_keys(d))
    {
        h = _dict_hashKeyS(_n_keyS(n), _n_lenS(n));

        for (table = 0; table <= 1; table++) {
            idx = h & _c_ht(d)[table].sizemask;
            he = _c_ht(d)[table].table[idx];
            prevHe = NULL;
            while(he) {
                if ( !strcmp(_n_keyS(n), _n_keyS(he)) ) {
                    if(he != n)   return NULL;    // not match

                    /* Unlink the element from the list */
                    if (prevHe) prevHe->next            = he->next;
                    else        _c_ht(d)[table].table[idx] = he->next;
                    _c_ht(d)[table].used--;
                    return obj;
                }
                prevHe = he;
                he = he->next;
            }
            if (!_c_isRehashing(d)) break;
        }
    }
    else
    {
        h = _dict_hashKeyU(_n_keyU(n), _n_lenU(n));

        for (table = 0; table <= 1; table++) {
            idx = h & _c_ht(d)[table].sizemask;
            he = _c_ht(d)[table].table[idx];
            prevHe = NULL;
            while(he) {
                if ( _n_keyS(n) == _n_keyS(he) ) {
                    if(he != n)   return NULL;    // not match

                    /* Unlink the element from the list */
                    if (prevHe) prevHe->next            = he->next;
                    else        _c_ht(d)[table].table[idx] = he->next;
                    _c_ht(d)[table].used--;
                    return obj;
                }
                prevHe = he;
                he = he->next;
            }
            if (!_c_isRehashing(d)) break;
        }
    }

    return NULL; /* not found */
}

int   edict_freeOne(edict d, ekey key)
{
    cptr o = edict_takeOne(d, key);

    is1_exeret(o, is1_exe(_c_keys(d), _eo_freeK(o)); _eo_free(o);, 1);

    return 0;
}

int   edict_freeO(edict d, eobj obj)
{
    is0_ret(obj, 0);

    if(!d)
    {
        obj = (eobj)_eo_dn(obj);
        is1_ret(_n_linked((_edictn)obj), 0);

        _n_free(obj);
        return 1;
    }

    obj = edict_takeO(d, obj);

    is1_exeret(obj, _eo_free(obj), 1);

    return 0;
}

editr edict_getItr(edict d, int safe)
{
    editr iter = malloc(sizeof(*iter));

    iter->d         = d;
    iter->table     = 0;
    iter->index     = -1;
    iter->safe      = (safe == 1);
    iter->entry     = NULL;
    iter->nextEntry = NULL;

    return iter;
}

int   edict_resetItr(editr iter, edict d, int safe)
{
    is0_ret(iter, 0);

    if(d)                iter->d    = d;

    if     (safe == 0)   iter->safe = 0;
    else if(safe == 1)   iter->safe = 1;

    iter->table     = 0;
    iter->index     = -1;
    iter->entry     = NULL;
    iter->nextEntry = NULL;

    return 1;
}

eobj edict_next(editr iter)
{
    while (1) {
        if (iter->entry == NULL) {
            _edictht ht = &_c_ht(iter->d)[iter->table];
            if (iter->index == -1 && iter->table == 0) {
                if (iter->safe) _c_iterators(iter->d)++;
                else            iter->fingerprint = _edict_fingerprint(iter->d);
            }
            iter->index++;
            if (iter->index >= (long) ht->size) {
                if (_c_isRehashing(iter->d) && iter->table == 0) {
                    iter->table++;
                    iter->index = 0;
                    ht = &_c_ht1(iter->d);
                } else {
                    break;
                }
            }
            iter->entry = ht->table[iter->index];
        } else {
            iter->entry = iter->nextEntry;
        }
        if (iter->entry) {
            /* We need to save the 'next' here, the iterator user
             * may delete the entry we are returning. */
            iter->nextEntry = iter->entry->next;
            return _n_o(iter->entry);
        }
    }
    return NULL;
}

void  edict_freeItr(editr iter)
{
    if (!(iter->index == -1 && iter->table == 0)) {
        if (iter->safe) _c_iterators(iter->d)--;
        else
            assert(iter->fingerprint == _edict_fingerprint(iter->d));
    }
    free(iter);
}

constr edict_version()
{
    return EDICT_VERSION;
}
