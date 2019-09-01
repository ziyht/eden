/// =====================================================================================
///
///       Filename:  etime.c
///
///    Description:  a compat time impletation for different platforms
///
///        Version:  1.1
///        Created:  06/11/2019 11:00:34 AM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#include <time.h>
#include <stdio.h>
#include <errno.h>

#include "ecompat.h"
#include "eutils.h"

#include "etime.h"

#define  ETIME_VERION "etime 1.1.0"   // compat to main platforms(WIN, LINUX, MAXOS)

#if defined(__linux) || defined(__APPLE__)
#include <ctype.h>
#endif

typedef enum {
  _CLOCK_PRECISE = 0,  /* Use the highest resolution clock available. */
  _CLOCK_FAST    = 1   /* Use the fastest clock with <= 1ms granularity. */
} clocktype_t;

#ifdef __APPLE__
#define CLOCK_MONOTONIC_COARSE CLOCK_MONOTONIC
#define CLOCK_REALTIME_COARSE  CLOCK_REALTIME
#endif

/* Available from 2.6.32 onwards. */
#ifndef CLOCK_MONOTONIC_COARSE
#define CLOCK_MONOTONIC_COARSE 6
#endif

#ifndef _WIN32
#include <sys/time.h>
static int     __hrtime_init_needed = 1;
static i64     __hrtime_nsec_offset;
static clock_t __fast_mono_id;
static clock_t __fast_real_id;
static inline int __hrtime_init()
{

    struct timespec t1 = {0, 0},
                    t2 = {0, 0};

    if(0 == clock_gettime(CLOCK_REALTIME_COARSE, &t1))
        __fast_real_id = CLOCK_REALTIME_COARSE;
    else
        __fast_real_id = CLOCK_REALTIME;

    if(0 != clock_gettime(__fast_real_id, &t1))
    {
        perror("clock_gettime not working correctly");
        abort();        /* Not really possible. */
    }

    if(0 == clock_gettime(CLOCK_MONOTONIC_COARSE, &t2))
    {
        __fast_mono_id = CLOCK_MONOTONIC_COARSE;
    }
    else if(0 == clock_gettime(CLOCK_MONOTONIC, &t2))
    {
        __fast_mono_id = CLOCK_MONOTONIC;
    }
    else
    {
        __fast_mono_id = __fast_real_id;
        t2 = t1;
    }

    __hrtime_nsec_offset = (i64)t1.tv_sec * 1000000000ll + (i64)t1.tv_nsec  -
                           (i64)t2.tv_sec * 1000000000ll + (i64)t2.tv_nsec;

    return 1;
}

static inline u64 __hrtime_ns(clocktype_t type)
{
    struct timespec t;

    if( __hrtime_init_needed ) {
        __hrtime_init();
        __hrtime_init_needed = 0;
    }

    if(clock_gettime((type == _CLOCK_FAST) ? __fast_real_id : CLOCK_REALTIME, &t))
    {
        perror("clock_gettime not working correctly");
        abort();        /* Not really possible. */
    }

    return t.tv_sec * 1000000000 + t.tv_nsec;
}

static inline u64 __hrtick_ns(clocktype_t type)
{
    struct timespec t;

    if( __hrtime_init_needed ) {
        __hrtime_init();
        __hrtime_init_needed = 0;
    }

    if(clock_gettime((type == _CLOCK_FAST) ? __fast_mono_id : CLOCK_MONOTONIC, &t))
    {
        perror("clock_gettime not working correctly");
        abort();        /* Not really possible. */
    }

    return t.tv_sec * 1000000000 + t.tv_nsec + __hrtime_nsec_offset;
}

#else

#ifdef _MSC_VER
struct timezone
{
    int  tz_minuteswest; /* minutes west of Greenwich */
    int  tz_dsttime;     /* type of DST correction */
};

#define tzset _tzset
#else
extern void tzset(void);
#endif

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

#include <sys/timeb.h>

static inline u64 __hrtime_ns(clocktype_t type)
{
    // Define a structure to receive the current Windows filetime
    FILETIME ft;

    // Initialize the present time to 0 and the timezone to UTC
    u64 tmpres = 0;

#ifdef _WIN32_WCE
        SYSTEMTIME st;
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);
#else
        GetSystemTimeAsFileTime(&ft);
#endif

    // The GetSystemTimeAsFileTime returns the number of 100 nanosecond
    // intervals since Jan 1, 1601 in a structure. Copy the high bits t
    // the 64 bit tmpres, shift it left by 32 then or in the low 32 bit
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    // The Unix epoch starts on Jan 1 1970.  Need to subtract the diffe
    // in seconds from Jan 1 1601.
    return tmpres * 100 - DELTA_EPOCH_IN_MICROSECS * 1000;
}

static i64           __hrtime_nsec_offset;
static double        __hrtime_interval;
static LARGE_INTEGER __perf_frequency;
static inline int __hrtick_init()
{
    if(__hrtime_interval == 0)
    {
         LARGE_INTEGER counter;

        if (!QueryPerformanceFrequency(&__perf_frequency))
        {
            perror("clock_gettime not working correctly");
            abort();        /* Not really possible. */
        }

        QueryPerformanceCounter(&counter);
        __hrtime_nsec_offset = __hrtime_ns(_CLOCK_PRECISE) -
                              counter.QuadPart * 1000000000ll / __perf_frequency.QuadPart;
    }

    return 1;
}

static inline u64 __hrtick_ns(clocktype_t type)
{
    LARGE_INTEGER counter;

    if(__hrtime_interval == 0)
        __hrtick_init();

    QueryPerformanceCounter(&counter);

    return (u64) (counter.QuadPart * 1000000000ll / __perf_frequency.QuadPart) + __hrtime_nsec_offset;
}

static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if (NULL != tv)
    {
        u64 tmpres  = __hrtime_ns(_CLOCK_PRECISE) / 1000;

        tv->tv_sec  = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        static int tzflag = 0;
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime     = _daylight;
    }

    return 0;
}

#endif

i64  e_nowns()  { return __hrtime_ns(_CLOCK_PRECISE)             ; }
i64  e_nowus()  { return __hrtime_ns(_CLOCK_PRECISE) / 1000      ; }
i64  e_nowms()  { return __hrtime_ns(_CLOCK_FAST   ) / 1000000   ; }
i64  e_nows ()  { return __hrtime_ns(_CLOCK_FAST   ) / 1000000000; }

i64  e_tickns() { return __hrtick_ns(_CLOCK_PRECISE)             ; }
i64  e_tickus() { return __hrtick_ns(_CLOCK_PRECISE) / 1000      ; }
i64  e_tickms() { return __hrtick_ns(_CLOCK_FAST   ) / 1000000   ; }
i64  e_ticks () { return __hrtick_ns(_CLOCK_FAST   ) / 1000000000; }

i64  e_ticker_ns(i64* tick) { i64 old = *tick; *tick = e_tickns(); return *tick - old;  }
i64  e_ticker_us(i64* tick) { i64 old = *tick; *tick = e_tickus(); return *tick - old;  }
i64  e_ticker_ms(i64* tick) { i64 old = *tick; *tick = e_tickms(); return *tick - old;  }
i64  e_ticker_s (i64* tick) { i64 old = *tick; *tick = e_ticks (); return *tick - old;  }

static inline cstr __secstr(cstr desc, int dlen, i64 ns)
{
    struct tm time; time_t sec;

    sec = ns / 1000000000;

    localtime_r(&sec, &time);

    dlen < 17 ? snprintf(desc, dlen, "%4d%02d%02d%02d%02d%02d "           , time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec)
              : snprintf(desc, dlen, "%4d%02d%02d%02d%02d%02d.%09"PRIi64"", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, ns % 1000000000);

    return desc;
}

cstr e_nowstr(cstr desc, int dlen)
{
    return __secstr(desc, dlen, __hrtime_ns(_CLOCK_PRECISE));
}

cstr e_secstr(cstr desc, int dlen, time_t sec)
{
    return __secstr(desc, dlen, 1000000000ll * sec);
}


