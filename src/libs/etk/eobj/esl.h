// =====================================================================================
//
//       Filename:  esl.h
//
//    Description:  easy skiplist - rebuild from redis
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

#ifndef __ESL_H__
#define __ESL_H__

#include "etype.h"
#include "eobj.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESL_MAXLEVEL 16       // Should be enough for 2^16 elements
#define ESL_P        0.25     // Skiplist P = 1/4

/** -----------------------------------------------------
 *
 *  esl basic
 *
 */

esl  esl_new(etypek type);      // lg: EKEY_I, EKEY_I | EKEY_DES

bool esl_setType(esl l, etypek type);           // will set failed if skiplist is not empty
bool esl_setPrvt(esl l, eval   prvt);           // will never failed
bool esl_setCmp (esl l, eobj_cmp_ex_cb cmp);    // will set failed if skiplist is not empty
bool esl_setRls (esl l, eobj_rls_ex_cb rls);    // release func, only have effect on ERAW and EPTR type of obj,
                                                //  you can set it at any time, but be careful when you want to free a node
int  esl_clear  (esl l);
int  esl_clearEx(esl l, eobj_rls_ex_cb rls);    // passed in rls have higher priority

int  esl_free   (esl l);
int  esl_freeEx (esl l, eobj_rls_ex_cb rls);    // passed in rls have higher priority

uint esl_size   (esl l);
bool esl_isEmpty(esl l);

//void esl_show(esl l, uint len);

/** -----------------------------------------------------
 *
 *  esl add
 *
 */
eobj   esl_newO(etypeo type, uint len);    // create new eobj by type

eobj   esl_addI(esl l, ekey key, i64    val);               // ENUM i64
eobj   esl_addF(esl l, ekey key, f64    val);               // ENUM f64
eobj   esl_addP(esl l, ekey key, conptr ptr);               // EPTR
eobj   esl_addS(esl l, ekey key, constr str);               // ESTR
eobj   esl_addR(esl l, ekey key, uint   len);               // ERAW
eobj   esl_addO(esl l, ekey key, eobj   obj);               // EOBJ

//! multi add
eobj   esl_addMI(esl l, ekey key, i64    val);              // ENUM i64
eobj   esl_addMF(esl l, ekey key, f64    val);              // ENUM f64
eobj   esl_addMP(esl l, ekey key, conptr ptr);              // EPTR
eobj   esl_addMS(esl l, ekey key, constr str);              // ESTR
eobj   esl_addMR(esl l, ekey key, uint   len);              // ERAW
eobj   esl_addMO(esl l, ekey key, eobj   obj);              // EOBJ

/** -----------------------------------------------------
 *
 *  esl val
 *
 */
eobj   esl_k      (esl l, ekey key);   // Returns the first found eobj by key
i64    esl_kValI  (esl l, ekey key);   // Returns the value i64  of eobj if exist and type matchs ENUM, else return 0
f64    esl_kValF  (esl l, ekey key);   // Returns the value f64  of eobj if exist and type matchs ENUM, else return 0
cstr   esl_kValS  (esl l, ekey key);   // Returns the cstr       of eobj if exist and type matchs ESTR, else return 0
cptr   esl_kValR  (esl l, ekey key);   // Returns the ptr        of eobj if exist and type matchs ERAW, else return 0
cptr   esl_kValP  (esl l, ekey key);   // Returns the ptr of raw in eobj if exist and type matchs EPTR, else return 0
etypeo esl_kType  (esl l, ekey key);   // Returns eobj's type if exist, else return EOBJ_UNKNOWN
uint   esl_kLen   (esl l, ekey key);   // Returns eobj's len  if exist and type matchs ESTR, ERAW, EOBJ, else return 0
bool   esl_kIsTrue(esl l, ekey key);   // Returns true if the val in eobj is likely true:
                                                //  1. the type of obj is ETRUE
                                                //  2. the val of i64 or f64 is not 0
                                                //  3. the ptr val is not 0
                                                //  4. the str val is not empty
                                                //  5. the len of raw is not 0

/** -----------------------------------------------------
 *
 *  esl travese
 *
 */

eobj esl_first(esl  l);
eobj esl_last (esl  l);
eobj esl_next (eobj o);
eobj esl_prev (eobj o);

#define esl_foreach(  l, itr) for(eobj itr = esl_first(l); itr; itr = esl_next(itr) )
#define esl_foreach_s(l, itr) for(eobj _INNER_ = esl_first(l), itr; (itr = _INNER_, _INNER_ = esl_next(_INNER_), itr); )


/** -----------------------------------------------------
 *
 *  esl take and free
 *
 */

eobj esl_takeH(esl l);
eobj esl_takeT(esl l);
eobj esl_takeO(esl l, eobj o);

eobj esl_takeOne(esl l, ekey key);
eobj esl_takeAll(esl l, ekey key);      // todo

int  esl_freeH(esl l);
int  esl_freeT(esl l);
int  esl_freeO(esl l, eobj o);

int  esl_freeOne(esl l, ekey key);
int  esl_freeAll(esl l, ekey key);      // todo

/** -----------------------------------------------------
 *
 *  esl version
 *
 */
constr esl_version();

#ifdef __cplusplus
}
#endif

#endif
