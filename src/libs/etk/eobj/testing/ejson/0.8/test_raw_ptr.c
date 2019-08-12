#include "test_main.h"

void ejson_raw_ptr_test()
{
    ejson e; cstr s = 0;

    e = ejso_new(_OBJ_);

    ejso_addR(e, "raw1", 100);
    ejso_addR(e, "raw2", 100);
    ejso_addR(e, "raw3", 100);
    ejso_addP(e, "e", e);
    ejso_addP(e, "w", s);
    puts("before set:");
    s = ejso_toFStr(e, s); ejss_show(s);
    puts("");

    ejsk_setR(e, "raw1", 50);
    ejsk_setR(e, "raw2", 100);
    ejsk_setR(e, "raw3", 150);
    puts("raw1 -> 50 raw3 -> 150");
    s = ejso_toFStr(e, s); ejss_show(s);
    puts("");


    ejso_free(e);
    ejss_free(s);
}
