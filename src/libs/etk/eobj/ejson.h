/// =====================================================================================
///
///       Filename:  ejson.h
///
///    Description:  a easier way to handle json, you can also using it as a simple dic
///
///             1. we using list and dict to handle items in list and obj
///             2. we recorded the item which you accessed last time in list, we'll search
///                from the recorded item in next time
///
///        Version:  1.1
///        Created:  12/18/2016 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EJSON_H__
#define __EJSON_H__

#include "etype.h"
#include "eobj.h"
#include "estr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** -----------------------------------------------------
 *
 *  ejson basic
 *
 */

eobj   ejson_new(etypeo type, eval v);                          // create a ejson obj

eobj   ejson_parseS  (constr json);                             // parse str to ejson obj
eobj   ejson_parseSEx(constr json, constr* err, eopts opts);    // parse str to ejson obj
eobj   ejson_parseF  (constr path);                             // parse file to ejson obj
eobj   ejson_parseFEx(constr path, constr* err, eopts opts);    // parse file to ejson obj

uint   ejson_checkS  (constr json);                             // check if json format is correct in str, returns the checked obj's cnt
uint   ejson_checkSEx(constr json, constr* err, eopts opts);    // check if json format is correct in str, returns the checked obj's cnt
uint   ejson_checkF  (constr path);                             // check if json format is correct in file, returns the checked obj's cnt
uint   ejson_checkFEx(constr path, constr* err, eopts opts);    // check if json format is correct in filr, returns the checked obj's cnt

int    ejson_clear  (eobj r);                                   // clear a ejson obj, only effect on EOBJ and EARR
int    ejson_clearEx(eobj r, eobj_rls_ex_cb rls, eval prvt);    // clear a ejson obj, only effect on EOBJ and EARR

int    ejson_free  (eobj o);                                    // release a ejson obj, including all children
int    ejson_freeEx(eobj o, eobj_rls_ex_cb rls, eval prvt);     // release a ejson obj, including all children

etypeo ejson_type   (eobj o);
uint   ejson_size   (eobj o);         // Returns the size of ESTR or ERAW or number of items in EOBJ or EARR, else return 0
bool   ejson_isEmpty(eobj o);         // Returns false if ejson contains items, otherwise return true

void   ejson_show(ejson r);

constr ejson_errp();
constr ejson_err ();

/** -----------------------------------------------------
 *
 *  ejson add
 *
 *  @note
 *      1. if root obj to add to is a ARR obj, the key from
 *  param of obj/src will never check, use the exist key in
 *  obj or src.
 *
 *      2. if root obj to add to is a OBJ obj, must have a
 *  key from param or obj/src, if key is set in param, the
 *  key in obj/src will always be replaced by it
 *
 *      3. see 'ejson val's note to get the meaning of 'k',
 *  'i', 'p' APIs.
 *
 */
eobj ejson_addJ(eobj r, constr key, constr json);                 // parse a json to obj and add it to root
eobj ejson_addT(eobj r, constr key, etypeo type);                 // add an obj to root, the obj can be defined as EFLASE, ETURE, ENULL, EARR, EOBJ
eobj ejson_addI(eobj r, constr key, i64    val );                 // add an NUM obj to root
eobj ejson_addF(eobj r, constr key, f64    val );                 // add an NUM obj to root
eobj ejson_addS(eobj r, constr key, constr str );                 // add a  STR obj to root
eobj ejson_addP(eobj r, constr key, conptr ptr );                 // add a  PTR obj to root
eobj ejson_addR(eobj r, constr key, uint   len );                 // add a  RAW obj to root, alloc a new space(len) for data
eobj ejson_addO(eobj r, constr key, eobj   o   );                 // add an exist obj to obj

eobj ejson_rAddJ(eobj r, constr rawk, constr key, constr json);   // add an json  obj to specific obj in root via rawk
eobj ejson_rAddT(eobj r, constr rawk, constr key, etypeo type);   // add an type  obj to specific obj in root via rawk, only support EFLASE, ETURE, ENULL, EARR, EOBJ
eobj ejson_rAddI(eobj r, constr rawk, constr key, i64    val );   // add an NUM   obj to specific obj in root via rawk
eobj ejson_rAddF(eobj r, constr rawk, constr key, f64    val );   // add an NUM   obj to specific obj in root via rawk
eobj ejson_rAddS(eobj r, constr rawk, constr key, constr str );   // add a  STR   obj to specific obj in root via rawk
eobj ejson_rAddP(eobj r, constr rawk, constr key, conptr ptr );   // add a  PTR   obj to specific obj in root via rawk
eobj ejson_rAddR(eobj r, constr rawk, constr key, uint   len );   // add a  RAW   obj to specific obj in root via rawk, alloc a new space(len) for data
eobj ejson_rAddO(eobj r, constr rawk, constr key, eobj   o   );   // add an exist obj to specific obj in root via rawk

