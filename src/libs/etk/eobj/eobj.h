/// =====================================================================================
///
///       Filename:  eobj.h
///
///    Description:  some shared obj operation for elist, erb..., for internal use.
///
///        Version:  1.0
///        Created:  06/09/2017 05:15:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EOBJ_H__
#define __EOBJ_H__

#include "ecompat.h"
#include "etype.h"

#ifdef __cplusplus
extern "C" {
#endif

//! eobj container type
typedef enum{
    EJSON   = 0x01u,    // (1) json
    ELL     ,           // (2) linklist
    EDICT   ,           // (3) dict
    ERB     ,           // (4) rbtree
    ESL     ,           // (5) skiplist
    EQL     ,           // (6) quicklist
    EZL     ,           // (7) ziplist
    EVEC    ,           // (8) vector

    ECON_UNKNOWN = 0
}etypec;

//! eobj val type
typedef enum{
    EFALSE = 0x00u ,    // 0
    ETRUE  ,            // 1
    ENULL  ,            // 2
    ENUM   ,            // 3
    ESTR   ,            // 4
    EPTR   ,            // 5
    ERAW   ,            // 6
    EOBJ   ,            // 7
    EARR   ,            // 8  // only for ejson

    //! only for add val using
    ENUM_I = ENUM,
    ENUM_F = ENUM | 1 << 3,

    EOBJ_UNKNOWN  = 0x0f
}etypeo;

typedef enum{

    //! -- key types --
    EKEY_NO     = 0,    // 000
    EKEY_I      = 1,    // 001
    EKEY_U      = 2,    // 010
    EKEY_F      = 3,    // 011
    EKEY_S      = 4,    // 100

    //! -- key sort types --
    EKEY_ASC    = 0,    // 0000 : default
    EKEY_DES    = 8,    // 1000

    //! -- mask --
    EKEY_MASK      = 0x7,  // 0111
    EKEY_SORT_MASK = 0x8,  // 1000
}etypek;

//! opitions
typedef enum {
    ALL_ON      =  0xFF,
    ALL_OFF     =  0x00,

    //! -- ejson opts --

    // parsing opts
    ENDCHECK    =  0x01,     // requre null-terminated check in parse end
    COMMENT     =  0x02,     // requre comment supported

    // format opts
    COMPACT     =  0x00,    // format a obj to a compat formated json string
    PRETTY      =  0x01,    // format a obj to a pretty formated json string

}eopts;

/// ---------------------- ecan -------------------------
///
///     cantainer type for obj using;
///

typedef union  eobj_s  * ejson;
typedef struct _ell_s  * ell;
typedef struct _edict_s* edict;
typedef struct _erb_s  * erb;
typedef struct _esl_s  * esl;


/// ---------------------- ekey -------------------------
///
///     key type for obj using;
///

typedef union __ekey_s{
    u64    u;
    i64    i;
    f64    f;
    cptr   p;
    cstr   s;
}ekey_t, ekey;

#define ekey_i(I) (ekey){.i = I}
#define ekey_s(S) (ekey){.s = S}


/// ---------------------- eobj -------------------------
///
///     a obj type for elist, erb, edict...;
///

typedef union eobj_s{
    i64     i;          // i64
    u64     u;          // u64
    f64     f;          // f64
    cptr    p;          // ptr
    cstr    s;          // str
    char    r[1];       // raw
    eval_t  v;          // val
}eobj_t, *eobj;

typedef void (*EOBJ_RLS_CB)(eobj);

typedef void (*eobj_rls_cb )(eobj o);                       /// release eobj's data, note: do not free @param o
typedef int  (*eobj_cmp_cb )(eobj a, eobj b);               /// compare two eobj, we assume returned -1 when @param a less than @param b

typedef void (*eobj_rls_ex_cb )(eobj o, eval prvt);         /// release eobj's data, note: do not free @param o
typedef int  (*eobj_cmp_ex_cb )(eobj a, eobj b, eval prvt); /// compare two eobj, we assume returned -1 when @param a less than @param b

eobj  eobj_setKeyI(eobj o, i64    key);
eobj  eobj_setKeyS(eobj o, constr key);

/// - if you kown the detail of eobj, you can using those macros to get the value directly
#define EOBJ_VALI(o) (o)->i
#define EOBJ_VALF(o) (o)->f
#define EOBJ_VALS(o) ((cstr)(o))
#define EOBJ_VALP(o) (o)->p
#define EOBJ_VALR(o) ((cptr)(o))

/// - else, you can using the following APIs
ekey   eobj_key  (eobj o);
i64    eobj_keyI (eobj o);
constr eobj_keyS (eobj o);
etypeo eobj_typeo(eobj o);
etypec eobj_typec(eobj o);
constr eobj_typeS(eobj o);          // Return obj type as str
uint   eobj_len  (eobj o);
i64    eobj_valI (eobj o);
f64    eobj_valF (eobj o);
constr eobj_valS (eobj o);
cptr   eobj_valP (eobj o);
cptr   eobj_valR (eobj o);
bool   eobj_isTrue(eobj o);

/** ---------------------------------------------
 * @brief eobj comparing
 *
 * @return
 *       1: obj >  val
 *       0: obj == val
 *      -1: obj <  val
 *      -2: obj is NULL
 *      -3: type not match
 *      -4: val of cstr is null
 */
int    eobj_cmpI(eobj o, i64    val);
int    eobj_cmpF(eobj o, f64    val);
int    eobj_cmpS(eobj o, constr str);

#ifdef __cplusplus
}
#endif

#endif
