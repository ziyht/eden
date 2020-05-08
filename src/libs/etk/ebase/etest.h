/// =====================================================================================
///
///       Filename:  etest.h
///
///    Description:  a test helper for etools
///
///        Version:  1.0
///        Created:  2018.07.07 04:03:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __E_TEST_H__
#define __E_TEST_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ETEST_VERSION "etest 1.1.0"      // adjust implementations

#ifndef ETEST_OK
#define ETEST_ERR 1
#define ETEST_OK  0
#endif

#define ETEST_RUN( func)        __ETEST_EXEC(func, __FILE__, __LINE__)

#define eexpect_1(  cond)       __etest_expect(cond, __FILE__, __LINE__, true)
#define eexpect_0(  cond)       __etest_expect(cond, __FILE__, __LINE__, false)

#define eexpect_eq(a, b)        __etest_expect_num(a, b, __FILE__, __LINE__, 1, __FUNCTION__)   // equal
#define eexpect_ne(a, b)        __etest_expect_num(a, b, __FILE__, __LINE__, 0, __FUNCTION__)   // not equal
#define eexpect_gt(a, b)        __etest_expect_num(a, b, __FILE__, __LINE__, 2, __FUNCTION__)   // greater than
#define eexpect_ge(a, b)        __etest_expect_num(a, b, __FILE__, __LINE__, 3, __FUNCTION__)   // greater than or equal
#define eexpect_lt(a, b)        __etest_expect_num(a, b, __FILE__, __LINE__, 4, __FUNCTION__)   // less than
#define eexpect_le(a, b)        __etest_expect_num(a, b, __FILE__, __LINE__, 5, __FUNCTION__)   // less than or equal

#define eexpect_num(a, b)       __etest_expect_num(a, b, __FILE__, __LINE__, 1, __FUNCTION__)
#define eexpect_ptr(a, b)       __etest_expect_ptr(a, b, __FILE__, __LINE__, 1)
#define eexpect_str(a, b)       __etest_expect_str(a, b, __FILE__, __LINE__, 1)
#define eexpect_raw(a, b, l)    __etest_expect_raw(a, b, __FILE__, __LINE__, l, 1)

#define eunexpc_num(a, b)       __etest_expect_num(a, b, __FILE__, __LINE__, 0, __FUNCTION__)
#define eunexpc_ptr(a, b)       __etest_expect_ptr(a, b, __FILE__, __LINE__, 0)
#define eunexpc_str(a, b)       __etest_expect_str(a, b, __FILE__, __LINE__, 0)
#define eunexpc_raw(a, b, l)    __etest_expect_raw(a, b, __FILE__, __LINE__, l, 0)

//! -----------------------------------------------------------
//! etest definition
//!

