// =====================================================================================
//
//       Filename:  esl.c
//
//    Description:  easy skiplist
//
//        Version:  1.0
//        Created:  03/08/2017 02:55:34 PM
//       Revision:  none
//       Compiler:  gcc
//
//         Author:  Haitao Yang, joyhaitao@foxmail.com
//        Company:
//
// =====================================================================================

#define ESL_VERSION "esl 1.2.2"     // fix coredump in release build

#include <stdlib.h>
#include <stdio.h>

#include "esl.h"
#include "eobj_p.h"

#pragma pack(1)

typedef struct _eskiplist_node_s* _esln;

typedef struct _eskiplist_link_s{
    _esln       forward;             // forward's real cnt is alloc dynamiclly
    _esln       backward;
    int         level;
}_esll_t, * _esll;
#define _FORWARD_CNT 1

typedef struct _eskiplist_node_s {
    _esll_t         link;
    ekey            key;
    _ehdr_t         hdr;

    eobj_t          obj;
} _esln_t;

typedef struct _esl_s {
    _esln           header;
    _esln           tail;
    i16             level;
    i16             keyt;

    eval            prvt;       // private data
    eobj_cmp_ex_cb  cmp;        // compare func
    eobj_rls_ex_cb  rls;        // release func, only have effect on ERAW and EPTR type of obj
}_esl_t;

typedef struct esl_root_s {
    _esll_t         link;
    ekey            key;
    _ehdr_t         hdr;

    _esl_t          sl;
}_eslr_t, * _eslr;

#pragma pack()

/// -------------------------- micros helper ---------------------------------

#define _CUR_C_TYPE             ESL

#define _DNODE_TYPE             _esln_t
#define _DNODE_LNK_FIELD        link
#define _DNODE_KEY_FIELD        key
#define _DNODE_HDR_FIELD        hdr
#define _DNODE_OBJ_FIELD        obj

#define _RNODE_TYPE             _eslr_t
#define _RNODE_TYP_FIELD        type
#define _RNODE_OBJ_FIELD        sl

#define _cur_dupkeyS            strdup
#define _cur_cmpkeyS            strcmp
#define _cur_freekeyS           efree

//!
//! vn: virtual node
//!
//!  unlike other cantainers, esl nodes' link field is in dynamicly length
//!
//!  _esln_t | link | forward[?]    ------------ real    node addr
//!          |      | forward[0]    ------------ virtual node addr: & _esln_t.forward[0]
//!          |      | backward
//!          |      | level
//!          | key
//!          | hdr
//!          | obj
//!
//!     when convert real node addr to virtual node addr, we can handle the node
//! as the same way like other cantainer's obj, so we can using the macros in
//! eobj_p.h
//!
//!

#undef  _n_init     // not using the one in eobj_p

#define _n_vn(n, lv)            ((_esln)&((char*)n)[ lv * sizeof(void*)])
#define _n_r(n, lv)             ((_eslr)&((char*)n)[ lv * sizeof(void*)])
#define _n_c(n, lv)            &(_n_r(n, lv)->_RNODE_OBJ_FIELD)
#define _n_init(n, lv)          memset(vn, 0, sizeof(_DNODE_TYPE*) * lv + sizeof(_DNODE_TYPE) - sizeof(eobj_t));

#define _vn_n(n, lv)            ((_esln)&((char*)n)[-lv * sizeof(void*)])
#define _vn_l(n)                _n_l(n)
#define _vn_o(n)                _n_o(n)
#define _vn_c(n)                (esl)_n_o(n)
#define _vn_key(n)              _n_key(n)
#define _vn_keyS(n)             _n_keyS(n)
#define _vn_keyI(n)             _n_keyI(n)
#define _vn_keyU(n)             _n_keyU(n)
#define _vn_keyF(n)             _n_keyF(n)
#define _vn_len(n)              _n_len(n)
#define _vn_typek(n)            _n_typek(n)

