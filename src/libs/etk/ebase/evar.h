/// =====================================================================================
///
///       Filename:  evar.h
///
///    Description:  a struct to hold variant data, like evec, but more simple
///
///        Version:  1.1
///        Created:  04/01/2019 08:51:34 AM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EVAR_H__
#define __EVAR_H__

#include "etype.h"

#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum etypev_s{

    //! val
    E_NAV =  0,         // not a evar

    E_CHAR=  1,         // treat CHAR as I8, they are typically the same

    E_I8  =  1,         //
    E_I16 =  2,         //
    E_I32 =  3,         //
    E_I64 =  4,         //

    E_U8  =  5,         //
    E_U16 =  6,         //
    E_U32 =  7,         //
    E_U64 =  8,         //

    E_F32 =  9,         //
    E_F64 = 10,         //

    E_PTR = 11,         //
    E_STR = 12,         // handle as estr internal, so the esize of this type is 8
    E_RAW = 13,         // handle as estr internal, so the esize of this type is 8

    E_SIG = 14,
    E_VAR = 15,

    E_USER= 16,         // E_USER

    E_UNKOWN = 0xff,
}etypev;

#define __EVAR_ITEM_LEN_MAP {0,  1, 2, 4, 8,  1, 2, 4, 8,  4, 8,  8, 8, 8,  0, 16}  // up to

//! __ETYPEV_ARR_MASK:
//!
//!   |type|size|cnt|v          |
//!                  (size *cnt)
//!
//! __ETYPEV_PTR_MASK:
//!
//!    1    2    4   8
//!   |type|size|cnt|ptr|
//!                   |-------> |   data    |
//!                              (size *cnt)
//!
typedef struct evar_s{
    u8      __;             // internal using
    u8      type  ;         // type
    u16     esize ;         // element size
    uint    cnt   ;         // element count
    eval    v     ;         // val(data)
}evar_t, evar, * evarp;

/** -----------------------------------------------------
 *
 *  evar quick makers
 *
 */

#define __EVAR_MK(t, s, c, v)   (evar){0, t, s, c, v }

#define EVAR_NAV            __EVAR_MK(E_NAV, 0, 0, EVAL_0)

#define EVAR_I8( v)         (evar){0, E_I8 , 1, 1, EVAL_I8 (v)}
#define EVAR_I16(v)         (evar){0, E_I16, 2, 1, EVAL_I16(v)}
#define EVAR_I32(v)         (evar){0, E_I32, 4, 1, EVAL_I32(v)}
#define EVAR_I64(v)         (evar){0, E_I64, 8, 1, EVAL_I64(v)}

#define EVAR_U8( v)         (evar){0, E_U8 , 1, 1, EVAL_U8 (v)}
#define EVAR_U16(v)         (evar){0, E_U16, 2, 1, EVAL_U16(v)}
#define EVAR_U32(v)         (evar){0, E_U32, 4, 1, EVAL_U32(v)}
#define EVAR_U64(v)         (evar){0, E_U64, 8, 1, EVAL_U64(v)}

#define EVAR_F32(v)         (evar){0, E_F32, 4, 1, EVAL_F32(v)}
#define EVAR_F64(v)         (evar){0, E_F64, 8, 1, EVAL_F64(v)}

#define EVAR_S(  v)         (evar){0, E_STR, 8, 1, EVAL_S(v)}
#define EVAR_P(  v)         (evar){0, E_PTR, 8, 1, EVAL_P(v)}

#define EVAR_CS( v)         (evar){0, E_STR, 8, 1, EVAL_CS(v)}
#define EVAR_CP( v)         (evar){0, E_PTR, 8, 1, EVAL_CP(v)}

#define EVAR_RAW( p, l)     (evar){0, E_RAW , (l), 1, EVAL_CP(p)}
#define EVAR_USER(p, l)     (evar){0, E_USER, (l), 1, EVAL_CP(p)}

/** -----------------------------------------------------
 *
 *  evar basic APIs
 *
 */
evar   evar_gen  (etypev t, int cnt, int size);   // create automaticlly, call evar_free() after using it

