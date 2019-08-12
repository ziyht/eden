#include "test_main.h"

int main()
{
    elist_etest_basic();

    //elist_basic_test();
    elist_performance_test();

#if(_WIN32)
    getchar();
#endif

}