/*
 * strftime.c
 *
 * Public-domain implementation of ISO C library routine.
 *
 * If you can't do prototypes, get GCC.
 *
 * The C99 standard now specifies just about all of the formats
 * that were additional in the earlier versions of this file.
 *
 * For extensions from SunOS, add SUNOS_EXT.
 * For extensions from HP/UX, add HPUX_EXT.
 * For VMS dates, add VMS_EXT.
 * For complete POSIX semantics, add POSIX_SEMANTICS.
 *
 * The code for %X follows the C99 specification for
 * the "C" locale.
 *
 * The code for %c, and %x follows the C11 specification for
 * the "C" locale.
 *
 * With HAVE_NL_LANGINFO defined, locale-based values are used.
 *
 * This version doesn't worry about multi-byte characters.
 * So there.
 *
 * Arnold Robbins
 * January, February, March, 1991
 * Updated March, April 1992
 * Updated April, 1993
 * Updated February, 1994
 * Updated May, 1994
 * Updated January, 1995
 * Updated September, 1995
 * Updated January, 1996
 * Updated July, 1997
 * Updated October, 1999
 * Updated September, 2000
 * Updated December, 2001
 * Updated January, 2011
 * Updated April, 2012
 * Updated March, 2015
 * Updated June, 2015
 *
 * Fixes from ado@elsie.nci.nih.gov,
 * February 1991, May 1992
 * Fixes from Tor Lillqvist tml@tik.vtt.fi,
 * May 1993
 * Further fixes from ado@elsie.nci.nih.gov,
 * February 1994
 * %z code from chip@chinacat.unicom.com,
 * Applied September 1995
 * %V code fixed (again) and %G, %g added,
 * January 1996
 * %v code fixed, better configuration,
 * July 1997
 * Moved to C99 specification.
 * September 2000
 * Fixes from Tanaka Akira <akr@m17n.org>
 * December 2001
 */

#include <stdio.h>
#include <ctype.h>
#include <time.h>

#if defined(TM_IN_SYS_TIME)
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <stdlib.h>
#include <string.h>

/* defaults: season to taste */
#define SUNOS_EXT           1	/* stuff in SunOS strftime routine */
#define VMS_EXT             1	/* include %v for VMS date format */
#define HPUX_EXT            1	/* non-conflicting stuff in HP-UX date */
#define POSIX_SEMANTICS     1	/* call tzset() if TZ changes */
#define POSIX_2008          1   /* flag and fw for C, F, G, Y formats */
//#define HAVE_NL_LANGINFO	1	/* locale-based values */

#undef strchr	/* avoid AIX weirdness */

#ifdef HAVE_NL_LANGINFO
#include <langinfo.h>
#endif

static int weeknumber(const struct tm *timeptr, int firstweekday);
static int iso8601wknum(const struct tm *timeptr);

#ifndef inline
#ifdef __GNUC__
#define inline	__inline__
#else
#define inline	/**/
#endif
#endif

#define range(low, item, hi)	max(low, min(item, hi))

/* Whew! This stuff is a mess. */
#if !defined(OS2) && !defined(MSDOS) && !defined(__CYGWIN__) && defined(HAVE_TZNAME)
extern char *tzname[2];
extern int daylight;
#if defined(SOLARIS) || defined(mips) || defined (M_UNIX)
extern long int timezone, altzone;
#else
#if defined(__hpux)
extern long int timezone;
#else
#if !defined(__CYGWIN__)
extern int timezone, altzone;
#endif
#endif
#endif
#endif

#undef min	/* just in case */

/* min --- return minimum of two numbers */

static inline int
min(int a, int b)
{
    return (a < b ? a : b);
}

#undef max	/* also, just in case */

/* max --- return maximum of two numbers */

static inline int
max(int a, int b)
{
    return (a > b ? a : b);
}

#ifdef POSIX_2008
/* iso_8601_2000_year --- format a year per ISO 8601:2000 as in 1003.1 */

static void
iso_8601_2000_year(char *buf, int year, size_t fw)
{
    int extra;
    char sign = '\0';

    if (year >= -9999 && year <= 9999) {
        sprintf(buf, "%0*d", (int) fw, year);
        return;
    }

    /* now things get weird */
    if (year > 9999) {
        sign = '+';
    } else {
        sign = '-';
        year = -year;
    }

    extra = year / 10000;
    year %= 10000;
    sprintf(buf, "%c_%04d_%d", sign, extra, year);
}
#endif /* POSIX_2008 */

/* days_a --- return the short name for the day of the week */

static const char *
days_a(int index)
{
#ifdef HAVE_NL_LANGINFO
    static const nl_item data[] = {
        ABDAY_1,
        ABDAY_2,
        ABDAY_3,
        ABDAY_4,
        ABDAY_5,
        ABDAY_6,
        ABDAY_7,
    };

    return nl_langinfo(data[index]);
#else
    static const char *data[] = {
        "Sun", "Mon", "Tue", "Wed",
        "Thu", "Fri", "Sat",
    };

    return data[index];
#endif
}

/* days_l --- return the long name for the day of the week */

static const char *
days_l(int index)
{
#ifdef HAVE_NL_LANGINFO
    static const nl_item data[] = {
        DAY_1,
        DAY_2,
        DAY_3,
        DAY_4,
        DAY_5,
        DAY_6,
        DAY_7,
    };

    return nl_langinfo(data[index]);
#else
    static const char *data[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday",
    };

    return data[index];
#endif
}

/* months_a --- return the short name for the month */

