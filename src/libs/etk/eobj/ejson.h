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

ejson  ejson_new(etypeo type, eval v);                          // create a ejson obj

ejson  ejson_parseS  (constr json);                             // parse str to ejson obj
ejson  ejson_parseSEx(constr json, constr* err, eopts opts);    // parse str to ejson obj
ejson  ejson_parseF  (constr path);                             // parse file to ejson obj
ejson  ejson_parseFEx(constr path, constr* err, eopts opts);    // parse file to ejson obj

uint   ejson_checkS  (constr json);                             // check if json format is correct in str, returns the checked obj's cnt
uint   ejson_checkSEx(constr json, constr* err, eopts opts);    // check if json format is correct in str, returns the checked obj's cnt
uint   ejson_checkF  (constr path);                             // check if json format is correct in file, returns the checked obj's cnt
uint   ejson_checkFEx(constr path, constr* err, eopts opts);    // check if json format is correct in filr, returns the checked obj's cnt

int    ejson_clear  (ejson r);                                   // clear a ejson obj, only effect on EOBJ and EARR
int    ejson_clearEx(ejson r, eobj_rls_ex_cb rls, eval prvt);    // clear a ejson obj, only effect on EOBJ and EARR

int    ejson_free  (ejson o);                                    // release a ejson obj, including all children
int    ejson_freeEx(ejson o, eobj_rls_ex_cb rls, eval prvt);     // release a ejson obj, including all children

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
ejson ejson_addJ(ejson r, constr key, constr json);                 // parse a json to obj and add it to root
ejson ejson_addT(ejson r, constr key, etypeo type);                 // add an obj to root, the obj can be defined as EFLASE, ETURE, ENULL, EARR, EOBJ
ejson ejson_addI(ejson r, constr key, i64    val );                 // add an NUM obj to root
ejson ejson_addF(ejson r, constr key, f64    val );                 // add an NUM obj to root
ejson ejson_addS(ejson r, constr key, constr str );                 // add a  STR obj to root
ejson ejson_addP(ejson r, constr key, conptr ptr );                 // add a  PTR obj to root
ejson ejson_addR(ejson r, constr key, uint   len );                 // add a  RAW obj to root, alloc a new space(len) for data
ejson ejson_addO(ejson r, constr key, ejson  o   );                 // add an exist obj to obj

ejson ejson_rAddJ(ejson r, constr rawk, constr key, constr json);   // add an json  obj to specific obj in root via rawk
ejson ejson_rAddT(ejson r, constr rawk, constr key, etypeo type);   // add an type  obj to specific obj in root via rawk, only support EFLASE, ETURE, ENULL, EARR, EOBJ
ejson ejson_rAddI(ejson r, constr rawk, constr key, i64    val );   // add an NUM   obj to specific obj in root via rawk
ejson ejson_rAddF(ejson r, constr rawk, constr key, f64    val );   // add an NUM   obj to specific obj in root via rawk
ejson ejson_rAddS(ejson r, constr rawk, constr key, constr str );   // add a  STR   obj to specific obj in root via rawk
ejson ejson_rAddP(ejson r, constr rawk, constr key, conptr ptr );   // add a  PTR   obj to specific obj in root via rawk
ejson ejson_rAddR(ejson r, constr rawk, constr key, uint   len );   // add a  RAW   obj to specific obj in root via rawk, alloc a new space(len) for data
ejson ejson_rAddO(ejson r, constr rawk, constr key, ejson  o   );   // add an exist obj to specific obj in root via rawk

