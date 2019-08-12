/// =====================================================================================
///
///       Filename:  edict.h
///
///    Description:  an easier dict, rebuild from redis dict
///
///        Version:  1.0
///        Created:  2017-05-03 10:00:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EDICT_H__
#define __EDICT_H__

#include "etype.h"
#include "eobj.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _editr_s* editr;

/** -----------------------------------------------------
 *
 *  edict basic
 *
 */

edict edict_new(etypek type);       // 0 or EKEY_S

bool  edict_setType(edict d, etypek        type);    // will set failed if skiplist is not empty
bool  edict_setPrvt(edict d, eval          priv);    // will never failed
bool  edict_setRls (edict d, eobj_rls_ex_cb rls);    // release func, only have effect on ERAW and EPTR type of obj,
                                                     //  you can set it at any time, but be careful when you want to free a node

int   edict_clear  (edict d);
int   edict_clearEx(edict d, eobj_rls_ex_cb rls);

int   edict_free   (edict d);
int   edict_freeEx (edict d, eobj_rls_ex_cb rls);


bool  edict_isEmpty(edict d);               // Returns true if the dict contains no items, else return false
uld   edict_size   (edict d);               // Returns the number of items in the dict.

void  edict_show(edict d, uint cnt);

/** -----------------------------------------------------
 *
 *  edict add
 *
 *      add a val or obj(todo) to a edict
 *
 */

eobj  edict_addI(edict d, ekey key, i64    val);      // Num
eobj  edict_addF(edict d, ekey key, f64    val);      // Num
eobj  edict_addP(edict d, ekey key, conptr ptr);      // Ptr
eobj  edict_addS(edict d, ekey key, constr str);      // Str
eobj  edict_addR(edict d, ekey key, uint   len);      // Raw

//! multi add
eobj  edict_addMI(edict d, ekey key, i64    val);     // Num
eobj  edict_addMF(edict d, ekey key, f64    val);     // Num
eobj  edict_addMP(edict d, ekey key, conptr ptr);     // Ptr
eobj  edict_addMS(edict d, ekey key, constr str);     // Str
eobj  edict_addMR(edict d, ekey key, uint   len);     // Raw

/** -----------------------------------------------------
 *
 * edict val
 *
 *      get obj or val from a edict
 */
eobj   edict_k      (edict d, ekey key);    // Returns the eobj with the specific key
i64    edict_kValI  (edict d, ekey key);    // Returns the value i64  of eobj if exist and type matchs ENUM, else return 0
f64    edict_kValF  (edict d, ekey key);    // Returns the value f64  of eobj if exist and type matchs ENUM, else return 0
cptr   edict_kValP  (edict d, ekey key);    // Returns the ptr        of eobj if exist and type matchs EPTR, else return 0
cstr   edict_kValS  (edict d, ekey key);    // Returns the cstr       of eobj if exist and type matchs ESTR, else return 0
cptr   edict_kValR  (edict d, ekey key);    // Returns the ptr of raw in eobj if exist and type matchs ERAW, else return 0
etypeo edict_kType  (edict d, ekey key);    // Returns eobj's type if exist, else return EOBJ_UNKNOWN
uint   edict_kLen   (edict d, ekey key);    // Returns eobj's len  if found and type matchs ESTR, ERAW, EOBJ, else return 0
bool   edict_kIsTrue(edict d, ekey key);    // Returns true if the val in eobj is likely true:
                                                //  1. the type of obj is ETRUE
                                                //  2. the val of i64 or f64 is not 0
                                                //  3. the ptr val is not 0
                                                //  4. the str val is not empty
                                                //  5. the len of raw is not 0

/** -----------------------------------------------------
 *
 *  edict take
 *
 *      take objs from a edict
 *
 */

eobj  edict_takeO  (edict d, eobj obj);       // Takes the specific obj from the dict, returns the took obj if exist, else return 0
eobj  edict_takeOne(edict d, ekey key);     // Takes the obj with the specific key from the dict, returns the took obj if exist, else return 0
//evec  edict_takeAll(edict d, ekey key);    // todo: Takes all objs with the specific key from the dict, returns a evec if exist, else return 0, the returned evec should be free'd after using


int   edict_freeO  (edict d, eobj obj);
int   edict_freeOne(edict d, ekey key);
int   edict_freeAll(edict d, ekey key);

int   edict_freeOEx  (edict d, eobj obj, eobj_rls_cb rls);
int   edict_freeOneEx(edict d, ekey key, eobj_rls_cb rls);
int   edict_freeAllEx(edict d, ekey key, eobj_rls_cb rls);

/** -----------------------------------------------------
 *
 *  edict iterationg
 *
 * @note:
 *     1. if it is a save iterator, you can using edict_rm*()
 * and edict_free*() when iterating, else, you can only
 * iterating it.
 *
 */

editr edict_getItr(edict d, int safe);                     // if safe == 1, the itr will create in safe mode, else not

int   edict_resetItr(editr itr, edict d, int safe);        // if d == 0, it will keep the inner edict; if safe != 0 && safe != 1,  the safe mode will not changed
void  edict_freeItr (editr itr);

eobj  edict_next(editr itr);

#define edict_foreach(ditr, oitr) for(eobj itr; (itr = edict_next(ditr)); )

constr edict_version();

#ifdef __cplusplus
}
#endif

#endif