static const char *
months_a(int index)
{
#ifdef HAVE_NL_LANGINFO
    static const nl_item data[] = {
        ABMON_1,
        ABMON_2,
        ABMON_3,
        ABMON_4,
        ABMON_5,
        ABMON_6,
        ABMON_7,
        ABMON_8,
        ABMON_9,
        ABMON_10,
        ABMON_11,
        ABMON_12,
    };

    return nl_langinfo(data[index]);
#else
    static const char *data[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

    return data[index];
#endif
}

/* months_l --- return the short name for the month */

static const char *
months_l(int index)
{
#ifdef HAVE_NL_LANGINFO
    static const nl_item data[] = {
        MON_1,
        MON_2,
        MON_3,
        MON_4,
        MON_5,
        MON_6,
        MON_7,
        MON_8,
        MON_9,
        MON_10,
        MON_11,
        MON_12,
    };

    return nl_langinfo(data[index]);
#else
    static const char *data[] = {
        "January", "February", "March", "April",
        "May", "June", "July", "August", "September",
        "October", "November", "December",
    };

    return data[index];
#endif
}

/* days_a --- return am/pm string */

static const char *
ampm(int index)
{
#ifdef HAVE_NL_LANGINFO
    static const nl_item data[] = {
        AM_STR,
        PM_STR,
    };

    return nl_langinfo(data[index]);
#else
    static const char *data[] = {
        "AM", "PM",
    };

    return data[index];
#endif
}

/* strftime --- produce formatted time */

#if defined(__linux) || defined(__APPLE__)
#define HAVE_TM_ZONE
#endif

static size_t
__strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr)
{
    char *endp = s + maxsize;
    char *start = s;
    auto char tbuf[100];
    long off;
    int i, w;
    long y;
    static short first = 1;
#ifdef POSIX_SEMANTICS
    static char *savetz = NULL;
    static int savetzlen = 0;
    char *tz;
#endif /* POSIX_SEMANTICS */
#ifndef HAVE_TM_ZONE
#ifndef HAVE_TM_NAME
#ifndef HAVE_TZNAME
#ifndef __CYGWIN__
    extern char *timezone();
    struct timeval tv;
    struct timezone zone;
#endif /* __CYGWIN__ */
#endif /* HAVE_TZNAME */
#endif /* HAVE_TM_NAME */
#endif /* HAVE_TM_ZONE */
#ifdef POSIX_2008
    int pad;
    size_t fw;
    char flag;
#endif /* POSIX_2008 */


    if (s == NULL || format == NULL || timeptr == NULL || maxsize == 0)
        return 0;

    /* quick check if we even need to bother */
    if (strchr(format, '%') == NULL && strlen(format) + 1 >= maxsize)
        return 0;

#ifndef POSIX_SEMANTICS
    if (first) {
        tzset();
        first = 0;
    }
#else	/* POSIX_SEMANTICS */
    tz = getenv("TZ");
    if (first) {
        if (tz != NULL) {
            int tzlen = strlen(tz);

            savetz = (char *) malloc(tzlen + 1);
            if (savetz != NULL) {
                savetzlen = tzlen + 1;
                strcpy(savetz, tz);
            }
        }
        tzset();
        first = 0;
    }
    /* if we have a saved TZ, and it is different, recapture and reset */
    if (tz && savetz && (tz[0] != savetz[0] || strcmp(tz, savetz) != 0)) {
        i = strlen(tz) + 1;
        if (i > savetzlen) {
            savetz = (char *) realloc(savetz, i);
            if (savetz) {
                savetzlen = i;
                strcpy(savetz, tz);
            }
        } else
            strcpy(savetz, tz);
        tzset();
    }
#endif	/* POSIX_SEMANTICS */

    for (; *format && s < endp - 1; format++) {
        tbuf[0] = '\0';
        if (*format != '%') {
            *s++ = *format;
            continue;
        }
#ifdef POSIX_2008
        pad = '\0';
        fw = 0;
        flag = '\0';
        switch (*++format) {
        case '+':
            flag = '+';
            /* fall through */
        case '0':
            pad = '0';
            format++;
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            break;

        default:
            format--;
            goto again;
        }
        for (; isdigit(*format); format++) {
            fw = fw * 10 + (*format - '0');
        }
        format--;
#endif /* POSIX_2008 */

    again:
        switch (*++format) {
        case '\0':
            *s++ = '%';
            goto out;

        case '%':
            *s++ = '%';
            continue;

        case 'a':	/* abbreviated weekday name */
            if (timeptr->tm_wday < 0 || timeptr->tm_wday > 6)
                strcpy(tbuf, "?");
            else
                strcpy(tbuf, days_a(timeptr->tm_wday));
            break;

        case 'A':	/* full weekday name */
            if (timeptr->tm_wday < 0 || timeptr->tm_wday > 6)
                strcpy(tbuf, "?");
            else
                strcpy(tbuf, days_l(timeptr->tm_wday));
            break;

        case 'b':	/* abbreviated month name */
        short_month:
            if (timeptr->tm_mon < 0 || timeptr->tm_mon > 11)
                strcpy(tbuf, "?");
            else
                strcpy(tbuf, months_a(timeptr->tm_mon));
            break;

        case 'B':	/* full month name */
            if (timeptr->tm_mon < 0 || timeptr->tm_mon > 11)
                strcpy(tbuf, "?");
            else
                strcpy(tbuf, months_l(timeptr->tm_mon));
            break;

        case 'c':	/* appropriate date and time representation */
            /*
             * This used to be:
             *
             * strftime(tbuf, sizeof tbuf, "%a %b %e %H:%M:%S %Y", timeptr);
             *
             * Per the ISO 1999 C standard, it was this:
             * strftime(tbuf, sizeof tbuf, "%A %B %d %T %Y", timeptr);
             *
             * Per the ISO 2011 C standard, it is now this:
             */
#ifdef HAVE_NL_LANGINFO
            __strftime(tbuf, sizeof tbuf, nl_langinfo(D_T_FMT), timeptr);
#else
            __strftime(tbuf, sizeof tbuf, "%a %b %e %T %Y", timeptr);
#endif
            break;

        case 'C':
#ifdef POSIX_2008
            if (pad != '\0' && fw > 0) {
                size_t min_fw = (flag ? 3 : 2);

                fw = max((int)fw, (int)min_fw);
                sprintf(tbuf, flag
                        ? "%+0*ld"
                        : "%0*ld", (int) fw,
                        (timeptr->tm_year + 1900L) / 100);
            } else
#endif /* POSIX_2008 */
        century:
                sprintf(tbuf, "%02ld", (timeptr->tm_year + 1900L) / 100);
            break;

        case 'd':	/* day of the month, 01 - 31 */
            i = range(1, timeptr->tm_mday, 31);
            sprintf(tbuf, "%02d", i);
            break;

        case 'D':	/* date as %m/%d/%y */
            __strftime(tbuf, sizeof tbuf, "%m/%d/%y", timeptr);
            break;

        case 'e':	/* day of month, blank padded */
            sprintf(tbuf, "%2d", range(1, timeptr->tm_mday, 31));
            break;

        case 'E':
            /* POSIX (now C99) locale extensions, ignored for now */
            goto again;

        case 'F':	/* ISO 8601 date representation */
        {
#ifdef POSIX_2008
            /*
             * Field width for %F is for the whole thing.
             * It must be at least 10.
             */
            char m_d[10];
            __strftime(m_d, sizeof m_d, "-%m-%d", timeptr);
            size_t min_fw = 10;

            if (pad != '\0' && fw > 0) {
                fw = max((int)fw, (int)min_fw);
            } else {
                fw = min_fw;
            }

            fw -= 6;	/* -XX-XX at end are invariant */

            iso_8601_2000_year(tbuf, timeptr->tm_year + 1900, fw);
            strcat(tbuf, m_d);
#else
            __strftime(tbuf, sizeof tbuf, "%Y-%m-%d", timeptr);
#endif /* POSIX_2008 */
        }
            break;

        case 'g':
        case 'G':
            /*
             * Year of ISO week.
             *
             * If it's December but the ISO week number is one,
             * that week is in next year.
             * If it's January but the ISO week number is 52 or
             * 53, that week is in last year.
             * Otherwise, it's this year.
             */
            w = iso8601wknum(timeptr);
            if (timeptr->tm_mon == 11 && w == 1)
                y = 1900L + timeptr->tm_year + 1;
            else if (timeptr->tm_mon == 0 && w >= 52)
                y = 1900L + timeptr->tm_year - 1;
            else
                y = 1900L + timeptr->tm_year;

            if (*format == 'G') {
#ifdef POSIX_2008
                if (pad != '\0' && fw > 0) {
                    size_t min_fw = 4;

                    fw = max((int)fw, (int)min_fw);
                    sprintf(tbuf, flag
                            ? "%+0*ld"
                            : "%0*ld", (int) fw,
                            y);
                } else
#endif /* POSIX_2008 */
                    sprintf(tbuf, "%ld", y);
            }
            else
                sprintf(tbuf, "%02ld", y % 100);
            break;

        case 'h':	/* abbreviated month name */
            goto short_month;

        case 'H':	/* hour, 24-hour clock, 00 - 23 */
            //i = range(0, timeptr->tm_hour, 23);
            sprintf(tbuf, "%02d", timeptr->tm_hour);
            break;

        case 'I':	/* hour, 12-hour clock, 01 - 12 */
            i = range(0, timeptr->tm_hour, 23);
            if (i == 0)
                i = 12;
            else if (i > 12)
                i -= 12;
            sprintf(tbuf, "%02d", i);
            break;

        case 'j':	/* day of the year, 001 - 366 */
            sprintf(tbuf, "%03d", timeptr->tm_yday + 1);
            break;

        case 'm':	/* month, 01 - 12 */
            i = range(0, timeptr->tm_mon, 11);
            sprintf(tbuf, "%02d", i + 1);
            break;

        case 'M':	/* minute, 00 - 59 */
            i = range(0, timeptr->tm_min, 59);
            sprintf(tbuf, "%02d", i);
            break;

        case 'n':	/* same as \n */
            tbuf[0] = '\n';
            tbuf[1] = '\0';
            break;

        case 'O':
            /* POSIX (now C99) locale extensions, ignored for now */
            goto again;

        case 'p':	/* am or pm based on 12-hour clock */
            i = range(0, timeptr->tm_hour, 23);
            if (i < 12)
                strcpy(tbuf, ampm(0));
            else
                strcpy(tbuf, ampm(1));
            break;

        case 'r':	/* time as %I:%M:%S %p */
            __strftime(tbuf, sizeof tbuf, "%I:%M:%S %p", timeptr);
            break;

        case 'R':	/* time as %H:%M */
            __strftime(tbuf, sizeof tbuf, "%H:%M", timeptr);
            break;

#if defined(HAVE_MKTIME)
        case 's':	/* time as seconds since the Epoch */
        {
            struct tm non_const_timeptr;

            non_const_timeptr = *timeptr;
            sprintf(tbuf, "%ld", mktime(& non_const_timeptr));
            break;
        }
#endif /* defined(HAVE_MKTIME) */

        case 'S':	/* second, 00 - 60 */
            i = range(0, timeptr->tm_sec, 60);
            sprintf(tbuf, "%02d", i);
            break;

        case 't':	/* same as \t */
            tbuf[0] = '\t';
            tbuf[1] = '\0';
            break;

        case 'T':	/* time as %H:%M:%S */
        the_time:
            __strftime(tbuf, sizeof tbuf, "%H:%M:%S", timeptr);
            break;

        case 'u':
        /* ISO 8601: Weekday as a decimal number [1 (Monday) - 7] */
            sprintf(tbuf, "%d", timeptr->tm_wday == 0 ? 7 :
                    timeptr->tm_wday);
            break;

        case 'U':	/* week of year, Sunday is first day of week */
            sprintf(tbuf, "%02d", weeknumber(timeptr, 0));
            break;

        case 'V':	/* week of year according ISO 8601 */
            sprintf(tbuf, "%02d", iso8601wknum(timeptr));
            break;

        case 'w':	/* weekday, Sunday == 0, 0 - 6 */
            i = range(0, timeptr->tm_wday, 6);
            sprintf(tbuf, "%d", i);
            break;

        case 'W':	/* week of year, Monday is first day of week */
            sprintf(tbuf, "%02d", weeknumber(timeptr, 1));
            break;

        case 'x':	/* appropriate date representation */
            /*
             * Up to the 2011 standard, this code used:
             * strftime(tbuf, sizeof tbuf, "%A %B %d %Y", timeptr);
             *
             * Now, per the 2011 C standard, this is:
             */
#ifdef HAVE_NL_LANGINFO
            __strftime(tbuf, sizeof tbuf, nl_langinfo(D_FMT), timeptr);
#else
            __strftime(tbuf, sizeof tbuf, "%m/%d/%y", timeptr);
#endif
            break;

        case 'X':	/* appropriate time representation */
#ifdef HAVE_NL_LANGINFO
            __strftime(tbuf, sizeof tbuf, nl_langinfo(T_FMT), timeptr);
#else
            goto the_time;
#endif
            break;

        case 'y':	/* year without a century, 00 - 99 */
        year:
            i = timeptr->tm_year % 100;
            sprintf(tbuf, "%02d", i);
            break;

        case 'Y':	/* year with century */
#ifdef POSIX_2008
            if (pad != '\0' && fw > 0) {
                size_t min_fw = 4;

                fw = max((int)fw, (int)min_fw);
                sprintf(tbuf, flag
                        ? "%+0*ld"
                        : "%0*ld", (int) fw,
                        1900L + timeptr->tm_year);
            } else
#endif /* POSIX_2008 */
            sprintf(tbuf, "%ld", 1900L + timeptr->tm_year);
            break;

        /*
         * From: Chip Rosenthal <chip@chinacat.unicom.com>
         * Date: Sun, 19 Mar 1995 00:33:29 -0600 (CST)
         *
         * Warning: the %z [code] is implemented by inspecting the
         * timezone name conditional compile settings, and
         * inferring a method to get timezone offsets. I've tried
         * this code on a couple of machines, but I don't doubt
         * there is some system out there that won't like it.
         * Maybe the easiest thing to do would be to bracket this
         * with an #ifdef that can turn it off. The %z feature
         * would be an admittedly obscure one that most folks can
         * live without, but it would be a great help to those of
         * us that muck around with various message processors.
         */
        case 'z':	/* time zone offset east of GMT e.g. -0600 */
            if (timeptr->tm_isdst < 0)
                break;
#ifdef HAVE_TM_NAME
            /*
             * Systems with tm_name probably have tm_tzadj as
             * secs west of GMT.  Convert to mins east of GMT.
             */
            off = -timeptr->tm_tzadj / 60;
#else /* !HAVE_TM_NAME */
#ifdef HAVE_TM_ZONE
            /*
             * Systems with tm_zone probably have tm_gmtoff as
             * secs east of GMT.  Convert to mins east of GMT.
             */
            off = timeptr->tm_gmtoff / 60;
#else /* !HAVE_TM_ZONE */
#if HAVE_TZNAME
            /*
             * Systems with tzname[] probably have timezone as
             * secs west of GMT.  Convert to mins east of GMT.
             */
#if defined(__hpux) || defined(__CYGWIN__)
            off = -timezone / 60;
#else
            /* ADR: 4 August 2001, fixed this per gazelle@interaccess.com */
            off = -(daylight ? altzone : timezone) / 60;
#endif
#else /* !HAVE_TZNAME */
            gettimeofday(& tv, & zone);
            off = -zone.tz_minuteswest;
#endif /* !HAVE_TZNAME */
#endif /* !HAVE_TM_ZONE */
#endif /* !HAVE_TM_NAME */
            if (off < 0) {
                tbuf[0] = '-';
                off = -off;
            } else {
                tbuf[0] = '+';
            }
            sprintf(tbuf+1, "%02ld%02ld", off/60, off%60);
            break;

        case 'Z':	/* time zone name or abbrevation */
#ifdef _MSC_VER
            strcpy(tbuf, *__tzname());
#else
#ifdef HAVE_TZNAME
            i = (daylight && timeptr->tm_isdst > 0); /* 0 or 1 */
            strcpy(tbuf, tzname[i]);
#else
#ifdef HAVE_TM_ZONE
            strcpy(tbuf, timeptr->tm_zone);
#else
#ifdef HAVE_TM_NAME
            strcpy(tbuf, timeptr->tm_name);
#else
            gettimeofday(& tv, & zone);
            strcpy(tbuf, timezone(zone.tz_minuteswest,
                        timeptr->tm_isdst > 0));
#endif /* HAVE_TM_NAME */
#endif /* HAVE_TM_ZONE */
#endif /* HAVE_TZNAME */
#endif /* _MSC_VER */
            break;

#ifdef SUNOS_EXT
        case 'k':	/* hour, 24-hour clock, blank pad */
            sprintf(tbuf, "%2d", range(0, timeptr->tm_hour, 23));
            break;

        case 'l':	/* hour, 12-hour clock, 1 - 12, blank pad */
            i = range(0, timeptr->tm_hour, 23);
            if (i == 0)
                i = 12;
            else if (i > 12)
                i -= 12;
            sprintf(tbuf, "%2d", i);
            break;
#endif

#ifdef HPUX_EXT
        case 'N':	/* Emperor/Era name */
#ifdef HAVE_NL_LANGINFO
            __strftime(tbuf, sizeof tbuf, nl_langinfo(ERA), timeptr);
#else
            /* this is essentially the same as the century */
            goto century;	/* %C */
#endif

        case 'o':	/* Emperor/Era year */
            goto year;	/* %y */
#endif /* HPUX_EXT */


#ifdef VMS_EXT
        case 'v':	/* date as dd-bbb-YYYY */
            sprintf(tbuf, "%2d-%3.3s-%4ld",
                range(1, timeptr->tm_mday, 31),
                months_a(range(0, timeptr->tm_mon, 11)),
                timeptr->tm_year + 1900L);
            for (i = 3; i < 6; i++)
                if (islower(tbuf[i]))
                    tbuf[i] = toupper(tbuf[i]);
            break;
#endif

        default:
            tbuf[0] = '%';
            tbuf[1] = *format;
            tbuf[2] = '\0';
            break;
        }
        i = strlen(tbuf);
        if (i) {
            if (s + i < endp - 1) {
                strcpy(s, tbuf);
                s += i;
            } else
                return 0;
        }
    }
out:
    if (s < endp && *format == '\0') {
        *s = '\0';
        return (s - start);
    } else
        return 0;
}

