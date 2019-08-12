/// =====================================================================================
///
///       Filename:  eobj_p.h
///
///    Description:  this is a eobj header file for internal using
///
///        Version:  1.0
///        Created:  02/28/2017 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///         Needed:  eobj.h
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EOBJ_HEADER_H__
#define __EOBJ_HEADER_H__

#include "etype.h"
#include "ealloc.h"
#include "eobj.h"
#include "eutils.h"

//! ================================================
//! eobj header type
//! ================================================

#pragma pack(1)

/**
 *
 *            (len)       (field)
 *  node ->  --------------------
 *           | <dynamic> : link |
 *           |-------------------
 *           | 64        : key  |              <- exist if needed
 *           |------------------------------
 *           | 64        : hdr  | 32: len  |
 *           |                  | 16: ref  |   <- not used now
 *           |                  |-----------------------
 *           |                  | 16: hdt  | 4: t_c    |  EJSON ELL ERB EDICT ESL ...
 *           |                  |          | 4: t_o    |  EFALSE ETRUE ENULL ENUM ESTR EPTR ERAW EOBJ EARR
 *           |                  |          | 1: t_e    |  ENUM : is float?   ESTR ERAW: is a ptr?
 *           |                  |          | 2: t_k    |  EKEY_S EKEY_I EKEY_NO
 *           |                  |          | 1: linked |
 *           |                  |          | 4: __     |  reserved
 *  eobj ->  |-----------------------------------------
 *           | <dynamic> : obj  |
 *           --------------------
 *
 *
 */
typedef union {
    struct {
        uint t_c    : 4;         // class type: ejson elist erb edict ...
        uint t_o    : 4;         // obj   type: FALSE TRUE NUM ...
        uint t_e    : 1;         // ex    type: NUM: float STR: ptr RAW: ptr
        uint t_k    : 2;         // key   type: key_s key_i no_key
    }__1;

    struct {
        uint t_c    :  4;
        uint t_oe   :  5;
        uint keys   :  1;       // is a str key ?
        uint hkey   :  1;       // have a key   ?
    }__2;

    uint     t_co   : 8;
    uint     t_coe  : 9;
    uint     t_all  : 11;

    struct {
        uint __     : 11;
        uint linked :  1;       // linked?

        uint lentype:  4;

        uint ref    : 16;
    }__3;

}_ehdt_t, * _ehdt_p;

#pragma pack()

#define _EHDT_TYPE_OFFSET_O         4 // offsetof(_ehdt_t, __1.t_o) // 4 object type offset
#define _EHDT_TYPE_OFFSET_N_F       8 // offsetof(_ehdt_t, __1.t_n) // 8 float type offset

//! _ehdt_type_k()
#define _EHDT_NOKEY         0   // 00
#define _EHDT_KEYI          2   // 10
#define _EHDT_KEYS          3   // 11

#define _EHDT_NUMI          ENUM
#define _EHDT_NUMF          (ENUM | 1 << 4)
#define _EHDT_STRP          (ESTR | 1 << 4)
#define _EHDT_RAWP          (ERAW | 1 << 4)

#define _ehdt_type_c(t)     (t).__1.t_c
#define _ehdt_type_o(t)     (t).__1.t_o
#define _ehdt_type_e(t)     (t).__1.t_e
#define _ehdt_type_k(t)     (t).__1.t_k
#define _ehdt_type_co(t)    (t).t_co
#define _ehdt_type_oe(t)    (t).__2.t_oe
#define _ehdt_type_coe(t)   (t).t_coe

#define _ehdt_keys(t)       (t).__2.keys
#define _ehdt_hkey(t)       (t).__2.hkey
#define _ehdt_linked(t)     (t).__3.linked
#define _ehdt_refed(t)      (t).__3.refed

//! ================================================
//! eobj header
//! ================================================
#pragma pack(push, 1)
typedef struct _eobj_header_s{
    uint        _len;       // 32
    _ehdt_t     _typ;       // 32
}_ehdr_t, * _ehdr_p;
#pragma pack(pop)

#define _ehdr_len(h)       (h)._len
#define _ehdr_typ(h)       (h)._typ

