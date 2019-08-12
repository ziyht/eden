/// =====================================================================================
///
///       Filename:  erb.c
///
///    Description:  easy rb_tree, rebuild from linux-4.6.3
///
///        Version:  1.0
///        Created:  03/09/2017 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///            SRC:  linux-4.6.3/include/linux/rbtree.h
///                  linux-4.6.3/include/linux/rbtree_augmented.h
///                  linux-4.6.3/lib/rbtree.c
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#include "etype.h"

#include "erb_p.h"
#include "erb.h"

#define ERB_VERSION     "erb 1.2.2"     // adjust API

#ifdef _WIN32
#undef offsetof
#endif // _WIN32

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "ecompat.h"
#include "eutils.h"

#include "erb.h"
#include "eobj_p.h"

#pragma pack(1)

typedef struct _erb_node_s{
    rb_node         link;
    ekey            key;
    _ehdr_t         hdr;
    eobj_t          obj;
}_erbn_t, * _erbn;

typedef struct _erb_s{

    rb_root         rb;

    i32             keyt;

    eval            prvt;       // private data
    eobj_cmp_ex_cb  cmp;        // compare func
    eobj_rls_ex_cb  rls;        // release func, only have effect on ERAW and EPTR type of obj

}_erb_t, * _erb;

typedef struct erb_root_s{
    rb_node         link;
    ekey            key;
    _ehdr_t         hdr;
    _erb_t          tree;
}_erbr_t, * _erbr;

#pragma pack()

/// -------------------------- micros helper ---------------------------------

#define _CUR_C_TYPE             ERB

#define _DNODE_TYPE             _erbn_t
#define _DNODE_LNK_FIELD        link
#define _DNODE_KEY_FIELD        key
#define _DNODE_HDR_FIELD        hdr
#define _DNODE_OBJ_FIELD        obj

#define _RNODE_TYPE             _erbr_t
#define _RNODE_TYP_FIELD        type
#define _RNODE_OBJ_FIELD        tree

#define _cur_dupkeyS            strdup
#define _cur_cmpkeyS            strcmp
#define _cur_freekeyS           efree

#define _c_r(t)                 container_of(t, _erbr_t, tree)
#define _c_len(l)               _eo_len(l)
#define _c_keys(l)              _eo_keys(l)
#define _c_typecoe(l)           _eo_typecoe(l)
#define _c_typecoe_set(l)       _c_typecoe(l) = _cur_type(_CUR_C_TYPE, COE_OBJ  );
#define _c_free(t)              _r_free(_c_r(t))

#define _c_rb(t)                t->rb
#define _c_rn(t)                _c_rb(t).rb_node
#define _c_keyt(t)              (t)->keyt
#define _c_prvt(t)              (t)->prvt
#define _c_cmp(t)               (t)->cmp
#define _c_rls(t)               (t)->rls

/// ------------------------ inline compat ------------------------
#if defined(_WIN32) && !defined(__cplusplus)
#define INLINE
#ifndef _MSC_VER
#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member) ))
#define offsetof(TYPE, MEMBER)          ((size_t) &((TYPE *)0)->MEMBER)
#endif
#else
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION < 40500
#define INLINE
#else
#define INLINE inline
#endif
#endif

/// -------------------------- erb APIs ---------------------------

erb erb_new (etypek type)
{
    _erbr r = _r_new();

    _r_typeco_set(r);
    _r_keys(r) = (type & EKEY_S) > 0;

    _c_keyt((erb)_r_o(r)) = type;

    return _r_o(r);
}

bool erb_setType(erb t, etypek type)
{
    is1_ret(!t || 0 == _c_len(t), false);

    _c_keys(t)      = (type & EKEY_S) > 0;
    _c_keyt(t)      = type;

    return true;
}

bool erb_setPrvt(erb t, eval          prvt) { is0_ret( t                  , false); _c_prvt(t) = prvt; return true; }
bool erb_setCmp (erb t, eobj_cmp_ex_cb cmp) { is1_ret(!t || 0 == _c_len(t), false); _c_cmp(t)  = cmp ; return true; }
bool erb_setRls (erb t, eobj_rls_ex_cb rls) { is0_ret( t                  , false); _c_rls(t)  = rls ; return true; }

