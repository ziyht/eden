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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include <inttypes.h>
#include <errno.h>
#include <stdbool.h>

#if (!_WIN32)
#include <termios.h>        // tcsetattr()
#endif

#include "eutils.h"
#include "ecompat.h"

#define   STB_SPRINTF_USING_STRCHR                      // strchr is very fast now
#define   STB_SPRINTF_USING_MEMCPY                      // memcpy is very fast now
#define   STB_SPRINTF_COPY_STR_PROMOTION                // memcpy is ok
#define   STB_SPRINTF_MIN              ((int*)user)[0]  //
#define   STB_SPRINTF_DECORATE(name)   __estr_##name    // defined it will not conflict with other using
#include "stb_sprintf.h"

#include "estr.h"
#include "estr_p.h"

#define ESTR_VERSION "estr 1.3.0"       // import stb_sprintf.c

/// -- helper ------------------------------

#define _ERR_HANDLE   -1
#define _ERR_ALLOC    -2

#undef  strlen
#define strlen(s) ((uint)(strchr(s, '\0') - s))       // this operation is more efficient in windows

#define __get_word_s_e(w, e)                \
do {                                        \
    if   ( !w         ) return 0;           \
    while( isspace(*w)) w++;                \
    if   (!isalpha(*w)) return 0; e = w;    \
    while( isalpha(*e)) e++;                \
}while(0)

static inline void _show(constr tag, estr s)
{
    size_t len; u8 flags;

    if(!s)
    {
        printf("(%s: nullptr)\n", tag);fflush(stdout);
        return;
    }

    tag = tag[1] == 'b' ? _s_isStack(s) ? "ebuf: sstr" : "ebuf: estr"
                        : _s_isStack(s) ? "sstr" : "estr" ;

    len   = _s_len(s);
    flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5 : printf("(%s: e05 %"PRIi64"/%"PRIi64"):[", tag, (u64)len, (u64)_s_cap(s)); break;
        case SDS_TYPE_8 : printf("(%s: e08 %"PRIi64"/%"PRIi64"):[", tag, (u64)len, (u64)_s_cap(s)); break;
        case SDS_TYPE_16: printf("(%s: e16 %"PRIi64"/%"PRIi64"):[", tag, (u64)len, (u64)_s_cap(s)); break;
        case SDS_TYPE_32: printf("(%s: e32 %"PRIi64"/%"PRIi64"):[", tag, (u64)len, (u64)_s_cap(s)); break;
        case SDS_TYPE_64: printf("(%s: e64 %"PRIi64"/%"PRIi64"):[", tag, (u64)len, (u64)_s_cap(s)); break;
    }
    fflush(stdout);

#if (!_WIN32)
    write(STDOUT_FILENO, s, len);
#else
    fwrite(s, len, 1, stdout);
#endif

    printf("]\n");
    fflush(stdout);
}

/// ------------------ win32 API setting -------------------
#if (_WIN32)
#define inline
#endif

/// =====================================================================================
/// estr
/// =====================================================================================

/// -- estr creator and destroyer -----------------------
inline estr estr_new(size cap)
{
    return estr_newLen(0, cap);
}

estr estr_newLen(conptr ptr, size initlen) {
    cstr sh; sds  s; size_t datalen; char type; int hdrlen; unsigned char *fp; /* flags pointer. */

    datalen = ptr ? initlen : 0;
    type    = _s_reqT(initlen);

    /* Empty strings are usually created in order to append. Use type 8
     * since type 5 is not good at this. */
    if (type == SDS_TYPE_5 && datalen == 0 ) type = SDS_TYPE_8;
    hdrlen = _s_lenH(type);
    sh = emalloc(hdrlen + initlen + 1);
    is0_ret(sh, 0);

    s  = sh + hdrlen;
    fp = ((unsigned char*)s)-1;
    switch(type) {
        case SDS_TYPE_5 : {*fp = type | (u8)(initlen << SDS_TYPE_BITS); break; }
        case SDS_TYPE_8 : {SDS_HDR_VAR( 8,s); sh->len = (u8) datalen; sh->alloc = (u8) initlen; *fp = type; break; }
        case SDS_TYPE_16: {SDS_HDR_VAR(16,s); sh->len = (u16)datalen; sh->alloc = (u16)initlen; *fp = type; break; }
        case SDS_TYPE_32: {SDS_HDR_VAR(32,s); sh->len = (u32)datalen; sh->alloc = (u32)initlen; *fp = type; break; }
        case SDS_TYPE_64: {SDS_HDR_VAR(64,s); sh->len =      datalen; sh->alloc =      initlen; *fp = type; break; }
    }
    if (datalen) { memcpy(s, ptr, initlen); s[datalen] = '\0';}
    else           memset(s, 0, initlen+1);
    return s;
}

estr estr_fromI64(i64 val) { char buf[SDS_LLSTR_SIZE]; return estr_newLen(buf,  ll2str(val, buf)); }
estr estr_fromU64(u64 val) { char buf[SDS_LLSTR_SIZE]; return estr_newLen(buf, ull2str(val, buf)); }

#if (!_WIN32)
/// @brief set_disp_mode
/// @param fd     : STDIN_FILENO for stdin
/// @param option : 0: off, 1: on ;
/// @return [0] - set ok
///         [1] - set err
///
#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)
static inline int __set_disp_mode(int fd, int option)
{
   int err; struct termios term;

   if(tcgetattr(fd, &term) == -1)
   {
        perror("Cannot get the attribution of the terminal");
        return 1;
   }
   if(option) term.c_lflag |=  ECHOFLAGS;
   else       term.c_lflag &= ~ECHOFLAGS;

   err = tcsetattr(fd, TCSAFLUSH, &term);

   if(err == -1 && err == EINTR){
        perror("Cannot set the attribution of the terminal");
        return 1;
   }

   return 0;
}
#endif

estr estr_fromInput(constr tag, int hide)
{
    char c; estr input;

    input = estr_newLen(0, 16);

    is0_ret(input, 0);

#if (!_WIN32)
    if(tag)  write(STDOUT_FILENO, tag, strlen(tag));
    if(hide) __set_disp_mode(STDIN_FILENO, 0);
#else
    if(tag)  fwrite(tag, strlen(tag), 1, stdout); fflush(stdout);
#endif

    do{
       c = getchar();
       if (c != '\n' && c !='\r'){
         __estr_catB(&input, &c, 1);

       }
    }while(c != '\n' && c !='\r');

    if(hide)
    {
#if (!_WIN32)
        __set_disp_mode(STDIN_FILENO, 1);
        write(STDOUT_FILENO, "\n", 1);
#else
        fwrite("\n", 1, 1, stdout); fflush(stdout);
#endif
    }

    return input;
}

estr estr_fromFile(constr path, int mlen)
{
    estr s; int fd, rd_len, cat_len; uint size;

    is1_ret(!path || !mlen, 0);
    is1_ret((fd = open(path, O_RDONLY)) == -1, 0);

    size = lseek(fd, 0L, SEEK_END); lseek(fd, 0L, SEEK_SET);

    if(size == 0)
    {
        s = _s_new(64);

        while(mlen)
        {
            cat_len = mlen > 4096 ? 4096 : mlen;
            mlen   -= cat_len;

            _s_ensure(s, (size_t)cat_len);

            rd_len = read(fd, s + _s_len(s), cat_len);
            is1_exeret(rd_len <= 0, close(fd), s);

            _s_incLen(s, rd_len);

            if(rd_len < cat_len) break;
        }
    }
    else
    {
        s = _s_new(size);

        if(size > (uint)mlen) size = mlen;

        read(fd, s, size);

        _s_setLen(s, size);
    }

    close(fd);

    return s;
}