ejson ejson_iAddJ(ejson r, u32    idx , constr key, constr json);   // add an json  obj to specific obj in root via idx
ejson ejson_iAddT(ejson r, u32    idx , constr key, etypeo type);   // add an type  obj to specific obj in root via idx, only support EFLASE, ETURE, ENULL, EARR, EOBJ
ejson ejson_iAddI(ejson r, u32    idx , constr key, i64    val );   // add an NUM   obj to specific obj in root via idx
ejson ejson_iAddF(ejson r, u32    idx , constr key, f64    val );   // add an NUM   obj to specific obj in root via idx
ejson ejson_iAddS(ejson r, u32    idx , constr key, constr str );   // add a  STR   obj to specific obj in root via idx
ejson ejson_iAddP(ejson r, u32    idx , constr key, conptr ptr );   // add a  PTR   obj to specific obj in root via idx
ejson ejson_iAddR(ejson r, u32    idx , constr key, uint   len );   // add a  RAW   obj to specific obj in root via idx, alloc a new space(len) for data
ejson ejson_iAddO(ejson r, u32    idx , constr key, ejson  o   );   // add an exist obj to specific obj in root via idx

ejson ejson_pAddJ(ejson r, constr path, constr key, constr json);   // add an json  obj to specific obj in root via path
ejson ejson_pAddT(ejson r, constr path, constr key, etypeo type);   // add an type  obj to specific obj in root via path, only support EFLASE, ETURE, ENULL, EARR, EOBJ
ejson ejson_pAddI(ejson r, constr path, constr key, i64    val );   // add an NUM   obj to specific obj in root via path
ejson ejson_pAddF(ejson r, constr path, constr key, f64    val );   // add an NUM   obj to specific obj in root via path
ejson ejson_pAddS(ejson r, constr path, constr key, constr str );   // add a  STR   obj to specific obj in root via path
ejson ejson_pAddP(ejson r, constr path, constr key, conptr ptr );   // add a  PTR   obj to specific obj in root via path
ejson ejson_pAddR(ejson r, constr path, constr key, uint   len );   // add a  RAW   obj to specific obj in root via path, alloc a new space(len) for data
ejson ejson_pAddO(ejson r, constr path, constr key, ejson  o   );   // add an exist obj to specific obj in root via path

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
 *   ejson_isTrue:
 *      Returns true if the val in ejson is likely true:
 *        1. the type of obj is ETRUE
 *        2. the val of i64 or f64 is not 0
 *        3. the ptr val is not 0
 *        4. the str val is not empty
 *        5. the len of raw is not 0
 */

#define ejson_key(o)    eobj_key   ((eobj)(o))      // Returns the key  of a ejson obj as ekey
#define ejson_keyI(o)   eobj_keyI  ((eobj)(o))      // Returns the key  of a ejson obj as i64   , return 0 if key not exist
#define ejson_keyS(o)   eobj_keyS  ((eobj)(o))      // Returns the key  of a ejson obj as constr, return 0 if key not exist
#define ejson_valI(o)   eobj_valI  ((eobj)(o))      // Returns the val  of a ejson obj as i64
#define ejson_valF(o)   eobj_valF  ((eobj)(o))      // Returns the val  of a ejson obj as f64
#define ejson_valS(o)   eobj_valS  ((eobj)(o))      // Returns the val  of a ejson obj as constr
#define ejson_valP(o)   eobj_valP  ((eobj)(o))      // Returns the val  of a ejson obj as cptr
#define ejson_valR(o)   eobj_valR  ((eobj)(o))      // Returns the val  of a ejson obj as cptr
#define ejson_type(o)   eobj_typeo ((eobj)(o))      // Returns the type of a ejson obj as etypeo
#define ejson_typeS(o)  eobj_typeS ((eobj)(o))      // Returns the type of a ejson obj as constr
#define ejson_len(o)    eobj_len   ((eobj)(o))      // Returns the len  of a ejson obj
#define ejson_isTrue(o) eobj_isTrue((eobj)(o))      // Returns true if the val in ejson is likely true

bool   ejson_isEmpty(ejson r);