/* isleap --- is a year a leap year? */

static int
isleap(long year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}


/* iso8601wknum --- compute week number according to ISO 8601 */

static int
iso8601wknum(const struct tm *timeptr)
{
    /*
     * From 1003.2:
     *	If the week (Monday to Sunday) containing January 1
     *	has four or more days in the new year, then it is week 1;
     *	otherwise it is the highest numbered week of the previous
     *	year (52 or 53), and the next week is week 1.
     *
     * ADR: This means if Jan 1 was Monday through Thursday,
     *	it was week 1, otherwise week 52 or 53.
     *
     * XPG4 erroneously included POSIX.2 rationale text in the
     * main body of the standard. Thus it requires week 53.
     */

    int weeknum, jan1day;

    /* get week number, Monday as first day of the week */
    weeknum = weeknumber(timeptr, 1);

    /*
     * With thanks and tip of the hatlo to tml@tik.vtt.fi
     *
     * What day of the week does January 1 fall on?
     * We know that
     *	(timeptr->tm_yday - jan1.tm_yday) MOD 7 ==
     *		(timeptr->tm_wday - jan1.tm_wday) MOD 7
     * and that
     * 	jan1.tm_yday == 0
     * and that
     * 	timeptr->tm_wday MOD 7 == timeptr->tm_wday
     * from which it follows that. . .
     */
    jan1day = timeptr->tm_wday - (timeptr->tm_yday % 7);
    if (jan1day < 0)
        jan1day += 7;

    /*
     * If Jan 1 was a Monday through Thursday, it was in
     * week 1.  Otherwise it was last year's highest week, which is
     * this year's week 0.
     *
     * What does that mean?
     * If Jan 1 was Monday, the week number is exactly right, it can
     *	never be 0.
     * If it was Tuesday through Thursday, the weeknumber is one
     *	less than it should be, so we add one.
     * Otherwise, Friday, Saturday or Sunday, the week number is
     * OK, but if it is 0, it needs to be 52 or 53.
     */
    switch (jan1day) {
    case 1:		/* Monday */
        break;
    case 2:		/* Tuesday */
    case 3:		/* Wednesday */
    case 4:		/* Thursday */
        weeknum++;
        break;
    case 5:		/* Friday */
    case 6:		/* Saturday */
    case 0:		/* Sunday */
        if (weeknum == 0) {
#ifdef USE_BROKEN_XPG4
            /* XPG4 (as of March 1994) says 53 unconditionally */
            weeknum = 53;
#else
            /* get week number of last week of last year */
            struct tm dec31ly;	/* 12/31 last year */
            dec31ly = *timeptr;
            dec31ly.tm_year--;
            dec31ly.tm_mon = 11;
            dec31ly.tm_mday = 31;
            dec31ly.tm_wday = (jan1day == 0) ? 6 : jan1day - 1;
            dec31ly.tm_yday = 364 + isleap(dec31ly.tm_year + 1900L);
            weeknum = iso8601wknum(& dec31ly);
#endif
        }
        break;
    }

    if (timeptr->tm_mon == 11) {
        /*
         * The last week of the year
         * can be in week 1 of next year.
         * Sigh.
         *
         * This can only happen if
         *	M   T  W
         *	29  30 31
         *	30  31
         *	31
         */
        int wday, mday;

        wday = timeptr->tm_wday;
        mday = timeptr->tm_mday;
        if (   (wday == 1 && (mday >= 29 && mday <= 31))
            || (wday == 2 && (mday == 30 || mday == 31))
            || (wday == 3 &&  mday == 31))
            weeknum = 1;
    }

    return weeknum;
}

