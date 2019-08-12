#include "test_main.h"

int main()
{
#if 1
    erb_add_test();
    //erb_find_test();
    //erb_obj_test();
    erb_set_test();

#else
    erb_performance_test();
#endif
#ifdef _WIN32
    getchar();
#endif

    return 0;
}