inline estr estr_dup (estr   s) { return s ? estr_newLen(s, _s_len(s)) : 0; }
inline estr estr_dupS(constr s) { return s ? estr_newLen(s, strlen(s)) : 0; }

inline void estr_free(estr   s) { if(s){ _s_free(s); } }

/// -- estr writer --------------------------------------

static __always_inline i64 __estr_wrtA_P(estr*s, constr fmt, va_list ap);
static __always_inline i64 __estr_catA_F(estr*s, constr fmt, va_list ap);
static __always_inline i64 __estr_catA_P(estr*s, constr fmt, va_list ap);

i64  __estr_wrtB(estr*_s, conptr ptr, size len)
{
    estr s = *_s;

    if(0 == s)
    {
        is0_ret(s = _s_new(len), _ERR_ALLOC);
    }
    else if(_s_cap(s) < len)
    {
        size_t need = len - _s_len(s);
        _s_ensure(s, need);
        is0_ret(s, _ERR_ALLOC);
    }

    memcpy(s, ptr, len);
    s[len] = '\0';
    _s_setLen(s, len);

    *_s = s;

    return len;
}

i64 __estr_wrtC(estr*_s, char   c  , size    len)
{
    estr s = *_s;

    if(0 == s)
    {
        is0_ret(s = _s_new(len), _ERR_ALLOC);
    }
    else if(_s_cap(s) < len)
    {
        size need = len - _s_len(s);
        _s_ensure(s, need);
        is0_ret(s, _ERR_ALLOC);
    }

    memset(s, c, len);
    s[len] = '\0';
    _s_setLen(s, len);

    *_s = s;

    return len;
}

i64 __estr_wrtE(estr*s, estr   s2  ) { return s2  ? __estr_wrtB(s, s2 , _s_len(s2 )) : 0; }
i64 __estr_wrtS(estr*s, constr src ) { return src ? __estr_wrtB(s, src, strlen(src)) : 0; }
i64 __estr_wrtW(estr*s, constr w   ) { constr e;  __get_word_s_e(w, e); return __estr_wrtB(s, w, e - w); }
i64 __estr_wrtL(estr*s, constr line) { return line ? __estr_wrtB(s, line, strchrnul(line, '\n') - line) : 0; }

i64 __estr_wrtT(estr*s, constr src, char end  ) { return src ? __estr_wrtB(s, src, strchrnul(src, end) - src) : 0; }
i64 __estr_wrtA(estr*s, constr fmt, va_list ap) {                                                         return __estr_wrtA_P(s, fmt, ap); }
i64 __estr_wrtP(estr*s, constr fmt, ...       ) { va_list ap;                          va_start(ap, fmt); return __estr_wrtA_P(s, fmt, ap); }
i64 __estr_wrtF(estr*s, constr fmt, ...       ) { va_list ap; if(*s) _s_setLen(*s, 0); va_start(ap, fmt); return __estr_catA_F(s, fmt, ap); }

inline i64 __estr_catB(estr*_s, conptr ptr, size len)
{
    estr s = *_s;

    if(0 == s)
    {
        is0_ret(s = _s_new(len), _ERR_ALLOC);
        memcpy(s, ptr, len);
        _s_setLen(s, len);
    }
    else
    {
        size_t curlen;

        curlen = _s_len(s); _s_ensure(s, len);
        is0_exeret(s, *_s = 0, _ERR_ALLOC);

        memcpy(s+curlen, ptr, len); curlen += len;
        _s_setLen(s, curlen);
        s[curlen] = '\0';
    }

    *_s = s;

    return len;
}

i64 __estr_catC(estr*_s, char   c  , size    len)
{
    estr s = *_s;

    if(0 == s)
    {
        is0_ret(s = _s_new(len), _ERR_ALLOC);
        memset(s, c, len);
        _s_setLen(s, len);
    }
    else
    {
        size_t curlen;

        curlen = _s_len(s);
        _s_ensure(s, len);
        is0_exeret(s, *_s = 0, _ERR_ALLOC);

        memset(s+curlen, c, len); curlen += len;
        _s_setLen(s, curlen);
        s[curlen] = '\0';
    }

    *_s = s;

    return len;
}

i64 __estr_catE(estr*s, estr   s2 ) { return s2  ? __estr_catB(s, s2 , _s_len( s2)) : 0; }
i64 __estr_catS(estr*s, constr src) { return src ? __estr_catB(s, src, strlen(src)) : 0; }
i64 __estr_catW(estr*s, constr w  ) { constr e; __get_word_s_e(w, e); return __estr_catB(s, w, e - w); }

i64 __estr_catL(estr*s, constr line           ) { return line ? __estr_catB(s, line, strchrnul(line, '\n') - line) : 0; }
i64 __estr_catT(estr*s, constr src, char   end) { return src  ? __estr_catB(s, src , strchrnul(src , end ) - src ) : 0; }
i64 __estr_catA(estr*s, constr fmt, va_list ap) {                                return __estr_catA_P(s, fmt, ap); }
i64 __estr_catP(estr*s, constr fmt, ...       ) { va_list ap; va_start(ap, fmt); return __estr_catA_P(s, fmt, ap); }
i64 __estr_catF(estr*s, constr fmt, ...       ) { va_list ap; va_start(ap, fmt); return __estr_catA_F(s, fmt, ap); }

typedef struct estr__context {
    int  avail;      // set in the first place so we can get it directly
    estr s;          // handle for compat estr
    cstr buf;
}estr__context;

static char* __estr__clamp_callback(char *buf, void *user, int len)
{
    estr__context *c = (estr__context *)user;

    _s_incLen(c->s, len);
    _s_ensure(c->s, 16);

    c->avail = (int)_s_avail(c->s);

    return c->s + _s_len(c->s); // go direct into buffer if you can
}

static __always_inline i64 __estr_wrtA_P(estr*s, constr fmt, va_list ap)
{
    estr__context c;
    i64 l;

    if((c.s = *s)) _s_setLen(c.s, 0);
    else           c.s = _s_new(8);

    c.avail = (int)_s_cap(c.s);
    c.buf   = c.s;

    l =  __estr_vsprintfcb( __estr__clamp_callback, &c, c.buf, fmt, ap );

    *s = c.s;

    return l;
}

static __always_inline i64 __estr_catA_P(estr*s, constr fmt, va_list ap)
{
    estr__context c;
    i64 l;

    if((c.s = *s)) c.buf = c.s + _s_len(c.s);
    else           c.buf = c.s = _s_new(8);

    c.avail = (int)_s_avail(c.s);

    l =  __estr_vsprintfcb( __estr__clamp_callback, &c, c.buf, fmt, ap );

    *s = c.s;

    return l;
}

/* This function is similar to sdscatprintf, but much faster as it does
 * not rely on sprintf() family functions implemented by the libc that
 * are often very slow. Moreover directly handling the sds string as
 * new data is concatenated provides a performance improvement.
 *
 * However this function only handles an incompatible subset of printf-alike
 * format specifiers:
 *
 * %s - C String
 * %S - estr
 * %i - signed int
 * %I - 64 bit signed integer (long long, int64_t)
 * %u - unsigned int
 * %U - 64 bit unsigned integer (unsigned long long, uint64_t)
 * %% - Verbatim "%" character.
 */