#define _vn_lv(n)               _vn_l(n).level
#define _vn_backward(n)         _vn_l(n).backward
#define _vn_forward(n, lv)      (&_vn_l(n).forward)[-lv]
#define _vn_free(n)             _eobj_free(_vn_n(n, (_vn_lv(n) - _FORWARD_CNT)))
#define _vn_freekeyS(n)         _cur_freekeyS(_vn_keyS(n))

#define _c_len(l)               _eo_len(l)
#define _c_keys(l)              _eo_keys(l)
#define _c_typecoe(l)           _eo_typecoe(l)
#define _c_typecoe_set(l)       _c_typecoe(l) = _cur_type(_CUR_C_TYPE, COE_OBJ  );

#define _c_header(l)            (l)->header
#define _c_header_init(l)       memset(_vn_n(_c_header(l), (_vn_lv(_c_header(l)) - _FORWARD_CNT)), 0, _vn_lv(_c_header(l)) * sizeof(_DNODE_TYPE*) + sizeof(_esll_t) - sizeof(_DNODE_TYPE*) - sizeof(int));
#define _c_tail(l)              (l)->tail
#define _c_lv(l)                (l)->level
#define _c_keyt(l)              (l)->keyt
#define _c_prvt(l)              (l)->prvt
#define _c_cmp(l)               (l)->cmp
#define _c_rls(l)               (l)->rls

#define _c_free(l)              _vn_free((_esln)_o_vn(l))

#define _o_vn(o)                _eo_dn(o)

/// -------------------------- esl helper ---------------------------

static __always_inline int __esl_random_level() {
    int level = 1;
    while ((rand()&0xFFFF) < (ESL_P * 0xFFFF))
        level += 1;
    return (level < ESL_MAXLEVEL) ? level : ESL_MAXLEVEL;
}

static __always_inline esl __esl_create_sl()
{
    int level = __esl_random_level() - _FORWARD_CNT;

    esl sl  = _n_newc(sizeof(_eslr_t) + level * sizeof(void*));

    _esln vn = _n_vn(sl, level);

    _vn_lv(vn) = level + _FORWARD_CNT;

    return _n_c(sl, level);
}

static __always_inline _esln __esl_create_node(int level, int len)
{
    _esln vn; level = level - _FORWARD_CNT;

    //! now vn is actual node
    vn  = _n_newm(len + level * sizeof(void*));
    _n_init(vn, level);

    //! convert to virtual addr
    vn = _n_vn(vn, level);

    //! set level
    _vn_lv(vn) = level + _FORWARD_CNT;

    return vn;
}

typedef struct __pos_s
{
    bool      canadd;
    _esln     update[ESL_MAXLEVEL];
    _esln     x;
}__pos_t, * __pos;

/**
 * @brief __esl_get_key_pos
 * @param l     - esl handle
 * @param obj   - obj to find
 * @param pos   - output pos info
 * @param multi - enable multi add?
 * @param find  - need find the @param obj
 */