eobj ejson_iAddJ(eobj r, u32    idx , constr key, constr json);   // add an json  obj to specific obj in root via idx
eobj ejson_iAddT(eobj r, u32    idx , constr key, etypeo type);   // add an type  obj to specific obj in root via idx, only support EFLASE, ETURE, ENULL, EARR, EOBJ
eobj ejson_iAddI(eobj r, u32    idx , constr key, i64    val );   // add an NUM   obj to specific obj in root via idx
eobj ejson_iAddF(eobj r, u32    idx , constr key, f64    val );   // add an NUM   obj to specific obj in root via idx
eobj ejson_iAddS(eobj r, u32    idx , constr key, constr str );   // add a  STR   obj to specific obj in root via idx
eobj ejson_iAddP(eobj r, u32    idx , constr key, conptr ptr );   // add a  PTR   obj to specific obj in root via idx
eobj ejson_iAddR(eobj r, u32    idx , constr key, uint   len );   // add a  RAW   obj to specific obj in root via idx, alloc a new space(len) for data
eobj ejson_iAddO(eobj r, u32    idx , constr key, eobj   o   );   // add an exist obj to specific obj in root via idx

eobj ejson_pAddJ(eobj r, constr path, constr key, constr json);   // add an json  obj to specific obj in root via path
eobj ejson_pAddT(eobj r, constr path, constr key, etypeo type);   // add an type  obj to specific obj in root via path, only support EFLASE, ETURE, ENULL, EARR, EOBJ
eobj ejson_pAddI(eobj r, constr path, constr key, i64    val );   // add an NUM   obj to specific obj in root via path
eobj ejson_pAddF(eobj r, constr path, constr key, f64    val );   // add an NUM   obj to specific obj in root via path
eobj ejson_pAddS(eobj r, constr path, constr key, constr str );   // add a  STR   obj to specific obj in root via path
eobj ejson_pAddP(eobj r, constr path, constr key, conptr ptr );   // add a  PTR   obj to specific obj in root via path
eobj ejson_pAddR(eobj r, constr path, constr key, uint   len );   // add a  RAW   obj to specific obj in root via path, alloc a new space(len) for data
eobj ejson_pAddO(eobj r, constr path, constr key, eobj   o   );   // add an exist obj to specific obj in root via path

/** -----------------------------------------------------
 *
 * ejson val
 *
 *      get obj or val from a ejson obj
 *
 * @note
 *
 *      1. we have three race of APIs:
 *
 *          RACE  MEAN        KEY_TYPE   SUPPORT
 *          ---------------------------------
 *          r     raw key     constr     EOBJ/EARR
 *          i     idx         uint       EARR
 *          p     path        constr     EOBJ/EARR
 *
 *
 *      for r race APIs, we consider rawk as a whole key and
 *  can not be splited
 *
 *      for p race APIs, we consider path as a continues key
 *  chan like "fruits[0].name", then we will found the target
 *  like this: 'fruits' -> '0' -> 'name'.
 *
 *     {
 *          "fruits[0].name" : "tomato",
 *                             --------
 *                                ^---------------------------- k found this
 *
 *          "fruits": [ {"name":"apple"}, {"name":"pear"} ]
 *                              -------
 *                                 ^--------------------------- p found this
 *     }
 *
 *     3. for p race APIs, you can split a key with '.' or '[]',
 *  they are simply the same:
 *
 *      fruits[0].name      : fruits -> 0 -> name
 *      fruits.0[name]      : fruits -> 0 -> name
 *      fruits.0.[name]     : fruits -> 0 -> "" -> name
 *
 *     4. for p race APIs, the key in '[]' can not be split again
 *
 *      fruits[0.name]      : fruits -> 0.name
 *
 * @return
 *   ejson_kisTrue:
 *      Returns true if the val in eobj is likely true:
 *        1. the type of obj is ETRUE
 *        2. the val of i64 or f64 is not 0
 *        3. the ptr val is not 0
 *        4. the str val is not empty
 *        5. the len of raw is not 0
 */
