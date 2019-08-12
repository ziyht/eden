
#include "ecompat.h"
#include "etype.h"


//#include <string.h>
//#include <stdbool.h>
//#include "etest.h"
//#include "erb.h"
//#include "eutils.h"

//int erb_set_i_test(erb rb, int cnt)
//{
//    for(int i = 0; i < cnt; i++)
//    {
////        erb_setI(rb, ekey_i(i), i);

////        eexpect_num(erb_kValI(rb, ekey_i(i)), i);
////        eexpect_num(erb_kLen (rb, ekey_i(i)), 0);
////        eexpect_num(erb_kType(rb, ekey_i(i)), ENUM);
//    }

//    return ETEST_OK;
//}

//int erb_set_f_test(erb rb, int cnt)
//{
//    for(int i = 0; i < cnt; i++)
//    {
////        erb_setF(rb, ekey_i(i), i * 1.0);

////        eexpect_num(erb_kValF(rb, ekey_i(i)), i * 1.0);
////        eexpect_num(erb_kLen (rb, ekey_i(i)), 0);
////        eexpect_num(erb_kType(rb, ekey_i(i)), ENUM);
//    }

//    return ETEST_OK;
//}

//int erb_set_p_test(erb rb, int cnt)
//{
//    for(int i = 0; i < cnt; i++)
//    {
////        erb_setP(rb, ekey_i(i), (cptr)&i);

////        eexpect_ptr(erb_kValP(rb, ekey_i(i)), (cptr)&i);
////        eexpect_num(erb_kLen (rb, ekey_i(i)), 0);
////        eexpect_num(erb_kType(rb, ekey_i(i)), EPTR);
//    }

//    return ETEST_OK;
//}

//int erb_set_s_test(erb rb, int cnt)
//{
//    char buf[128];

//    for(int i = 0; i < cnt; i++)
//    {
////        memset(buf, i, i);
////        buf[i] = '\0';

////        erb_setS(rb, ekey_i(i), buf);

////        eexpect_str(erb_kValS(rb, ekey_i(i)), buf);
////        eexpect_num(erb_kLen (rb, ekey_i(i)), i);
////        eexpect_num(erb_kType(rb, ekey_i(i)), ESTR);
//    }

//    return ETEST_OK;
//}

//int erb_set_r_test(erb rb, int cnt)
//{
//    char buf[1280] = {0};

//    for(int i = 0; i < cnt; i++)
//    {
////        erb_setR(rb, ekey_i(i), i * 100);

////        eexpect_raw(erb_kValR(rb, ekey_i(i)), buf, i * 100);
////        eexpect_num(erb_kLen (rb, ekey_i(i)), i * 100);
////        eexpect_num(erb_kType(rb, ekey_i(i)), ERAW);
//    }

//    return ETEST_OK;
//}

//int erb_set_types_test(int cnt)
//{
//    //erb rb = erb_new(EKEY_I);

////    ETEST_RUN(erb_set_i_test(rb, cnt));
////    ETEST_RUN(erb_set_f_test(rb, cnt));
////    ETEST_RUN(erb_set_p_test(rb, cnt));
////    ETEST_RUN(erb_set_s_test(rb, cnt));
////    ETEST_RUN(erb_set_r_test(rb, cnt));

//    //erb_free(rb);

//    return ETEST_OK;
//}

int test_set(int argc, char* argv[])
{
    //ETEST_RUN(erb_set_types_test(10));

    return ETEST_OK;
}