static inline void __esl_get_key_pos(esl l, eobj obj, __pos pos, bool multi, bool find)
{
    int i; _esln x;

    x = _c_header(l);

    if(_c_cmp(l))
    {
        eobj_cmp_ex_cb cmp = _c_cmp(l);

        x = _c_header(l);

        for (i = _c_lv(l) - 1; i >= 0; i--)
        {
            while(_vn_forward(x, i) && cmp(_vn_o(_vn_forward(x, i)) , obj, _c_prvt(l)) < 0)
                x = _vn_forward(x, i);

            pos->update[i] = x;
        }

        if(_vn_forward(x, 0))
        {
            x = _vn_forward(x, 0);
            if( 0 == cmp(_vn_o(_vn_forward(x, i)) , obj, _c_prvt(l)) )
            {
                pos->canadd = multi;

                if(!find) { pos->x  = x; return;}
                else      { do{
                                if(_vn_o(x) == obj) { pos->x  = x; return; }
                            }while((x = _vn_forward(x, 0)) && 0 == cmp(_vn_o(_vn_forward(x, i)) , obj, _c_prvt(l)));
                          }
            }
        }

        pos->canadd = 1;
    }
    else
    {
        switch (_c_keyt(l)) {
            case EKEY_I :   for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i) && _vn_keyI(_vn_forward(x, i)) < _eo_keyI(obj))
                                    x = _vn_forward(x, i);

                                pos->update[i] = x;
                            }
                            break;

            case EKEY_U :   for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i) && _vn_keyU(_vn_forward(x, i)) < _eo_keyU(obj))
                                    x = _vn_forward(x, i);

                                pos->update[i] = x;
                            }
                            break;

            case EKEY_F :   for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i) && _vn_keyF(_vn_forward(x, i)) < _eo_keyF(obj))
                                    x = _vn_forward(x, i);

                                pos->update[i] = x;
                            }
                            break;

            case EKEY_I | EKEY_DES :
                            for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i) && _vn_keyI(_vn_forward(x, i)) > _eo_keyI(obj))
                                    x = _vn_forward(x, i);

                                pos->update[i] = x;
                            }
                            break;

            case EKEY_U | EKEY_DES :
                            for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i) && _vn_keyU(_vn_forward(x, i)) > _eo_keyU(obj))
                                    x = _vn_forward(x, i);

                                pos->update[i] = x;
                            }
                            break;
            case EKEY_F | EKEY_DES :   for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i) && _vn_keyF(_vn_forward(x, i)) > _eo_keyF(obj))
                                    x = _vn_forward(x, i);

                                pos->update[i] = x;
                            }
                            break;

            case EKEY_S:    for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i))
                                {
                                    if(_cur_cmpkeyS(_vn_keyS(_vn_forward(x, i)), _eo_keyS(obj)) < 0)
                                        x = _vn_forward(x, i);
                                    else
                                        break;
                                }

                                pos->update[i] = x;
                            }

                            if(_vn_forward(x, 0))
                            {
                                x = _vn_forward(x, 0);
                                if(0 == _cur_cmpkeyS(_vn_keyS(x), _eo_keyS(obj)))
                                {
                                    pos->canadd = multi;

                                    if(!find) { pos->x  = x; return;}
                                    else      { do{
                                                    if(_vn_o(x) == obj) { pos->x  = x; return; }
                                                }while((x = _vn_forward(x, 0)) && 0 == _cur_cmpkeyS(_vn_keyS(x), _eo_keyS(obj)));
                                              }
                                }
                            }

                            pos->canadd = 1;

                            return; // whole logic, return now

            case EKEY_S | EKEY_DES:
                            for (i = _c_lv(l) - 1; i >= 0; i--)
                            {
                                while(_vn_forward(x, i) && _cur_cmpkeyS(_vn_keyS(_vn_forward(x, i)), _eo_keyS(obj)) > 0)
                                    x = _vn_forward(x, i);

                                pos->update[i] = x;
                            }

                            if(_vn_forward(x, 0))
                            {
                                x = _vn_forward(x, 0);
                                if(0 == _cur_cmpkeyS(_vn_keyS(x), _eo_keyS(obj)))
                                {
                                    pos->canadd = multi;

                                    if(!find) { pos->x  = x; return;}
                                    else      { do{
                                                    if(_vn_o(x) == obj) { pos->x  = x; return; }
                                                }while((x = _vn_forward(x, 0)) && 0 == _cur_cmpkeyS(_vn_keyS(x), _eo_keyS(obj)));
                                              }
                                }
                            }

                            pos->canadd = 1;

                            return; // whole logic, return now
        }

        if(_vn_forward(x, 0))
        {
            x = _vn_forward(x, 0);
            if( _vn_keyU(x) == _eo_keyU(obj))   // using u64 to check if is equal nums
            {
                pos->canadd = multi;

                if(!find) { pos->x  = x; return;}
                else      { do{
                                if(_vn_o(x) == obj) { pos->x  = x; return; }
                            }while((x = _vn_forward(x, 0)) && _vn_keyU(x) == _eo_keyU(obj));
                          }
            }
        }

        pos->canadd = 1;
    }
}