static __always_inline i64 __estr_catA_F(estr*_s, constr fmt, va_list ap)
{
    constr f; size_t i, his_len, l; char next, *str; eval num; char buf[SDS_LLSTR_SIZE];

    estr s = *_s;

    is0_exe(s, is0_ret(s = _s_new((uint)(strlen(fmt) * 1.2f)), _ERR_ALLOC));

    f = fmt;                    /* Next format specifier byte to process. */
    i = his_len = _s_len(s);    /* Position of the next byte to write to dest str. */

    while(*f)
    {
        /* Make sure there is always space for at least 1 char. */
        _s_ensure(s, 1);

        switch(*f)
        {
            case '%': next = *(f + 1);
                      f++;

                      switch(next)
                      {
                          case 's':
                          case 'S': str = va_arg(ap, char*);

                                    if(!str) { str = "(null)"; l= 6;}
                                    else
                                        l = (next == 's') ? strlen(str) : _s_len(str);

                                    _s_ensure(s, l);
                                    memcpy(s + i, str, l);
                                    _s_incLen(s, l);
                                    i += l;
                                    break;
                          case 'i':
                          case 'I': num.i = (next == 'i') ? va_arg(ap,int)
                                                          : va_arg(ap,long long);

                                    l = ll2str(num.i, buf);
                                    _s_ensure(s, l);
                                    memcpy(s+i ,buf, l);
                                    _s_incLen(s,l);
                                    i += l;

                                    break;
                          case 'u':
                          case 'U': num.u =  (next == 'u') ? va_arg(ap,unsigned int)
                                                          : va_arg(ap,unsigned long long);

                                    l = ull2str(num.u, buf);
                                    _s_ensure(s,l);
                                    memcpy(s+i, buf, l);
                                    _s_incLen(s,l);
                                    i += l;

                                    break;

                          default : s[i++] = next;      /* Handle %% and generally %<unknown>. */
                                    _s_incLen(s, 1);
                                    break;
                      }
                      break;

            default: s[i++] = *f;
                     _s_incLen(s, 1);
                     break;
        }

        f++;
    }

    /* Add null-term */
    s[i] = '\0';

    *_s = s;

    return _s_len(s) - his_len;
}

inline int estr_setT(estr s, char    c)
{
    size_t pos;

    is0_ret(s, _ERR_HANDLE);

    pos = _s_len(s);
    if(pos)
    {
        s[pos - 1] = c;

        if(c == '\0')
            _s_decLen(s, 1);

        return 1;
    }

    return 0;
}

void estr_clear(estr s) { if(s){ s[0] = '\0';             _s_setLen(s, 0);} }
void estr_wipe (estr s) { if(s){ memset(s, 0, _s_len(s)); _s_setLen(s, 0);} }

/// =====================================================
/// estr getter
/// =====================================================
size estr_len  (estr s) { return s ? _s_len(s)        : 0; }
size estr_cap  (estr s) { return s ? _s_cap(s)        : 0; }
cstr estr_cur  (estr s) { return s ? s + _s_len(s)    : 0; }
size estr_avail(estr s) { return s ? _s_avail(s)      : 0; }
char estr_tail (estr s) { return s ? s[_s_len(s) - 1] : 0; }

/// =====================================================
/// estr utils
/// =====================================================
void estr_show(estr s) { _show("estr", s); }

int  estr_cmp (estr s, estr   s2)
{
    size_t l1, l2, minlen;
    int cmp;

    is1_ret(s == s2, 0);

    l1 = s  ? _s_len(s ) : 0;
    l2 = s2 ? _s_len(s2) : 0;
    minlen = (l1 < l2) ? l1 : l2;
    cmp = memcmp(s, s2, minlen);
    if (cmp == 0) return (int)(l1-l2);
    return cmp;
}

int  estr_cmpS(estr s, constr src )
{
    size_t l1, l2, minlen;
    int cmp;

    is1_ret(s == src, 0);

    l1 = s   ? _s_len(s)   : 0;
    l2 = src ? strlen(src) : 0;
    minlen = (l1 < l2) ? l1 : l2;
    cmp = memcmp(s, src, minlen);
    if (cmp == 0) return (int)(l1-l2);
    return cmp;
}


/**
 * Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminted C string.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = estr_new("AA...AA.a.aa.aHelloWorld     :::");
 * s = estr_strim(s, "Aa. :");
 * printf("%s\n", s);
 *
 * Output will be just "Hello World".
 */
#if 1
inline i64 estr_trim(estr s, constr cset)
{
    bool check[256] = {0};
    char* sp, * ep, * end; size_t his_len, len;

    is0_ret(s, 0);

    for(; (*cset); cset++) check[(u8)*cset] = 1;

    his_len = _s_len(s);

    sp = s;
    ep = end = s + his_len -1;
    while(sp <= end && check[(u8)*sp]) sp++;
    while(ep >  sp  && check[(u8)*ep]) ep--;

    len = (sp > ep) ? 0 : ((ep - sp) + 1);
    if (s != sp) memmove(s, sp, len);
    s[len] = '\0';
    _s_setLen(s, len);

    return his_len - len;
}
#else
inline i64 estr_trim(estr s, constr cset)
{
    char *end, *sp, *ep; size_t his_len, len;

    is0_ret(s, 0);

    his_len = _estr_len(s);

    sp = s;
    ep = end   = s + _estr_len(s) -1;
    while(sp <= end && strchr(cset, *sp)) sp++;
    while(ep >  sp  && strchr(cset, *ep)) ep--;
    len = (sp > ep) ? 0 : ((ep - sp) + 1);
    if (s != sp) memmove(s, sp, len);
    s[len] = '\0';
    _estr_setLen(s,len);

    return his_len - len;
}
#endif

inline i64 estr_lower(estr s)
{
    size_t len, i; i64 cnt = 0;

    is0_ret(s, _ERR_HANDLE);

    len = _s_len(s);
    for (i = 0; i < len; i++)
    {
        if(s[i] >= 'A' && s[i] <= 'Z')
        {
            s[i] += 0x20;
            cnt++;
        }
    }

    return cnt;
}

inline i64 estr_upper(estr s)
{
    size_t len, i;  i64 cnt = 0;

    is0_ret(s, 0);

    len = _s_len(s);
    for (i = 0; i < len; i++)
    {
        if(s[i] >= 'a' && s[i] <= 'z')
        {
            s[i] -= 0x20;
            cnt++;
        }
    }

    return cnt;
}

/**
 * Turn the string into a smaller (or equal) string containing only the
 * substring specified by the 'start' and 'end' indexes.
 *
 * start and end can be negative, where -1 means the last character of the
 * string, -2 the penultimate character, and so forth.
 *
 * The interval is inclusive, so the start and end characters will be part
 * of the resulting string.
 *
 * The string is modified in-place.
 *
 * Example:
 *
 * s = estr_new("Hello World");
 * estr_range(s,1,-1); => "ello World"
 */
