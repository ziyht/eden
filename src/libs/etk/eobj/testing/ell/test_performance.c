#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include "ell.h"

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

static void performance_obj_test(uint scale)
{
    ell l = ell_new();  u64 t; uint j;

    // appd
    t = utils_timer_now_ms();
    for(i64 i = 0; i < scale; i++)
    {
        ell_appdI(l, i);
    }

    if(ell_size(l) != scale)
    {
        printf("error: add %d elements, but only have %d elements in e\n", scale, ell_size(l)); fflush(stdout);
        exit(1);
    }

    printf("add   %d \telem: %6"PRIi64"ms\n", scale, utils_timer_now_ms() - t); fflush(stdout);

    // traves
    t = utils_timer_now_ms();
    uint cnt = 0;
    ell_foreach(l, obj)
    {
        cnt++;
    }

    if(ell_size(l) != cnt)
    {
        printf("error: list have %d elements, but only traves %d elements\n", ell_size(l), cnt); fflush(stdout);
        exit(1);
    }
    printf("travs %d \telem: %6"PRIu64"ms\n", cnt, utils_timer_now_ms() - t); fflush(stdout);

#if 1
    t = utils_timer_now_ms();
    j = 0;
    for(uint i = 0; i < scale; i++)
    {
        if(ell_i(l, i)) j++;
    }

    if(ell_size(l) != j)
    {
        printf("error: e have %d elements, but only found %d elements in e\n", ell_size(l), j); fflush(stdout);
        exit(1);
    }

    printf("at    %d \telem: %6"PRIu64"ms\n", j, utils_timer_now_ms() - t); fflush(stdout);
#endif

#if 0
    t = utils_timer_now_ms();
    j = 0; srand(0);
    for(uint i = 0; i < scale; i++)
    {
        if(ell_at(l, rand()%scale)) j++;
    }

    if(ell_size(l) != j)
    {
        printf("error: e have %d elements, but only found %d elements in e\n", ell_size(l), j); fflush(stdout);
        exit(1);
    }

    printf("at    %d \telem: %6"PRIu64"ms\n", j, utils_timer_now_ms() - t); fflush(stdout);
#endif

    // release
    t = utils_timer_now_ms();
    ell_free(l);
    printf("free  %d \telem: %6"PRIu64"ms\n\n", j, utils_timer_now_ms() - t); fflush(stdout);
}


int ell_performance_test()
{
    printf("-- performance_obj_test --\n"); fflush(stdout);

    performance_obj_test(10000);
    performance_obj_test(20000);

#if 0
    performance_obj_test(50000);
    performance_obj_test(100000);
    performance_obj_test(200000);
    performance_obj_test(500000);
    performance_obj_test(1000000);
    performance_obj_test(2000000);
#endif

    return ETEST_OK;
}

int test_performance(int argc, char* argv[])
{
    ell_performance_test();

    return ETEST_OK;
}
