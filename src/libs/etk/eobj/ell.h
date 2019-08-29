/// =====================================================================================
///
///       Filename:  ell.h
///
///    Description:  double link list of etools
///
///        Version:  1.3
///        Created:  05/04/2017 04:50:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __ELL_H__
#define __ELL_H__

#include "etype.h"
#include "eobj.h"
#include "ealloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** -----------------------------------------------------
 *
 *  ell basic
 *
 */

ell  ell_new();

eobj ell_newO(etypeo t, uint len);    // Returns a new eobj used by elist, len will have effect only when @param type is ESTR, ERAW

ell  ell_clear  (ell l);
ell  ell_clearEx(ell l, eobj_rls_cb rls);

int  ell_free  (ell l);
int  ell_freeEx(ell l, eobj_rls_cb rls);

uint ell_size   (ell l);             // Returns the number of items in the list.
bool ell_isEmpty(ell l);             // Returns 1 if the list contains no items; otherwise returns 0

void ell_show(ell l, uint max);                    // todo
void ell_showSpan(ell l, uint begin, uint end);    // todo

/** -----------------------------------------------------
 *
 *  ell add
 *
 *      add a val or obj to ell
 *
 * @fn
 *
 *   push: create or add a obj in front of the list
 *   appd: create or add a obj in last  of the list
 *
 *  @todo:
 *   add : insert an obj at the idx of list
 *
 * @return
 *
 *   returns the new obj or added obj if succeed, otherwise return 0
 *
 */

eobj  ell_pushI(ell l, i64    val);         // ENUM i64
eobj  ell_pushF(ell l, f64    val);         // ENUM f64
eobj  ell_pushS(ell l, constr str);         // ESTR
eobj  ell_pushP(ell l, conptr ptr);         // EPTR
eobj  ell_pushR(ell l, uint   len);         // ERAW
eobj  ell_pushO(ell l, eobj   obj);         // EOBJ

eobj  ell_appdI(ell l, i64    val);         // ENUM i64
eobj  ell_appdF(ell l, f64    val);         // ENUM f64
eobj  ell_appdS(ell l, constr str);         // ESTR
eobj  ell_appdP(ell l, conptr ptr);         // EPTR
eobj  ell_appdR(ell l, uint   len);         // ERAW
eobj  ell_appdO(ell l, eobj   obj);         // EOBJ

eobj  ell_addI(ell l, int idx, i64    val);        // todo
eobj  ell_addF(ell l, int idx, f64    val);        // todo
eobj  ell_addS(ell l, int idx, constr str);        // todo
eobj  ell_addP(ell l, int idx, conptr ptr);        // todo
eobj  ell_addR(ell l, int idx, uint   len);        // todo
eobj  ell_addO(ell l, int idx, eobj   obj);        // todo

/** -----------------------------------------------------
 *
 *  ell set
 *
 *      reset the value of specific obj in ell
 *
 * @note:
 *
 *  1. if idx > 0, we search from the beginning
 *  2. if idx < 0, we search from the end
 *
 *     [ *  *  *  *  *  *  *  *  *  *]
 *       0  1  2  3  4  5  6  7  8  9
 *     -10 -9 -8 -7 -6 -5 -4 -3 -2 -1    idx = idx + ${len}
 *
 *  3. if idx can not be found, the operation will failed
 *     else the operaion will success always
 *  4. it may do realloc opearation, be careful
 *
 * @return
 *
 *  returns the setted obj if succeed, otherwise return 0
 *
 */
eobj  ell_isetI(ell l, int idx, i64    val);         // ENUM i64
eobj  ell_isetF(ell l, int idx, f64    val);         // ENUM f64
eobj  ell_isetP(ell l, int idx, conptr ptr);         // ESTR
eobj  ell_isetS(ell l, int idx, constr str);         // EPTR
eobj  ell_isetR(ell l, int idx, uint   len);         // ERAW
eobj  ell_isetO(ell l, int idx, eobj   obj);         // todo

/** -----------------------------------------------------
 *
 *  ell val
 *
 *      get obj or val from ell
 *
 */