/**

  header                update[i]     x    update[i]->forward
  |-----------|-----------|-----------|-----------|-----------|-----------|
                          |<---update[i].span---->|
  |<-------rank[i]------->|
  |<-------------------rank[0]------------------->|

*/
#define __esl_insert_node(l, x, update, level, i)                           \
do{                                                                         \
    if (level > _c_lv(l))                                                   \
    {                                                                       \
        /* new level must point to header */                                \
        for (i = _c_lv(l); i < level; i++)                                  \
            update[i] = _c_header(l);                                       \
                                                                            \
        _c_lv(l) = level;                                                   \
    }                                                                       \
                                                                            \
    /* insert new node to skiplist*/                                        \
    for (i = 0; i < level; i++)                                             \
    {                                                                       \
        _vn_forward(x        , i) = _vn_forward(update[i], i);              \
        _vn_forward(update[i], i) = x;                                      \
    }                                                                       \
                                                                            \
    _vn_backward(x) = (update[0] == _c_header(l)) ? NULL                    \
                                                  : update[0];              \
                                                                            \
    if (_vn_forward(x, 0)) _vn_backward(_vn_forward(x, 0)) = x;             \
    else                   _c_tail(l)                      = x;             \
                                                                            \
    _n_linked(x) = 1;                                                       \
    _c_len(l)++;                                                            \
}while(0)

#define __esl_take_node(l, x, update, i)                                    \
do{                                                                         \
    /* update ptr before x */                                               \
    for(i = 0; i < _c_lv(l); i++)                                           \
    {                                                                       \
        if (_vn_forward(update[i], i) == x)                                 \
            _vn_forward(update[i], i) = _vn_forward(x, i);                  \
    }                                                                       \
                                                                            \
    /* update ptr after x */                                                \
    if(_vn_forward(x, 0)) _vn_backward(_vn_forward(x, 0)) = _vn_backward(x);\
    else                  _c_tail(l)                      = _vn_backward(x);\
                                                                            \
    /* update level */                                                      \
    while(_c_lv(l) > 1 && _vn_forward(_c_header(l), _c_lv(l) - 1) == NULL)  \
        _c_lv(l)--;                                                         \
                                                                            \
    _n_linked(x) = 0;                                                       \
    _c_len(l)--;                                                            \
}while(0)

static eobj __esl_makeRoom(esl l, eobj obj, bool multi, bool overwrite, bool find)
{
    int i, level; __pos_t pos = {0, {0, }, 0};

    is0_ret(l, 0);

    //! get pos info
    __esl_get_key_pos(l, obj, &pos, multi, find);

    if(overwrite && pos.x)
    {
        //! do overwrite operation

    }
    else
    {
        if(!pos.canadd)
            return 0;

        level = __esl_random_level();

        // create a new node
        pos.x = __esl_create_node(level, _eo_len(obj));

        // insert it
        __esl_insert_node(l, pos.x, pos.update, level, i);

        // update key
        if(_c_keys(l)) _vn_keyS(pos.x) = _cur_dupkeyS(_eo_keyS(obj));
        else           _vn_key (pos.x) = _eo_key (obj);

        _vn_typek(pos.x) = (_c_keyt(l) == EKEY_S) ? _EO_KEYS : _EO_KEYI;

        return _vn_o(pos.x);
    }

    return 0;
}

