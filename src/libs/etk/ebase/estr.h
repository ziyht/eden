/// =====================================================================================
///
///       Filename:  estr.h
///
///    Description:  a easier way to handle string in C, rebuild based on sds from redis,
///                  including two tools:
///                     estr - for heap using
///                     sstr - for stack using
///
///        Version:  1.3
///        Created:  02/25/2017 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================


#ifndef __ESTR_H__
#define __ESTR_H__

#include <stdarg.h>

#include "etype.h"

#ifdef  __GNUC__
#define __format_printf __attribute__((format(printf, 2, 3)))
#else
#define __format_printf
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// ---------------------- estr -------------------------
///
///     estr - for heap using
///
///     when using a estr, you can deal with it as a cstr,
/// but it will be more convenient and High-Performance
/// with our APIs.
///
///     for convenient using, the write of estr will create
/// a estr autolly when you passed NULL:
///     estr s = 0; estr_wrtS(s, "1234");
///     now the s is a estr of "1234", so you can also using
/// the writer to create a estr, note that this operation is
/// not applicable for sstr;
///
/// @note:
///     you need to free it by using estr_free()
///

typedef char* estr;

/// -- estr creator and destroyer -----------------------
estr estr_new(size cap);
estr estr_newLen(conptr ptr, size len);

estr estr_fromI64(i64 val);
estr estr_fromU64(u64 val);

estr estr_fromInput(constr tag, int hide);          // if hide = 1, the character you type in will be hidden, note: no multi thread lock
estr estr_fromFile(constr path, int mlen);          // mlen is the max len you want get

estr estr_dup (estr   s);
estr estr_dupS(constr s);

void estr_clear(estr s);
void estr_wipe (estr s);

void estr_free(estr s);

/// -- estr writer --------------------------------------
///
///   for convenience, if s is 0, we will create a new estr
/// automaticly
///
/// note:
///     be careful when using those macros, it pass in the
/// addr of estr automaticlly, the addr of this estr maybe
/// changed after operation, so the estr must not be a
/// passed in estr from another func
///

//! estr_wrt*: write from the beginning
#define estr_wrtE(s, s2)        __estr_wrtE(&(s), (s2))             // estr
#define estr_wrtS(s, cstr)      __estr_wrtS(&(s), (cstr))           // cstr
#define estr_wrtW(s, word)      __estr_wrtW(&(s), (word))           // word, continues alpha
#define estr_wrtL(s, line)      __estr_wrtL(&(s), (line))           // line, not include '\n'
#define estr_wrtT(s, src, end)  __estr_wrtT(&(s), (src), end)       // to  , not include end char
#define estr_wrtB(s, ptr, len)  __estr_wrtB(&(s), (ptr), (len))     // bin
#define estr_wrtC(s, c  , cnt)  __estr_wrtC(&(s), (c  ), (cnt))     // char int cnt
#define estr_wrtA(s, fmt, ap)   __estr_wrtA(&(s), fmt, ap)          // vargs : like vsprintf
#define estr_wrtP(s, ...)       __estr_wrtP(&(s), ##__VA_ARGS__)    // format: like sprintf
#define estr_wrtF(s, ...)       __estr_wrtF(&(s), ##__VA_ARGS__)    // format: only support %s(cstr) %S(estr) %i(i32) %I(i64) %u(u32) %U(u64) %%(%)

//! estr_cat*: write continued
#define estr_catE(s, s2)        __estr_catE(&(s), (s2))             // estr
#define estr_catS(s, cstr)      __estr_catS(&(s), (cstr))           // cstr
#define estr_catW(s, word)      __estr_catW(&(s), (word))           // word? continues alpha
#define estr_catL(s, line)      __estr_catL(&(s), (line))           // line, not include '\n'
#define estr_catT(s, src, end)  __estr_catT(&(s), (src), end)       // to  , not include end char
#define estr_catB(s, ptr, len)  __estr_catB(&(s), (ptr), (len))     // bin
#define estr_catC(s, c  , cnt)  __estr_catC(&(s), (c  ), (cnt))     // char
#define estr_catA(s, fmt, ap)   __estr_catA(&(s), fmt, ap)          // vargs : like vsprintf
#define estr_catP(s, ...)       __estr_catP(&(s), ##__VA_ARGS__)    // format: like sprintf
#define estr_catF(s, ...)       __estr_catF(&(s), ##__VA_ARGS__)    // format: only support %s(cstr) %S(estr) %i(i32) %I(i64) %u(u32) %U(u64) %%(%)

