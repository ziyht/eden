#include <stdint.h>
#include <time.h>
#include "test_main.h"
#include "eobj.h"

int main()
{
#if 1


    //ejson_check_test();

    //ejson_cJSON_test();

    ejson_eval_test();
    //ejson_obj_add_test();
    //ejson_obj_get_test();
    //ejson_del_test();

    //ejson_sub_test();

    //ejson_sort_test();

    //ejson_raw_ptr_test();

#else
    ejson_performance_test();
#endif

    printf("ejson version(%d): %s\n", ejss_len(ejson_version()), ejson_version()); fflush(stdout);

#if (_WIN32)
    getchar();
#endif
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