#define _ehdr_type_c(h)    _ehdt_type_c(_ehdr_typ(h))
#define _ehdr_type_o(h)    _ehdt_type_o(_ehdr_typ(h))
#define _ehdr_type_e(h)    _ehdt_type_e(_ehdr_typ(h))
#define _ehdr_type_k(h)    _ehdt_type_k(_ehdr_typ(h))
#define _ehdr_type_co(h)   _ehdt_type_co(_ehdr_typ(h))
#define _ehdr_type_oe(h)   _ehdt_type_oe(_ehdr_typ(h))
#define _ehdr_type_coe(h)  _ehdt_type_coe(_ehdr_typ(h))

#define _ehdr_keys(h)      _ehdt_keys(_ehdr_typ(h))
#define _ehdr_hkey(h)      _ehdt_hkey(_ehdr_typ(h))
#define _ehdr_linked(h)    _ehdt_linked(_ehdr_typ(h))
#define _ehdr_refed(h)     _ehdt_refed(_ehdr_typ(h))


//! ================================================
//! types definition
//! ================================================
#undef  _XX
#define _XX(ectype)                                                           \
    _ ## ectype ## _CO_FALSE  = ((ectype) | (EFALSE << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_TRUE   = ((ectype) | (ETRUE  << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_NULL   = ((ectype) | (ENULL  << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_NUM    = ((ectype) | (ENUM   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_PTR    = ((ectype) | (EPTR   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_STR    = ((ectype) | (ESTR   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_RAW    = ((ectype) | (ERAW   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_OBJ    = ((ectype) | (EOBJ   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_ARR    = ((ectype) | (EARR   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _CO_UNKNOWN= ((ectype) | (EOBJ_UNKNOWN   << _EHDT_TYPE_OFFSET_O)),   \
    _ ## ectype ## _COE_FALSE = ((ectype) | (EFALSE << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_TRUE  = ((ectype) | (ETRUE  << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_NULL  = ((ectype) | (ENULL  << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_NUM_I = ((ectype) | (ENUM   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_NUM_F = ((ectype) | (ENUM   << _EHDT_TYPE_OFFSET_O) | (1 << _EHDT_TYPE_OFFSET_N_F)), \
    _ ## ectype ## _COE_PTR   = ((ectype) | (EPTR   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_STR   = ((ectype) | (ESTR   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_STR_P = ((ectype) | (ESTR   << _EHDT_TYPE_OFFSET_O) | (1 << _EHDT_TYPE_OFFSET_N_F)), \
    _ ## ectype ## _COE_RAW   = ((ectype) | (ERAW   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_RAW_P = ((ectype) | (ERAW   << _EHDT_TYPE_OFFSET_O) | (1 << _EHDT_TYPE_OFFSET_N_F)), \
    _ ## ectype ## _COE_OBJ   = ((ectype) | (EOBJ   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_ARR   = ((ectype) | (EARR   << _EHDT_TYPE_OFFSET_O)), \
    _ ## ectype ## _COE_UNKNOWN=((ectype) | (EOBJ_UNKNOWN   << _EHDT_TYPE_OFFSET_O))    \


typedef enum _eo_type_map{

    _EFALSE = EFALSE ,          // 0
    _ETRUE  = ETRUE,            // 1
    _ENULL  = ENULL,            // 2
    _ENUM   = ENUM,             // 3
    _ESTR   = ESTR,             // 4
    _EPTR   = EPTR,             // 5
    _ERAW   = ERAW,             // 6
    _EOBJ   = EOBJ,             // 7
    _EARR   = EARR,             // 8

    _ENUM_I = _EHDT_NUMI,
    _ENUM_F = _EHDT_NUMF,

    _ESTR_P = _EHDT_STRP,
    _ERAW_P = _EHDT_RAWP,

    _EO_NOKEY = _EHDT_NOKEY,
    _EO_KEYI  = _EHDT_KEYI,
    _EO_KEYS  = _EHDT_KEYS,

    _XX(EJSON),
    _XX(ELL),
    _XX(EDICT),
    _XX(ERB),
    _XX(ESL),
    _XX(EQL),

}_eotype;
#undef _XX

