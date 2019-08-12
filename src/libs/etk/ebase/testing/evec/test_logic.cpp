/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>
#include "evec.h"
#include "ell.h"

static int random_add_evec(int scale)
{
    int i, id;

    evec v = evec_new(E_I64, 0);

    srand(0);

    for(i = 0; i < scale; i ++)
    {
        id = rand() % (evec_len(v) + 1);
        evec_addI(v, id, i);
        eexpect_num(evec_iValI(v, id), i);
    }
    eexpect_num(evec_len(v), scale);

    u8* check = (u8*)calloc(sizeof(u8), scale);

    for(i = 0; i < scale; i ++)
    {
        eexpect_1(evec_iValI(v, i) < scale);

        check[evec_iValI(v, i)] = 1;
    }

    for(i = 0; i < scale; i ++)
    {
        eexpect_1(check[i] != 0);
    }

    free(check);
    evec_free(v);

    return ETEST_OK;
}

//#include <QVector>
//static int random_add_qvec(int scale)
//{
//    int i, id;

//    QVector<i64> v;

//    srand(0);

//    for(i = 0; i < scale; i ++)
//    {
//        id = rand() % (v.count() + 1);
//        v.insert(id, i);
//        eexpect_num(v.value(id), i);
//    }
//    eexpect_num(v.count(), scale);

//    u8* check = (u8*)calloc(sizeof(u8), scale);

//    for(i = 0; i < scale; i ++)
//    {
//        eexpect_1(v.value(i) < scale);

//        check[v.value(i)] = 1;
//    }

//    for(i = 0; i < scale; i ++)
//    {
//        eexpect_1(check[i]);
//    }

//    free(check);

//    return ETEST_OK;
//}

extern "C"
int test_logic(int argc, char* argv[])
{
    int scale = 10240;

    ETEST_RUN( random_add_evec(scale) );
    //ETEST_RUN( random_add_qvec(scale) );

    return ETEST_OK;
}

