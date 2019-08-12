#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#include "test_main.h"
#include "etype.h"

int64_t utils_timer_now_ms();

uint inner_test(uint cnt, int Segment)
{
    struct ab_t{
        uint a : 1;
        uint b : 2;
        uint c : 3;
        uint d : 4;
        uint e : 5;
        uint f : 6;
        uint g : 11;
    }ab = {1, 2, 3, 4, 5, 6, 11};

    int64_t t;

    t = utils_timer_now_ms();

    uint _cnt = 0;

#define MASK_A 0x80000000   // 1000 0000 0000 0000 0000 0000 0000 0000
#define MASK_B 0x60000000   // 0110 0000 0000 0000 0000 0000 0000 0000
#define MASK_C 0x1C000000   // 0001 1100 0000 0000 0000 0000 0000 0000
#define MASK_D 0x03C00000   // 0000 0011 1100 0000 0000 0000 0000 0000
#define MASK_E 0x003E0000   // 0000 0000 0011 1110 0000 0000 0000 0000
#define MASK_F 0x0001F800   // 0000 0000 0000 0001 1111 1000 0000 0000
#define MASK_G 0x000007FF   // 0000 0000 0000 0000 0000 0111 1111 1111

    if(!Segment)
    {
        uint cd = (1<<31) + (2<<29) + (3<<26) + (4<<22) + (5<<17) + (6<<11) + 11;

        for(uint i = 0; i < cnt; i++)
        {
            if((cd&MASK_A) == (1u<<31) && (cd&MASK_B) == (2<<29) && (cd&MASK_C) == (3<<26) && (cd&MASK_D) == (4<<22) && (cd&MASK_E) == (5<<17) && (cd&MASK_F) == (6<<11) && (cd&MASK_G) == 11 )
                _cnt++;
        }

        printf("!Segment: %6"PRIi64"ms\n", utils_timer_now_ms() - t); fflush(stdout);
    }
    else
    {
        for(uint i = 0; i < cnt; i++)
        {
            if(ab.a == 1 && ab.b == 2 && ab.c == 3 && ab.d == 4 && ab.e == 5 && ab.f == 6 && ab.g == 11)
                _cnt++;
        }
        printf(" Segment: %6"PRIi64"ms\n", utils_timer_now_ms() - t); fflush(stdout);
    }

    return _cnt;
}

int main(int argc, char *argv[])
{
    estr_basic_test();
    estr_subc_test();
    estr_subs_test();
//    estr_auto_create_test();
    estr_split_test();

//    sstr_basic_test();
//    sstr_subc_test();
//    sstr_subs_test();

//    inner_test(10000000, 0);
//    inner_test(10000000, 1);

    //estr_trim_perf_test();
    estr_split_perf_test();

    return 0;
}




static cptr __memmem(conptr haystack, size_t haystacklen, conptr needle, size_t needlelen)
{
    char match; const unsigned char* a, * b, *ja, *jb, *itr, *end; char tag[256] = {0};

    if(haystacklen < needlelen) return 0;

    a = haystack; b = needle;

    for(ja = b, jb = b + needlelen; ja < jb; ja++)
        tag[*(ja)] = 1;

    end = a + haystacklen - needlelen--;
    for (itr = a; itr <= end;)
    {
        for(ja = itr + needlelen, jb = b + needlelen, match = 1; jb >= b; --ja, --jb)
        {
            if (!tag[*ja])
            {
                itr = ja;
                match = 0;
                break;
            }
            if (match && *ja != *jb)
            {
                match = 0;
            }
        }
        if (match)
        {
            return (cptr)itr;
        }
        itr++;
    }

    return 0;
}

static int _memmem(unsigned char * a, int alen, unsigned char * b, int blen)
{
    int i, ja, jb, match, off; char tag[256] = {0};

    for (i = 0; i < blen; ++ i)
    {
        tag[*(b+i)] = 1;
    }

    off = alen - blen--;
    for (i = 0; i <= off;)
    {
        for(ja = i + blen, jb = blen, match = 1; jb >= 0; --ja, --jb)
        {
            if (!tag[a[ja]])
            {
                i = ja;
                match = 0;
                break;
            }
            if (match && a[ja] != b[jb])
            {
                match = 0;
            }
        }
        if (match)
        {
            return i;
        }
        ++ i;
    }
    return -1;
}

#if _WIN32
#include <Windows.h>
#include <sys/timeb.h>
static double hrtime_interval_ = -1;
static CRITICAL_SECTION process_title_lock;
#endif

int64_t utils_timer_now_ms() {
#if _WIN32
    #define PRECISE		1000
    static int64_t offset; LARGE_INTEGER counter;
    if(hrtime_interval_ == 0)
        return 0;
    else if(hrtime_interval_ == -1)
    {
        LARGE_INTEGER perf_frequency; struct timeb tm; LARGE_INTEGER counter;
        InitializeCriticalSection(&process_title_lock);
        if (QueryPerformanceFrequency(&perf_frequency)) {	hrtime_interval_ = 1.0 / perf_frequency.QuadPart;}
        else											{	hrtime_interval_ = 0; return 0;	     			 }

        ftime(&tm);
        QueryPerformanceCounter(&counter);
        offset = tm.time * PRECISE + tm.millitm * PRECISE / 1000 - (int64_t) ((double) counter.QuadPart * hrtime_interval_ * PRECISE);
        return tm.time * PRECISE + tm.millitm * PRECISE / 1000;
    }

    QueryPerformanceCounter(&counter);
    return (int64_t) ((double) counter.QuadPart * hrtime_interval_ * PRECISE) + offset;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000 + (int64_t)ts.tv_nsec / 1000000;
#endif
}

void estr_trim_perf_test()
{
    estr s = 0; i64 t;

#define SRC  "  AA...AA.a.aa.aHelloWorld     :::   \n"
#define CSET " \n"

    int cnt = 10000000;

//    t = utils_timer_now_ms();
//    for(int i = 0; i < cnt; i++)
//    {
//        estr_wrtB(s, SRC, sizeof(SRC) - 1);
//        estr_trim2(s, CSET);
//    }
//    printf("estr_trim2: %6"PRIi64"ms %s\n", utils_timer_now_ms() - t, s); fflush(stdout);

    t = utils_timer_now_ms();
    for(int i = 0; i < cnt; i++)
    {
        estr_wrtB(s, SRC, sizeof(SRC) - 1);
        estr_trim(s, CSET);
    }
    printf("estr_trim : %6"PRId64"ms %s\n", utils_timer_now_ms() - t, s); fflush(stdout);



}