//! rawkey
ejson  ejson_r      (ejson r, constr rawk);  // Returns the ejson with the specific rawk
i64    ejson_rValI  (ejson r, constr rawk);  // Returns the value i64  of ejson if exist and type matchs ENUM, else return 0
f64    ejson_rValF  (ejson r, constr rawk);  // Returns the value f64  of ejson if exist and type matchs ENUM, else return 0
constr ejson_rValS  (ejson r, constr rawk);  // Returns the cstr       of ejson if exist and type matchs EPTR, else return 0
cptr   ejson_rValP  (ejson r, constr rawk);  // Returns the ptr        of ejson if exist and type matchs ESTR, else return 0
cptr   ejson_rValR  (ejson r, constr rawk);  // Returns the ptr of raw in ejson if exist and type matchs ERAW, else return 0
etypeo ejson_rType  (ejson r, constr rawk);  // Returns ejson's type if exist, else return EOBJ_UNKNOWN
constr ejson_rTypeS (ejson r, constr rawk);  // Returns ejson's type in string type
uint   ejson_rLen   (ejson r, constr rawk);  // Returns ejson's len  if found and type matchs ESTR, ERAW, EOBJ, EARR else return 0
bool   ejson_rIsTrue(ejson r, constr rawk);  // Returns true if the val in ejson is likely true
//! idx
ejson  ejson_i      (ejson r, uint   idx);   // Returns the ejson in the specific idx
i64    ejson_iValI  (ejson r, uint   idx);   // Returns the value i64  of ejson if exist and type matchs ENUM, else return 0
f64    ejson_iValF  (ejson r, uint   idx);   // Returns the value f64  of ejson if exist and type matchs ENUM, else return 0
constr ejson_iValS  (ejson r, uint   idx);   // Returns the cstr       of ejson if exist and type matchs EPTR, else return 0
cptr   ejson_iValP  (ejson r, uint   idx);   // Returns the ptr        of ejson if exist and type matchs ESTR, else return 0
cptr   ejson_iValR  (ejson r, uint   idx);   // Returns the ptr of raw in ejson if exist and type matchs ERAW, else return 0
etypeo ejson_iType  (ejson r, uint   idx);   // Returns ejson's type if exist, else return EOBJ_UNKNOWN
constr ejson_iTypeS (ejson r, uint   idx);   // Returns ejson's type in string type
uint   ejson_iLen   (ejson r, uint   idx);   // Returns ejson's len  if found and type matchs ESTR, ERAW, EOBJ, EARR else return 0
bool   ejson_iIsTrue(ejson r, uint   idx);   // Returns true if the val in ejson is likely true
//! path
ejson  ejson_p      (ejson r, constr path);  // Returns the ejson with the specific path
i64    ejson_pValI  (ejson r, constr path);  // Returns the value i64  of ejson if exist and type matchs ENUM, else return 0
f64    ejson_pValF  (ejson r, constr path);  // Returns the value f64  of ejson if exist and type matchs ENUM, else return 0
constr ejson_pValS  (ejson r, constr path);  // Returns the cstr       of ejson if exist and type matchs EPTR, else return 0
cptr   ejson_pValP  (ejson r, constr path);  // Returns the ptr        of ejson if exist and type matchs ESTR, else return 0
cptr   ejson_pValR  (ejson r, constr path);  // Returns the ptr of raw in ejson if exist and type matchs ERAW, else return 0
etypeo ejson_pType  (ejson r, constr path);  // Returns ejson's type if exist, else return EOBJ_UNKNOWN
constr ejson_pTypeS (ejson r, constr path);  // Returns ejson's type in string type
uint   ejson_pLen   (ejson r, constr path);  // Returns ejson's len  if exist and type matchs ESTR, ERAW, EOBJ, EARR else return 0
bool   ejson_pIsTrue(ejson r, constr path);  // Returns true if the val in ejson is likely true

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
estr ejson_toS (ejson o,              estr* out, eopts opts);

estr ejson_kToS(ejson o, constr rawk, estr* out, eopts opts);
estr ejson_iToS(ejson o, u32    idx , estr* out, eopts opts);
estr ejson_pToS(ejson o, constr path, estr* out, eopts opts);

/** -----------------------------------------------------
 *
 *  ejson take and free
 *
 */