//! ================================================
//! macro definition
//! ================================================
#pragma pack(1)

typedef struct _eroot_s{
    _ehdt_t     typ;
    eobj_t      obj;
}_eroot_t, * _eroot_p;

typedef struct _enode_s{
    _ehdr_t     hdr;
    eobj_t      obj;
}_enode_t;

#pragma pack()

#define _eobj_newm(len)       emalloc((len))
#define _eobj_newc(len)       ecalloc((len), 1)
#define _eobj_newr(o, len)    erealloc(o, (len))
#define _eobj_free(eo)        efree(eo)

#define __cur_type(t, post)   _ ## t ## _ ## post
#define _cur_type(t, post)    __cur_type(t, post)

#define _k_keyS(k)          (k).s
#define _k_keyU(k)          (k).u
#define _k_keyI(k)          (k).i
#define _k_keyF(k)          (k).f
#define _k_lenS(k)          sizeof(_k_keyS(k))
#define _k_lenU(k)          sizeof(_k_keyU(k))
#define _k_lenI(k)          sizeof(_k_keyI(k))
#define _k_lenF(K)          sizeof(_k_keyF(k))

//! -- root node macros --
//!
#define _r_new()            _eobj_newc(sizeof(_RNODE_TYPE))
#define _r_free(r)          _eobj_free(r)

#define _r_o(r)            &(r)->_RNODE_OBJ_FIELD
#define _r_h(r)             (r)->hdr
#define _r_len(r)           _ehdr_len(_r_h(r))
#define _r_typec(r)         _ehdr_type_c(_r_h(r))
#define _r_typeo(r)         _ehdr_type_o(_r_h(r))
#define _r_typek(r)         _ehdr_type_k(_r_h(r))
#define _r_typeco(r)        _ehdr_type_co(_r_h(r))
#define _r_keys(r)          _ehdr_keys(_r_h(r))

#define _r_hkey(r)          _ehdr_hkey(_r_h(r))
#define _r_linked(r)        _ehdr_linked(_r_h(r))
#define _r_refed(r)         _ehdr_refed(_r_h(r))

#define _r_typeco_set(r)    _ehdr_type_co(_r_h(r)) = _cur_type(_CUR_C_TYPE, CO_OBJ)

//! -- data node macros --
//!
#define _n_newm(l)          _eobj_newm(   sizeof(_DNODE_TYPE) - sizeof(eobj_t) + l)
#define _n_newc(l)          _eobj_newc(   sizeof(_DNODE_TYPE) - sizeof(eobj_t) + l)
#define _n_newr(n, l)       _eobj_newr(n, sizeof(_DNODE_TYPE) - sizeof(eobj_t) + l)
#define _n_init(n)          memset(n,  0, sizeof(_DNODE_TYPE) - sizeof(eobj_t));

#define _n_newT(n)          do{ n = _n_newc((0));                       }while(0)
#define _n_newNm(n)         do{ n = _n_newm(8);             _n_init(n); }while(0)
#define _n_newNc(n)         do{ n = _n_newc(8);                         }while(0)

#define _n_newTF(n)         do{ _n_newT(n);                              _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_FALSE);                 }while(0)
#define _n_newTT(n)         do{ _n_newT(n);                              _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_TRUE );                 }while(0)
#define _n_newTN(n)         do{ _n_newT(n);                              _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_NULL );                 }while(0)
#define _n_newIc(n)         do{ _n_newNc(n);                             _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_NUM_I);                 }while(0)
#define _n_newFc(n)         do{ _n_newNc(n);                             _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_NUM_F);                 }while(0)
#define _n_newPc(n)         do{ n = _n_newc(sizeof(void*));              _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_PTR  );                 }while(0)
#define _n_newSc(n, l)      do{ n = _n_newc((l) + 1);                    _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_STR  ); _n_len(n) = l;  }while(0)
#define _n_newRc(n, l)      do{ n = _n_newc((l) + 1);                    _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_RAW  ); _n_len(n) = l;  }while(0)
#define _n_newOc(n)         do{ n = _n_newc(sizeof(_RNODE_TYPE));        _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_OBJ  );                 }while(0)

