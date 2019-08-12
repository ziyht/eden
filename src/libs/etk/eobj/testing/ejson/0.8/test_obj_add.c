#include "test_main.h"

void ejson_obj_add_test()
{
    ejson e; cstr k, ks, s, os; double d; int i;

    printf("\n-- ejson_obj_add_test 1: ejso_addE() --\n");
    e = ejso_new(_OBJ_);
    printf("e: %s \n", os = ejso_toUStr(e, 0)); k = 0;
    ejso_addE(e, 0, s = "\"null\":null");              printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, 0, s = "\"false\":false");            printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, 0, s = "\"true\":true");              printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, 0, s = "\"int\": 100");               printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, 0, s = "\"double\":100.132");         printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, 0, s = "\"str\":\"this is a str\"");  printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, 0, s = "\"arr\": []");                printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, 0, s = "\"obj\": {}");                printf("add eval to e: k:%s val:%s \n", k, s);
    printf("e to pretty str:\n%s\n", os = ejso_toFStr(e, os)); ejso_free(e); fflush(stdout);

    printf("\n-- ejson_obj_add_test 2: ejso_addE() --\n");
    e = ejso_new(_OBJ_);
    printf("e: %s \n", os = ejso_toUStr(e, os));
    ejso_addE(e, k = "key1", s = "\"1\":null");               printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key2", s = "\"2\":false");              printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key3", s = "\"3\":true");               printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key4", s = "\"4\": 100");               printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key5", s = "\"5\":100.132");            printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key6", s = "\"6\":\"this is a str\"");  printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key7", s = "\"6\": []");                printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key8", s = "\"7\": {}");                printf("add eval to e: k:%s val:%s \n", k, s);
    printf("e to pretty str:\n%s\n", os = ejso_toFStr(e, os)); ejso_free(e); fflush(stdout);

    printf("\n-- ejson_obj_add_test 3: ejso_addE() --\n");
    e = ejso_new(_OBJ_);
    printf("e: %s \n", os = ejso_toUStr(e, os));
    ejso_addE(e, k = "key1", s = "null");             printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key2", s = "false");            printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key3", s = "true");             printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key4", s = "100");              printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key5", s = "100.132");          printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key6", s = "\"this is a str\"");    printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key7", s = "[]");               printf("add eval to e: k:%s val:%s \n", k, s);
    ejso_addE(e, k = "key8", s = "{}");               printf("add eval to e: k:%s val:%s \n", k, s);
    printf("e to pretty str:\n%s\n", os = ejso_toFStr(e, os)); ejso_free(e); fflush(stdout);

    printf("\n-- ejson_obj_add_test 4: ejso_add*() --\n");
    e = ejso_new(_OBJ_);
    printf("e: %s \n", os = ejso_toUStr(e, os));
    ejso_addT(e, k = "key1", _FALSE_);                  printf("add false  to e: k:%s val:none \n", k);
    ejso_addT(e, k = "key2", _TRUE_);                   printf("add true   to e: k:%s val:none \n", k);
    ejso_addT(e, k = "key3", _NULL_);                   printf("add null   to e: k:%s val:none \n", k);
    ejso_addN(e, k = "key4", i = 100);                  printf("add number to e: k:%s val:%d   \n", k, i);
    ejso_addN(e, k = "key5", d = 100.1214);             printf("add number to e: k:%s val:%f   \n", k, d);
    ejso_addS(e, k = "key6", s = "this is a str");      printf("add str    to e: k:%s val:%s   \n", k, s);
    ejso_addT(e, k = "key7", _ARR_);                    printf("add arrar  to e: k:%s val:none   \n", k);
    ejso_addT(e, k = "key8", _OBJ_);                    printf("add obj    to e: k:%s val:none   \n", k);
    printf("to pretty str:\n%s\n", os = ejso_toFStr(e, os)); ejso_free(e); fflush(stdout);


    printf("\n-- ejson_obj_add_test 5: ejsk_addE() --\n");
    e = ejss_eval("{}");
    ejso_addO(e, 0, ejss_eval("\"obj\":{}"));
    printf("e: %s \n", os = ejso_toUStr(e, os));
    ks = "obj";
    ejsk_addE(e, ks, 0, s = "\"null\":null");             printf("add eval to e.%s: %s \n", ks, s);
    ejsk_addE(e, ks, 0, s = "\"false\":false");           printf("add eval to e.%s: %s \n", ks, s);
    ejsk_addE(e, ks, 0, s = "\"true\":true");             printf("add eval to e.%s: %s \n", ks, s);
    ejsk_addE(e, ks, 0, s = "\"int\": 100");              printf("add eval to e.%s: %s \n", ks, s);
    ejsk_addE(e, ks, 0, s = "\"double\":100.132");        printf("add eval to e.%s: %s \n", ks, s);
    ejsk_addE(e, ks, 0, s = "\"str\":\"this is a str\""); printf("add eval to e.%s: %s \n", ks, s);
    ejsk_addE(e, ks, 0, s = "\"arr\": []");               printf("add eval to e.%s: %s \n", ks, s);
    ejsk_addE(e, ks, 0, s = "\"obj\": {}");               printf("add eval to e.%s: %s \n", ks, s);
    printf("to pretty str:\n%s\n", os = ejso_toFStr(e, os)); ejso_free(e); fflush(stdout);

    printf("\n-- ejson_obj_add_test 6: ejsk_add*() --\n");
    e = ejss_eval("{}");
    ejso_addO(e, 0, ejss_eval("\"obj\":{}"));
    ejsk_addO(e, "obj", 0, ejss_eval("\"obj\":{}"));
    printf("e: %s \n", os = ejso_toUStr(e, os));
    ks = "obj";
    ejsk_addT(e, ks, k = "key1", _FALSE_);               printf("add false  to e.%s: k:%s val:none \n", ks, k);
    ejsk_addT(e, ks, k = "key2", _TRUE_);                printf("add true   to e.%s: k:%s val:none \n", ks, k);
    ejsk_addT(e, ks, k = "key3", _NULL_);                printf("add null   to e.%s: k:%s val:none \n", ks, k);
    ejsk_addN(e, ks, k = "key4", i = 100);               printf("add number to e.%s: k:%s val:%d   \n", ks, k, i);
    ejsk_addN(e, ks, k = "key5", d = 100.1214);          printf("add number to e.%s: k:%s val:%f   \n", ks, k, d);
    ejsk_addS(e, ks, k = "key6", s = "this is a str");   printf("add str    to e.%s: k:%s val:%s   \n", ks, k, s);
    ejsk_addT(e, ks, k = "key7", _ARR_);                 printf("add arrar  to e.%s: k:%s val:none \n", ks, k);
    ejsk_addT(e, ks, k = "key8", _OBJ_);                 printf("add obj    to e.%s: k:%s val:none \n", ks, k);
    printf("to pretty str:\n%s\n", os = ejso_toFStr(e, os)); ejso_free(e); fflush(stdout);

    ejss_free(os);

}

int test_obj_add(int argc, char* argv[])
{
    ejson_obj_add_test();

    return ETEST_OK;
}
