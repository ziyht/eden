/// =====================================================================================
///
///       Filename:  evec.h
///
///    Description:  easy vector for c
///
///        Version:  0.1
///        Created:  04/01/2019 08:51:34 AM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EVEC_H__
#define __EVEC_H__

typedef struct evec_s * evec;

#include "eobj.h"
#include "evar.h"

#ifdef __cplusplus
extern "C" {
#endif

/** -----------------------------------------------------
 *
 *  evec basic
 *
 *
 */
evec   evec_new( etypev type);                          // not accept type >= E_USER
evec   evec_new2(etypev type, u16 esize, uint cap);     // when cap is 0, the internal cap is infinit(max to UINT_MAX), else the internal cap will limit to setted value

evec   evec_dup(evec v);                    // TODO:
evec   evec_slice(evec v);                  // TODO: get a slice from a evec
evec   evec_split(evec v);                  // TODO:
evec   evec_merge(evec v, evec v2);         // TODO:

bool   evec_reserve(evec v);

uint   evec_len  (evec v);      // Returns the cnt of elements
uint   evec_cnt  (evec v);      // Returns the cnt of elements
uint   evec_cap  (evec v);      // Returns the captition of vector
uint   evec_avail(evec v);      // Returns the available cnt of vec
uint   evec_esize(evec v);      // Returns the size of single element
etypev evec_type (evec v);      // Returns the type of element
constr evec_typeS(evec v);      // Returns the type of element

int    evec_clear  (evec v);
int    evec_clearEx(evec v, eobj_rls_ex_cb rls, eval prvt);

int    evec_free  (evec v);
int    evec_freeEx(evec v, eobj_rls_ex_cb rls, eval prvt);

/** -----------------------------------------------------
 *  -- evec add --
 *
 * @note:
 *    - push :  append ahead
 *    - appd :  append tail
 *    - add  :  if i  = 0  , push
 *              if i >= len, appd
 *              else       , insert at the specific pos
 *
 * @note:
 *
 *      we create new base for 'str' and 'raw' data, and
 *  manager the ptr internal
 *
 */
bool evec_pushV(evec v, evar   var);
bool evec_pushI(evec v, i64    val);    // only for E_I64
bool evec_pushF(evec v, f64    val);    // only for E_F64
bool evec_pushS(evec v, constr str);    // only for E_STR
bool evec_pushP(evec v, conptr ptr);    // only for E_PTR
bool evec_pushR(evec v, uint   len);    // only for E_RAW

bool evec_appdV(evec v, evar   var);
bool evec_appdI(evec v, i64    val);
bool evec_appdF(evec v, f64    val);
bool evec_appdS(evec v, constr str);
bool evec_appdP(evec v, conptr ptr);
bool evec_appdR(evec v, uint   len);

bool evec_addV(evec v, uint i, evar   var);
bool evec_addI(evec v, uint i, i64    val);
bool evec_addF(evec v, uint i, f64    val);
bool evec_addS(evec v, uint i, constr str);
bool evec_addP(evec v, uint i, conptr str);
bool evec_addR(evec v, uint i, uint   len);

/** -----------------------------------------------------
 *  -- evec val --
 *
 * @note:
 *
 *      we assume you known the detail of item in the vec,
 *  and you can using eval to get the correct value.
 *
 *      using .p to get the ptr of item if type is E_USER
 *
 *
 */
eval evec_first(evec v);                // Retruns the val of first    element, returns EVAL_0 if not exist
eval evec_last (evec v);                // Retruns the val of last     element, returns EVAL_0 if not exist

eval evec_i    (evec v, uint idx);      // Retruns the val of specific element, returns EVAL_0 if not exist
cptr evec_iPtr (evec v, uint idx);      // Retruns the ptr of specific element, returns 0 if not exist
i64  evec_iValI(evec v, uint idx);      // Retruns the val of specific element, returns 0 if not exist, E_I64, E_U64, E_F64 is ok
f64  evec_iValF(evec v, uint idx);      // Retruns the val of specific element, returns 0 if not exist, E_I64, E_U64, E_F64 is ok
cstr evec_iValS(evec v, uint idx);      // Retruns the ptr of specific element, returns 0 if not exist, only E_STR
cptr evec_iValP(evec v, uint idx);      // Retruns the ptr of specific element, returns 0 if not exist, only E_PTR
cptr evec_iValR(evec v, uint idx);      // Retruns the ptr of specific element, returns 0 if not exist, only E_RAW



/// -----------------------------------------------------
/// evec take
///
///

evar evec_takeH(evec v);                // Takes the first    element
evar evec_takeT(evec v);                // Takes the last     element
evar evec_takeI(evec v, uint idx);      // Takes the specific element

//! TODO
evar evec_takeOne(evec v, evar var);    // Takes the first matched element
evar evec_takeAll(evec v, evar var);    // Takes all the   matched element


#ifdef __cplusplus
}
#endif

#endif