static void __esl_clear_nodes(esl l, eobj_rls_ex_cb rls)
{
    _esln node, next;

    node = _vn_forward(_c_header(l), 0);

    if(!rls)
        rls = _c_rls(l);

    if(!rls)
    {
        if(!_c_keys(l))
        {
            for(; node; node = next)
            {
                next = _vn_forward(node, 0);

                if(EOBJ == _n_typeo(node))
                {
                    esl_freeEx(_vn_c(node), rls);
                    continue;
                }

                _vn_free(node);
            }
        }
        else
        {
            for(; node; node = next)
            {
                next = _vn_forward(node, 0);

                if(EOBJ == _n_typeo(node))
                {
                    _vn_freekeyS(node);
                    esl_freeEx(_vn_c(node), rls);
                    continue;
                }

                _vn_freekeyS(node);
                _vn_free(node);
            }
        }
    }
    else
    {
        if(!_c_keys(l))
        {
            for(; node; node = next)
            {
                next = _vn_forward(node, 0);

                switch (_n_typeo(node))
                {
                    case EPTR:
                    case ERAW: rls(_vn_o(node), _c_prvt(l)); break;
                    case EOBJ: esl_freeEx(_vn_c(node), rls);

                               continue;
                }

                rls(_vn_o(node), _c_prvt(l));
                _vn_free(node);
            }
        }
        else
        {
            for(; node; node = next)
            {
                next = _vn_forward(node, 0);

                switch (_n_typeo(node))
                {
                    case EPTR:
                    case ERAW: rls(_vn_o(node), _c_prvt(l)); break;
                    case EOBJ: _vn_freekeyS(node);

                               esl_freeEx(_vn_c(node), rls);

                               continue;
                }

                _vn_freekeyS(node);
                _vn_free(node);
            }
        }
    }
}

esl  esl_new(etypek type)
{
    esl l = __esl_create_sl();

    _c_typecoe_set(l);
    _c_keys(l)      = (type & EKEY_S) > 0;

    _c_lv(l)        = 1;
    _c_keyt(l)      = type;

    _c_header(l)    = __esl_create_node(ESL_MAXLEVEL, 0);

    return l;
}

bool esl_setType(esl l, etypek type)
{
    is1_ret(!l || 0 == _c_len(l), false);

    _c_keys(l)      = (type & EKEY_S) > 0;
    _c_keyt(l)      = type;

    return true;
}

bool esl_setPrvt(esl l, eval          prvt) { is0_ret( l                  , false); _c_prvt(l) = prvt; return true; }
bool esl_setCmp (esl l, eobj_cmp_ex_cb cmp) { is1_ret(!l || 0 == _c_len(l), false); _c_cmp(l)  = cmp ; return true; }
bool esl_setRls (esl l, eobj_rls_ex_cb rls) { is0_ret( l                  , false); _c_rls(l)  = rls ; return true; }

int esl_clear(esl l) { return esl_clearEx(l, 0); }
int esl_clearEx(esl l, eobj_rls_ex_cb rls)
{
    int len;

    is0_ret(l, 0);

    len = _c_len(l);

    //! release all nodes inside the skiplist
    __esl_clear_nodes(l, rls);

    //! reset skiplist
    _c_header_init(l);
    _c_len(l)  = 0;
    _c_lv(l)   = 1;
    _c_tail(l) = NULL;

    return len;
}

int esl_free(esl l)
{
    return esl_freeEx(l, 0);
}

int esl_freeEx(esl l, eobj_rls_ex_cb rls)
{
    is0_ret(l, 0);

    //! release all nodes inside the skiplist
    __esl_clear_nodes(l, rls);

    //! release the skiplist itself
    _vn_free(_c_header(l));
    _c_free(l);

    return 1;
}

uint esl_size   (esl l) { return l ?  _eo_len(l) : 0; }
bool esl_isEmpty(esl l) { return l ? !_eo_len(l) : 1; }

