/// =====================================================================================
///
///       Filename:  estr_p.h
///
///    Description:
///
///        Version:  1.0
///        Created:  02/25/2017 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================


#ifndef __ESTR_P_H__
#define __ESTR_P_H__

#include <string.h>
#include <assert.h>

#include "etype.h"
#include "eutils.h"

#pragma pack(1)
typedef struct sdstype_s{
    uint type : 3;
    uint stack: 1;
    uint split: 1;
    uint __   : 3;
}sdstype_t, sdstype;        // not used now

struct sdshdr5 {
    u8   flags;  /* 3 lsb of type, and 5 msb of string length */
    char buf[];
};
struct sdshdr8 {
    u8   len;    /* used */
    u8   alloc;  /* excluding the header and null terminator */
    u8   flags;  /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct sdshdr16 {
    u16  len;   /* used */
    u16  alloc; /* excluding the header and null terminator */
    u8   flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct sdshdr32 {
    u32  len;   /* used */
    u32  alloc; /* excluding the header and null terminator */
    u8   flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct sdshdr64 {
    u64  len;   /* used */
    u64  alloc; /* excluding the header and null terminator */
    u8   flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
#pragma pack()

typedef char *sds;

#define SDS_TYPE_5          3
#define SDS_TYPE_8          1
#define SDS_TYPE_16         2
#define SDS_TYPE_32         0
#define SDS_TYPE_64         4
#define SDS_TYPE_BITS       3
#define SDS_TYPE_MASK       7    // 0000 0111
#define SDS_STACK_MASK      8    // 0000 1000
#define SDS_SPLIT_MASK      16   // 0001 0000
#define SDS_LLSTR_SIZE      21
#define SDS_MAX_PREALLOC    (1024*1024)

#define SDS_TYPE(s)       (s)[-1]
#define SDS_HDR_VAR(T,s)  struct sdshdr##T *sh = (void*)((s)-(sizeof(struct sdshdr##T)));
#define SDS_HDR(T,s)      ((struct sdshdr##T *)((s)-(sizeof(struct sdshdr##T))))
#define SDS_TYPE_5_LEN(f) ((f)>>SDS_TYPE_BITS)

/** -----------------------------------------------------
 *
 *  macro APIs
 *
 *     using those is ok
 *
 * ------------------------------------------------------
 */
#define _s_new(l)           __sdsNewRoom(l)
#define _s_free(s)          efree(s - _s_lenH(SDS_TYPE(s)))
#define _s_reqT(s)          __sdsReqType(s)
#define _s_lenH(t)          __sdsHdrSize(t)
#define _s_len(s)           __sdslen(s)
#define _s_avail(s)         __sdsavail(s)
#define _s_cap(s)           __sdsalloc(s)
#define _s_setLen(s,l)      __sdssetlen(s,l)
#define _s_setCap(s,l)      __sdssetalloc(s,l)
#define _s_incLen(s,l)      __sdsinclen(s,l)        // for inner using
#define _s_decLen(s,l)      __sdsdeclen(s,l)
#define _s_incrLen(s,l)     __sdsincrlen(s,l)       // for Low level functions using, more safe than __sdsinclen()
#define _s_ensure(s,l)      do{ if(_s_avail(s) < (l)) s = __sdsMakeRoomFor(s, (l)); } while(0)
#define _s_setStack(s)      (SDS_TYPE(s) |= SDS_STACK_MASK)
#define _s_isStack(s)       ((SDS_TYPE(s) & SDS_STACK_MASK) && (SDS_TYPE_5 != (SDS_TYPE(s)&SDS_TYPE_MASK)))

/** -----------------------------------------------------
 *
 *  definitions
 *
 * ------------------------------------------------------
 */
static __always_inline size_t __sdslen(const sds s) {
    unsigned char flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5 : return (size_t)SDS_TYPE_5_LEN(flags);
        case SDS_TYPE_8 : return (size_t)SDS_HDR( 8,s)->len;
        case SDS_TYPE_16: return (size_t)SDS_HDR(16,s)->len;
        case SDS_TYPE_32: return (size_t)SDS_HDR(32,s)->len;
        case SDS_TYPE_64: return (size_t)SDS_HDR(64,s)->len;
    }
    return 0;
}

static __always_inline size_t __sdsavail(const sds s) {
    unsigned char flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5 : {                   return 0;                            }
        case SDS_TYPE_8 : { SDS_HDR_VAR( 8,s);return (size_t)(sh->alloc - sh->len);}
        case SDS_TYPE_16: { SDS_HDR_VAR(16,s);return (size_t)(sh->alloc - sh->len);}
        case SDS_TYPE_32: { SDS_HDR_VAR(32,s);return (size_t)(sh->alloc - sh->len);}
        case SDS_TYPE_64: { SDS_HDR_VAR(64,s);return (size_t)(sh->alloc - sh->len);}
    }
    return 0;
}

static __always_inline void __sdssetlen(sds s, size_t newlen) {
    unsigned char flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5 : {unsigned char *fp = ((unsigned char*)s)-1;*fp = SDS_TYPE_5 | (u8)(newlen << SDS_TYPE_BITS);}break;
        case SDS_TYPE_8 : SDS_HDR( 8,s)->len = (u8) newlen; break;
        case SDS_TYPE_16: SDS_HDR(16,s)->len = (u16)newlen; break;
        case SDS_TYPE_32: SDS_HDR(32,s)->len = (u32)newlen; break;
        case SDS_TYPE_64: SDS_HDR(64,s)->len =      newlen; break;
    }
}

static __always_inline void __sdsinclen(sds s, size_t inc) {
    unsigned char flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5:{unsigned char *fp = ((unsigned char*)s)-1;unsigned char newlen = (u8)(SDS_TYPE_5_LEN(flags)+inc);*fp = SDS_TYPE_5 | (u8)(newlen << SDS_TYPE_BITS);}break;
        case SDS_TYPE_8 : SDS_HDR( 8,s)->len += (u8) inc;break;
        case SDS_TYPE_16: SDS_HDR(16,s)->len += (u16)inc;break;
        case SDS_TYPE_32: SDS_HDR(32,s)->len += (u32)inc;break;
        case SDS_TYPE_64: SDS_HDR(64,s)->len +=      inc;break;
    }
}