static void __erb_free_root_nokey(erb t, rb_node* node)
{
    if(node->rb_left ) __erb_free_root_nokey(t, node->rb_left);
    if(node->rb_right) __erb_free_root_nokey(t, node->rb_right);

    efree(_l_n(node));
}

static void __erb_free_root_key(erb t, rb_node* node)
{
    if(node->rb_left ) __erb_free_root_key(t, node->rb_left);
    if(node->rb_right) __erb_free_root_key(t, node->rb_right);

    efree(_l_keyS(node));

    efree(_l_n(node));
}

static void __erb_free_root_nokey_rls(erb t, rb_node* node, eobj_rls_ex_cb rls)
{
    if(node->rb_left ) __erb_free_root_nokey_rls(t, node->rb_left, rls);
    if(node->rb_right) __erb_free_root_nokey_rls(t, node->rb_right, rls);

    rls(_l_o(node), _c_prvt(t));

    efree(_l_n(node));
}

static void __erb_free_root_key_rls(erb t, rb_node* node, eobj_rls_ex_cb rls)
{
    if(node->rb_left ) __erb_free_root_key_rls(t, node->rb_left , rls);
    if(node->rb_right) __erb_free_root_key_rls(t, node->rb_right, rls);

    efree(_l_keyS(node)); rls(_l_o(node), _c_prvt(t));

    efree(_l_n(node));
}

int erb_clear(erb t)
{
    return erb_clearEx(t, 0);
}

int  erb_clearEx(erb t, eobj_rls_ex_cb rls)
{
    int len;

    is0_ret(t, 0);

    len = _c_len(t);

    is0_ret(len, 0);

    if(_c_rn(t))
    {
        if(!rls)
            rls = _c_rls(t);

        if(rls)
        {
            _eo_keys(t) ? __erb_free_root_key_rls  (t, _c_rn(t), rls)
                        : __erb_free_root_nokey_rls(t, _c_rn(t), rls) ;
        }
        else
        {
            _eo_keys(t) ? __erb_free_root_key  (t, _c_rn(t))
                        : __erb_free_root_nokey(t, _c_rn(t)) ;
        }
    }

    _c_len(t) = 0;
    _c_rn(t)  = 0;

    return len;
}

uint  erb_size   (erb r) { return r ?  _c_len(r) : 0   ;}
bool  erb_isEmpty(erb r) { return r ? !_c_len(r) : true;}

int  erb_free(erb t)
{
    return erb_freeEx(t, 0);
}

int erb_freeEx(erb t, eobj_rls_ex_cb rls)
{
    is0_ret(t, 0);

    if(_c_rn(t))
    {
        if(!rls)
            rls = _c_rls(t);

        if(rls)
        {
            _eo_keys(t) ? __erb_free_root_key_rls  (t, _c_rn(t), rls)
                        : __erb_free_root_nokey_rls(t, _c_rn(t), rls) ;
        }
        else
        {
            _eo_keys(t) ? __erb_free_root_key  (t, _c_rn(t))
                        : __erb_free_root_nokey(t, _c_rn(t)) ;
        }
    }

    _c_free(t);

    return 1;
}

eobj erb_newO(etypeo type, uint len)
{
    _erbn n;

    switch (type) {
        case EFALSE : _n_newTF(n); break;
        case ETRUE  : _n_newTT(n); break;
        case ENULL  : _n_newTN(n); break;
        case ENUM   : _n_newIc(n); break;
        case EPTR   : _n_newPc(n); break;
        case ESTR   : _n_newSc(n, len); break;
        case ERAW   : _n_newRc(n, len); break;
        case EOBJ   : _n_newOc(n); break;

        default     : return 0;
    }

    return _n_o(n);
}

typedef struct __pos_s
{
    u8        canadd;
    u8        find;
    rb_node  *parent;
    rb_node **pos;

}__pos_t, *__pos;