eobj   esl_newO(etypeo type, uint len)
{
    _esln n;

    switch (type) {
        case EFALSE : n = __esl_create_node(__esl_random_level(), 0); _n_typecoe(n) = _ESL_COE_FALSE; _n_len(n) = 0; break;
        case ETRUE  : n = __esl_create_node(__esl_random_level(), 0); _n_typecoe(n) = _ESL_COE_TRUE ; _n_len(n) = 0; break;
        case ENULL  : n = __esl_create_node(__esl_random_level(), 0); _n_typecoe(n) = _ESL_COE_NULL ; _n_len(n) = 0; break;
        case ENUM   : n = __esl_create_node(__esl_random_level(), 8); _n_typecoe(n) = _ESL_COE_NUM_I; _n_len(n) = 0; _n_keyI(n) = 0; break;
        case EPTR   : n = __esl_create_node(__esl_random_level(), 8); _n_typecoe(n) = _ESL_COE_PTR  ; _n_len(n) = 0; _n_keyI(n) = 0; break;
        case ESTR   : n = __esl_create_node(__esl_random_level(), len + 1); _n_typecoe(n) = _ESL_COE_STR  ; _n_len(n) = len; _n_wipeR(n, len + 1); break;
        case ERAW   : n = __esl_create_node(__esl_random_level(), len + 1); _n_typecoe(n) = _ESL_COE_RAW  ; _n_len(n) = len; _n_wipeR(n, len + 1); break;
        case EOBJ   : return (eobj)esl_new(EKEY_I);

        default     : return 0;
    }

    return _n_o(n);
}

static eobj __esl_addO(esl l, ekey key, eobj   o, bool multi)
{
    int i, level; ekey key_backup; bool need_dupkey = false; __pos_t pos = {0, {0, }, 0};

    is0_ret(l, 0);

    //! backup key
    key_backup = _eo_key(o);
    if(_c_keys(l))
    {
        if(_k_keyS(key) && _k_keyS(key)[0] != '\0')
        {
            _eo_key(o)  = key;
            need_dupkey = true;
        }
    }
    else
        _eo_key(o) = key;

    //! check if can insert this obj
    __esl_get_key_pos(l, o, &pos, multi, 0);
    if(!pos.canadd)
    {
        _eo_key(o) = key_backup;
        return 0;
    }

    //! do insert operation
    pos.x = _o_vn(o);
    level = _vn_lv(pos.x);

    __esl_insert_node(l, pos.x, pos.update, level, i);

    //! update key
    if(need_dupkey)
    {
        _cur_freekeyS(_k_keyS(key_backup));
        _eo_keyS(o) = _cur_dupkeyS(_k_keyS(key));
    }

    return o;
}

eobj   esl_addI(esl l, ekey key, i64    val) { _esln_t b = { { 0 , 0, 0}, key, {._len =                         8, ._typ = {.t_coe = _ESL_COE_NUM_I}}, {.i =       val} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 0, 0, 0); if(o){                _eo_setI (o, val             ); _eo_typecoe(o) = _EDICT_COE_NUM_I; } return o; }
eobj   esl_addF(esl l, ekey key, f64    val) { _esln_t b = { { 0 , 0, 0}, key, {._len =                         8, ._typ = {.t_coe = _ESL_COE_NUM_I}}, {.f =       val} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 0, 0, 0); if(o){                _eo_setI (o, val             ); _eo_typecoe(o) = _EDICT_COE_NUM_F; } return o; }
eobj   esl_addP(esl l, ekey key, conptr ptr) { _esln_t b = { { 0 , 0, 0}, key, {._len =                         8, ._typ = {.t_coe = _ESL_COE_PTR  }}, {.p = (cptr)ptr} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 0, 0, 0); if(o){                _eo_setP (o, ptr             ); _eo_typecoe(o) = _EDICT_COE_PTR  ; } return o; }
eobj   esl_addR(esl l, ekey key, uint   len) { _esln_t b = { { 0 , 0, 0}, key, {._len =                   len + 1, ._typ = {.t_coe = _ESL_COE_RAW_P}}, {             0} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 0, 0, 0); if(o){                _eo_wipeR(o, len             ); _eo_typeco(o)  = _EDICT_CO_RAW   ; } return o; }
eobj   esl_addS(esl l, ekey key, constr str) { _esln_t b = { { 0 , 0, 0}, key, {._len = str ? strlen(str) + 1 : 1, ._typ = {.t_coe = _ESL_COE_STR_P}}, {.s = (cstr)str} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 0, 0, 0); if(o){ _vn_len(&b)--; _eo_setS (o, str, _vn_len(&b)); _eo_typeco(o)  = _EDICT_CO_STR   ; } return o; }
eobj   esl_addO(esl l, ekey key, eobj   obj) { return __esl_addO(l, key, obj, 0); }

