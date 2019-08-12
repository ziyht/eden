/// =====================================================================================
///
///       Filename:  eutils.h
///
///    Description:  tools for etools
///
///        Version:  1.0
///        Created:  09/27/2017 05:58:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EUTILS_H__
#define __EUTILS_H__

#include <string.h>

#include "ecompat.h"
#include "etype.h"

#ifdef __cplusplus
extern "C" {
#endif

/** =====================================================
 *
 *  llog tools
 *
 * ------------------------------------------------------
 */
#ifdef  EUTILS_LLOG
#define EUTILS_LLOG_DBG 0
#define EUTILS_LLOG_INF 1
#define EUTILS_LLOG_WRN 2
#define EUTILS_LLOG_ERR 3
#ifdef _WIN32
static constr _llog_basename(constr path){static constr slash; if (slash) {return slash + 1;}else{slash = strrchr(path, '\\');}if (slash) {return slash + 1;}return 0;}
#else
static constr _llog_basename(constr path){static constr slash; if (slash) {return slash + 1;}else{slash = strrchr(path, '/');}if (slash) {return slash + 1;}return 0;}
#endif
#define _log(fmt, ...)       fprintf(stdout, "%s(%d):" fmt "%s", _llog_basename(__FILE__), __LINE__, __VA_ARGS__)
#define _log_dbg(fmt, ...)   fprintf(stdout, "%s(%d) DBG:" fmt "%s", _llog_basename(__FILE__), __LINE__, __VA_ARGS__)
#define _log_inf(fmt, ...)   fprintf(stdout, "%s(%d) INF:" fmt "%s", _llog_basename(__FILE__), __LINE__, __VA_ARGS__)
#define _log_wrn(fmt, ...)   fprintf(stdout, "%s(%d) WRN:" fmt "%s", _llog_basename(__FILE__), __LINE__, __VA_ARGS__)
#define _log_err(fmt, ...)   fprintf(stderr, "%s(%d) ERR:" fmt "%s", _llog_basename(__FILE__), __LINE__, __VA_ARGS__)
#ifndef _WIN32
#define _llog(...)           _log(__VA_ARGS__, "\n");fflush(stdout)
#define _llog_dbg(...)       _log_dbg(__VA_ARGS__, "\n");fflush(stdout)
#define _llog_inf(...)       _log_inf(__VA_ARGS__, "\n");fflush(stdout)
#define _llog_wrn(...)       _log_wrn(__VA_ARGS__, "\n");fflush(stdout)
#define _llog_err(...)       _log_err(__VA_ARGS__, "\n");fflush(stdout)
#else
#ifdef _MSC_VER
#define _llog(fmt, ...)      fprintf(stdout, "%s(%d):" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stdout)
#define _llog_dbg(fmt, ...)  fprintf(stdout, "%s(%d) DBG:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stdout)
#define _llog_inf(fmt, ...)  fprintf(stdout, "%s(%d) INF:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stdout)
#define _llog_wrn(fmt, ...)  fprintf(stdout, "%s(%d) WRN:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stdout)
#define _llog_err(fmt, ...)  fprintf(stderr, "%s(%d) ERR:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stderr)
#else
#define _llog(fmt, ...)      fprintf(stdout, "%s(%d):" fmt "\n", _llog_basename(__FILE__), __LINE__, ##__VA_ARGS__);fflush(stdout)
#define _llog_dbg(fmt, ...)  fprintf(stdout, "%s(%d) DBG:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stdout)
#define _llog_inf(fmt, ...)  fprintf(stdout, "%s(%d) INF:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stdout)
#define _llog_wrn(fmt, ...)  fprintf(stdout, "%s(%d) WRN:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stdout)
#define _llog_err(fmt, ...)  fprintf(stderr, "%s(%d) ERR:" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stderr)
#endif
#endif

#if (EUTILS_LLOG+0)
#define llog                _llog
#define llog_dbg            _llog_dbg
#define llog_inf            _llog_inf
#define llog_wrn            _llog_wrn
#define llog_err            _llog_err

#ifdef EUTILS_LLOG_LEVEL

#if EUTILS_LLOG_LEVEL + 0 > 0
#undef  llog_dbg
#define llog_dbg(...)
#endif

#if EUTILS_LLOG_LEVEL + 0 > 1
#undef  llog_inf
#define llog_inf(...)
#endif

#if EUTILS_LLOG_LEVEL + 0 > 2
#undef  llog_wrn
#define llog_wrn(...)
#endif

#if EUTILS_LLOG_LEVEL + 0 > 3
#undef  llog_err
#define llog_err(...)
#endif

#undef EUTILS_LLOG_LEVEL
#endif

#else
#define llog(...)
#define llog_dbg(...)
#define llog_inf(...)
#define llog_wrn(...)
#define llog_err(...)
#endif
#else
#define _llog(...)
#define _llog_dbg(...)
#define _llog_inf(...)
#define _llog_wrn(...)
#define _llog_err(...)
#define llog(...)
#define llog_dbg(...)
#define llog_inf(...)
#define llog_wrn(...)
#define llog_err(...)
#endif

#ifdef  __ELOG_H__
#undef  _llog
#undef  _llog_dbg
#undef  _llog_inf
#undef  _llog_wrn
#undef  _llog_err
#define _llog(...)           elog_inf(0, __VA_ARGS__);
#define _llog_dbg(...)       elog_dbg(0, __VA_ARGS__);
#define _llog_inf(...)       elog_inf(0, __VA_ARGS__);
#define _llog_wrn(...)       elog_wrn(0, __VA_ARGS__);
#define _llog_err(...)       elog_err(0, __VA_ARGS__);
#endif


/** =====================================================
 *
 *  some macro helpers
 *
 *  ------------------------------------------------------
 */

#define exe_ret(expr, ret ) { expr;      return ret;}
#define is0_ret(cond, ret ) if(!(cond)){ return ret;}
#define is1_ret(cond, ret ) if( (cond)){ return ret;}
#define is0_exe(cond, expr) if(!(cond)){ expr;}
#define is1_exe(cond, expr) if( (cond)){ expr;}

#define is0_exeret(cond, expr, ret) if(!(cond)){ expr;        return ret;}
#define is1_exeret(cond, expr, ret) if( (cond)){ expr;        return ret;}
#define is0_elsret(cond, expr, ret) if(!(cond)){ expr;} else{ return ret;}
#define is1_elsret(cond, expr, ret) if( (cond)){ expr;} else{ return ret;}

#undef  container_of
#undef  offsetof
#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member) ))   // hava a operation, minimize related operations
#define offsetof(TYPE, MEMBER)          ((size_t) &((TYPE *)0)->MEMBER)

#define is_eq(a, b) ((a) == (b))
#define un_eq(a, b) ((a) != (b))

#define E_UNUSED(p) (void)p

/** ------------------------------------------------------
 *
 *  math tools
 *
 */
static __always_inline int pow2gt(int x)	{	--x;	x|=x>>1;	x|=x>>2;	x|=x>>4;	x|=x>>8;	x|=x>>16;	return x+1;	}

/** ------------------------------------------------------
 *
 *  string tools
 *
 * @note:
 *      doing the actual number -> string conversion. 's' must point
 *  to a string with room for at least 21 bytes.
 *
 *      The function returns the length of the null-terminated string
 *  representation stored at 'dest'.
 */

#define __ __always_inline

int  ll2str(i64 v, cstr dest);
int ull2str(u64 v, cstr dest);

static __ cstr  llstr(i64 v) { static char dest[21];  ll2str(v, dest); return dest; }
static __ cstr ullstr(u64 v) { static char dest[21]; ull2str(v, dest); return dest; }

static __ cstr  llstr_r(i64 v, cstr dest) {  ll2str(v, dest); return dest; }
static __ cstr ullstr_r(u64 v, cstr dest) { ull2str(v, dest); return dest; }

int e_strcasecmp (const char *s1, const char *s2);
int e_strncasecmp(const char *s1, const char *s2, size_t n);

#undef __


/** -----------------------------------------------------
 *
 *  rand tools
 *
 *  rand48 - a stand-alone replacement for the 48-bit random number functions
 *           available in Linux, OS/X, etc.
 *
 *  @note: rebuilt from squeamish-ossifrage/rand48
 *
 */

int  e_rand();
void e_srand(int seed);

long e_lrand48();                   // Generate a random unsigned         int by iterating the internal buffer
long e_mrand48();                   // Generate a random signed           int by iterating the internal buffer
f64  e_drand48();                   // Generate a random value of type double by iterating the internal buffer

long e_jrand48(u16 xsubi[3]);       // Generate a random unsigned         int by iterating an external buffer
long e_nrand48(u16 xsubi[3]);       // Generate a random signed           int by iterating an external buffer
f64  e_erand48(u16 xsubi[3]);       // Generate a random value of type double by iterating an external buffer

void e_srand48(long seedval);       // Update seed value from a 32-bit unsigned integer
u16* e_seed48( u16 seed16v[3]);     // Change the seed value and reset the other LCG parameters, return old seed

void e_lcong48(u16 param[7]);       // Change the LCG parameters

/** -----------------------------------------------------
 *
 *  system tools
 *
 */
int  e_get_nprocs();


cstr eutils_version();

#ifdef __cplusplus
}
#endif

#endif