inline estr estr_range(estr s, i64 start, i64 end)
{
    size_t newlen, len;

    is0_ret(s, 0);

    len = _s_len(s);
    if (len == 0) return 0;
    if (start < 0) { start = len + start; if (start < 0) start = 0;}
    if (end   < 0) {   end = len + end  ; if (end   < 0) end   = 0;}
    newlen = (start > end) ? 0 : (end - start) + 1;
    if (newlen != 0) {
        if (start >= (signed)len) {
            newlen = 0;
        } else if (end >= (signed)len) {
            end = len-1;
            newlen = (start > end) ? 0 : (end-start)+1;
        }
    } else {
        start = 0;
    }
    if (start && newlen) memmove(s, s + start, newlen);
    s[newlen] = 0;
    _s_setLen(s, newlen);

    return s;
}

/**
 * Modify the string substituting all the occurrences of the set of
 * characters specified in the 'from' string to the corresponding character
 * in the 'to' array.
 *
 * For instance: estr_mapc(mystring, "ho", "01")
 * will have the effect of turning the string "hello" into "0ell1".
 *
 * The function returns the sds string pointer, that is always the same
 * as the input pointer since no resize is needed.
 */
inline i64 estr_mapc (estr s, constr from, constr to)
{
    return estr_mapcl(s, from, to, UINT32_MAX);
}

inline i64 estr_mapcl(estr s, constr from, constr to, size_t len)
{
    size_t j, i, l, cnt;

    is0_ret(s, 0); is1_ret(!from || !to, 0);

    cnt = strlen(from); if(len > cnt) len = cnt;
    cnt = strlen(to  ); if(len > cnt) len = cnt;

    is0_ret(len, 0);

    cnt = 0;
    l   = _s_len(s);

    for (j = 0; j < l; j++) {
        for (i = 0; i < len; i++) {
            if (s[j] == from[i]) {
                s[j] = to[i];
                cnt++;
                break;
            }
        }
    }

    return cnt;
}

/**
 *  @brief estr_subc - to replace all continuous character made up
 *      from input character set by new str, but we do not expand
 *      any of them
 *
 *      cset: abc           to  : 1234
 *            ___ __        __ _ _  ___ __________
 *      src : abcdcbd       aascdasdabcsbcabbccabcdf
 *            ___ __        __ _ _  ___ ____
 *      out : 123d12d       12s1d1sd123s1234df
 */
i64 estr_subc (estr s, constr cset, constr to)
{
    int subLen, newLen, offNow; i64 cnt; u8* fd_s, * cp_s; bool tag[256] = {0};

    is0_ret(s, 0); is1_ret(!cset || !to, 0);

    newLen = strlen(cset);
    for(subLen = 0; subLen < newLen; subLen++)
        if(!tag[(u8)cset[subLen]]) tag[(u8)cset[subLen]] = 1;

    cnt    = 0;
    newLen = strlen(to);
    offNow = 0;
    fd_s   = cp_s = (u8*)s;
    while(*fd_s)
    {
        subLen = 0;

        while(*fd_s && !tag[*fd_s]) {*cp_s++ = *fd_s++;}
        while(          tag[*fd_s]) {fd_s++; subLen++; }

        if(!subLen) break;

        cnt++;

        if(newLen)
        {
            if(subLen > newLen)
            {
                offNow += subLen - newLen;
                subLen = newLen;
            }
            memcpy(cp_s, to, subLen);
            cp_s += subLen;
        }
        else
        {
            offNow += subLen;
        }
    }

    *cp_s = '\0';

    _s_decLen(s, offNow);

    return cnt;
}

/// \brief __cstr_replace_str - replace str @param from in s to str @param to
///
/// \param s       : a _s type str, we assume it is valid
/// \param end     : a cstr pointer, point to a pointer who point to the end '\0' of @param s now
/// \param last    : a cstr pointer, point to a pointer who point to the end '\0' of @param s when finish replacing, this ptr is predicted before call this func
/// \param from    : the cstr you want to be replaced, we assume it is valid
/// \param fromlen : the length of @param from, it must < tolen
/// \param to      : the cstr you want to replace to, we assume it is valid
/// \param tolen   : the length of @param to, it must > fromlen
///
/// @note:
///     1. fromlen < tolen needed, or you will not get the correct result, and you do not need this func if fromlen <= tolen
///     2. s must have enough place to hold the whole str when after
///
/// from: aaa        fromlen: 3
/// to  : abcdefg    tolen  : 7
/// s   : 11111aaa111aaa11111111_____________________
///                             |       |-- last = end + (7 - 3)*2
///                             |---------- end
///
///
static void __cstr_replace_str(cstr s, cstr* end, cstr* last, constr from, size fromlen, constr to, size tolen)
{
    cstr fd;

    if((fd = strstr(s, from)))
    {
        cstr mv_from, mv_to; size mv_len;

        __cstr_replace_str(fd + 1, end, last, from, fromlen, to, tolen);

        mv_from = fd + fromlen;
        mv_len  = *end  - fd - fromlen;
        mv_to   = *last - mv_len;

        memmove(mv_to, mv_from, mv_len);

        *last = mv_to - tolen;
        memcpy(*last, to, tolen);

        *end = fd;
    }

    return ;
}

/**
 * @brief __estr_replace_str
 * @return  >= 0, the replaced cnt
 *          _ERR_HANDLE, not have enough space, only when @param enable_realloc == false may return this value
 *          _ERR_ALLOC , alloc failed         , only when @param enable_realloc == true  may return this value
 *
 */
static i64 __estr_replace_str(estr*_s, constr from, constr to)
{
    i64 flen, tlen, offlen, offnow; cstr fd_s, cp_s, end_p; i64 cnt;

    estr s = *_s;

    is0_ret(s, _ERR_HANDLE); is1_ret(!from || !to, 0);

    flen   = strlen(from);
    tlen   = strlen(to);
    offlen = tlen - flen;

    if(offlen < 0)
    {
        offlen = -offlen;
        offnow = 0;
        fd_s   = s;
        end_p  = s + _s_len(s);

        if((fd_s = strstr(fd_s, from)))
        {
            memcpy(fd_s, to, tlen);     // replace it

            cp_s = (fd_s += flen);          // record the pos of str need copy
            offnow += offlen;               // record the off of str need copy

            while((fd_s = strstr(fd_s, from)))
            {
                memmove(cp_s - offnow, cp_s, fd_s - cp_s);   // move the str-need-copy ahead

                memcpy(fd_s - offnow, to, tlen);
                cp_s = (fd_s += flen);
                offnow += offlen;
            }

            cnt = offnow / offlen;

            memmove(cp_s - offnow, cp_s, end_p - cp_s);
            _s_decLen(s, offnow);

            *(end_p - offnow) = '\0';
        }
        else cnt = 0;
    }
    else if(offlen == 0)
    {
        cnt    = 0;
        end_p  = s + _s_len(s);

        fd_s = strstr(s, from);

        while(fd_s)
        {
            cnt++;

            memcpy(fd_s, to, tlen);
            fd_s += flen;
            fd_s =  strstr(fd_s, from);
        }
    }
    else
    {
        offnow = _s_len(s);
        end_p  = s + offnow;
        fd_s   = s;

        if((fd_s = strstr(fd_s, from)))
        {

            // -- get len need to expand
            offnow += offlen; fd_s += flen;

            while((fd_s = strstr(fd_s, from)))
            {
                offnow += offlen; fd_s += flen;
            }

            cnt = (offnow - (end_p - s)) / offlen;

            // -- have enough place, let's do it
            if((size)offnow <= _s_cap(s))
            {
                //! this operation is more efficient
                //! we set the up limit of stack call to 128
                if(cnt <= 128)
                {
                    cstr last = s + offnow;
                    __cstr_replace_str(s, &end_p, &last, from, flen, to, tlen);
                    _s_setLen(s, offnow);
                }
                else
                {
                    cstr last, lpos; i64 mlen;

                    last = s + offnow;
                    lpos = end_p - flen;

                    while(lpos >= s)
                    {
                        if(lpos[0] == from[0] && 0 == memcmp(lpos, from, flen))
                        {
                            mlen  = end_p - lpos - flen;

                            last -= mlen;
                            memmove(last, lpos + flen, mlen);

                            last -= tlen;
                            memcpy(last, to, tlen);

                            end_p = lpos;
                        }

                        lpos--;
                    }
                }

                s[offnow] = '\0';
            }
            else if(!_s_isStack(s))
            {
                cstr new_s, new_p; i64 len;

                is0_ret(new_s = _s_new(offnow), _ERR_ALLOC);  // new str

                // -- to new str
                cp_s  = fd_s = s;
                new_p = new_s;
                while((fd_s = strstr(fd_s, from)))
                {
                    memcpy(new_p, cp_s, (len = fd_s - cp_s)); new_p += len;
                    memcpy(new_p, to, tlen);                new_p += tlen;

                    cp_s = (fd_s += flen);
                }

                memcpy(new_p, cp_s, end_p - cp_s);

                efree((char*)s - _s_lenH(SDS_TYPE(s)));
                _s_setLen(new_s, offnow);

                *_s = new_s;
            }
            else
                return _ERR_HANDLE;
        }
        else cnt = 0;
    }

    return cnt;
}