static __always_inline void __erb_get_key_pos(erb t, eobj obj, __pos pos, int multi)
{
    eval ret;

    pos->canadd = 0;
    pos->find   = 0;
    pos->parent = NULL;
    pos->pos    = &_c_rn(t);

    if(_c_cmp(t))
    {
        // todo
    }
    else
    {
        switch (_c_keyt(t))
        {
            case EKEY_I :   while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.i = _eo_keyI(obj) - _l_keyI(pos->parent);

                                if     (ret.i < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.i > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }
                            break;

            case EKEY_U :   while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.i = _eo_keyU(obj) - _l_keyU(pos->parent);

                                if     (ret.i < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.i > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }
                            break;

            case EKEY_F :   while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.f = _eo_keyF(obj) - _l_keyF(pos->parent);

                                if     (ret.f < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.f > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }
                            break;

            case EKEY_I | EKEY_DES :
                            while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.i = _l_keyI(pos->parent) - _eo_keyI(obj);

                                if     (ret.i < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.i > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }
                            break;

            case EKEY_U | EKEY_DES :
                            while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.i = _l_keyU(pos->parent) - _eo_keyU(obj);

                                if     (ret.i < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.i > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }
                            break;

            case EKEY_F | EKEY_DES :
                            while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.f = _l_keyF(pos->parent) - _eo_keyF(obj);

                                if     (ret.f < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.f > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }
                            break;

            case EKEY_S :   is0_ret(_eo_keyS(obj), );

                            while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.i = strcmp(_eo_keyS(obj), _l_keyS(pos->parent));

                                if     (ret.i < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.i > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }

                            break;

            case EKEY_S | EKEY_DES:
                            is0_ret(_eo_keyS(obj), );

                            while(*pos->pos)
                            {
                                pos->parent = *pos->pos;
                                ret.i = strcmp(_l_keyS(pos->parent), _eo_keyS(obj));

                                if     (ret.i < 0)          pos->pos = &pos->parent->rb_left;
                                else if(ret.i > 0)          pos->pos = &pos->parent->rb_right;
                                else if(!multi)           { pos->find = 1; return;}
                                else                      { pos->find = 1; pos->pos = &pos->parent->rb_right;  }
                            }

                            break;
        }
    }

    pos->canadd = 1;
}

static eobj __erb_makeRoom(erb t, eobj obj, int multi, int overwrite)
{
    _erbn n; __pos_t pos;

    is0_ret(t, 0);

    // -- get position
    __erb_get_key_pos(t, obj, &pos, multi);

    if(overwrite && pos.find)
    {
        uint cur_len;

        n = _l_n(pos.parent);

        switch(_n_typeo(n))
        {
            case EFALSE:
            case ETRUE :
            case ENULL : cur_len = _n_len(n); break;
            case ENUM  : cur_len = 8; break;
            case EPTR  : cur_len = 8; break;
            case ESTR  : cur_len = _n_len(n) + 1; break;
            case ERAW  : cur_len = _n_len(n) + 1; break;
            case EOBJ  : cur_len = 8;
                         erb_free(_n_valP(n)); break;
            default:
                return 0;
        }

        if(cur_len < _eo_len(obj))
        {
            _erbn newn = _n_newr(n, _eo_len(obj));

            if(newn != n)
            {
                *pos.pos = &_n_l(newn);

                n = newn;
            }
        }
    }
    else
    {
        if(!pos.canadd)
            return 0;

        // -- make node
        is0_ret(n = _n_newm(_eo_len(obj)), 0);
        _n_init(n);

        if(_eo_keys(t)) _n_setKeyS(n, _cur_dupkeyS(_eo_keyS(obj)));
        else            _n_setKeyI(n, _eo_keyI(obj));

        // -- link to rbtree
        rb_link_node(&_n_l(n), pos.parent, pos.pos);
        rb_insert_color(&_n_l(n), &_c_rb(t));

        _c_len(t)++;
    }

    return _n_o(n);
}