//! rawkey
eobj   ejson_r      (eobj r, constr rawk);   // Returns the eobj with the specific rawk
i64    ejson_rValI  (eobj r, constr rawk);   // Returns the value i64  of eobj if exist and type matchs ENUM, else return 0
f64    ejson_rValF  (eobj r, constr rawk);   // Returns the value f64  of eobj if exist and type matchs ENUM, else return 0
constr ejson_rValS  (eobj r, constr rawk);   // Returns the cstr       of eobj if exist and type matchs EPTR, else return 0
cptr   ejson_rValP  (eobj r, constr rawk);   // Returns the ptr        of eobj if exist and type matchs ESTR, else return 0
cptr   ejson_rValR  (eobj r, constr rawk);   // Returns the ptr of raw in eobj if exist and type matchs ERAW, else return 0
etypeo ejson_rType  (eobj r, constr rawk);   // Returns eobj's type if exist, else return EOBJ_UNKNOWN
constr ejson_rTypeS (eobj r, constr rawk);   // Returns eobj's type in string type
uint   ejson_rLen   (eobj r, constr rawk);   // Returns eobj's len  if found and type matchs ESTR, ERAW, EOBJ, EARR else return 0
bool   ejson_rIsTrue(eobj r, constr rawk);   // Returns true if the val in eobj is likely true
//! idx
eobj   ejson_i      (eobj r, uint   idx);    // Returns the eobj in the specific idx
i64    ejson_iValI  (eobj r, uint   idx);    // Returns the value i64  of eobj if exist and type matchs ENUM, else return 0
f64    ejson_iValF  (eobj r, uint   idx);    // Returns the value f64  of eobj if exist and type matchs ENUM, else return 0
constr ejson_iValS  (eobj r, uint   idx);    // Returns the cstr       of eobj if exist and type matchs EPTR, else return 0
cptr   ejson_iValP  (eobj r, uint   idx);    // Returns the ptr        of eobj if exist and type matchs ESTR, else return 0
cptr   ejson_iValR  (eobj r, uint   idx);    // Returns the ptr of raw in eobj if exist and type matchs ERAW, else return 0
etypeo ejson_iType  (eobj r, uint   idx);    // Returns eobj's type if exist, else return EOBJ_UNKNOWN
constr ejson_iTypeS (eobj r, uint   idx);    // Returns eobj's type in string type
uint   ejson_iLen   (eobj r, uint   idx);    // Returns eobj's len  if found and type matchs ESTR, ERAW, EOBJ, EARR else return 0
bool   ejson_iIsTrue(eobj r, uint   idx);    // Returns true if the val in eobj is likely true
//! path
eobj   ejson_p      (eobj r, constr path);   // Returns the eobj with the specific path
i64    ejson_pValI  (eobj r, constr path);   // Returns the value i64  of eobj if exist and type matchs ENUM, else return 0
f64    ejson_pValF  (eobj r, constr path);   // Returns the value f64  of eobj if exist and type matchs ENUM, else return 0
constr ejson_pValS  (eobj r, constr path);   // Returns the cstr       of eobj if exist and type matchs EPTR, else return 0
cptr   ejson_pValP  (eobj r, constr path);   // Returns the ptr        of eobj if exist and type matchs ESTR, else return 0
cptr   ejson_pValR  (eobj r, constr path);   // Returns the ptr of raw in eobj if exist and type matchs ERAW, else return 0
etypeo ejson_pType  (eobj r, constr path);   // Returns eobj's type if exist, else return EOBJ_UNKNOWN
constr ejson_pTypeS (eobj r, constr path);   // Returns eobj's type in string type
uint   ejson_pLen   (eobj r, constr path);   // Returns eobj's len  if exist and type matchs ESTR, ERAW, EOBJ, EARR else return 0
bool   ejson_pIsTrue(eobj r, constr path);   // Returns true if the val in eobj is likely true

/** -----------------------------------------------------
 *
 *  ejson format
 *
 *  @param
 *    opts - can be EO_COMPACT or EO_PRETTY
 *
 *  @note
 *      if passed in 'out' is 0, create and returned a new
 *  buf, else write to it;
 *
 */
estr ejson_toS (eobj o,              estr* out, eopts opts);

