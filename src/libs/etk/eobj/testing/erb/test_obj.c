#include <stdlib.h>
#include "test_main.h"


typedef struct obj_s
{
    int    keyI;
    char   keyS[16];

    int    dataI;
    char   dataS[16];

    cptr   dynamic_data;
}obj_t, * obj;


void obj_init(obj o, uint len)
{
    if(len == sizeof(obj_t))
        o->dynamic_data = malloc(4);
}

void obj_rls(obj o, uint len)
{
    if(len == sizeof(obj_t) && o->dynamic_data)
        free(o->dynamic_data);
}

int obj_cmp(obj o1, obj o2)
{
    int type1, type2;

    type1 = eobj_typeo((eobj)o1);
    type2 = eobj_typeo((eobj)o2);

    if(type1 == type2)
    {
        if(type1 != EOBJ) return 1;
        else
            return o1->keyI < o2->keyI ? -1 : o1->keyI == o2->keyI ? 0 : 1;
    }

    return type1 < type2;
}

int erb_obj_test()
{
//    erb_type_t my_type = {
//        sizeof(obj_t),
//        0,
//        (erb_initial_cb)obj_init,
//        (erb_release_cb)obj_rls,
//        (erb_compare_cb)obj_cmp
//    };


//    erb rb = erb_new(ERB_MULT, &my_type);

//    obj o; obj itr;

//    for(int i = 20; i > 0; i--)
//    {
//        o = (obj)erb_newO(rb, 0);

//        o->keyI = i;

//        erb_addO(rb, (ekey){0}, (eobj)o);
//    }

//    printf("erb len: %d\n", erb_len(rb));

//    for(itr = (cptr)erb_first(rb); itr; itr = (cptr)erb_next((eobj)itr))
//    {
//        printf("%"PRId64" %d\n", eobj_keyI((eobj)itr), itr->keyI);
//    }

//    erb_free(rb);
    return ETEST_OK;
}

int test_obj(int argc, char* argv[])
{
    erb_obj_test();

    return ETEST_OK;
}