#define _n_newSm(n, l)      do{ n = _n_newm((l) + 1);                    _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_STR  ); _n_len(n) = l;  }while(0)

#define _n_newN(n, v, t)    do{ n = _n_newm(8);             _n_init(n);  _n_typecoe(n) = t;                                 _n_setV(n, v);                          }while(0)
#define _n_newI(n, v)       do{ n = _n_newm(8);             _n_init(n);  _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_NUM_I); _n_setI(n, v);                          }while(0)
#define _n_newF(n, v)       do{ n = _n_newm(8);             _n_init(n);  _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_NUM_F); _n_setF(n, v);                          }while(0)
#define _n_newP(n, p)       do{ n = _n_newm(sizeof(void*)); _n_init(n);  _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_PTR  ); _n_setP(n, p);                          }while(0)
#define _n_newSl(n,s, l)    do{ n = _n_newm((l) + 1);       _n_init(n);  _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_STR  ); _n_setS(n, s, l);                       }while(0)
#define _n_newR(n, r, l)    do{ n = _n_newm((l) + 1);       _n_init(n);  _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_RAW  ); _n_setR(n, r, l); _n_valR(n)[l] = '\0'; }while(0)
#define _n_newS(n, s)       do{ int l = s ? strlen(s) : 0; _n_newSl(n, s, l); }while(0)

#define _n_free(n)          _eobj_free(n)
#define _n_freeK(n)         _cur_freekeyS(_n_keyS(n))

#define _n_l(n)             (n)->_DNODE_LNK_FIELD
#define _n_key(n)           (n)->_DNODE_KEY_FIELD
#define _n_h(n)             (n)->_DNODE_HDR_FIELD

#define _n_typec(n)         _ehdr_type_c(_n_h(n))
#define _n_typeo(n)         _ehdr_type_o(_n_h(n))
#define _n_typee(n)         _ehdr_type_e(_n_h(n))
#define _n_typek(n)         _ehdr_type_k(_n_h(n))
#define _n_typeco(n)        _ehdr_type_co(_n_h(n))
#define _n_typeoe(n)        _ehdr_type_oe(_n_h(n))
#define _n_typecoe(n)       _ehdr_type_coe(_n_h(n))
#define _n_len(n)           _ehdr_len(_n_h(n))
#define _n_linked(n)        _ehdr_linked(_n_h(n))

#define _n_keyS(n)          _k_keyS(_n_key(n))
#define _n_keyI(n)          _k_keyI(_n_key(n))
#define _n_keyU(n)          _k_keyU(_n_key(n))
#define _n_keyF(n)          _k_keyF(_n_key(n))

#define _n_setKeyS(n, s)    do{_k_keyS(_n_key(n)) = s; _n_typek(n) = _EO_KEYS; }while(0)
#define _n_setKeyI(n, i)    do{_k_keyI(_n_key(n)) = i; _n_typek(n) = _EO_KEYI; }while(0)

#define _n_o(n)            &(n)->_DNODE_OBJ_FIELD
#define _n_valI(n)          (n)->_DNODE_OBJ_FIELD.i
#define _n_valF(n)          (n)->_DNODE_OBJ_FIELD.f
#define _n_valS(n)          (n)->_DNODE_OBJ_FIELD.r
#define _n_valR(n)          (n)->_DNODE_OBJ_FIELD.r
#define _n_valP(n)          (n)->_DNODE_OBJ_FIELD.p
#define _n_valV(n)          (n)->_DNODE_OBJ_FIELD.v

#define _n_setV(n, v)       _n_valV(n) = v
#define _n_setN(n, v)       _n_setV(n, *(eval*)&v)
#define _n_setI(n, v)       _n_setV(n, *(eval*)&v)
#define _n_setF(n, v)       _n_setV(n, *(eval*)&v)
#define _n_setP(n, v)       _n_setV(n, *(eval*)&v)
#define _n_setS(n, s, l)    _n_setR(n, s, l); _n_valS(n)[l] = '\0'
#define _n_setR(n, r, l)    memcpy(_n_o(n), r, l); _n_len(n) = l