estr ejson_kToS(eobj o, constr rawk, estr* out, eopts opts);
estr ejson_iToS(eobj o, u32    idx , estr* out, eopts opts);
estr ejson_pToS(eobj o, constr path, estr* out, eopts opts);

/** -----------------------------------------------------
 *
 *  ejson take and free
 *
 */
eobj ejson_takeH(eobj r);               // for EOBJ, EARR
eobj ejson_takeT(eobj r);               // for EOBJ, EARR
eobj ejson_takeO(eobj r, eobj      o);  // for EOBJ, EARR
eobj ejson_takeR(eobj r, constr rawk);  // for EOBJ, EARR
eobj ejson_takeI(eobj r, int     idx);  // for EARR
eobj ejson_takeP(eobj r, constr path);  // for EOBJ, EARR

int  ejson_freeH(eobj r);               // for EOBJ, EARR
int  ejson_freeT(eobj r);               // for EOBJ, EARR
int  ejson_freeO(eobj r, eobj    obj);  // for EOBJ, EARR
int  ejson_freeR(eobj r, constr rawk);  // for EOBJ, EARR
int  ejson_freeI(eobj r, int     idx);  // for EARR
int  ejson_freeP(eobj r, constr path);  // for EOBJ, EARR

int  ejson_freeHEx(eobj r,              eobj_rls_ex_cb rls, eval prvt);
int  ejson_freeTEx(eobj r,              eobj_rls_ex_cb rls, eval prvt);
int  ejson_freeOEx(eobj r, eobj    obj, eobj_rls_ex_cb rls, eval prvt);
int  ejson_freeREx(eobj r, constr rawk, eobj_rls_ex_cb rls, eval prvt);
int  ejson_freeIEx(eobj r, int     idx, eobj_rls_ex_cb rls, eval prvt);
int  ejson_freePEx(eobj r, constr pkey, eobj_rls_ex_cb rls, eval prvt);

/** -----------------------------------------------------
 *
 *  ejson comparing
 *
 * @return
 *       1: obj >  val
 *       0: obj == val
 *      -1: obj <  val
 *      -2: obj is NULL
 *      -3: type not match
 *      -4: val of cstr is null
 */

#define ejson_cmpI eobj_cmpI
#define ejson_cmpF eobj_cmpF
#define ejson_cmpS eobj_cmpS

int  ejson_rCmpI(eobj r, constr rawk, i64    val);
int  ejson_rCmpF(eobj r, constr rawk, f64    val);
int  ejson_rCmpS(eobj r, constr rawk, constr str);

int  ejson_iCmpI(eobj r, u32    idx , i64    val);
int  ejson_iCmpF(eobj r, u32    idx , f64    val);
int  ejson_iCmpS(eobj r, u32    idx , constr str);

int  ejson_pCmpI(eobj r, constr path, i64    val);
int  ejson_pCmpF(eobj r, constr path, f64    val);
int  ejson_pCmpS(eobj r, constr path, constr str);

/** -----------------------------------------------------
 *
 *  ejson iterationg
 *
 *  @note
 *      for perfomance, we do not check type of o in
 *  ejson_next() and ejson_prev()
 *
 */
eobj  ejson_first(eobj r);
eobj  ejson_last (eobj r);
eobj  ejson_next (eobj o);
eobj  ejson_prev (eobj o);

eobj  ejson_rFirst(eobj r, constr rawk);
eobj  ejson_rLast (eobj r, constr rawk);

eobj  ejson_pFirst(eobj r, constr path);
eobj  ejson_pLast (eobj r, constr path);

#define ejson_foreach( r,       itr)   for(itr = ejson_first (r      ); (itr); itr = ejson_next(itr))
#define ejson_kForeach(r, rawk, itr)   for(itr = ejson_rFirst(r, rawk); (itr); itr = ejson_next(itr))
#define ejson_pForeach(r, path, itr)   for(itr = ejson_pFirst(r, path); (itr); itr = ejson_next(itr))

#define ejson_foreach_s( r,       itr) for(eobj _INNER_ = ejson_first (r      ), itr; (itr = _INNER_, _INNER_ = ejson_next(_INNER_), itr); )
#define ejson_kForeach_s(r, rawk, itr) for(eobj _INNER_ = ejson_rFirst(r, rawk), itr; (itr = _INNER_, _INNER_ = ejson_next(_INNER_), itr); )
#define ejson_pForeach_s(r, path, itr) for(eobj _INNER_ = ejson_pFirst(r, path), itr; (itr = _INNER_, _INNER_ = ejson_next(_INNER_), itr); )