static __always_inline void __sdsdeclen(sds s, size_t dec) {
    unsigned char flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5:{unsigned char *fp = ((unsigned char*)s)-1;unsigned char newlen = (u8)(SDS_TYPE_5_LEN(flags)-dec);*fp = SDS_TYPE_5 | (u8)(newlen << SDS_TYPE_BITS);}break;
        case SDS_TYPE_8 : SDS_HDR( 8,s)->len -= (u8) dec;break;
        case SDS_TYPE_16: SDS_HDR(16,s)->len -= (u16)dec;break;
        case SDS_TYPE_32: SDS_HDR(32,s)->len -= (u32)dec;break;
        case SDS_TYPE_64: SDS_HDR(64,s)->len -=      dec;break;
    }
}

static __always_inline void __sdsincrlen(sds s, int incr) {
    unsigned char flags = SDS_TYPE(s);
    i64 len;
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5: {
            unsigned char *fp    = ((unsigned char*)s)-1;
            unsigned char oldlen = SDS_TYPE_5_LEN(flags);
            assert((incr > 0 && oldlen + incr < 32) || (incr < 0 && oldlen >= (unsigned int)(-incr)));
            *fp = SDS_TYPE_5 | ((oldlen+incr) << SDS_TYPE_BITS);
            len = oldlen+incr;
            break;
        }
        case SDS_TYPE_8: {
            SDS_HDR_VAR(8,s);
            assert((incr >= 0 && sh->alloc - sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
            len = (sh->len += incr);
            break;
        }
        case SDS_TYPE_16: {
            SDS_HDR_VAR(16,s);
            assert((incr >= 0 && sh->alloc - sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
            len = (sh->len += incr);
            break;
        }
        case SDS_TYPE_32: {
            SDS_HDR_VAR(32,s);
            assert((incr >= 0 && sh->alloc - sh->len >= (unsigned int)incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
            len = (sh->len += incr);
            break;
        }
        case SDS_TYPE_64: {
            SDS_HDR_VAR(64,s);
            assert((incr >= 0 && sh->alloc - sh->len >= (uint64_t)incr) || (incr < 0 && sh->len >= (uint64_t)(-incr)));
            len = (sh->len += incr);
            break;
        }
        default: len = 0; /* Just to avoid compilation warnings. */
    }
    s[len] = '\0';
}

/* sdsalloc() = sdsavail() + sdslen() */
static __always_inline size_t __sdsalloc(const sds s) {
    unsigned char flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5 : return (size_t)SDS_TYPE_5_LEN(flags);
        case SDS_TYPE_8 : return (size_t)SDS_HDR(8 ,s)->alloc;
        case SDS_TYPE_16: return (size_t)SDS_HDR(16,s)->alloc;
        case SDS_TYPE_32: return (size_t)SDS_HDR(32,s)->alloc;
        case SDS_TYPE_64: return (size_t)SDS_HDR(64,s)->alloc;
    }
    return 0;
}

static __always_inline void __sdssetalloc(sds s, size_t newlen) {
    unsigned char flags = SDS_TYPE(s);
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5 : break; /* Nothing to do, this type has no total allocation info. */
        case SDS_TYPE_8 : SDS_HDR( 8,s)->alloc = (u8) newlen;break;
        case SDS_TYPE_16: SDS_HDR(16,s)->alloc = (u16)newlen;break;
        case SDS_TYPE_32: SDS_HDR(32,s)->alloc = (u32)newlen;break;
        case SDS_TYPE_64: SDS_HDR(64,s)->alloc =      newlen;break;
    }
}

static __always_inline int __sdsHdrSize(char type) {
    switch(type&SDS_TYPE_MASK) {
        case SDS_TYPE_5 : return sizeof(struct sdshdr5 );
        case SDS_TYPE_8 : return sizeof(struct sdshdr8 );
        case SDS_TYPE_16: return sizeof(struct sdshdr16);
        case SDS_TYPE_32: return sizeof(struct sdshdr32);
        case SDS_TYPE_64: return sizeof(struct sdshdr64);
    }
    return 0;
}

static __always_inline char __sdsReqType(size_t string_size) {
    if (string_size < 1<<5)    return SDS_TYPE_5;
    if (string_size < 1<<8)    return SDS_TYPE_8;
    if (string_size < 1<<16)   return SDS_TYPE_16;
#if (LONG_MAX == LLONG_MAX)
    if (string_size < 1ll<<32) return SDS_TYPE_32;
#endif
    return SDS_TYPE_64;
}



static sds __sdsNewRoom(size_t len)
{
    cstr sh; sds s; char type; int hdrlen;

    if(len < 1024) len = (size_t)(len * 1.2);

    type = __sdsReqType(len);

    /* Don't use type 5: the user is appending to the string and type 5 is
     * not able to remember empty space, so sdsMakeRoomFor() must be called
     * at every appending operation. */

    if (type == SDS_TYPE_5) type = SDS_TYPE_8;
    hdrlen = __sdsHdrSize(type);

    is0_ret(sh = ecalloc(1, hdrlen+len+1), 0);

    s = sh + hdrlen;
    SDS_TYPE(s) = type;
    __sdssetalloc(s, len);

    return s;
}

static sds __sdsMakeRoomFor(sds s, size_t addlen) {
    char *sh, *newsh; size_t len, newlen; char type, oldtype; int hdrlen;

    oldtype = SDS_TYPE(s) & SDS_TYPE_MASK;
    len     = __sdslen(s);
    sh      = (char*)s - __sdsHdrSize(oldtype);
    newlen  = (len+addlen);

    if (newlen < SDS_MAX_PREALLOC) newlen *= 2;
    else                           newlen += SDS_MAX_PREALLOC;

    type = __sdsReqType(newlen);

    /* Don't use type 5: the user is appending to the string and type 5 is
     * not able to remember empty space, so sdsMakeRoomFor() must be called
     * at every appending operation. */
    if (type == SDS_TYPE_5) type = SDS_TYPE_8;

    hdrlen = __sdsHdrSize(type);
    if (oldtype == type) {
        newsh = erealloc(sh, hdrlen+newlen+1);
        if (newsh == NULL) return NULL;
        s = newsh+hdrlen;
    } else {
        /* Since the header size changes, need to move the string forward,
         * and can't use realloc */
        newsh = emalloc(hdrlen+newlen+1);
        if (newsh == NULL) return NULL;
        memcpy(newsh+hdrlen, s, len+1);

        efree(sh);
        s = newsh + hdrlen;
        SDS_TYPE(s) = type;
        __sdssetlen(s, len);
    }
    __sdssetalloc(s, newlen);

    return s;
}

#endif