#define _n_wipeR(n, l)      memset(_n_o(n), 0, l)

//! -- link filed macros --
//!
#define _l_n(l)             container_of(l, _DNODE_TYPE, _DNODE_LNK_FIELD)
#define _l_o(l)             _n_o(_l_n(l))
#define _l_type(l)          _n_type(_l_n(l))
#define _l_typeo(l)         _n_typeo(_l_n(l))
#define _l_typeoe(l)        _n_typeoe(_l_n(l))

#define _l_len(l)           _n_len(_l_n(l))

#define _l_keyI(l)          _n_keyI(_l_n(l))
#define _l_keyU(l)          _n_keyU(_l_n(l))
#define _l_keyF(l)          _n_keyF(_l_n(l))
#define _l_keyS(l)          _n_keyS(_l_n(l))
#define _l_valI(l)          _eo_valI(_l_o(l))
#define _l_valF(l)          _eo_valF(_l_o(l))
#define _l_valP(l)          _eo_valP(_l_o(l))
#define _l_valS(l)          _eo_valS(_l_o(l))
#define _l_valR(l)          _eo_valR(_l_o(l))
#define _l_valV(l)          _eo_valV(_l_o(l))

//! -- obj field macros
//!
#define _eo_en(o)           container_of(o, _enode_t, obj)
#define _eo_h(o)            _eo_en(o)->hdr
#define _eo_t(o)            _eo_h(o)._typ

#define _eo_dn(o)           container_of(o, _DNODE_TYPE, _DNODE_OBJ_FIELD)
#define _eo_rn(o)           container_of(o, _RNODE_TYPE, _RNODE_OBJ_FIELD)

#define _eo_free(o)         _eobj_free(_eo_dn(o))
#define _eo_freeK(o)        _cur_freekeyS(_eo_keyS(o))
#define _eo_l(o)            _n_l(_eo_dn(o))

#define _eo_typec(o)        _ehdt_type_c(_eo_t(o))      // class type
#define _eo_typeo(o)        _ehdt_type_o(_eo_t(o))      // obj   type
#define _eo_typen(o)        _ehdt_type_n(_eo_t(o))      // num   type
#define _eo_typek(o)        _ehdt_type_k(_eo_t(o))      // key   type
#define _eo_typeco(o)       _ehdt_type_co(_eo_t(o))     // class and obj
#define _eo_typeoe(o)       _ehdt_type_oe(_eo_t(o))
#define _eo_typecoe(n)      _ehdt_type_coe(_eo_t(n))

#define _eo_keys(n)         _ehdt_keys(_eo_t(n))
#define _eo_hkey(n)         _ehdt_hkey(_eo_t(n))
#define _eo_linked(n)       _ehdt_linked(_eo_t(n))
#define _eo_refed(n)        _ehdt_refed(_eo_t(n))

#define _eo_len(n)          _ehdr_len(_eo_h(n))

#define _eo_key(o)          ( (ekey*)(o))[-2]   // todo: not safe in x86 between x64
#define _eo_keyI(o)         ( (i64* )(o))[-2]
#define _eo_keyU(o)         ( (u64* )(o))[-2]
#define _eo_keyF(o)         ( (f64* )(o))[-2]
#define _eo_keyS(o)         ( (cstr*)(o))[-2]
#define _eo_valI(o)         (*(i64* )(o))
#define _eo_valF(o)         (*(f64* )(o))
#define _eo_valP(o)         (*(cptr*)(o))
#define _eo_valS(o)         ( (cstr )(o))
#define _eo_valR(o)         ( (cptr )(o))
#define _eo_valV(o)         (*(eval*)(o))

#define _eo_setKeyS(o, s)   do{_eo_keyS(o) = s; _eo_typek(o) = _EO_KEYS; }while(0)
#define _eo_setKeyI(o, i)   do{_eo_keyI(o) = i; _eo_typek(o) = _EO_KEYI; }while(0)

