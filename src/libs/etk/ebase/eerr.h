#ifndef __EERR_H__
#define __EERR_H__

#include "etype.h"

static constr __g_err__;
static char   __g_err_buf__[1024];


#define eerrset(err)  __g_err__ = err
#define eerrfmt(...)  {snprintf(__g_err_buf__, 1024, ##__VA_ARGS__); __g_err__ = __g_err_buf__;}
#define eerrlog(...)  fprintf(stderr, ##__VA_ARGS__);

#define eerrget()     __g_err__

#define _checkOBJ(o)        is0_exeret(_isOBJ(o)     , errset("invalid" #o " (is not a OBJ obj)"), 0)
#define _checkNULL(o)       is0_exeret(o             , errset("invalid" #o " (is nullptr)"), 0)
#define _checkZERO(i)       is0_exeret(i             , errset("invalid" #i " (is 0)"), 0)
#define _checkSRC(s)        is1_exeret(_invalidS(s)  , errset("invalid" #s " (is nullptr or empty)");g_errp = _NIL_, 0)
#define _checkParent(o)     is0_exeret(_isParent(o)  , errset("invalid" #o " (is nullptr or not in ARR/OBJ type)"), 0)
#define _checkInvldS(s)     is1_exeret(_invalidS(s)  , errset("invalid" #s " (is nullptr or empty)"), 0)
#define _checkCanAdd(r,o)   is1_exeret(_canotAdd(r,o), errset("invalid" #o " (is nullptr or is the root obj self or is in another ejson already)"), 0)
#define _checkCanRm(o)      is1_exeret(_canotRm(o)   , errset("invalid" #o " (is nullptr or not a child obj)");, 0);

#define _ERRSTR_NOKEY       "found no key in param and obj"
#define _ERRSTR_PARSE       "src parsing error"
#define _ERRSTR_TYPEDF      "invalid type (root obj is not in OBJ/ARR type)"
#define _ERRSTR_RMFOBJ      "obj to remove is not in the root(OBJ obj)"
#define _ERRSTR_RMFARR      "obj to remove is not in the root(ARR obj)"

#define _ERRSTR_ALLOC(tag)      "alloc faild for" #tag
#define _ERRSTR_KEYINPRM(key)   "key \"%s\" in param is already exist in root obj", key
#define _ERRSTR_KEYINOBJ(key)   "key \"%s\" in obj is already exist in root obj", key

#endif