/* weeknumber --- figure how many weeks into the year */

/* With thanks and tip of the hatlo to ado@elsie.nci.nih.gov */

static int
weeknumber(const struct tm *timeptr, int firstweekday)
{
    int wday = timeptr->tm_wday;
    int ret;

    if (firstweekday == 1) {
        if (wday == 0)	/* sunday */
            wday = 6;
        else
            wday--;
    }
    ret = ((timeptr->tm_yday + 7 - wday) / 7);
    if (ret < 0)
        ret = 0;
    return ret;
}

typedef struct etm_s {
        int     tm_sec;         /* 秒            [0-60] */
        int     tm_min;         /* 分            [0-59] */
        int     tm_hour;        /* 小时          [0-23] */
        int     tm_mday;        /* 一月中的第几天  [1-31] */
        int     tm_mon;         /* 月份          [0-11] */
        int     tm_year;        /* 年            [1900+] */
        int     tm_wday;        /* 一周中的第几天  [0-6] */
        int     tm_yday;        /* 一年中的第几天  [0-365] */
        int     tm_isdst;       /* 夏令时 */
        long    tm_gmtoff;      /* 以秒为单位的UTC偏移量 */
        char    *tm_zone;       /* 时区信息 */
        long    tm_nsec;        /* nanoseconds */
}etm_t, * etm;

char* estrptime(const char *buf, const char* fmt, etm tm);
cstr e_strfstr(cstr dest, int dlen, constr dfmt, constr from, constr ffmt)
{
    etm_t _tm = {0};

    if(estrptime(from, ffmt, &_tm))
    {
        return __strftime(dest, dlen, dfmt, (const struct tm*)&_tm) ? dest : 0;
    }

    return 0;
}

cstr e_strfsec(cstr dest, int dlen, constr dfmt, time_t sec)
{
    struct tm _tm;

#ifdef _MSC_VER
    return !localtime_r(&sec, &_tm) ? __strftime(dest, dlen, dfmt, &_tm) ? dest
                                                                         : 0
                                    : 0;
#else
    return localtime_r(&sec, &_tm) ? __strftime(dest, dlen, dfmt, &_tm) ? dest
                                                                        : 0
                                   : 0;
#endif
}

i64 e_strpsec(constr from, constr ffmt)
{
    etm_t tm = {0};

    if(estrptime(from, ffmt, &tm))
    {
        i64 sec;

        if(tm.tm_year == 0)
            tm.tm_year = 1;

        sec = mktime((struct tm* const)&tm);

        return sec < 0 ? -1 : sec;
    }

    return -1;
}

i64 e_strpms(constr from, constr ffmt)
{
    etm_t tm = {0};

    if(estrptime(from, ffmt, &tm))
    {
        i64 sec;

        if(tm.tm_year == 0)
            tm.tm_year = 1;

        sec = mktime((struct tm* const)&tm);

        return sec < 0 ? -1 : sec * 1000;
    }

    return -1;
}

#define locale_t int

#undef isspace_l
#undef isdigit_l
#undef isupper_l
#undef strtol_l
#undef isleap
#undef strcasecmp_l
#undef strncasecmp_l

#define isspace_l(A, _)         isspace(A)
#define isdigit_l(A, _)         isdigit(A)
#define isupper_l(A, _)         isupper(A)
#define strtol_l(A, B, C, _)    strtol(A, B, C)

#define strcasecmp_l(A, B, _)       e_strcasecmp(A, B)
#define strncasecmp_l(A, B, C, _)   e_strncasecmp(A, B, C)

#define isleap(y)   (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

#define YEARSPERREPEAT		400	/* years before a Gregorian repeat */

#define SECSPERMIN	60
#define MINSPERHOUR	60
#define HOURSPERDAY	24
#define DAYSPERWEEK	7
#define DAYSPERNYEAR	365
#define DAYSPERLYEAR	366
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR	12

#define TM_SUNDAY	0
#define TM_MONDAY	1
#define TM_TUESDAY	2
#define TM_WEDNESDAY	3
#define TM_THURSDAY	4
#define TM_FRIDAY	5
#define TM_SATURDAY	6

#define TM_JANUARY	0
#define TM_FEBRUARY	1
#define TM_MARCH	2
#define TM_APRIL	3
#define TM_MAY		4
#define TM_JUNE		5
#define TM_JULY		6
#define TM_AUGUST	7
#define TM_SEPTEMBER	8
#define TM_OCTOBER	9
#define TM_NOVEMBER	10
#define TM_DECEMBER	11

#define TM_YEAR_BASE	1900

#define EPOCH_YEAR	1970
#define EPOCH_WDAY	TM_THURSDAY