static __always_inline eobj __erb_addO(erb r, ekey key, eobj obj, int multi)
{
    rb_node **pos, * parent; i64 ret;

    pos      = &_c_rn(r);
    parent   = NULL;

    if(_eo_keys(r))
    {
        while(*pos)
        {
            parent = *pos;
            ret = strcmp(_k_keyS(key), _l_keyS(parent));

            if     (ret < 0)            pos = &parent->rb_left;
            else if(ret > 0)            pos = &parent->rb_right;
            else if(_c_cmp(r))
            {
                do{
                    parent = *pos;
                    ret = _c_cmp(r)(obj, _l_o(parent), _c_prvt(r));

                    if     (ret < 0)            pos = &parent->rb_left;
                    else if(ret > 0)            pos = &parent->rb_right;
                    else if(!multi)             return NULL;
                    else                        pos = &parent->rb_right;
                }while(*pos);
                break;
            }
            else if(!multi)             return NULL;
            else                        pos = &parent->rb_right;
        }

        _eo_setKeyS(obj, _cur_dupkeyS(_k_keyS(key)));
    }
    else
    {
        while(*pos)
        {
            parent = *pos;
            ret = _k_keyI(key) -  _l_keyI(parent);

            if     (ret < 0)            pos = &parent->rb_left;
            else if(ret > 0)            pos = &parent->rb_right;
            else if(_c_cmp(r))
            {
                do{
                    parent = *pos;
                    ret = _c_cmp(r)(obj, _l_o(parent), _c_prvt(r));

                    if     (ret < 0)            pos = &parent->rb_left;
                    else if(ret > 0)            pos = &parent->rb_right;
                    else if(!multi)             return NULL;
                    else                        pos = &parent->rb_right;
                }while(*pos);
                break;
            }
            else if(!multi)             return NULL;
            else                        pos = &parent->rb_right;
        }

        _eo_setKeyI(obj, _k_keyI(key));
    }

    // -- link to rbtree
    rb_link_node(&_eo_l(obj), parent, pos);
    rb_insert_color(&_eo_l(obj), &_c_rb(r));

    _c_len(r)++;

    return obj;
}

eobj erb_addI( erb t, ekey key, i64    val) { _erbn_t b = {{0}, key, {._len =                         8, ._typ = {.t_coe = _ERB_COE_NUM_I}}, {.i =       val} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 0); if(o) {              _eo_setI (o, val            ); _eo_typecoe(o) = _ERB_COE_NUM_I; } return o; }
eobj erb_addF( erb t, ekey key, f64    val) { _erbn_t b = {{0}, key, {._len =                         8, ._typ = {.t_coe = _ERB_COE_NUM_F}}, {.f =       val} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 0); if(o) {              _eo_setF (o, val            ); _eo_typecoe(o) = _ERB_COE_NUM_F; } return o; }
eobj erb_addP( erb t, ekey key, conptr ptr) { _erbn_t b = {{0}, key, {._len =               sizeof(ptr), ._typ = {.t_coe = _ERB_COE_PTR  }}, {.p = (cptr)ptr} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 0); if(o) {              _eo_setP (o, ptr            ); _eo_typeco (o) = _ERB_CO_PTR   ; } return o; }
eobj erb_addR( erb t, ekey key, uint   len) { _erbn_t b = {{0}, key, {._len =                   len + 1, ._typ = {.t_coe = _ERB_COE_RAW  }}, {             0} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 0); if(o) {              _eo_wipeR(o, len            ); _eo_typeco (o) = _ERB_CO_RAW   ; } return o; }
eobj erb_addS( erb t, ekey key, constr str) { _erbn_t b = {{0}, key, {._len = str ? strlen(str) + 1 : 1, ._typ = {.t_coe = _ERB_COE_STR  }}, {.s = (cstr)str} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 0); if(o) {_n_len(&b)--; _eo_setS (o, str, _n_len(&b)); _eo_typeco (o) = _ERB_CO_STR   ; } return o; }
eobj erb_addO( erb t, ekey key, eobj   obj) { is1_ret(!t || !obj || (_eo_keys(t) && !_k_keyS(key)), 0); return __erb_addO(t, key, obj, 0); }