estr    estr_setB(estr s, int  start, conptr in, size len);         // todo
int     estr_setT(estr s, char c);                                  // set the last char of s, if c == '\0', the len of s will decrease 1 automaticlly


/// -- estr getter ---------------------------------------
size    estr_len  (estr s);
size    estr_cap  (estr s);
cstr    estr_cur  (estr s);          // Returns the current write pos in estr: s + estr_len(s)
size    estr_avail(estr s);
char    estr_tail (estr s);

/// -- estr utils ---------------------------------------
void    estr_show(estr s);

int     estr_cmp (estr s, estr   s2 );
int     estr_cmpS(estr s, constr src);

i64     estr_lower(estr s);
i64     estr_upper(estr s);
estr    estr_range(estr s, i64    start, i64 end);
i64     estr_trim (estr s, constr cset);
i64     estr_mapc (estr s, constr from, constr to);
i64     estr_mapcl(estr s, constr from, constr to, size_t len);
i64     estr_subc (estr s, constr cset, constr to);

#define estr_subs(s, from, to) __estr_subs(&(s), from, to)

void    estr_cntc (estr s, u8 cnts[256], char end);  // you can also cnt cstr derectly

cstr    estr_tok  (estr s, char delim);

/// -- Low level functions exposed to the user API ------  << be careful >>
#define estr_ensure(      s,         len)   __estr_ensure(&(s), len)
void    estr_incrLen(estr s, size_t incr);
void    estr_decrLen(estr s, size_t decr);
void    estr_shrink (estr s);

/// -- split tools -------------------------------------

typedef cstr* esplt;

esplt   esplt_new (int need, bool keep, bool trim);
void    esplt_set (esplt sp, bool keep, bool trim);
void    esplt_free(esplt sp);

void    esplt_show(esplt sp, int max);

int     esplt_cnt   (esplt sp);
int     esplt_unique(esplt sp);

#define esplt_splitE(sp, s, sep)                __esplt_splitE(&(sp), s, sep)
#define esplt_splitS(sp, s, sep)                __esplt_splitS(&(sp), s, sep)
#define esplt_splitB(sp, b, blen, sep, seplen)  __esplt_splitB(&(sp), b, blen, sep, seplen)

#define esplt_splitArgv(sp, argv, argc)         __espli_splitArgv(&(sp), argv, argc)
#define esplt_splitCmdl(sp, cmdline)            __espli_splitCmdl(&(sp), cmdline)

#define estr_joinStrs(s, sarr, cnt, sep)        __estr_joinStrs(&(s), sarr, cnt, sep)
#define estr_joinSplt(s, sp, sep)               __estr_joinSplt(&(s), sp, sep)

/// ---------------------- sstr -------------------------
///
///     sstr - for stack using
///
///     the using of sstr is almost the same as estr,
/// we using the passed buf to init it, so we will not
/// expand it autolly when there is no enough space to
/// do the operation, in this case, we'll do nothing and
/// always return 0;
///
/// @note:
///     1. all estr can using those following API, but
///        you may not get the correct result when you
///        using it as first @param s, and you can also
///        consider those APIs is a safety version for
///        estr.
///     2. do not using the above estr APIs to operate
///        sstrs inited by sstr_init()
///

typedef char* sstr;
#define sstr_needLen(l) (9u + (l))     // in stack, 9 byte for header

#define SSTR_DEST(n, l) char #n[9u+l]

/// -- sstr initializer ---------------------------------
sstr    sstr_init(cptr buf, uint len);

/// -- sstr writer --------------------------------------
i64     sstr_wrtE(sstr s, sstr   s2 );                          // estr
i64     sstr_wrtS(sstr s, constr src);                          // cstr
i64     sstr_wrtW(sstr s, constr word);                         // word, continues alpha
i64     sstr_wrtL(sstr s, constr line);                         // line, not include '\n'
i64     sstr_wrtT(sstr s, constr src, char    end);             // to  , not include end char
i64     sstr_wrtB(sstr s, conptr ptr, size    len);             // bin
i64     sstr_wrtC(sstr s, char   c  , size    cnt);             // char in cnt
i64     sstr_wrtA(sstr s, constr fmt, va_list ap );             // vargs : like vsprintf
i64     sstr_wrtP(sstr s, constr fmt, ...) __format_printf;     // format: like sprintf
i64     sstr_wrtF(sstr s, constr fmt, ...);                     // format: only support %s(cstr) %S(estr) %i(i32) %I(i64) %u(u32) %U(u64) %%(%)

