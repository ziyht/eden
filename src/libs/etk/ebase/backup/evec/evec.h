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
evec   evec_new(etypev type, u16 esize);    // esize only have effect when type is E_USER

bool   evec_reserve(evec v);

uint   evec_len  (evec v);
uint   evec_cap  (evec v);
uint   evec_esize(evec v);
etypev evec_type (evec v);

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


/// -----------------------------------------------------
/// -- evec val --
///
///
///
cptr evec_at   (evec v, uint idx);      // Retruns the ptr of specific element, returns 0 if not exist, see evec_val()
cptr evec_first(evec v);                // Retruns the ptr of first    element, returns 0 if not exist
cptr evec_last (evec v);                // Retruns the ptr of last     element, returns 0 if not exist

cptr evec_val (evec v, uint idx);       // Retruns the ptr of specific element, returns 0 if not exist, see evec_at()
i64  evec_valI(evec v, uint idx);       // Retruns the val of specific element, returns 0 if not exist
f64  evec_valF(evec v, uint idx);       // Retruns the val of specific element, returns 0 if not exist
cstr evec_valS(evec v, uint idx);       // Retruns the ptr of specific element, returns 0 if not exist
cptr evec_valP(evec v, uint idx);       // Retruns the ptr of specific element, returns 0 if not exist
cptr evec_valR(evec v, uint idx);       // Retruns the ptr of specific element, returns 0 if not exist



/// -----------------------------------------------------
/// evec take
///
///

evar evec_takeH(evec v);                // Takes the first    element
evar evec_takeT(evec v);                // Takes the last     element
evar evec_takeI(evec v, uint idx);      // Takes the specific element



#ifdef __cplusplus
}
#endif

#endif