eobj erb_addMI(erb t, ekey key, i64    val) { _erbn_t b = {{0}, key, {._len =                         8, ._typ = {.t_coe = _ERB_COE_NUM_I}}, {.i =       val} }; eobj o = __erb_makeRoom(t, _n_o(&b), 1, 0); if(o) {              _eo_setI (o, val            ); _eo_typecoe(o) = _ERB_COE_NUM_I; } return o; }
eobj erb_addMF(erb t, ekey key, f64    val) { _erbn_t b = {{0}, key, {._len =                         8, ._typ = {.t_coe = _ERB_COE_NUM_F}}, {.f =       val} }; eobj o = __erb_makeRoom(t, _n_o(&b), 1, 0); if(o) {              _eo_setF (o, val            ); _eo_typecoe(o) = _ERB_COE_NUM_F; } return o; }
eobj erb_addMP(erb t, ekey key, conptr ptr) { _erbn_t b = {{0}, key, {._len =               sizeof(ptr), ._typ = {.t_coe = _ERB_COE_PTR  }}, {.p = (cptr)ptr} }; eobj o = __erb_makeRoom(t, _n_o(&b), 1, 0); if(o) {              _eo_setP (o, ptr            ); _eo_typeco (o) = _ERB_CO_PTR   ; } return o; }
eobj erb_addMR(erb t, ekey key, uint   len) { _erbn_t b = {{0}, key, {._len =                   len + 1, ._typ = {.t_coe = _ERB_COE_RAW  }}, {             0} }; eobj o = __erb_makeRoom(t, _n_o(&b), 1, 0); if(o) {              _eo_wipeR(o, len            ); _eo_typeco (o) = _ERB_CO_RAW   ; } return o; }
eobj erb_addMS(erb t, ekey key, constr str) { _erbn_t b = {{0}, key, {._len = str ? strlen(str) + 1 : 1, ._typ = {.t_coe = _ERB_COE_STR  }}, {.s = (cstr)str} }; eobj o = __erb_makeRoom(t, _n_o(&b), 1, 0); if(o) {_n_len(&b)--; _eo_setS (o, str, _n_len(&b)); _eo_typeco (o) = _ERB_CO_STR   ; } return o; }
eobj erb_addMO(erb t, ekey key, eobj   obj) { is1_ret(!t || !obj || (_eo_keys(t) && !_k_keyS(key)), 0); return __erb_addO(t, key, obj, 1); }

eobj erb_setI( erb t, ekey key, i64    val) { _erbn_t b = {{0}, key, {._len =                         8, ._typ = {.t_coe = _ERB_COE_NUM_I}}, {.i =       val} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 1); if(o) {               _eo_setI (o, val            ); _eo_typecoe(o) = _ERB_COE_NUM_I; } return o; }
eobj erb_setF( erb t, ekey key, f64    val) { _erbn_t b = {{0}, key, {._len =                         8, ._typ = {.t_coe = _ERB_COE_NUM_F}}, {.f =       val} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 1); if(o) {               _eo_setF (o, val            ); _eo_typecoe(o) = _ERB_COE_NUM_F; } return o; }
eobj erb_setP( erb t, ekey key, conptr ptr) { _erbn_t b = {{0}, key, {._len =               sizeof(ptr), ._typ = {.t_coe = _ERB_COE_PTR  }}, {.p = (cptr)ptr} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 1); if(o) {               _eo_setP (o, ptr            ); _eo_typeco (o) = _ERB_CO_PTR   ; } return o; }
eobj erb_setR( erb t, ekey key, uint   len) { _erbn_t b = {{0}, key, {._len =                   len + 1, ._typ = {.t_coe = _ERB_COE_RAW  }}, {             0} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 1); if(o) {               _eo_wipeR(o, len            ); _eo_typeco (o) = _ERB_CO_RAW   ; } return o; }
eobj erb_setS( erb t, ekey key, constr str) { _erbn_t b = {{0}, key, {._len = str ? strlen(str) + 1 : 1, ._typ = {.t_coe = _ERB_COE_STR  }}, {             0} }; eobj o = __erb_makeRoom(t, _n_o(&b), 0, 1); if(o) { _n_len(&b)--; _eo_setS (o, str, _n_len(&b)); _eo_typeco (o) = _ERB_CO_STR   ; } return o; }