#define __etest_expect(cond, f, l, t)                                   \
do{                                                                     \
    if(ETEST_ERR == __etest_expect_bool((bool)(cond), #cond, f, l, t))  \
    {                                                                   \
        return ETEST_ERR;                                               \
    }                                                                   \
}while(0)

#define __etest_expect_num(a, b, f, l, eq, F)                       \
do{                                                                 \
    if(ETEST_ERR == __etest_cmp_num                                 \
        ((double)(a), (double)(b),                                  \
            #a, #b, f, l, eq))                                      \
        return ETEST_ERR;                                           \
}while(0)

#define __etest_expect_ptr(a, b, f, l, eq)                          \
do{                                                                 \
    if(ETEST_ERR == __etest_cmp_ptr(a, b, #a, #b, f, l, eq))        \
        return ETEST_ERR;                                           \
}while(0)

#define __etest_expect_str(a, b, f, l, eq)                          \
do{                                                                 \
    if(ETEST_ERR == __etest_cmp_str(a, b, #a, #b, f, l, eq))        \
        return ETEST_ERR;                                           \
}while(0)

#define __etest_expect_raw(a, b, f, l, len, eq)                     \
do{                                                                 \
    if(ETEST_ERR == __etest_cmp_raw(a, b, #a, #b, len, f, l, eq))   \
        return ETEST_ERR;                                           \
}while(0)

#define __ETEST_CALL(func, f, l)                                    \
do{                                                                 \
    if((func) != ETEST_OK)                                          \
    {                                                               \
        printf(__ETEST_ERR_BT_FMT, f, l, #func);                    \
        fflush(stdout);                                             \
        return ETEST_ERR;                                           \
    }                                                               \
}while(0)

static int __etest_exec_arg;

#define __ETEST_EXEC(func, f, l)                                    \
do{                                                                 \
    __etest_exec_arg++;                                             \
    __ETEST_CALL(func, f, l);                                       \
    __etest_exec_arg--;                                             \
                                                                    \
    if(__etest_exec_arg == 0)                                       \
    {                                                               \
        printf("  PASSED %s\n", #func);                             \
        fflush(stdout);                                             \
    }                                                               \
}while(0)



#define __ETEST_ERR_HINT_LINE     "===========FAILED============\n"
#define __ETEST_ERR_BT_FMT        "  %s(%d): %s\n"

static int __etest_expect_bool(bool cond, const char* tag, const char* file, int line, bool _true)
{
    int passed;

    passed = cond == _true;

    if(passed)
        return ETEST_OK;

    printf(__ETEST_ERR_HINT_LINE
        "etest expect %s:\n"
        "    %s: %s\n"
        __ETEST_ERR_BT_FMT,
            _true ? "true" : "false",
            tag, cond ? "true" : "false", file, line, "");
    fflush(stdout);

    return ETEST_ERR;
}

static int __etest_cmp_num(double fa, double fb, const char* taga, const char* tagb, const char* file, int line, int cmp_type)
{
    int passed;

    enum {
        __ETEST_NUM_EXPECT_NE = 0,
        __ETEST_NUM_EXPECT_EQ,
        __ETEST_NUM_EXPECT_GT,
        __ETEST_NUM_EXPECT_GE,
        __ETEST_NUM_EXPECT_LT,
        __ETEST_NUM_EXPECT_LE,
    };

    static const char* __tags[] =
    {
        "not_equal",
        "equal",
        "greater_than",
        "greater_than_or_equal",
        "less_than",
        "less_than_or_equal",
    };

    switch (cmp_type) {
        case __ETEST_NUM_EXPECT_NE: passed = (fa != fb); break;
        case __ETEST_NUM_EXPECT_EQ: passed = (fa == fb); break;
        case __ETEST_NUM_EXPECT_GT: passed = (fa >  fb); break;
        case __ETEST_NUM_EXPECT_GE: passed = (fa >= fb); break;
        case __ETEST_NUM_EXPECT_LT: passed = (fa <  fb); break;
        case __ETEST_NUM_EXPECT_LE: passed = (fa <= fb); break;
        default                   : passed =          0; break;
    }

    if(passed)
        return ETEST_OK;

    if(fa - (long long)fa != 0 || fb - (long long)fb != 0)
    {
        printf(__ETEST_ERR_HINT_LINE
            "etest expect num %s:\n"
            "    %s: %f\n"
            "    %s: %f\n"
            __ETEST_ERR_BT_FMT,
                __tags[cmp_type],
                taga, fa, tagb, fb, file, line, "");
    }
    else
    {
        printf(__ETEST_ERR_HINT_LINE
            "etest expect num %s:\n"
            "    %s: %lld\n"
            "    %s: %lld\n"
            __ETEST_ERR_BT_FMT,
               __tags[cmp_type],
               taga, (long long)fa, tagb, (long long)fb, file, line, "");
    }

    fflush(stdout);

    return ETEST_ERR;
}

static int __etest_cmp_ptr(const void* pa, const void* pb, char* taga, char* tagb, char* file, int line, int equal)
{
    int passed;

    passed = pa == pb ? equal ? 1 : 0
                      : equal ? 0 : 1 ;

    if(passed)
        return ETEST_OK;

    printf(__ETEST_ERR_HINT_LINE
        "etest expect ptr %s:\n"
        "    %s: %p\n"
        "    %s: %p\n"
        __ETEST_ERR_BT_FMT,
            equal ? "equal" : "unequal",
            taga, pa, tagb, pb, file, line, "");
    fflush(stdout);

    return ETEST_ERR;
}

static int __etest_cmp_str(const char* sa, const char* sb, char* taga, char* tagb, char* file, int line, int equal)
{
    int passed;

    if(!sa || !sb)
    {
        passed = sa == sb;
    }
    else
        passed = (0 == strcmp(sa, sb)) ? equal ? 1 : 0
                                       : equal ? 0 : 1 ;

    if(passed)
        return ETEST_OK;

    printf(__ETEST_ERR_HINT_LINE
        "etest expect str %s:\n"
        "    %s: %s\n"
        "    %s: %s\n"
        __ETEST_ERR_BT_FMT,
            equal ? "equal" : "unequal",
            taga, sa, tagb, sb, file, line, "");
    fflush(stdout);

    return ETEST_ERR;
}

static int __etest_cmp_raw(const void* ra, const void* rb, char* taga, char* tagb, long long len, char* file, int line, int equal)
{
    int passed;

    if(!ra || !rb)
    {
        passed = ra == rb;
    }
    else
        passed = (0 == memcmp(ra, rb, len)) ? equal ? 1 : 0
                                            : equal ? 0 : 1 ;

    if(passed)
        return ETEST_OK;

    printf(__ETEST_ERR_HINT_LINE
        "etest expect ptr %s:\n"
        "    %s: %s\n"
        "    %s: %s\n"
        __ETEST_ERR_BT_FMT,
            equal ? "equal" : "unequal",
            taga, (char*)ra, tagb, (char*)rb, file, line, "");
    fflush(stdout);

    return ETEST_ERR;
}

#endif