ejson ejson_takeH(ejson r);               // for EOBJ, EARR
ejson ejson_takeT(ejson r);               // for EOBJ, EARR
ejson ejson_takeO(ejson r, ejson      o); // for EOBJ, EARR
ejson ejson_takeR(ejson r, constr rawk);  // for EOBJ, EARR
ejson ejson_takeI(ejson r, int     idx);  // for EARR
ejson ejson_takeP(ejson r, constr path);  // for EOBJ, EARR

int   ejson_freeH(ejson r);               // for EOBJ, EARR
int   ejson_freeT(ejson r);               // for EOBJ, EARR
int   ejson_freeO(ejson r, ejson    obj);  // for EOBJ, EARR
int   ejson_freeR(ejson r, constr rawk);  // for EOBJ, EARR
int   ejson_freeI(ejson r, int     idx);  // for EARR
int   ejson_freeP(ejson r, constr path);  // for EOBJ, EARR

int   ejson_freeHEx(ejson r,              eobj_rls_ex_cb rls, eval prvt);
int   ejson_freeTEx(ejson r,              eobj_rls_ex_cb rls, eval prvt);
int   ejson_freeOEx(ejson r, ejson   obj, eobj_rls_ex_cb rls, eval prvt);
int   ejson_freeREx(ejson r, constr rawk, eobj_rls_ex_cb rls, eval prvt);
int   ejson_freeIEx(ejson r, int     idx, eobj_rls_ex_cb rls, eval prvt);
int   ejson_freePEx(ejson r, constr pkey, eobj_rls_ex_cb rls, eval prvt);

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

int  ejson_rCmpI(ejson r, constr rawk, i64    val);
int  ejson_rCmpF(ejson r, constr rawk, f64    val);
int  ejson_rCmpS(ejson r, constr rawk, constr str);

int  ejson_iCmpI(ejson r, u32    idx , i64    val);
int  ejson_iCmpF(ejson r, u32    idx , f64    val);
int  ejson_iCmpS(ejson r, u32    idx , constr str);

int  ejson_pCmpI(ejson r, constr path, i64    val);
int  ejson_pCmpF(ejson r, constr path, f64    val);
int  ejson_pCmpS(ejson r, constr path, constr str);

/** -----------------------------------------------------
 *
 *  ejson iterationg
 *
 *  @note
 *      for perfomance, we do not check type of o in
 *  ejson_next() and ejson_prev()
 *
 */
ejson  ejson_first(ejson r);
ejson  ejson_last (ejson r);
ejson  ejson_next (ejson o);
ejson  ejson_prev (ejson o);

ejson  ejson_rFirst(ejson r, constr rawk);
ejson  ejson_rLast (ejson r, constr rawk);

ejson  ejson_pFirst(ejson r, constr path);
ejson  ejson_pLast (ejson r, constr path);

#define ejson_foreach( r,       itr)   for(itr = ejson_first (r      ); (itr); itr = ejson_next(itr))
#define ejson_kForeach(r, rawk, itr)   for(itr = ejson_rFirst(r, rawk); (itr); itr = ejson_next(itr))
#define ejson_pForeach(r, path, itr)   for(itr = ejson_pFirst(r, path); (itr); itr = ejson_next(itr))

#define ejson_foreach_s( r,       itr) for(ejson _INNER_ = ejson_first (r      ), itr; (itr = _INNER_, _INNER_ = ejson_next(_INNER_), itr); )
#define ejson_kForeach_s(r, rawk, itr) for(ejson _INNER_ = ejson_rFirst(r, rawk), itr; (itr = _INNER_, _INNER_ = ejson_next(_INNER_), itr); )
#define ejson_pForeach_s(r, path, itr) for(ejson _INNER_ = ejson_pFirst(r, path), itr; (itr = _INNER_, _INNER_ = ejson_next(_INNER_), itr); )

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
ejson ejson_rSetT(ejson r, constr rawk, etypeo   t);
ejson ejson_rSetI(ejson r, constr rawk, i64    val);
ejson ejson_rSetF(ejson r, constr rawk, f64    val);
ejson ejson_rSetS(ejson r, constr rawk, constr str);
ejson ejson_rSetP(ejson r, constr rawk, constr ptr);
ejson ejson_rSetR(ejson r, constr rawk, uint   len);