inline uint  erb_lenO(cptr obj) { return obj ? _eo_len(obj) : 0;}


eobj erb_k(erb t, ekey key)
{
    rb_node *itr; i64 ret;

    is0_ret(t, 0);

    itr = _c_rn(t);

    if(_eo_keys(t))
    {
        is0_ret(_k_keyS(key), 0);

        while(itr)
        {
            ret = strcmp(_k_keyS(key), _l_keyS(itr));

            if     (ret < 0) itr = itr->rb_left;
            else if(ret > 0) itr = itr->rb_right;
            else             return _l_o(itr);

        }
    }
    else
    {
        while(itr)
        {
            ret = _k_keyI(key) - _l_keyI(itr);

            if     (ret < 0) itr = itr->rb_left;
            else if(ret > 0) itr = itr->rb_right;
            else             return _l_o(itr);

        }
    }

    return NULL;
}
i64    erb_kValI  (erb t, ekey key) { eobj o = erb_k(t, key); _eo_retI(o); }
f64    erb_kValF  (erb t, ekey key) { eobj o = erb_k(t, key); _eo_retF(o); }
cptr   erb_kValP  (erb t, ekey key) { eobj o = erb_k(t, key); _eo_retP(o); }
cstr   erb_kValS  (erb t, ekey key) { eobj o = erb_k(t, key); _eo_retS(o); }
cptr   erb_kValR  (erb t, ekey key) { eobj o = erb_k(t, key); _eo_retR(o); }
etypeo erb_kType  (erb t, ekey key) { eobj o = erb_k(t, key); _eo_retT(o); }
uint   erb_kLen   (erb t, ekey key) { eobj o = erb_k(t, key); _eo_retL(o); }
bool   erb_kIsTrue(erb r, ekey key) { return __eobj_isTrue(erb_k(r, key)); }

eobj erb_first(erb  t) { if(t) { rb_node* l = rb_first(&_c_rb(t)); if(l) return _l_o(l);} return 0;}
eobj erb_last (erb  t) { if(t) { rb_node* l = rb_last (&_c_rb(t)); if(l) return _l_o(l);} return 0;}
eobj erb_next (eobj o) { if(o) { rb_node* l = rb_next (&_eo_l(o)); if(l) return _l_o(l);} return 0;}
eobj erb_prev (eobj o) { if(o) { rb_node* l = rb_prev (&_eo_l(o)); if(l) return _l_o(l);} return 0;}

eobj erb_takeH(erb  t) { eobj o = erb_first(t); if(o){ rb_erase(&_eo_l(o), &_c_rb(t)); _c_len(t)--;} return o; }
eobj erb_takeT(erb  t) { eobj o = erb_last (t); if(o){ rb_erase(&_eo_l(o), &_c_rb(t)); _c_len(t)--;} return o; }

eobj erb_takeO  (erb t, eobj obj) { is1_ret(!t || !obj || !_eo_linked(obj) || _eo_typeo(obj) != ERB, 0); rb_erase(&_eo_l(obj), &_c_rb(t)); _c_len(t)--; return obj; }
eobj erb_takeOne(erb t, ekey key) { eobj o = erb_k(t, key); if(o){ rb_erase(&_eo_l(o), &_c_rb(t)); _c_len(t)--; } return o; }