/// \brief _ssub - replace str @param from in s to str @param to
///
/// \param s    : a _s type str, we assume it is valid
/// \param from : the cstr you want to be replaced, we assume it is valid
/// \param to   : the cstr you want to replace to, we assume it is valid
/// \return : NULL - if replace faild, this will be happen only when the new buf is alloc faild
///           cstr - if replace ok, but the returned s maybe not the same as s
/// @note:
///     1. new str, note: we do not free the old s if replace faild
///
i64 __estr_subs (estr*_s, constr from, constr to)
{
    return __estr_replace_str(_s, from, to);
}

void estr_cntc (estr s, u8 cnts[256], char end)
{
    memset(cnts, 0, sizeof(u8) * 256);

    if(s)
    {
        if(!end)
        {
            while(*s)
            {
                cnts[(u8)*s]++;
                s++;
            }
        }
        else
        {
            while(*s && *s != end)
            {
                cnts[(u8)*s]++;
                s++;
            }
        }
    }
}

/// =====================================================
/// estr split tools
/// =====================================================

#pragma pack(push, 1)
typedef struct __split_s{
    uint keep_adjoin : 1;
    uint trim        : 1;
    uint __          :14;
    u16  cnt;
    estr dup;
    cstr sp[];
}__split_t, * __split_p;
#pragma pack(pop)

#define _spl_sp(s)   ((__split_p)s)->sp
#define _spl_keep(s) ((__split_p)s)->keep_adjoin
#define _spl_trim(s) ((__split_p)s)->trim
#define _spl_cnt(s)  ((__split_p)s)->cnt
#define _spl_dup(s)  ((__split_p)s)->dup

#define _sp_spl(sp)  (((__split_p)sp) - 1)
#define _sp_keep(sp) _sp_spl(sp)->keep_adjoin
#define _sp_trim(sp) _sp_spl(sp)->trim
#define _sp_cnt(sp)  _sp_spl(sp)->cnt
#define _sp_dup(sp)  _sp_spl(sp)->dup

#define _ERR_SPLT_CNT 0

static int __eplit_splitbin(cstr** _sp, size len, conptr sep, size seplen)
{
    estr spl, s_dup; cstr pitr; i64 cnt, prev_pos, j, search_len, left; u8 keep_adjoin, trim;

    spl   = (estr)(_sp_spl(*_sp));
    s_dup = _spl_dup(spl);

    _s_setLen(spl, sizeof(__split_t));

    keep_adjoin = _spl_keep(spl);
    trim        = _spl_trim(spl);

    search_len = (len - (seplen - 1));
    pitr       = s_dup;
    cnt        = 0;
    left       = _s_avail(spl) / sizeof(cstr);
    if(seplen == 1)
    {
        char c   = *(cstr)sep;
        prev_pos = 0;

        if(!trim)
        {
            for (j = 0; j < search_len; j++)
            {
                if (s_dup[j] == c) {
                    if(keep_adjoin || j != prev_pos)
                    {
                        _spl_sp(spl)[cnt++] = s_dup + prev_pos;
                        _s_incLen(spl, sizeof(cstr));
                        left--;
                    }

                    if(left < 2)
                    {
                        _s_ensure(spl, sizeof(cstr) * 2); is0_ret(spl, _ERR_SPLT_CNT);
                        left = _s_avail(spl) / sizeof(cstr);
                    }

                    s_dup[j] = '\0';
                    prev_pos = j + 1;
                }
            }

            // Add the final element. We are sure there is room in the tokens array.
            _spl_sp(spl)[cnt++] = s_dup + prev_pos;
        }
        else
        {
            for (j = 0; j < search_len; j++)
            {
                if (s_dup[j] == c) {
                    if(keep_adjoin || j != prev_pos)
                    {
                        // trim prev space
                        while(isspace(s_dup[prev_pos])) prev_pos++;

                        _spl_sp(spl)[cnt++] = s_dup + prev_pos;
                        _s_incLen(spl, sizeof(cstr));
                        left--;

                        // trim after space
                        prev_pos = j - 1;
                        while(isspace(s_dup[prev_pos])) prev_pos--;
                        s_dup[prev_pos + 1] = '\0';
                    }

                    if(left < 2)
                    {
                        _s_ensure(spl, sizeof(cstr) * 2); is0_ret(spl, _ERR_SPLT_CNT);
                        left = _s_avail(spl) / sizeof(cstr);
                    }

                    s_dup[j] = '\0';
                    prev_pos = j + 1;
                }
            }

            // trim prev space
            while(isspace(s_dup[prev_pos])) prev_pos++;

            // Add the final element. We are sure there is room in the tokens array.
            _spl_sp(spl)[cnt++] = s_dup + prev_pos;

            // trim after space
            prev_pos = j - 1;
            while(isspace(s_dup[prev_pos])) prev_pos--;
            s_dup[prev_pos + 1] = '\0';
        }
    }
    else if(seplen > 1)
    {
        cstr c;

        if(!trim)
        {
            while((c = memmem(pitr, len - (pitr - s_dup), sep, seplen)))
            {
                if(keep_adjoin || pitr != c)
                {
                    _spl_sp(spl)[cnt++] = pitr;
                    _s_incLen(spl, sizeof(cstr));
                    left--;
                }

                if(left < 2)
                {
                    _s_ensure(spl, sizeof(cstr) * 2); is0_ret(spl, _ERR_SPLT_CNT);
                    left = _s_avail(spl) / sizeof(cstr);
                }

                *c   = 0;
                pitr = c + seplen;
            }

            // Add the final element. We are sure there is room in the tokens array.
            _spl_sp(spl)[cnt++] = pitr;
        }
        else
        {
            while((c = memmem(pitr, len - (pitr - s_dup), sep, seplen)))
            {
                *c   = '\0';
                if(keep_adjoin || pitr != c)
                {
                    // trim prev space
                    while(isspace(*pitr)) pitr++;

                    _spl_sp(spl)[cnt++] = pitr;
                    _s_incLen(spl, sizeof(cstr));
                    left--;

                    // trim after space
                    pitr = c - 1;
                    while(isspace(*pitr)) pitr--;
                    pitr[1] = '\0';
                }

                if(left < 2)
                {
                    _s_ensure(spl, sizeof(cstr) * 2); is0_ret(spl, _ERR_SPLT_CNT);
                    left = _s_avail(spl) / sizeof(cstr);
                }

                pitr = c + seplen;
            }

            // trim prev space
            while(isspace(*pitr)) pitr++;

            // Add the final element. We are sure there is room in the tokens array.
            _spl_sp(spl)[cnt++] = pitr;

            // trim after space
            c = s_dup + len - 1;
            while(isspace(*c)) c--;
            c[1] = '\0';
        }
    }

    _spl_cnt(spl)      = (u16)cnt;
    _spl_sp (spl)[cnt] = 0;

    *_sp = _spl_sp(spl);

    return _spl_cnt(spl);
}