/** -----------------------------------------------------
 *
 *  ejson set
 *
 *  @note
 *      1. if target not exsit, create automatically if the last
 *         found obj is a EOBJ obj
 *      2. the found obj will be reset always, any children of it
 *         will be delete automatically, be careful by using it,
 *         it may cause memleak when have EPTR or ERAW obj associated
 *         with the delete obj
 *      3. we do not create any not exsit obj for EARR obj
 *
 */
eobj ejson_rSetT(eobj r, constr rawk, etypeo   t);
eobj ejson_rSetI(eobj r, constr rawk, i64    val);
eobj ejson_rSetF(eobj r, constr rawk, f64    val);
eobj ejson_rSetS(eobj r, constr rawk, constr str);
eobj ejson_rSetP(eobj r, constr rawk, constr ptr);
eobj ejson_rSetR(eobj r, constr rawk, uint   len);

eobj ejson_iSetT(eobj r, u32    idx , etypeo   t);
eobj ejson_iSetI(eobj r, u32    idx , i64    val);
eobj ejson_iSetF(eobj r, u32    idx , f64    val);
eobj ejson_iSetS(eobj r, u32    idx , constr str);
eobj ejson_iSetP(eobj r, u32    idx , constr ptr);
eobj ejson_iSetR(eobj r, u32    idx , uint   len);

eobj ejson_pSetT(eobj r, constr path, etypeo   t);
eobj ejson_pSetI(eobj r, constr path, i64    val);
eobj ejson_pSetF(eobj r, constr path, f64    val);
eobj ejson_pSetS(eobj r, constr path, constr str);
eobj ejson_pSetP(eobj r, constr path, constr ptr);
eobj ejson_pSetR(eobj r, constr path, uint   len);

/// -----------------------------------------------------
//! @brief ejson substitute string
///
/// @return the modified obj if setted
///
eobj ejson_rReplaceS(eobj root, constr rawk, constr from, constr to);
eobj ejson_pReplaceS(eobj root, constr path, constr from, constr to);

/// -----------------------------------------------------
/// @brief ejson counter
///
/// @note:
///     1. if NUM obj not exsit in EOBJ, will create automatically
///     2. only support NUM obj if target obj exist
///     3. return LLONG_MIN if failed
///
i64  ejson_pp  (eobj o);                // increase 1
i64  ejson_mm  (eobj o);                // decrease 1
i64  ejson_incr(eobj o, i64 v);         // increase v
i64  ejson_decr(eobj o, i64 v);         // decrease v

i64  ejson_rPP  (eobj r, constr rawk);
i64  ejson_rMM  (eobj r, constr rawk);
i64  ejson_rIncr(eobj r, constr rawk, i64 v);
i64  ejson_rDecr(eobj r, constr rawk, i64 v);

i64  ejson_iPP  (eobj r, u32    idx );
i64  ejson_iMM  (eobj r, u32    idx );
i64  ejson_iIncr(eobj r, u32    idx , i64 v);
i64  ejson_iDecr(eobj r, u32    idx , i64 v);

i64  ejson_pPP  (eobj r, constr path);
i64  ejson_pMM  (eobj r, constr path);
i64  ejson_pIncr(eobj r, constr path, i64 v);
i64  ejson_pDecr(eobj r, constr path, i64 v);

/** -----------------------------------------------
 *  @brief
 *      ejson sort operation
 *
 *  @note:
 *      it only effect on EOBJ and EARR obj of ejson
 *
 */

//! supplied default sort cb
int    __KEYS_ACS(eobj a, eobj b);      // Ascending  via key string in all obj, dictionary sequence
int    __KEYS_DES(eobj a, eobj b);      // Descending via key string in all obj, dictionary sequence
int    __VALI_ACS(eobj a, eobj b);      // Ascending  via int value in NUM obj
int    __VALI_DES(eobj a, eobj b);      // Descending via int value in NUM obj

eobj  ejson_sort (eobj r,              eobj_cmp_cb cmp);
eobj  ejson_kSort(eobj r, constr rawk, eobj_cmp_cb cmp);
eobj  ejson_pSort(eobj r, constr path, eobj_cmp_cb cmp);

/// -- ejson version
constr ejson_version();

#ifdef __cplusplus
}
#endif

#endif
