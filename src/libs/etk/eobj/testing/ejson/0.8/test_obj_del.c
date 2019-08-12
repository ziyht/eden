#include "test_main.h"


void ejson_del_basic()
{
    cstr json_str = "{"
                       "\"false\":false, "
                       "\"true\":true ,"
                       "\"null\":null, "
                       "\"int\":100, "
                       "\"double\":100.123, "
                       "\"str\":\"this is a str obj\","
                       "\"arr\":[false, true, null, 100, 100.123, \"this is a str in arr\", {}]"
                   "}";

    ejson e = ejss_eval(json_str);
    cstr s = ejso_toFStr(e, 0);
    printf("\n-- ejson_obj_del_test 1: ejson_delK() --\n");
    printf("test json(%d):(%d element)\n%s\n", ejss_len(s), ejso_len(e), s);

    ejson del;
    del = ejso_rmK(e, "false"); printf("del \"false\" :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s)); ejso_free(del);
    del = ejso_rmK(e, "true");  printf("del \"true\"  :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s)); ejso_free(del);
    del = ejso_rmK(e, "null");  printf("del \"null\"  :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s)); ejso_free(del);
    del = ejso_rmK(e, "int");   printf("del \"int\"   :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s)); ejso_free(del);
    del = ejso_rmK(e, "double");printf("del \"double\":(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s)); ejso_free(del);
    del = ejso_rmK(e, "str");   printf("del \"str\"   :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s)); ejso_free(del);
    del = ejso_rmK(e, "arr");   printf("del \"arr\"   :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s)); ejso_free(del);
    ejso_free(e);

    printf("\n-- ejson_obj_del_test 2: ejson_del() --\n");
    e = ejso_new(_OBJ_);
    ejson add[8];
    add[0] = ejso_addE(e, "false", "false");
    add[1] = ejso_addE(e, "true", "true");
    add[2] = ejso_addE(e, "null", "null");
    add[3] = ejso_addE(e, "int", "100");
    add[4] = ejso_addE(e, "double", "100.123");
    add[5] = ejso_addE(e, "str", "\"this is a str obj\"");
    add[6] = ejso_addE(e, "arr", "[false, true, null, 100, 100.123, \"this is a str in arr\", {}]");
    add[7] = ejso_addE(e, "obj", "{}");
    s = ejso_toFStr(e, s);
    printf("test json(%d):(%d element)\n%s\n", ejss_len(s), ejso_len(e), s);
    del = ejso_rmO(e, add[0]);printf("del \"false\" :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    del = ejso_rmO(e, add[1]);printf("del \"true\"  :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    del = ejso_rmO(e, add[2]);printf("del \"null\"  :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    del = ejso_rmO(e, add[3]);printf("del \"int\"   :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    del = ejso_rmO(e, add[4]);printf("del \"double\":(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    del = ejso_rmO(e, add[5]);printf("del \"str\"   :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    del = ejso_rmO(e, add[6]);printf("del \"arr\"   :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    del = ejso_rmO(e, add[7]);printf("del \"obj\"   :(%d element)\n%s\n", ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);

    ejso_free(e);
    ejss_free(s);
    printf("\n"); fflush(stdout);
}

void ejson_deep_del_test()
{
    cstr json_str = "{"
                        "\"1\": {\"2\": {\"3\": \"val\"}}"
                   "}";
    ejson e, del; cstr s, k;

    printf("\n-- ejson_obj_del_test 3: ejson_delK() deep test --\n");
    e = ejss_eval(json_str);
    s = ejso_toFStr(e, 0);
    printf("test json(%d):(%d element)\n%s\n", ejss_len(s), ejso_len(e), s);
    k ="1.2.3"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="1.2";   del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="1";     del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);

    ejso_free(e);
    ejss_free(s);
    printf("\n"); fflush(stdout);
}

void ejson_arr_del_test()
{
    cstr json_str = "{"
                        "\"arr\": [false, true, null, 100, 100.123, \"this is a str obj\", [false, true, null, 100, 100.123, \"this is a str obj\", [], {}],{}]"
                   "}";
    ejson e, del; cstr s, k;
    printf("\n-- ejson_obj_del_test 4: ejson_delk() arr test --\n");
    e = ejss_eval(json_str);
    s = ejso_toFStr(e, 0);
    printf("test json(%d):(%d element)\n%s\n", ejss_len(s), ejso_len(e), s);
    k ="arr[6][7]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[6][6]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[6][1]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[6][0]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[6][0]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[6][0]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[6][0]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[6][0]"; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[5]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[2]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[3]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[4]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[3]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[1]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[0]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[0]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr[3]"   ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);
    k ="arr"      ; del = ejso_rmK(e, k);printf("del \"%s\" :(%d element)\n%s\n", k, ejso_len(e), s = ejso_toFStr(e, s));ejso_free(del);

    ejso_free(e);
    ejss_free(s);
    printf("\n"); fflush(stdout);
}

void ejson_del_test()
{
    ejson_del_basic();
    ejson_deep_del_test();
    ejson_arr_del_test();
}

int test_obj_del(int argc, char* argv[])
{
    ejson_del_test();

    return ETEST_OK;
}