i64     sstr_catE(sstr s, sstr   s2 );                          // estr
i64     sstr_catS(sstr s, constr src);                          // cstr
i64     sstr_catW(sstr s, constr src);                          // word, continues alpha
i64     sstr_catL(sstr s, constr src);                          // line, not include '\n'
i64     sstr_catT(sstr s, constr src, char    end);             // to  , not include end char
i64     sstr_catB(sstr s, conptr ptr, size    len);             // bin
i64     sstr_catC(sstr s, char   c  , size    cnt);             // char in cnt
i64     sstr_catA(sstr s, constr fmt, va_list ap );             // vargs : like vsprintf
i64     sstr_catP(sstr s, constr fmt, ...) __format_printf;     // format: like sprintf
i64     sstr_catF(sstr s, constr fmt, ...);                     // format: only support %s(cstr) %S(estr) %i(i32) %I(i64) %u(u32) %U(u64) %%(%)

#define sstr_clear  estr_clear
#define sstr_wipe   estr_wipe

/// -- sstr getter --------------------------------------
#define sstr_len    estr_len
#define sstr_cap    estr_cap
#define sstr_cur    estr_cur
#define sstr_avail  estr_avail
#define sstr_tail   estr_tail

/// -- estr utils ------------------------------------
#define sstr_show   estr_show

#define sstr_cmp    estr_cmp
#define sstr_cmpS   estr_cmpS

#define sstr_lower  estr_lower
#define sstr_upper  estr_upper

#define sstr_range  estr_range
#define sstr_trim   estr_trim
#define sstr_mapc   estr_mapc
#define sstr_mapcl  estr_mapcl
#define sstr_subc   estr_subc
#define sstr_upper  estr_upper

#define sstr_subs   estr_subs

/// -- Low level functions exposed to the user API ------
void    sstr_decrLen(sstr s, size_t decr);


/// -- estr real declarition ----------------------------
///

i64  __estr_wrtE(estr*s, estr   s2);
i64  __estr_wrtS(estr*s, constr src);
i64  __estr_wrtW(estr*s, constr word);
i64  __estr_wrtL(estr*s, constr line);
i64  __estr_wrtT(estr*s, constr src, char    end);
i64  __estr_wrtB(estr*s, conptr ptr, size    len);
i64  __estr_wrtC(estr*s, char   c  , size    len);
i64  __estr_wrtA(estr*s, constr fmt, va_list ap);
i64  __estr_wrtP(estr*s, constr fmt, ...) __format_printf;
i64  __estr_wrtF(estr*s, constr fmt, ...);

i64  __estr_catE(estr*s, estr   s2 );
i64  __estr_catS(estr*s, constr src);
i64  __estr_catW(estr*s, constr word);
i64  __estr_catL(estr*s, constr line);
i64  __estr_catT(estr*s, constr src, char    end);
i64  __estr_catB(estr*s, conptr ptr, size    len);
i64  __estr_catC(estr*s, char   c  , size    len);
i64  __estr_catA(estr*s, constr fmt, va_list ap );
i64  __estr_catP(estr*s, constr fmt, ...) __format_printf;
i64  __estr_catF(estr*s, constr fmt, ...);

bool __estr_ensure(estr*s, size_t addlen);

i64  __estr_subs(estr*s, constr from, constr to);

int  __esplt_splitE(esplt*_sp, estr   s  , constr sep);
int  __esplt_splitS(esplt*_sp, constr src, constr sep);
int  __esplt_splitB(esplt*_sp, conptr ptr, int len, conptr sep, int seplen);

int  __espli_splitArgv(esplt*_sp, char** argv, int argc);
int  __espli_splitCmdl(esplt*_sp, constr cmdline);

i64  __estr_joinStrs(estr*s, cstr* sarr, int cnt, constr sep);
i64  __estr_joinSplt(estr*s, esplt sp, constr sep);

#ifdef __cplusplus
}
#endif

#endif