esplt esplt_new(int need, bool keep, bool trim)
{
    estr o_buf;

    if(need < 4)   need = 4;
    if(need > 512) need = 512;

    o_buf = estr_newLen(0, sizeof(__split_t) + (need + 1) * sizeof(estr));

    is0_ret(o_buf, 0);

    _s_incLen(o_buf, sizeof(__split_t));

    _spl_keep(o_buf) = keep > 0;
    _spl_trim(o_buf) = trim > 0;

    return _spl_sp(o_buf);
}

void esplt_set (esplt sp, bool keep, bool trim)
{
    is0_ret(sp, );

    _sp_keep(sp) = keep > 0;
    _sp_trim(sp) = trim > 0;
}

void esplt_free(esplt sp)
{
    is0_ret(sp, );

    estr_free(_sp_spl(sp)->dup);
    estr_free((estr)_sp_spl(sp));
}

int  esplt_cnt (esplt sp)
{
    return sp ? _sp_cnt(sp) : 0;
}

int esplt_unique(esplt sp)
{
    __split_p p; cstr cur; int ok, i;

    is1_ret(!sp || !_sp_cnt(sp), 0);

    p  = _sp_spl(sp);
    ok = 0;

    for(i = 0; i < p->cnt; i++)
    {
        cur = sp[i];

        if(*cur)
        {
            int j;
            for(j = 0; j < ok; j++)
            {
                if(0 == strcmp(sp[j], cur))
                    goto check_next;
            }

            sp[ok++] = cur;
        }
check_next:
        ;
    }

    p->cnt = ok;

    return ok;
}

void  esplt_show(esplt sp, int max)
{
    __split_p _split; uint i, cnt;

    is0_exeret(sp, printf("(estr_split 0/0): nullptr\n"); fflush(stdout), );

    _split = _sp_spl(sp);

    if(max == -1) cnt = _split->cnt;
    else          cnt = max < _split->cnt ? max : _split->cnt;

    //_show("estr", _split);
    printf("(estr_split %d/%d):\n", cnt, _split->cnt); fflush(stdout);
    for(i = 0; i < cnt; i++)
    {
        printf("%2d: %s\n", i + 1, _split->sp[i]); fflush(stdout);
    }
}

static __always_inline i64  __esplt_estr_wrtB_(estr*_s, conptr ptr, size len)
{
    estr s = *_s;

    if(0 == s)
    {
        is0_ret(s = _s_new(len), _ERR_ALLOC);
    }
    else if(_s_cap(s) < len)
    {
        size_t need = len - _s_len(s);
        _s_ensure(s, need);
        is0_ret(s, _ERR_ALLOC);
    }

    memmove(s, ptr, len);
    s[len] = '\0';
    _s_setLen(s, len);

    *_s = s;

    return len;
}

#define __esplt_estr_wrtB(s, ptr, len) __esplt_estr_wrtB_(&(s), ptr, len)

int __esplt_splitE(esplt*_sp, estr   s  , constr sep)
{
    size len;

    is0_exe(*_sp, is0_ret(*_sp = esplt_new(0, 0, 0), 0));

    is0_ret(len = __esplt_estr_wrtB(_sp_dup(*_sp), s, _s_len(s)), 0);

    return __eplit_splitbin(_sp, len, sep, strlen(sep));
}

int __esplt_splitS(esplt*_sp, constr src, constr sep)
{
    size len;

    is0_exe(*_sp, is0_ret(*_sp = esplt_new(0, 0, 0), 0));
    is0_ret(len = __esplt_estr_wrtB(_sp_dup(*_sp), src, strlen(src)), 0);

    return __eplit_splitbin(_sp, len, sep, strlen(sep));
}

int __esplt_splitB(esplt*_sp, conptr ptr, int len, conptr sep, int seplen)
{
    is0_exe(*_sp, is0_ret(*_sp = esplt_new(0, 0, 0), 0));
    is0_ret(__esplt_estr_wrtB(_sp_dup(*_sp), ptr, len), 0);

    return __eplit_splitbin(_sp, len, sep, seplen);
}

static inline int __is_hex_digit(char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
static inline int __hex_digit_to_int(char c) {
    switch(c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default: return 0;
    }
}

static int  __esplit_splitArgsLine(esplt*_sp)
{
    estr spl; constr p; cstr c_arg, wr_p; int cnt; int inq, insq, done, left;

    spl   = (estr)(_sp_spl(*_sp));

    _s_setLen(spl, sizeof(__split_t));

    cnt  = 0;
    p    = _spl_dup(spl);
    left = (int)(_s_avail(spl) / sizeof(cstr));
    while(1)
    {
        /* skip blanks */
        while(*p && isspace(*p)) p++;
        if (*p) {
            c_arg = wr_p = (cstr)p;

            if(*c_arg == '\"' || *c_arg == '\'') c_arg++;

            /* get a token */
            inq  = 0;  /* set to 1 if we are in "quotes" */
            insq = 0;  /* set to 1 if we are in 'single quotes' */
            done = 0;

            while(!done)
            {
                if (inq) {
                    if (*p == '\\' && *(p+1) == 'x' && __is_hex_digit(*(p+2)) &&
                                                       __is_hex_digit(*(p+3)))
                    {
                        unsigned char byte;

                        byte = (__hex_digit_to_int(*(p+2))*16)+
                                __hex_digit_to_int(*(p+3));

                        *wr_p = byte;
                        p += 3;
                    } else if (*p == '\\' && *(p+1)) {
                        char c;

                        p++;
                        switch(*p) {
                            case 'n': c = '\n'; break;
                            case 'r': c = '\r'; break;
                            case 't': c = '\t'; break;
                            case 'b': c = '\b'; break;
                            case 'a': c = '\a'; break;
                            default : c = *p  ; break;
                        }
                        *wr_p = c;
                        //current = sdscatlen(current,&c,1);
                    } else if (*p == '"') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) goto err;
                        done=1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        goto err;
                    } else {
                        if(wr_p != p) *wr_p = *p;
                        //current = sdscatlen(current,p,1);
                    }
                } else if (insq) {
                    if (*p == '\\' && *(p+1) == '\'') {
                        p++;
                        *wr_p = '\'';
                        //current = sdscatlen(current,"'",1);
                    } else if (*p == '\'') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) goto err;
                        done=1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        goto err;
                    } else {
                        if(wr_p != p) *wr_p = *p;
                        //current = sdscatlen(current,p,1);
                    }
                } else {
                    switch(*p) {
                        case ' ' :
                        case '\n':
                        case '\r':
                        case '\t':
                        case '\0': done = 1; break;
                        case '"' : inq  = 1; break;
                        case '\'': insq = 1; break;
                        default  : break;
                    }
                }

                if (*p)
                {
                    p++;
                    if(done) *wr_p = '\0';
                    else      wr_p++;
                }
            }

            // ensure space
            if(left < 2)
            {
                _s_ensure(spl, sizeof(cstr) * 2); is0_ret(spl, _ERR_SPLT_CNT);
                left = (int)(_s_avail(spl) / sizeof(cstr));
            }

            /* add the token to the vector */
            _spl_sp(spl)[cnt++] = c_arg;
            _s_incLen(spl, sizeof(cstr));
            left--;
        }
        else
            break;
    }

    _spl_cnt(spl)      = cnt;
    _spl_sp (spl)[cnt] = 0;

    *_sp = _spl_sp(spl);

    return cnt;