eobj   esl_addMI(esl l, ekey key, i64    val){ _esln_t b = { { 0 , 0, 0}, key, {._len =                         8, ._typ = {.t_coe = _ESL_COE_NUM_I}}, {.i =       val} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 1, 0, 0); if(o){                _eo_setI (o, val             ); _eo_typecoe(o) = _EDICT_COE_NUM_I; } return o; }
eobj   esl_addMF(esl l, ekey key, f64    val){ _esln_t b = { { 0 , 0, 0}, key, {._len =                         8, ._typ = {.t_coe = _ESL_COE_NUM_I}}, {.f =       val} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 1, 0, 0); if(o){                _eo_setI (o, val             ); _eo_typecoe(o) = _EDICT_COE_NUM_F; } return o; }
eobj   esl_addMP(esl l, ekey key, conptr ptr){ _esln_t b = { { 0 , 0, 0}, key, {._len =                         8, ._typ = {.t_coe = _ESL_COE_PTR  }}, {.p = (cptr)ptr} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 1, 0, 0); if(o){                _eo_setP (o, ptr             ); _eo_typecoe(o) = _EDICT_COE_PTR  ; } return o; }
eobj   esl_addMR(esl l, ekey key, uint   len){ _esln_t b = { { 0 , 0, 0}, key, {._len =                   len + 1, ._typ = {.t_coe = _ESL_COE_RAW_P}}, {             0} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 1, 0, 0); if(o){                _eo_wipeR(o, len             ); _eo_typeco(o)  = _EDICT_CO_RAW   ; } return o; }
eobj   esl_addMS(esl l, ekey key, constr str){ _esln_t b = { { 0 , 0, 0}, key, {._len = str ? strlen(str) + 1 : 1, ._typ = {.t_coe = _ESL_COE_STR_P}}, {.s = (cstr)str} }; eobj o = __esl_makeRoom(l, _vn_o(&b), 1, 0, 0); if(o){ _vn_len(&b)--; _eo_setS (o, str, _vn_len(&b)); _eo_typeco(o)  = _EDICT_CO_STR   ; } return o; }
eobj   esl_addMO(esl l, ekey key, eobj   obj){ return __esl_addO(l, key, obj, 1); }

eobj esl_k (esl l, ekey key)
{
    __pos_t pos = {0, {0, }, 0};

    is0_ret(l, 0);

    {
        _esln_t b = { { 0 , 0, 0}, key, {._typ = {.t_coe = _ESL_COE_UNKNOWN}}, {0} };

        __esl_get_key_pos(l, _vn_o(&b), &pos, 0, 0);
    }

    return pos.x ? _vn_o(pos.x) : NULL;
}

i64    esl_kValI(esl l, ekey key) { eobj o = esl_k(l, key); _eo_retI(o); }
f64    esl_kValF(esl l, ekey key) { eobj o = esl_k(l, key); _eo_retF(o); }
cstr   esl_kValS(esl l, ekey key) { eobj o = esl_k(l, key); _eo_retS(o); }
cptr   esl_kValR(esl l, ekey key) { eobj o = esl_k(l, key); _eo_retR(o); }
cptr   esl_kValP(esl l, ekey key) { eobj o = esl_k(l, key); _eo_retP(o); }

etypeo esl_kType  (esl l, ekey key) { eobj o = esl_k(l, key); _eo_retT(o); }
uint   esl_kLen   (esl l, ekey key) { eobj o = esl_k(l, key); _eo_retL(o); }
bool   esl_kIsTrue(esl l, ekey key) { return __eobj_isTrue(esl_k(l, key)); }