#define _eo_setV(o, v)       _eo_valV(o) = v
#define _eo_setN(o, v)       _eo_setV(o, *(eval*)&v)
#define _eo_setI(o, v)       _eo_setV(o, *(eval*)&v)
#define _eo_setF(o, v)       _eo_setV(o, *(eval*)&v)
#define _eo_setP(o, v)       _eo_setV(o, *(eval*)&v)
#define _eo_setS(o, s, l)    _eo_setR(o, s, l); _eo_valS(o)[l] = '\0';
#define _eo_setR(o, r, l)    memcpy((o), r, l); _eo_len(o) = l

#define _eo_wipeR(o, l)      memset((o), 0, l); _eo_len(o) = l

#define _eo_retT(o)         return o ? _eo_typeo(o) : EOBJ_UNKNOWN
#define _eo_retI(o)         if(o){ switch(_eo_typeoe(o)){ case _ENUM_I: return      _eo_valI(o); case _ENUM_F: return (i64)_eo_valF(o); }} return   0
#define _eo_retF(o)         if(o){ switch(_eo_typeoe(o)){ case _ENUM_I: return (f64)_eo_valI(o); case _ENUM_F: return      _eo_valF(o); }} return 0.0
#define _eo_retP(o)         if(o && EPTR == _eo_typeo(o)) return _eo_valP(o); return 0
#define _eo_retS(o)         if(o && ESTR == _eo_typeo(o)) return _eo_valS(o); return 0
#define _eo_retR(o)         if(o && ERAW == _eo_typeo(o)) return _eo_valR(o); return 0

#define _eo_retL(o)         if(o){ return _eo_len(o); } return 0
#define _ec_retSize(o)      if(o && _eo_typeo(o) >= EOBJ){ return _eo_len(o);} return 0
#define _ec_isEmpty(o)      ((o && _eo_typeo(o) >= EOBJ) ? !_eo_len(o) : true)

static __always_inline bool __eobj_isTrue(eobj o)
{
    if(o)
    {
        switch (_eo_typeo(o)) {
            case ETRUE:   return true;
            case ENUM :   return _eo_valI(o)    != 0;
            case ESTR :   return _eo_valS(o)[0] != '\0';
            case EPTR :   return _eo_valP(o)    != NULL;
            case ERAW :   return _eo_len(o)     != 0;
        }
    }

    return false;
}

static __always_inline i64 __eobj_valI(eobj o) { switch(_eo_typeoe(o)){ case _ENUM_I: return      _eo_valI(o); case _ENUM_F: return (i64)_eo_valF(o); } return 0;}
static __always_inline f64 __eobj_valF(eobj o) { switch(_eo_typeoe(o)){ case _ENUM_I: return (f64)_eo_valI(o); case _ENUM_F: return      _eo_valF(o); } return 0;}

constr __eobj_typeS(eobj o, bool beauty);

typedef eobj (*__eobj_alloc)(eobj o, int olen);

eobj   __eobj_subS(eobj o, constr from, constr to, __eobj_alloc alloc);


#endif

#ifdef _cur_cmpkeyS

static inline i64 __eobj_key_cmp_i64_asc(eobj o1, eobj o2) { return _eo_keyI(o1) - _eo_keyI(o2);  }
static inline i64 __eobj_key_cmp_i64_des(eobj o1, eobj o2) { return _eo_keyI(o2) - _eo_keyI(o1);  }
static inline i64 __eobj_key_cmp_u64_asc(eobj o1, eobj o2) { return _eo_keyU(o1) - _eo_keyU(o2);  }
static inline i64 __eobj_key_cmp_u64_des(eobj o1, eobj o2) { return _eo_keyU(o2) - _eo_keyU(o1);  }
static inline i64 __eobj_key_cmp_f64_asc(eobj o1, eobj o2) { return _eo_keyF(o1) - _eo_keyF(o2);  }
static inline i64 __eobj_key_cmp_f64_des(eobj o1, eobj o2) { return _eo_keyF(o2) - _eo_keyF(o1);  }
static inline i64 __eobj_key_cmp_str_asc(eobj o1, eobj o2) { return _cur_cmpkeyS(_eo_keyS(o1), _eo_keyS(o2));  }
static inline i64 __eobj_key_cmp_str_des(eobj o1, eobj o2) { return _cur_cmpkeyS(_eo_keyS(o2), _eo_keyS(o1));  }

#endif