err:
    return _ERR_SPLT_CNT;
}

int __espli_splitArgv(esplt*_sp, char** argv, int argc)
{
    esplt sp;

    is0_exe(*_sp, is0_ret(*_sp = esplt_new(argc, 0, 0), 0));

    sp = *_sp;
    is0_ret(__estr_joinStrs(&_sp_dup(sp), argv, argc, " "), 0);

    return __esplit_splitArgsLine(_sp);

}

int __espli_splitCmdl(esplt*_sp, constr cmdline)
{
    esplt sp;

    is0_exe(*_sp, is0_ret(*_sp = esplt_new(0, 0, 0), 0));

    sp = *_sp;
    is0_ret(__esplt_estr_wrtB(_sp_dup(sp), cmdline, strlen(cmdline)), 0);

    return __esplit_splitArgsLine(_sp);
}

i64 __estr_joinStrs(estr*s, cstr* sarr, int cnt, constr sep)
{
    int i, seplen;

    if(*s)
    {
        _s_setLen(*s, 0);
        **s = '\0';
    }

    seplen = sep ? strlen(sep) : 0;

    if(0 == seplen)
    {
        for (i = 0; i < cnt; i++)
            __estr_catS(s, sarr[i]);
    }
    else
    {
        for (i = 0; i < cnt; i++)
        {
            __estr_catS(s, sarr[i]);

            if (i != cnt-1)
                __estr_catB(s, sep, seplen);
        }
    }

    return _s_len(*s);
}

i64 __estr_joinSplt(estr*s, esplt sp, constr sep)
{
    if(!sp)
    {
        if(*s)
        {
            _s_setLen(*s, 0);
            **s = '\0';
        }

        return 0;
    }

    return __estr_joinStrs(s, sp, _sp_cnt(sp), sep);
}

/// -- Low level functions exposed to the user API ------
int __s_ensure (estr*_s, size_t addlen)
{
    if(*_s) _s_ensure(*_s, addlen);
    else    *_s = _s_new(addlen);

    return *_s ? 1 : 0;
}

/* Increment the sds length and decrements the left free space at the
 * end of the string according to 'incr'. Also set the null term
 * in the new end of the string.
 *
 * This function is used in order to fix the string length after the
 * user calls sdsMakeRoomFor(), writes something after the end of
 * the current string, and finally needs to set the new length.
 *
 * Note: it is possible to use a negative increment in order to
 * right-trim the string.
 *
 * Usage example:
 *
 * Using sdsIncrLen() and sdsMakeRoomFor() it is possible to mount the
 * following schema, to cat bytes coming from the kernel to the end of an
 * sds string without copying into an intermediate buffer:
 *
 * oldlen = estr_len(s);
 * s = estr_ensure(s, BUFFER_SIZE);
 * nread = read(fd, s+oldlen, BUFFER_SIZE);
 * ... check for nread <= 0 and handle it ...
 * estr_incrLen(s, nread);
 */
void estr_incrLen(estr s, size_t incr){if(s) _s_incrLen(s, (int)incr);}
void estr_decrLen(estr s, size_t decr){if(s) _s_decLen (s, decr);}
bool __estr_ensure(estr*s, size_t addlen)
{
    if(*s)  _s_ensure(*s, addlen);
    else    *s = _s_new(addlen);

    return !!(*s);
}

void estr_shrink (estr s)
{
    void *sh; size_t len, expect;
    int  hdrlen;

    if(!s)
        return;

    len    = _s_len(s);
    expect = (size_t)(len * 1.2) + 8;     // remain 8 byte at least

    if(_s_cap(s) <= expect)
        return ;

    if(expect > len + SDS_MAX_PREALLOC)
        expect = len + SDS_MAX_PREALLOC;

    hdrlen = _s_lenH(SDS_TYPE(s) & SDS_TYPE_MASK);
    sh     = erealloc((char*)s - hdrlen, hdrlen+len+1);

    //! it will never failed
    s = (char*)sh + hdrlen;

    _s_setCap(s, len);
}

/// =====================================================================================
/// sstr
/// =====================================================================================

/// -- sstr initializer ---------------------------------
sstr sstr_init(cptr buf, uint len)
{
    sds  s; size_t cap; char type; unsigned char *fp; /* flags pointer. */

    is0_ret(buf, 0); is0_ret(len > 4, 0);

    type    = _s_reqT(len);

    if (type == SDS_TYPE_5) type = SDS_TYPE_8;
    int hdrlen = _s_lenH(type);
    cap = len - hdrlen;

    s  = (cstr)buf + hdrlen;
    fp = ((unsigned char*)s)-1;
    switch(type) {
        case SDS_TYPE_8 : {SDS_HDR_VAR( 8,s); sh->len = 0; sh->alloc = (u8 )cap - 1; *fp = type; break; }
        case SDS_TYPE_16: {SDS_HDR_VAR(16,s); sh->len = 0; sh->alloc = (u16)cap - 1; *fp = type; break; }
        case SDS_TYPE_32: {SDS_HDR_VAR(32,s); sh->len = 0; sh->alloc = (u32)cap - 1; *fp = type; break; }
        case SDS_TYPE_64: {SDS_HDR_VAR(64,s); sh->len = 0; sh->alloc =      cap - 1; *fp = type; break; }
    }

    //memset(s, 0, cap);
    _s_setStack(s);
    s[0] = 0;

    return s;
}

/// -- sstr writer --------------------------------------

static __always_inline i64 __sstr_catA_F(sstr s, constr fmt, va_list ap);
static __always_inline i64 __sstr_catA_P(sstr s, constr fmt, va_list ap);

i64 sstr_wrtB(sstr s, conptr ptr, size    len)
{
    is0_ret(s, _ERR_HANDLE);

    if(_s_cap(s) < len)
        return _ERR_HANDLE;

    memcpy(s, ptr, len);
    s[len] = '\0';
    _s_setLen(s, len);

    return len;
}