struct lc_time_T {
    const char	*mon[12];
    const char	*month[12];
    const char	*wday[7];
    const char	*weekday[7];
    const char	*X_fmt;
    const char	*x_fmt;
    const char	*c_fmt;
    const char	*am;
    const char	*pm;
    const char	*date_fmt;
    const char	*alt_month[12];
    const char	*md_order;
    const char	*ampm_fmt;
};const struct lc_time_T _C_time_locale =
{
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    }, {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    }, {
        "Sun", "Mon", "Tue", "Wed",
        "Thu", "Fri", "Sat"
    }, {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    },

    /* X_fmt */
    "%H:%M:%S",

    /*
     * x_fmt
     * Since the C language standard calls for
     * "date, using locale's date format," anything goes.
     * Using just numbers (as here) makes Quakers happier;
     * it's also compatible with SVR4.
     */
    "%m/%d/%y",

    /*
     * c_fmt
     */
    "%a %b %e %H:%M:%S %Y",

    /* am */
    "AM",

    /* pm */
    "PM",

    /* date_fmt */
    "%a %b %e %H:%M:%S %Z %Y",

    /* alt_month
     * Standalone months forms for %OB
     */
    {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    },

    /* md_order
     * Month / day order in dates
     */
    "md",

    /* ampm_fmt
     * To determine 12-hour clock format time (empty, if N/A)
     */
    "%I:%M:%S %p"
};
#define __get_current_time_locale(X) (&_C_time_locale)

#define	asizeof(a)	(sizeof(a) / sizeof((a)[0]))

#define	FLAG_NONE	(1 << 0)
#define	FLAG_YEAR	(1 << 1)
#define	FLAG_MONTH	(1 << 2)
#define	FLAG_YDAY	(1 << 3)
#define	FLAG_MDAY	(1 << 4)
#define	FLAG_WDAY	(1 << 5)

/*
 * Calculate the week day of the first day of a year. Valid for
 * the Gregorian calendar, which began Sept 14, 1752 in the UK
 * and its colonies. Ref:
 * http://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
 */
static inline int first_wday_of(int year)
{
    return (((2 * (3 - (year / 100) % 4)) + (year % 100) +
        ((year % 100) / 4) + (isleap(year) ? 6 : 0) + 1) % 7);
}

static const int	mon_lengths[2][MONSPERYEAR] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int	year_lengths[2] = {
    DAYSPERNYEAR, DAYSPERLYEAR
};

#define LEAPS_THRU_END_OF(y)	((y) / 4 - (y) / 100 + (y) / 400)

static void timesub(
        const time_t * const    timep,
        const long				offset,
              etm_t *  const    tmp)
{
    long			days;
    long			 rem;
    long			   y;
    int			   yleap;
    const int *		  ip;

    days = (long)(*timep / SECSPERDAY);
    rem  = *timep % SECSPERDAY;
    rem += (offset);
    while (rem < 0) {
        rem += SECSPERDAY;
        --days;
    }
    while (rem >= SECSPERDAY) {
        rem -= SECSPERDAY;
        ++days;
    }
    tmp->tm_hour = (int) (rem / SECSPERHOUR);
    rem = rem % SECSPERHOUR;
    tmp->tm_min = (int) (rem / SECSPERMIN);
    /*
    ** A positive leap second requires a special
    ** representation.  This uses "... ??:59:60" et seq.
    */
    tmp->tm_sec = (int) (rem % SECSPERMIN) ;
    tmp->tm_wday = (int) ((EPOCH_WDAY + days) % DAYSPERWEEK);
    if (tmp->tm_wday < 0)
        tmp->tm_wday += DAYSPERWEEK;

    y = EPOCH_YEAR;

    while (days < 0 || days >= (long) year_lengths[yleap = isleap(y)]) {
        long	newy;

        newy = y + days / DAYSPERNYEAR;
        if (days < 0)
            --newy;
        days -= (newy - y) * DAYSPERNYEAR +
            LEAPS_THRU_END_OF(newy - 1) -
            LEAPS_THRU_END_OF(y - 1);
        y = newy;
    }
    tmp->tm_year = y - TM_YEAR_BASE;
    tmp->tm_yday = (int) days;
    ip = mon_lengths[yleap];
    for (tmp->tm_mon = 0; days >= (long) ip[tmp->tm_mon]; ++(tmp->tm_mon))
        days = days - (long) ip[tmp->tm_mon];
    tmp->tm_mday = (int) (days + 1);
    tmp->tm_isdst = 0;
}

/*
* Re-entrant version of gmtime.
*/
etm _gmtime_r(const time_t* timep, etm tm)
{
    timesub(timep, 0L, tm);
    return tm;
}

#define gmtime_r _gmtime_r