ejson ejson_iSetT(ejson r, u32    idx , etypeo   t);
ejson ejson_iSetI(ejson r, u32    idx , i64    val);
ejson ejson_iSetF(ejson r, u32    idx , f64    val);
ejson ejson_iSetS(ejson r, u32    idx , constr str);
ejson ejson_iSetP(ejson r, u32    idx , constr ptr);
ejson ejson_iSetR(ejson r, u32    idx , uint   len);

ejson ejson_pSetT(ejson r, constr path, etypeo   t);
ejson ejson_pSetI(ejson r, constr path, i64    val);
ejson ejson_pSetF(ejson r, constr path, f64    val);
ejson ejson_pSetS(ejson r, constr path, constr str);
ejson ejson_pSetP(ejson r, constr path, constr ptr);
ejson ejson_pSetR(ejson r, constr path, uint   len);

/// -----------------------------------------------------
//! @brief ejson substitute string
///
/// @return the modified obj if setted
///
ejson ejson_rReplaceS(ejson root, constr rawk, constr from, constr to);
ejson ejson_pReplaceS(ejson root, constr path, constr from, constr to);

/// -----------------------------------------------------
/// @brief ejson counter
///
/// @note:
///     1. if NUM obj not exsit in EOBJ, will create automatically
///     2. only support NUM obj if target obj exist
///     3. return LLONG_MIN if failed
///
i64  ejson_pp  (ejson o);                // increase 1
i64  ejson_mm  (ejson o);                // decrease 1
i64  ejson_incr(ejson o, i64 v);         // increase v
i64  ejson_decr(ejson o, i64 v);         // decrease v

i64  ejson_rPP  (ejson r, constr rawk);
i64  ejson_rMM  (ejson r, constr rawk);
i64  ejson_rIncr(ejson r, constr rawk, i64 v);
i64  ejson_rDecr(ejson r, constr rawk, i64 v);

i64  ejson_iPP  (ejson r, u32    idx );
i64  ejson_iMM  (ejson r, u32    idx );
i64  ejson_iIncr(ejson r, u32    idx , i64 v);
i64  ejson_iDecr(ejson r, u32    idx , i64 v);

i64  ejson_pPP  (ejson r, constr path);
i64  ejson_pMM  (ejson r, constr path);
i64  ejson_pIncr(ejson r, constr path, i64 v);
i64  ejson_pDecr(ejson r, constr path, i64 v);

/** -----------------------------------------------
 *  @brief
 *      ejson sort operation
 *
 *  @note:
 *      it only effect on when obj's etypeo is
 *  EOBJ and EARR
 *
 *      The  comparison function must return an integer less than, equal to,
 * or greater than zero if the first argument is considered to be respectively
 * less than, equal to, or greater than the second.  If two members compare
 * as equal, their order in the sorted array is undefined.
 *
 */

//! supplied default sort cb
int    __KEYS_ACS(eobj a, eobj b);      // Ascending  via key string in all obj
int    __KEYS_DES(eobj a, eobj b);      // Descending via key string in all obj
int    __VALI_ACS(eobj a, eobj b);      // Ascending  via int value in NUM obj
int    __VALI_DES(eobj a, eobj b);      // Descending via int value in NUM obj

ejson  ejson_sort (ejson r,              eobj_cmp_cb cmp);
ejson  ejson_rSort(ejson r, constr rawk, eobj_cmp_cb cmp);
ejson  ejson_pSort(ejson r, constr path, eobj_cmp_cb cmp);

ejson  ejson_sort_r (ejson r,              eobj_cmp_ex_cb cmp, eval prvt);
ejson  ejson_rSort_r(ejson r, constr rawk, eobj_cmp_ex_cb cmp, eval prvt);
ejson  ejson_pSort_r(ejson r, constr path, eobj_cmp_ex_cb cmp, eval prvt);

/// -- ejson version
constr ejson_version();

#ifdef __cplusplus
}
#endif

#endif