i64 sstr_wrtC(sstr s, char   c  , size    cnt)
{
    is0_ret(s, _ERR_HANDLE);

    if(_s_cap(s) < cnt)
        return _ERR_HANDLE;

    memset(s, c, cnt);
    s[cnt] = '\0';
    _s_setLen(s, cnt);

    return cnt;
}

i64 sstr_wrtE(sstr s, sstr   s2  ) { return s2   ? sstr_wrtB(s, s2  , _s_len(s2 ))                   : 0; }
i64 sstr_wrtS(sstr s, constr src ) { return src  ? sstr_wrtB(s, src , strlen(src))                   : 0; }
i64 sstr_wrtL(sstr s, constr line) { return line ? sstr_wrtB(s, line, strchrnul(line, '\n') - line)  : 0; }
i64 sstr_wrtW(sstr s, constr w   ) { constr e; __get_word_s_e(w, e); return sstr_wrtB(s, w, e - w); }

i64 sstr_wrtT(sstr s, constr src, char    end) { return src ? sstr_wrtB(s, src, strchrnul(src, end) - src) : 0; }
i64 sstr_wrtA(sstr s, constr fmt, va_list ap ) {             is0_ret(s, _ERR_HANDLE); _s_setLen(s, 0);                    return __sstr_catA_P(s, fmt, ap); }
i64 sstr_wrtP(sstr s, constr fmt, ...        ) { va_list ap; is0_ret(s, _ERR_HANDLE); _s_setLen(s, 0); va_start(ap, fmt); return __sstr_catA_P(s, fmt, ap); }
i64 sstr_wrtF(sstr s, constr fmt, ...        ) { va_list ap; is0_ret(s, _ERR_HANDLE); _s_setLen(s, 0); va_start(ap, fmt); return __sstr_catA_F(s, fmt, ap); }

i64 sstr_catB(sstr s, conptr ptr, size   len)
{
    size_t curlen;

    is0_ret(s, 0);

    curlen = _s_len(s);

    if(_s_avail(s) < len)
        return _ERR_HANDLE;

    memcpy(s+curlen, ptr, len);
    _s_setLen(s, curlen+len);
    s[curlen+len] = '\0';

    return len;
}

i64 sstr_catC(sstr s, char   c  , size    cnt)
{
    size_t curlen;

    is0_ret(s, 0);

    curlen = _s_len(s);

    if(_s_avail(s) < cnt)
        return _ERR_HANDLE;

    memset(s+curlen, c, cnt);
    _s_setLen(s, curlen+cnt);
    s[curlen+cnt] = '\0';

    return cnt;
}

i64 sstr_catE(sstr s, sstr   s2  ) { return s2   ? sstr_catB(s, s2 ,  _s_len(s2 )                 ) : 0; }
i64 sstr_catS(sstr s, constr src ) { return src  ? sstr_catB(s, src,  strlen(src)                 ) : 0; }
i64 sstr_catL(sstr s, constr line) { return line ? sstr_catB(s, line, strchrnul(line, '\n') - line) : 0; }
i64 sstr_catW(sstr s, constr w   ) { constr e; __get_word_s_e(w, e); return sstr_catB(s, w, e - w);}

i64 sstr_catT(sstr s, constr src, char    end) { return src ? sstr_catB(s, src, strchrnul(src, end) - src) : 0; }
i64 sstr_catA(sstr s, constr fmt, va_list ap ) {             is0_ret(s, _ERR_HANDLE);                    return __sstr_catA_P(s, fmt, ap); }
i64 sstr_catP(sstr s, constr fmt, ...        ) { va_list ap; is0_ret(s, _ERR_HANDLE); va_start(ap, fmt); return __sstr_catA_P(s, fmt, ap); }
i64 sstr_catF(sstr s, constr fmt, ...        ) { va_list ap; is0_ret(s, _ERR_HANDLE); va_start(ap, fmt); return __sstr_catA_F(s, fmt, ap); }

/* This function is similar to sdscatprintf, but much faster as it does
 * not rely on sprintf() family functions implemented by the libc that
 * are often very slow. Moreover directly handling the sds string as
 * new data is concatenated provides a performance improvement.
 *
 * However this function only handles an incompatible subset of printf-alike
 * format specifiers:
 *
 * %s - C String
 * %S - estr
 * %i - signed int
 * %I - 64 bit signed integer (long long, int64_t)
 * %u - unsigned int
 * %U - 64 bit unsigned integer (unsigned long long, uint64_t)
 * %% - Verbatim "%" character.
 */
static i64 __sstr_catA_F(sstr s, constr fmt, va_list ap)
{
    constr f; size_t i, l, his_len; char next, *str; eval num; char buf[SDS_LLSTR_SIZE];

    f = fmt;                    /* Next format specifier byte to process. */
    i = his_len = _s_len(s);    /* Position of the next byte to write to dest str. */

    while(*f) {

        /* Make sure there is always space for at least 1 char. */
        if (0 == _s_avail(s))
            goto err_ret;

        switch(*f)
        {
            case '%': next = *(f+1);
                      f++;

                      switch(next)
                      {
                          case 's':
                          case 'S': str = va_arg(ap, char*);

                                    if(!str) { str = "(null)"; l= 6;}
                                    else
                                        l = (next == 's') ? strlen(str) : _s_len(str);

                                    if (_s_avail(s) < l)
                                        goto err_ret;

                                    memcpy(s + i, str, l);
                                    _s_incLen(s, l);
                                    i += l;
                                    break;
                          case 'i':
                          case 'I': num.i = (next == 'i') ? va_arg(ap,int)
                                                          : va_arg(ap,long long);

                                    l = ll2str(num.i, buf);
                                    if (_s_avail(s) < l)
                                        goto err_ret;

                                    memcpy(s+i,buf,l);
                                    _s_incLen(s,l);
                                    i += l;

                                    break;
                          case 'u':
                          case 'U': num.u = (next == 'u') ? va_arg(ap,int)
                                                          : va_arg(ap,long long);

                                    l = ull2str(num.u, buf);
                                    if (_s_avail(s) < l)
                                        goto err_ret;

                                    memcpy(s+i,buf,l);
                                    _s_incLen(s,l);
                                    i += l;

                                    break;

                      default : s[i++] = next;      /* Handle %% and generally %<unknown>. */
                                _s_incLen(s, 1);
                                break;
                 }
                 break;

        default: s[i++] = *f;
                 _s_incLen(s, 1);
                 break;
        }

        f++;
    }

    /* Add null-term */
    s[i] = '\0';

    return _s_len(s) - his_len;

err_ret:
    return _ERR_HANDLE;
}

typedef struct sstr__context {
    int  avail;      // set in the first place so we can get it directly
    estr s;         // handle for compat estr
}sstr__context;

static char* __sstr_stbsp__clamp_callback(char *buf, void *user, int len)
{
   sstr__context *c = (sstr__context *)user;

   E_UNUSED(buf);

   _s_incLen(c->s, len);

   return _s_avail(c->s) ? c->s + _s_len(c->s) : 0;
}

static inline i64 __sstr_catA_P(sstr s, constr fmt, va_list ap )
{
    sstr__context c;

    c.s     = s + _s_len(s);
    c.avail = (int)_s_avail(s);

    return  __estr_vsprintfcb ( __sstr_stbsp__clamp_callback, &c, c.s, fmt, ap );
}

/// -- sstr utils ---------------------------------------

void sstr_decrLen(sstr s, size_t decr)
{
    if(s) _s_decLen(s, decr);
}