static char *
_strptime(const char *buf, const char *fmt, etm tm, int *GMTp,
        locale_t locale)
{
    char	c;
    const char *ptr;
    int	day_offset = -1, wday_offset;
    int week_offset;
    int	i, len;
    int flags;
    int Ealternative, Oalternative;
    const struct lc_time_T *tptr = __get_current_time_locale(locale);
    static int start_of_month[2][13] = {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
    };

    flags = FLAG_NONE;

    ptr = fmt;
    while (*ptr != 0) {
        c = *ptr++;

        if (c != '%') {
            if (isspace_l((unsigned char)c, locale))
                while (*buf != 0 &&
                       isspace_l((unsigned char)*buf, locale))
                    buf++;
            else if (c != *buf++)
                return (NULL);
            continue;
        }

        Ealternative = 0;
        Oalternative = 0;
label:
        c = *ptr++;
        switch (c) {
        case '%':
            if (*buf++ != '%')
                return (NULL);
            break;

        case '+':
            buf = _strptime(buf, tptr->date_fmt, tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            flags |= FLAG_WDAY | FLAG_MONTH | FLAG_MDAY | FLAG_YEAR;
            break;

        case 'C':
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            /* XXX This will break for 3-digit centuries. */
            len = 2;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i < 19)
                return (NULL);

            tm->tm_year = i * 100 - TM_YEAR_BASE;
            flags |= FLAG_YEAR;

            break;

        case 'c':
            buf = _strptime(buf, tptr->c_fmt, tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            flags |= FLAG_WDAY | FLAG_MONTH | FLAG_MDAY | FLAG_YEAR;
            break;

        case 'D':
            buf = _strptime(buf, "%m/%d/%y", tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            flags |= FLAG_MONTH | FLAG_MDAY | FLAG_YEAR;
            break;

        case 'E':
            // CockroachDB: unsupported
            return (NULL);
            if (Ealternative || Oalternative)
                break;
            Ealternative++;
            goto label;

        case 'O':
            // CockroachDB: unsupported
            return (NULL);
            if (Ealternative || Oalternative)
                break;
            Oalternative++;
            goto label;

        case 'F':
            buf = _strptime(buf, "%Y-%m-%d", tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            flags |= FLAG_MONTH | FLAG_MDAY | FLAG_YEAR;
            break;

        case 'R':
            buf = _strptime(buf, "%H:%M", tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            break;

        case 'r':
            buf = _strptime(buf, tptr->ampm_fmt, tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            break;

        case 'T':
            buf = _strptime(buf, "%H:%M:%S", tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            break;

        case 'X':
            buf = _strptime(buf, tptr->X_fmt, tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            break;

        case 'x':
            buf = _strptime(buf, tptr->x_fmt, tm, GMTp, locale);
            if (buf == NULL)
                return (NULL);
            flags |= FLAG_MONTH | FLAG_MDAY | FLAG_YEAR;
            break;

        case 'j':
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = 3;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++){
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i < 1 || i > 366)
                return (NULL);

            tm->tm_yday = i - 1;
            flags |= FLAG_YDAY;

            break;

        case 'f':
            /* CockroachDB extension: nanoseconds */
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = 9;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++){
                i *= 10;
                i += *buf - '0';
                len--;
            }
            while (len) {
                i *= 10;
                len--;
            }

            tm->tm_nsec = i;

            break;

        case 'M':
        case 'S':
            if (*buf == 0 ||
                isspace_l((unsigned char)*buf, locale))
                break;

            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = 2;
            for (i = 0; len && *buf != 0 &&
                isdigit_l((unsigned char)*buf, locale); buf++){
                i *= 10;
                i += *buf - '0';
                len--;
            }

            if (c == 'M') {
                if (i > 59)
                    return (NULL);
                tm->tm_min = i;
            } else {
                if (i > 60)
                    return (NULL);
                tm->tm_sec = i;
            }

            break;

        case 'H':
        case 'I':
        case 'k':
        case 'l':
            /*
             * Of these, %l is the only specifier explicitly
             * documented as not being zero-padded.  However,
             * there is no harm in allowing zero-padding.
             *
             * XXX The %l specifier may gobble one too many
             * digits if used incorrectly.
             */
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = 2;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (c == 'H' || c == 'k') {
                if (i > 23)
                    return (NULL);
            } else if (i > 12)
                return (NULL);

            tm->tm_hour = i;

            break;

        case 'p':
            /*
             * XXX This is bogus if parsed before hour-related
             * specifiers.
             */
            len = (int)strlen(tptr->am);
            if (strncasecmp_l(buf, tptr->am, len, locale) == 0) {
                if (tm->tm_hour > 12)
                    return (NULL);
                if (tm->tm_hour == 12)
                    tm->tm_hour = 0;
                buf += len;
                break;
            }

            len = (int)strlen(tptr->pm);
            if (strncasecmp_l(buf, tptr->pm, len, locale) == 0) {
                if (tm->tm_hour > 12)
                    return (NULL);
                if (tm->tm_hour != 12)
                    tm->tm_hour += 12;
                buf += len;
                break;
            }

            return (NULL);

        case 'A':
        case 'a':
            for (i = 0; (uint)i < asizeof(tptr->weekday); i++) {
                len = (int)strlen(tptr->weekday[i]);
                if (strncasecmp_l(buf, tptr->weekday[i],
                        len, locale) == 0)
                    break;
                len = (int)strlen(tptr->wday[i]);
                if (strncasecmp_l(buf, tptr->wday[i],
                        len, locale) == 0)
                    break;
            }
            if (i == asizeof(tptr->weekday))
                return (NULL);

            buf += len;
            tm->tm_wday = i;
            flags |= FLAG_WDAY;
            break;

        case 'U':
        case 'W':
            /*
             * XXX This is bogus, as we can not assume any valid
             * information present in the tm structure at this
             * point to calculate a real value, so just check the
             * range for now.
             */
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = 2;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i > 53)
                return (NULL);

            if (c == 'U')
                day_offset = TM_SUNDAY;
            else
                day_offset = TM_MONDAY;


            week_offset = i;

            break;

        case 'w':
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            i = *buf - '0';
            if (i > 6)
                return (NULL);

            tm->tm_wday = i;
            flags |= FLAG_WDAY;

            break;

        case 'u':
            // CockroachDB extension of the FreeBSD code
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            i = *buf - '0';
            if (i < 1 || i > 7)
                return (NULL);
            if (i == 7)
                i = 0;

            tm->tm_wday = i;
            flags |= FLAG_WDAY;

            break;

        case 'e':
            /*
             * With %e format, our strftime(3) adds a blank space
             * before single digits.
             */
            if (*buf != 0 &&
                isspace_l((unsigned char)*buf, locale))
                   buf++;
            /* FALLTHROUGH */
        case 'd':
            /*
             * The %e specifier was once explicitly documented as
             * not being zero-padded but was later changed to
             * equivalent to %d.  There is no harm in allowing
             * such padding.
             *
             * XXX The %e specifier may gobble one too many
             * digits if used incorrectly.
             */
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = 2;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i > 31)
                return (NULL);

            tm->tm_mday = i;
            flags |= FLAG_MDAY;

            break;

        case 'B':
        case 'b':
        case 'h':
            for (i = 0; (uint)i < asizeof(tptr->month); i++) {
                if (Oalternative) {
                    if (c == 'B') {
                        len = (int)strlen(tptr->alt_month[i]);
                        if (strncasecmp_l(buf,
                                tptr->alt_month[i],
                                len, locale) == 0)
                            break;
                    }
                } else {
                    len = (int)strlen(tptr->month[i]);
                    if (strncasecmp_l(buf, tptr->month[i],
                            len, locale) == 0)
                        break;
                }
            }
            /*
             * Try the abbreviated month name if the full name
             * wasn't found and Oalternative was not requested.
             */
            if (i == asizeof(tptr->month) && !Oalternative) {
                for (i = 0; (uint)i < asizeof(tptr->month); i++) {
                    len = (int)strlen(tptr->mon[i]);
                    if (strncasecmp_l(buf, tptr->mon[i],
                            len, locale) == 0)
                        break;
                }
            }
            if (i == asizeof(tptr->month))
                return (NULL);

            tm->tm_mon = i;
            buf += len;
            flags |= FLAG_MONTH;

            break;

        case 'm':
            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = 2;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (i < 1 || i > 12)
                return (NULL);

            tm->tm_mon = i - 1;
            flags |= FLAG_MONTH;

            break;

        case 's':
            {
            char *cp;
            int sverrno;
            long n;
            time_t t;

            sverrno = errno;
            errno = 0;
            n = strtol_l(buf, &cp, 10, locale);
            if (errno == ERANGE || (long)(t = n) != n) {
                errno = sverrno;
                return (NULL);
            }
            errno = sverrno;
            buf = cp;
            if (gmtime_r(&t, tm) == NULL)
                return (NULL);
            *GMTp = 1;
            flags |= FLAG_YDAY | FLAG_WDAY | FLAG_MONTH |
                FLAG_MDAY | FLAG_YEAR;
            }
            break;

        case 'Y':
        case 'y':
            if (*buf == 0 ||
                isspace_l((unsigned char)*buf, locale))
                break;

            if (!isdigit_l((unsigned char)*buf, locale))
                return (NULL);

            len = (c == 'Y') ? 4 : 2;
            for (i = 0; len && *buf != 0 &&
                 isdigit_l((unsigned char)*buf, locale); buf++) {
                i *= 10;
                i += *buf - '0';
                len--;
            }
            if (c == 'Y')
                i -= TM_YEAR_BASE;
            if (c == 'y' && i < 69)
                i += 100;
//            if (i < 0)
//                return (NULL);

            tm->tm_year = i;
            flags |= FLAG_YEAR;

            break;

        case 'Z':
            {
            const char *cp;
            char *zonestr;

            for (cp = buf; *cp &&
                 isupper_l((unsigned char)*cp, locale); ++cp) {
                /*empty*/}
            if (cp - buf) {
                zonestr = malloc(cp - buf + 1);
                strncpy(zonestr, buf, cp - buf);
                zonestr[cp - buf] = '\0';
                //tzset();
                if (0 == strcmp(zonestr, "GMT") ||
                    0 == strcmp(zonestr, "UTC")) {
                    *GMTp = 1;
                // } else if (0 == strcmp(zonestr, tzname[0])) {
                //     tm->tm_isdst = 0;
                // } else if (0 == strcmp(zonestr, tzname[1])) {
                //     tm->tm_isdst = 1;
                } else {
                    return (NULL);
                }
                buf += cp - buf;
            }
            }
            break;

        case 'z':
            {
            int sign = 1;

            if (*buf != '+') {
                if (*buf == '-')
                    sign = -1;
                else
                    return (NULL);
            }

            buf++;
            i = 0;
            for (len = 4; len > 0; len--) {
                if (isdigit_l((unsigned char)*buf, locale)) {
                    i *= 10;
                    i += *buf - '0';
                    buf++;
                } else
                    return (NULL);
            }

            tm->tm_hour -= sign * (i / 100);
            tm->tm_min  -= sign * (i % 100);
            *GMTp = 1;
            }
            break;

        case 'n':
        case 't':
            while (isspace_l((unsigned char)*buf, locale))
                buf++;
            break;

        default:
            return (NULL);
        }
    }

    if (!(flags & FLAG_YDAY) && (flags & FLAG_YEAR)) {
        if ((flags & (FLAG_MONTH | FLAG_MDAY)) ==
            (FLAG_MONTH | FLAG_MDAY)) {
            tm->tm_yday = start_of_month[isleap(tm->tm_year +
                TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
            flags |= FLAG_YDAY;
        } else if (day_offset != -1) {
            /* Set the date to the first Sunday (or Monday)
             * of the specified week of the year.
             */
            if (!(flags & FLAG_WDAY)) {
                tm->tm_wday = day_offset;
                flags |= FLAG_WDAY;
            }
            tm->tm_yday = (7 -
                first_wday_of(tm->tm_year + TM_YEAR_BASE) +
                day_offset) % 7 + (week_offset - 1) * 7 +
                tm->tm_wday - day_offset;
            flags |= FLAG_YDAY;
        }
    }

    if ((flags & (FLAG_YEAR | FLAG_YDAY)) == (FLAG_YEAR | FLAG_YDAY)) {
        if (!(flags & FLAG_MONTH)) {
            i = 0;
            while (tm->tm_yday >=
                start_of_month[isleap(tm->tm_year +
                TM_YEAR_BASE)][i])
                i++;
            if (i > 12) {
                i = 1;
                tm->tm_yday -=
                    start_of_month[isleap(tm->tm_year +
                    TM_YEAR_BASE)][12];
                tm->tm_year++;
            }
            tm->tm_mon = i - 1;
            flags |= FLAG_MONTH;
        }
        if (!(flags & FLAG_MDAY)) {
            tm->tm_mday = tm->tm_yday -
                start_of_month[isleap(tm->tm_year + TM_YEAR_BASE)]
                [tm->tm_mon] + 1;
            flags |= FLAG_MDAY;
        }
        if (!(flags & FLAG_WDAY)) {
            i = 0;
            wday_offset = first_wday_of(tm->tm_year);
            while (i++ <= tm->tm_yday) {
                if (wday_offset++ >= 6)
                    wday_offset = 0;
            }
            tm->tm_wday = wday_offset;
            flags |= FLAG_WDAY;
        }
    }

    return ((char *)buf);
}

char* estrptime(const char *buf, const char* fmt, etm tm)
{
    int GMP = 0;

    memset(tm, 0, sizeof(*tm));

    if(_strptime(buf, fmt, tm, &GMP, 0))
    {
        if(tm->tm_mday == 0)
            tm->tm_mday = 1;

        return (char*)buf;
    }

    return 0;
}

#define __e_strftime(desc, dlen, dfmt, etm) __strftime(desc, dlen, dfmt, (struct tm const*)etm)

static cstr __elapsestrftm(cstr buf, int len, constr fmt, etm time)
{
    int year_set = 0, day_set = 0;

    if(strstr(fmt, "%Y")) year_set = 1;
    if(strstr(fmt, "%j")) day_set  = 1;

    if(!year_set) time->tm_yday = time->tm_yday + time->tm_year * 365;
    if(!day_set)  time->tm_hour = time->tm_hour + time->tm_yday * 24;

    time->tm_yday--;
    time->tm_year -= 1900;

    return __e_strftime(buf, len, fmt, time) ? buf : 0;
}


//! Converts Gregorian date to seconds since 1970-01-01 00:00:00
static i64 __mktime64(uint year0, uint mon0, uint day, uint hour, uint min, uint sec)
{
    unsigned int mon = mon0, year = year0;

    /* 1..12 -> 11,12,1..10 */
    if (0 >= (int) (mon -= 2)) {
        mon += 12;	/* Puts Feb last since it has leap day */
        year -= 1;
    }

    return ((((i64)
          (year/4 - year/100 + year/400 + 367*mon/12 + day) +
          year*365 - 719499
        )*24 + hour /* now have hours */
      )*60 + min /* now have minutes */
    )*60 + sec; /* finally seconds */
}

static i64 __elapsesecffmt(constr from, constr ffmt)
{
    constr c = ffmt, s = from; cstr end; i64 sec = 0; int num;

    while(*c != '\0')
    {
        switch (*c) {
        case '%': c++;
                  switch (*c) {
                  case 'T': while(isspace(*s)) s++;  // hour
                            num = strtol(s, &end, 10); if(end == s) goto err_ret;
                            sec += num * 3600;

                            s = end;                 // min
                            while(isspace(*s)) s++; if(*s != ':') goto err_ret; s++;
                            while(isspace(*s)) s++;
                            num = strtol(s, &end, 10); if(end == s) goto err_ret;
                            sec += num * 60;

                            s = end;                // sec
                            while(isspace(*s)) s++; if(*s != ':') goto err_ret; s++;
                            while(isspace(*s)) s++;
                            num = strtol(s, &end, 10); if(end == s) goto err_ret;
                            sec += num;

                            s = end;
                            c++;

                            break;

                  case 'H': while(isspace(*s)) s++;  // hour
                            num = strtol(s, &end, 10); if(end == s) goto err_ret;
                            sec += num * 3600;

                            c++;
                            s = end;
                            break;

                  case 'M': while(isspace(*s)) s++;  // min
                            num = strtol(s, &end, 10); if(end == s) goto err_ret;
                            sec += num * 60;

                            c++;
                            s = end;
                            break;

                  case 'S': while(isspace(*s)) s++;  // set
                            num = strtol(s, &end, 10); if(end == s) goto err_ret;
                            sec += num;

                            c++;
                            s = end;
                            break;

                  case 'j': while(isspace(*s)) s++;
                            num = strtol(s, &end, 10); if(s == end) goto err_ret;

                            sec += num * 24 * 3600;

                            c++;
                            s = end;
                            break;
                  case 'Y':
                  case 'y': while(isspace(*s)) s++;
                            num = strtol(s, &end, 10); if(s == end) goto err_ret;

                            //sec += num * 365 * 24 * 3600;
                            sec += __mktime64(num + 1970, 1, 1, 0, 0, 0);

                            c++;
                            s = end;
                            break;

                  default : goto err_ret;;
                  }

                  break;

        default:  if(*c == *s)
                  { c++; s++;}
                  else
                      goto err_ret;
                  break;
        }

    }

    return sec;

err_ret:
    return -1;
}

cstr e_elpstrfstr(cstr dest, int dlen, constr dfmt, constr from, constr ffmt)
{
    i64 elapse_sec = __elapsesecffmt(from, ffmt);

    if(elapse_sec == -1)
    {
        etm_t time;

        if(estrptime(from, ffmt, &time))
        {
            time.tm_year += 1900;
            return __elapsestrftm(dest, dlen, dfmt, &time);
        }
    }

    return e_elpstrfsec(dest, dlen, dfmt, elapse_sec);
}

cstr e_elpstrfsec(cstr dest, int dlen, constr dfmt, time_t sec)
{
    etm_t _tm;

    if(gmtime_r(&sec, &_tm))
    {
        _tm.tm_year -= 70;
        return __elapsestrftm(dest, dlen, dfmt, &_tm);
    }

    return 0;
}

i64 e_elpstrpsec(constr from, constr ffmt)
{
    i64 elapse_sec = __elapsesecffmt(from, ffmt);

    if(elapse_sec == -1)
    {
        int GMP = 0;
        etm_t _tm = {0};

        _tm.tm_year -= 1900;

        if(_strptime(from, ffmt, &_tm, &GMP, 0))
        {
            _tm.tm_year += 1900 + 1970;
            _tm.tm_mon ++;

            elapse_sec  = __mktime64(_tm.tm_year, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec) + _tm.tm_gmtoff;

            if(elapse_sec < 0)
                return -1;
        }
    }

    return elapse_sec;
}

i64 e_elpstrpms(constr from, constr ffmt)
{
    i64 elapse_sec = __elapsesecffmt(from, ffmt);

    if(elapse_sec == -1)
    {
        etm_t _tm = {0};
        _tm.tm_mday = 1;
        _tm.tm_year = -1900;

        if(estrptime(from, ffmt, &_tm))
        {
            _tm.tm_year += 1970;
            elapse_sec  = (mktime((struct tm* const)&_tm) + _tm.tm_gmtoff) * 1000;
        }
    }
    else
        elapse_sec *= 1000;

    return elapse_sec;
}
