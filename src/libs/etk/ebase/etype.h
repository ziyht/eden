/// =====================================================================================
///
///       Filename:  etype.h
///
///    Description:  types for etools
///
///        Version:  1.1
///        Created:  03/09/2017 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __ETYPE_H__
#define __ETYPE_H__

#define ETYPE_VERSION "etype 1.1.0"

#include <stdbool.h>

#include "ealloc.h"

#ifndef __DEF_PTR__
#define __DEF_PTR__
typedef const char* constr;
typedef const void* conptr;
typedef char* cstr;
typedef void* cptr;
#endif

#ifndef __DEF_INT__
#define __DEF_INT__

#ifndef __APPLE__
#define __signed
#endif

#if defined(_MSC_VER)

#if  _MSC_VER < 1600
typedef __signed       char int8_t;
typedef unsigned       char uint8_t;
typedef __signed short int  int16_t;
typedef unsigned short int  uint16_t;
typedef __signed       int  int32_t;
typedef unsigned       int  uint32_t;
typedef __signed long  long int64_t;
typedef unsigned long  long uint64_t;
#endif

#else
#include <stdint.h>
#endif

#include <limits.h>
#include <inttypes.h>

typedef   int8_t i8;
typedef  uint8_t u8;
typedef  int16_t i16;
typedef uint16_t u16;
typedef  int32_t i32;
typedef uint32_t u32;
typedef  int64_t i64;
typedef uint64_t u64;

#ifndef _ASM_GENERIC_INT_LL64_H
#define _ASM_GENERIC_INT_LL64_H
typedef   int8_t __i8;
typedef  uint8_t __u8;
typedef  int16_t __i16;
typedef uint16_t __u16;
typedef  int32_t __i32;
typedef uint32_t __u32;
typedef  int64_t __i64;
typedef uint64_t __u64;
#endif

typedef const unsigned char conu8;
typedef const __signed char coni8;

typedef float  f32, __f32;
typedef double f64, __f64;

typedef i64 ild;
typedef u64 uld;

#if defined(_MSC_VER) || defined(__APPLE__)

typedef uint16_t ushort;
typedef uint32_t uint;

#undef  PRId64
#undef  PRIu64
#define PRId64 "lld"
#define PRIu64 "llu"

#else

#undef  PRId64
#undef  PRIu64
#define PRId64 "ld"
#define PRIu64 "lu"

#endif

#endif

#ifndef __DEF_TYPE__
#define __DEF_TYPE__

#include <sys/types.h>

typedef size_t size;

#endif

#pragma pack(1)

typedef union eval_s{
    __i8      i8,  i8a[8], *i8p;
    __u8      u8,  u8a[8], *u8p;
    __i16    i16, i16a[4], *i16p;
    __u16    u16, u16a[4], *u16p;
    __i32    i32, i32a[2], *i32p;
    __u32    u32, u32a[2], *u32p;
    __i64    i64, i64a[1], *i64p, i;
    __u64    u64, u64a[1], *u64p, u;
    __f32    f32, f32a[2], *f32p;
    __f64    f64, f64a[1], *f64p, f;

    cptr     p, pa[1], * pp;
    cstr     s, sa[1], * sp;
    char     c, ca[8], * cp;        // same as i8
    char        r [8], * rp;        // same as i8, but define a new type to handle raw data

    conptr   C_p, C_pa[1], * C_pp;  // const ver
    constr   C_s, C_sa[1], * C_sp;  // const ver
}eval_t, eval, * evalp;

#pragma pack()

#define EVAL_MK(_v, v)   (eval_t){._v = v}

#define EVAL_ZORE       EVAL_MK(p, 0)
#define EVAL_0          EVAL_ZORE

#define EVAL_I8( v)     EVAL_MK(i8 , v)
#define EVAL_I16(v)     EVAL_MK(i16, v)
#define EVAL_I32(v)     EVAL_MK(i32, v)
#define EVAL_I64(v)     EVAL_MK(i64, v)

#define EVAL_U8( v)     EVAL_MK(u8 , v)
#define EVAL_U16(v)     EVAL_MK(u16, v)
#define EVAL_U32(v)     EVAL_MK(u32, v)
#define EVAL_U64(v)     EVAL_MK(u64, v)

#define EVAL_F32(v)     EVAL_MK(f32, v)
#define EVAL_F64(v)     EVAL_MK(f64, v)

#define EVAL_S(  v)     EVAL_MK(s, v)
#define EVAL_P(  v)     EVAL_MK(p, v)

#define EVAL_CS( v)     EVAL_MK(C_s, v)
#define EVAL_CP( v)     EVAL_MK(C_p, v)

#endif