eobj   esl_first(esl  l) { return l ? _vn_forward(_c_header(l), 0) ? _vn_o( _vn_forward(_c_header(l), 0) ) : 0 : 0; }
eobj   esl_last (esl  l) { return l ? _c_tail(l) ? _vn_o(_c_tail(l)) : 0 : 0; }
eobj   esl_next (eobj o) { o = (eobj)_vn_forward (_o_vn(o), 0); return o ? _vn_o((_esln)o) : 0; }
eobj   esl_prev (eobj o) { o = (eobj)_vn_backward(_o_vn(o)   ); return o ? _vn_o((_esln)o) : 0; }

eobj esl_takeH(esl l)
{
    _esln update[ESL_MAXLEVEL], h; int i;

    is1_ret(!l || _c_tail(l), 0);  // empty list

    h = _c_header(l);

    for(i = _c_lv(l); i >= 0; i--)
    {
        update[i] = _vn_forward(h, i);
    }

    __esl_take_node(l, _vn_forward(h, 0), update, i);

    return _vn_o(_vn_forward(_c_header(l), 0));
}

eobj esl_takeT(esl l)
{
    _esln update[ESL_MAXLEVEL], x; int i;

    is1_ret(!l || _c_tail(l), 0);  // empty list

    x = _c_header(l);
    for (i = _c_lv(l) - 1; i >= 0; i--)
    {
        while(_vn_forward(x, i) && (_vn_forward(x, i) != _c_tail(l)))
            x = _vn_forward(x, i);

        update[i] = x;
    }

    x = _vn_forward(x, 0);
    __esl_take_node(l, x, update, i);

    return _vn_o(x);
}

eobj esl_takeO(esl l, eobj o)
{
    __pos_t pos = {0, {0, }, 0};

    is1_ret(!l || !o || !_eo_linked(o) || _eo_typeo(o) != ESL, 0);

    __esl_get_key_pos(l, o, &pos, 0, 1);

    if(pos.x)
    {
        int i;
        __esl_take_node(l, pos.x, pos.update, i);

        return _vn_o(pos.x);
    }

    return 0;
}

eobj esl_takeOne(esl l, ekey key)
{
    __pos_t pos = {0, {0}, 0};

    is0_ret(l, 0);

    {
        _esln_t b = { { 0 , 0, 0}, key, {._typ = {.t_coe = _ESL_COE_UNKNOWN}}, {0} };
        __esl_get_key_pos(l, _vn_o(&b), &pos, 0, 0);
    }

    if(pos.x)
    {
        int i;
        __esl_take_node(l, pos.x, pos.update, i);

        return _vn_o(pos.x);
    }

    return 0;
}

int  esl_freeH(esl l) { eobj o = esl_takeH(l); if(o) { if(_c_rls(l)) _c_rls(l)(o, _c_prvt(l)); if(_c_keys(l)) _cur_freekeyS(o); _vn_free(_o_vn(o)); return 1; } return 0; }
int  esl_freeT(esl l) { eobj o = esl_takeT(l); if(o) { if(_c_rls(l)) _c_rls(l)(o, _c_prvt(l)); if(_c_keys(l)) _cur_freekeyS(o); _vn_free(_o_vn(o)); return 1; } return 0; }

int  esl_freeOne(esl l, ekey key){ eobj o = esl_takeOne(l, key); if(o){ if(_c_rls(l)) _c_rls(l)(o, _c_prvt(l)); if(_c_keys(l)) _eo_freeK(o);  _vn_free(_o_vn(o)); return 1; } return 0; }
int  esl_freeO  (esl l, eobj   o)
{
    is0_ret(o, 0);

    if(!l)
    {
        is1_ret(_eo_linked(o) || _eo_typec(o) != ESL, 0);
    }
    else
    {
        o = esl_takeO  (l,   o);
    }

    if(o)
    {
        if(_c_rls(l))  _c_rls(l)(o, _c_prvt(l));
        if(_c_keys(l)) _eo_freeK(o);

        _vn_free(_o_vn(o));
        return 1;
    }

    return 0;
}

constr esl_version()
{
    return ESL_VERSION;
}