int  erb_freeH(erb t) { eobj o = erb_takeH(t); if(o) { if(_c_rls(t)) _c_rls(t)(o, _c_prvt(t)); if(_eo_keys(o)) _eo_freeK(o); _eo_free(o); return 1; } return 0; }
int  erb_freeT(erb t) { eobj o = erb_takeT(t); if(o) { if(_c_rls(t)) _c_rls(t)(o, _c_prvt(t)); if(_eo_keys(o)) _eo_freeK(o); _eo_free(o); return 1; } return 0; }

int  erb_freeOne(erb t, ekey key) { eobj o = erb_takeOne(t, key); if(o) { if(_eo_keys(t)) _eo_freeK(o); efree(&_eo_l(o)); return 1; } return 0; }
int  erb_freeO  (erb t, eobj   o)
{
    is0_ret(o, 0);

    if(!t)
    {
        is1_ret(_eo_linked(o) || _eo_typec(o) != ESL, 0);
    }
    else
    {
        o = erb_takeO  (t,   o);
    }

    if(o)
    {
        if(_c_rls(t))  _c_rls(t)(o, _c_prvt(t));
        if(_c_keys(t)) _eo_freeK(o);

        _n_free(_eo_dn(o));
        return 1;
    }

    return 0;
}

void erb_show (erb t, uint len)
{
    rb_node* itr;

    is0_ret(t, );

    if (len > _c_len(t)) len = _c_len(t);

    printf("(erb: %s %d/%d)", _eo_keys(t) ? "STR" : "INT", len, _c_len(t));

    is0_exeret(len > 0, puts("");fflush(stdout);, );

    printf(": {\n");

    if(_eo_keys(t))
    {
        for(itr = rb_first(&_c_rb(t)); itr; itr = rb_next(itr))
        {
            switch (_l_typeoe(itr)) {
            case _EFALSE: printf("    \"%s\": false,\n"          , _l_keyS(itr));break;
            case _ETRUE : printf("    \"%s\": true,\n"           , _l_keyS(itr));break;
            case _ENULL : printf("    \"%s\": null,\n"           , _l_keyS(itr));break;
            case _ENUM_I: printf("    \"%s\": \"%"PRIi64"\",\n"  , _l_keyS(itr), _l_valI(itr)); break;
            case _ENUM_F: printf("    \"%s\": \"%f\",\n"         , _l_keyS(itr), _l_valF(itr)); break;
            case _EPTR  : printf("    \"%s\": \"%p\",\n"         , _l_keyS(itr), _l_valP(itr)); break;
            case _ESTR  : printf("    \"%s\": \"%s\",\n"         , _l_keyS(itr), _l_valS(itr)); break;
            case _ERAW  : printf("    \"%s\": RAW(...),\n"       , _l_keyS(itr));break;
            case _EOBJ  : printf("    \"%s\": OBJ,\n"            , _l_keyS(itr));break;
            }
            if(--len == 0) break;
        }
    }
    else
    {
        for(itr = rb_first(&_c_rb(t)); itr; itr = rb_next (itr))
        {
            switch (_l_typeoe(itr)) {
            case _EFALSE     : printf("    \"%"PRIi64"\": false,\n"         , _l_keyI(itr)); break;
            case _ETRUE      : printf("    \"%"PRIi64"\": true,\n"          , _l_keyI(itr)); break;
            case _ENULL      : printf("    \"%"PRIi64"\": null,\n"          , _l_keyI(itr)); break;
            case _ENUM_I     : printf("    \"%"PRIi64"\": %"PRIi64",\n"     , _l_keyI(itr), _l_valI(itr)); break;
            case _ENUM_F     : printf("    \"%"PRIi64"\": %f,\n"            , _l_keyI(itr), _l_valF(itr)); break;
            case _EPTR       : printf("    \"%"PRIi64"\": %p,\n"            , _l_keyI(itr), _l_valP(itr)); break;
            case _ESTR       : printf("    \"%"PRIi64"\": \"%s\",\n"        , _l_keyI(itr), _l_valS(itr)); break;
            case _ERAW       : printf("    \"%"PRIi64"\": RAW(...),\n"      , _l_keyI(itr));break;
            }
            if(--len == 0) break;
        }
    }

    printf("}\n"); fflush(stdout);
}