eobj   ell_i      (ell l, uint idx);     // Returns the ptr        of eobj if exist
i64    ell_ivalI  (ell l, uint idx);     // Returns the value i64  of eobj if exist and type matchs ENUM, else return 0
f64    ell_ivalF  (ell l, uint idx);     // Returns the value f64  of eobj if exist and type matchs ENUM, else return 0
cptr   ell_ivalP  (ell l, uint idx);     // Returns the ptr        of eobj if exist and type matchs EPTR, else return 0
cstr   ell_ivalS  (ell l, uint idx);     // Returns the cstr       of eobj if exist and type matchs ESTR, else return 0
cptr   ell_ivalR  (ell l, uint idx);     // Returns the ptr of raw in eobj if exist and type matchs ERAW, else return 0
etypeo ell_itype  (ell l, uint idx);     // Returns eobj's type if exist, else return EOBJ_UNKNOWN
uint   ell_ilen   (ell l, uint idx);     // Returns eobj's len  if exist and type matchs ESTR, ERAW, EOBJ, else return 0
bool   ell_iisTrue(ell l, uint idx);     // Returns true if the val in eobj is likely true:
                                                //  1. the type of obj is ETRUE
                                                //  2. the val of i64 or f64 is not 0
                                                //  3. the ptr val is not 0
                                                //  4. the str val is not empty
                                                //  5. the len of raw is not 0

/** -----------------------------------------------------
 *
 *  ell iterating
 *
 *  @note
 *      for perfomance, we do not check type of o in
 *  ell_next() and ell_next()
 *
 */
eobj  ell_first(ell  l);                // Returns the first item of list if have; otherwise returns 0.
eobj  ell_last (ell  l);                // Returns the last  item of list if have; otherwise returns 0.
eobj  ell_next (eobj obj);              // Returns the next  item of eobj if have; otherwise returns 0. For performance, we do not check the obj type, be careful.
eobj  ell_prev (eobj obj);              // Returns the prev  item of eobj if have; otherwise returns 0. For performance, we do not check the obj type, be careful.

#define ell_foreach(l, itr)     for(eobj itr = ell_first(l); (itr); itr = ell_next(itr))
#define ell_foreach_s(l, itr)   for(eobj itr, __INNER__ = ell_first(l); ((itr) = __INNER__, __INNER__ = ell_next(__INNER__), (itr));)

int   ell_swap(ell l, uint idx1, uint idx2);    // todo

/** -----------------------------------------------------
 *
 *  ell sort
 *
 *     The  comparison function must return an integer less than, equal to,
 * or greater than zero if the first argument is considered to be respectively
 * less than, equal to, or greater than the second.  If two members compare
 * as equal, their order in the sorted array is undefined.
 *
 */

//! supplied default sort mode
#define ELL_VALI_ACS    EOBJ_VALI_ACS
#define ELL_VALI_DES    EOBJ_VALI_DES

void  ell_sort  (ell l, eobj_cmp_cb    cmp);
void  ell_sort_r(ell l, eobj_cmp_ex_cb cmp, eval prvt);

/** -----------------------------------------------------
 *
 *  ell take
 *
 *      take one or multi objs from ell
 *
 */
eobj  ell_takeH(ell l);                 // Takes the head obj
eobj  ell_takeT(ell l);                 // Takes the tail obj
eobj  ell_takeO(ell l, eobj obj);       // Takes the obj which passed in, it will success only when the obj is in this list
eobj  ell_takeI(ell l, uint idx);       // Takes the obj at the idx if exist

ell   ell_takeAll(ell l, eobj val);     // todo
eobj  ell_takeOne(ell l, eobj val);     // todo

ell   ell_takeAllEx(ell l, eobj val, eobj_cmp_cb cmp);  // todo
ell   ell_takeOneEx(ell l, eobj val, eobj_cmp_cb cmp);  // todo

/** -----------------------------------------------------
 *
 *  ell free
 *
 *      release one or multi objs in ell
 *
 */
int   ell_freeH(ell l);                 // Release the head obj
int   ell_freeT(ell l);                 // Release the tail obj
int   ell_freeO(ell l, eobj obj);       // Release the obj which passed in, it will success only when the obj is in this list or the pass in list is null and the obj is independent and a ell obj
int   ell_freeI(ell l, uint idx);       // Release the obj in idx

int   ell_freeAll(ell l, eobj val);     // todo
int   ell_freeOne(ell l, eobj val);     // todo

int   ell_freeAllEx(ell l, eobj val, eobj_cmp_cb cmp, eobj_rls_cb rls); // todo
int   ell_freeOneEx(ell l, eobj val, eobj_cmp_cb cmp, eobj_rls_cb rls); // todo

constr ell_version();

#ifdef __cplusplus
}
#endif

#endif
