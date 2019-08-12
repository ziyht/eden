/// =====================================================================================
///
///       Filename:  eutils.c
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

#define EUTILS_VERSION     "eutils 1.0.5"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "eutils.h"

int ll2str(i64 value, cstr s)
{
    char *p, aux;
    unsigned long long v;
    int l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    v = (value < 0) ? -value : value;
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);
    if (value < 0) *p++ = '-';

    /* Compute length and add null term. */
    l = (int)(p - s);
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

int ull2str(u64 v, cstr s)
{
    char *p, aux;
    int l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);

    /* Compute length and add null term. */
    l = (int)(p - s);
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

int e_strcasecmp(const char *s1, const char *s2)
{
    const unsigned char
    *us1 = (const unsigned char *)s1,
    *us2 = (const unsigned char *)s2;

    while (tolower(*us1) == tolower(*us2++))
        if (*us1++ == '\0')
            return (0);

    return (tolower(*us1) - tolower(*--us2));
}

int e_strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n != 0)
    {
        const unsigned char
        *us1 = (const unsigned char *)s1,
        *us2 = (const unsigned char *)s2;

        do {
            if (tolower(*us1) != tolower(*us2++))
                    return (tolower(*us1) - tolower(*--us2));
            if (*us1++ == '\0')
                    break;
        } while (--n != 0);
    }
    return (0);
}

/** ------------------------------------------------------
 *
 *  rand tools
*/
static long _rstate;

int e_rand()
{
    _rstate = _rstate * 214013L + 2531011L;
    return (_rstate >> 16) & 0x7fff;
}

void e_srand(int seed)
{
    _rstate = (long)seed;
}

#define R48_MULTIPLICAND 0x5deece66dULL
#define R48_ADDEND       11
#define R48_INITIAL_VAL  0x1234abcd330eULL
#define R48_MASK         0xffffffffffffULL


static u16 r48state[3]
    = { R48_INITIAL_VAL        & 0xffff,
       (R48_INITIAL_VAL >> 16) & 0xffff,
       (R48_INITIAL_VAL >> 32) & 0xffff };

static u16 r48multiplicand[3]
    = { R48_MULTIPLICAND        & 0xffff,
       (R48_MULTIPLICAND >> 16) & 0xffff,
       (R48_MULTIPLICAND >> 32) & 0xffff };

static u16 r48addend = R48_ADDEND;

/* Get a 48-bit number as a long integer */
static __always_inline long long _r48_getll(unsigned short s[3]) {
    return (long long)s[0] | ((long long)s[1] << 16) | ((long long)s[2] << 32);
}

static void _r48_iterate(unsigned short state[3]) {
    unsigned long long hi, lo, multiplicand, result;
    lo = state[0] | ((unsigned long)state[1] << 16);
    hi = state[2];
    multiplicand = _r48_getll(r48multiplicand);
    result = (((lo * multiplicand) & R48_MASK) +
             (((hi * (multiplicand & 0xffff)) << 32) & R48_MASK) + r48addend) & R48_MASK;
    state[0] = result & 0xffffUL;
    state[1] = (result >> 16) & 0xffffUL;
    state[2] = (result >> 32) & 0xffffUL;
}

long e_lrand48()
{
    long r;
    _r48_iterate(r48state);
    r = r48state[2];
    r = (r << 15) | (r48state[1] >> 1);
    return r;
}

long e_mrand48()
{
    int r;
    _r48_iterate(r48state);
    r = r48state[2];
    r = (r << 16) | r48state[1];
    return (long)r;
}

f64 e_drand48()
{
    _r48_iterate(r48state);
    return _r48_getll(r48state) * (1.0 / (1ll << 48));  /* (0x1.0p-48 = 1.0 / 2^48) */
}

long e_jrand48(u16 xsubi[3])
{
    int r;
    _r48_iterate(xsubi);
    r = xsubi[2];
    r = (r << 16) | xsubi[1];
    return (long)r;
}

long e_nrand48(u16 xsubi[3])
{
    long r;
    _r48_iterate(xsubi);
    r = xsubi[2];
    r = (r << 15) | (xsubi[1] >> 1);
    return r;
}

f64 e_erand48(u16 xsubi[3])
{
    _r48_iterate(xsubi);
    return _r48_getll(xsubi) * (1.0 / (1ll << 48));  /* (0x1.0p-48 = 1.0 / 2**48) */
}

void e_srand48(long seedval)
{
    r48state[0] = R48_INITIAL_VAL & 0xffff;   /* Arbitrarily set to 0x330e */
    r48state[1] = seedval & 0xffff;
    r48state[2] = (seedval >> 16) & 0xffff;
}

u16 *e_seed48(u16 seed16v[3])
{
    static unsigned short old_seed[3];
    /* Save old seed */
    old_seed[0] = r48state[0];
    old_seed[1] = r48state[1];
    old_seed[2] = r48state[2];
    /* Update new seed value */
    r48state[0] = seed16v[0];
    r48state[1] = seed16v[1];
    r48state[2] = seed16v[2];
    /* Reset multiplicand */
    r48multiplicand[0] =  R48_MULTIPLICAND        & 0xffff;
    r48multiplicand[1] = (R48_MULTIPLICAND >> 16) & 0xffff;
    r48multiplicand[2] = (R48_MULTIPLICAND >> 32) & 0xffff;
    /* Reset addend */
    r48addend = R48_ADDEND;
    return old_seed;
}

void e_lcong48(u16 param[7])
{
    /* First three shorts == seed */
    r48state[0] = param[0];
    r48state[1] = param[1];
    r48state[2] = param[2];
    /* Second three == multiplicand */
    r48multiplicand[0] = param[3];
    r48multiplicand[1] = param[4];
    r48multiplicand[2] = param[5];
    /* Last short == addend */
    r48addend = param[6];
}

/** ------------------------------------------------------
 *
 *  system tools
 *
*/
int  e_get_nprocs()
{
#ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;

#elif defined (__linux__)
    #include <sys/sysinfo.h>
    return get_nprocs();

#elif defined (__APPLE__)

    return sysconf(_SC_NPROCESSORS_ONLN);

#endif
}

cstr eutils_version()
{
    return EUTILS_VERSION;
}