uint   evar_cnt  (evar v);      // Returns the cnt  of evar if evar is a ARR
uint   evar_esize(evar v);      // Returns the size of element
u64    evar_space(evar v);      // Returns the alloc size of base slots,  cnt * esize
etypev evar_type (evar v);      // Returns the whole type field in evar
constr evar_typeS(evar v);      // Returns type of evar as String
bool   evar_isPtr(evar v);

#define evar_free(v)         __evarp_free(&(v))    //  Returns the cnt of element before free

/** -----------------------------------------------------
 *
 *  evar value set
 *
 * note:
 *
 *  1. this will have effect only when the var is created by evar_gen()
 *  2. the len or type must match the internal len and type
 *  3. for E_STR, E_RAW and E_USER, we make a copy
 *
 */

#define evar_iSet( v, idx, in, inlen)  __evarp_iSet( &(v), idx, in, inlen)
#define evar_iSetI(v, idx, val)        __evarp_iSetI(&(v), idx, val)
#define evar_iSetF(v, idx, val)        __evarp_iSetF(&(v), idx, val)
#define evar_iSetS(v, idx, str)        __evarp_iSetS(&(v), idx, str)
#define evar_iSetP(v, idx, ptr)        __evarp_iSetP(&(v), idx, ptr)
#define evar_iSetR(v, idx, var)        __evarp_iSetR(&(v), idx, var)
#define evar_iSetV(v, idx, var)        __evarp_iSetV(&(v), idx, var)

/** -----------------------------------------------------
 *
 *  evar value get
 *
 * @return:
 *   return EVAR_NAV or 0 if type not match or idx can not be found
 *
 */
i64  evar_valI(evar v);             // Returns the value i64  of item if type matchs E_I64 or E_F64
f64  evar_valF(evar v);             // Returns the value f64  of item if type matchs E_F64 or E_I64
cstr evar_valS(evar v);             // Returns the cstr  hold by item if type matchs E_STR
cptr evar_valP(evar v);             // Returns the value ptr  of item if type matchs E_PTR
cptr evar_valR(evar v);             // Returns the raw   hold by item if type matchs E_RAW

uint evar_lenS(evar v);             // Returns the len        of item if type matchs E_STR
uint evar_lenR(evar v);             // Returns the len        of item if type matchs E_RAW

evar evar_i    (evar v, uint idx);      // Returns the specific item as evar, if type >= E_USER, using evar.v.p to get val
eval evar_iVal (evar v, uint idx);      // Returns the specific item as eval, using the field in eval to get value
i64  evar_iValI(evar v, uint idx);      // Returns the value i64  of item if exist and type matchs E_I64 or E_F64
f64  evar_iValF(evar v, uint idx);      // Returns the value f64  of item if exist and type matchs E_F64 or E_I64
cstr evar_iValS(evar v, uint idx);      // Returns the cstr  hold by item if exist and type matchs E_STR, actually is a estr
cptr evar_iValP(evar v, uint idx);      // Returns the cptr       of item if exist and type matchs E_PTR
cptr evar_iValR(evar v, uint idx);      // Returns the rawp  hold by item if exist and type matchs E_RAW, actually is a estr

uint evar_iLenS(evar v, uint idx);      // Returns the len        of item if exist and type matchs E_STR
uint evar_iLenR(evar v, uint idx);      // Returns the len        of item if exist and type matchs E_RAW

#define evar_iPtr(v, idx)   __evarp_iPtr(&(v), idx)  // Returns the addr of specific item for whatever the type is

/** -----------------------------------------------------
 *
 *  evar tools
 *
 */
int  evar_cmp(evar a, evar b);


/** -----------------------------------------------------
 *
 *   adapter APIs for macro using, you can also using it directly
 *
 */

bool __evarp_iSet (evarp vp, uint idx, conptr  in, int inlen);
bool __evarp_iSetV(evarp vp, uint idx, evar    in);
bool __evarp_iSetI(evarp vp, uint idx, i64    val);
bool __evarp_iSetF(evarp vp, uint idx, f64    val);
bool __evarp_iSetS(evarp vp, uint idx, constr str);
bool __evarp_iSetP(evarp vp, uint idx, conptr ptr);
bool __evarp_iSetR(evarp vp, uint idx, evar   val);

cptr __evarp_iPtr (evarp vp, uint idx);
uint __evarp_free (evarp vp);



#ifdef __cplusplus
}
#endif

#pragma pack()

#endif
