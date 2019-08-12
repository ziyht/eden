#include "test_main.h"
#include <assert.h>
#include <string.h>

void ejson_check_basic_test()
{

    cstr json_str0 = "";
    cstr json_str1 = "false";
    cstr json_str2 = "true";
    cstr json_str3 = "null";
    cstr json_str4 = "100";
    cstr json_str5 = "100.123";
    cstr json_str6 = "\"this is a str\"";
    cstr json_str7 = "[]";
    cstr json_str8 = "{}";
	
    int ok = 0;

    printf("\n-- ejson_check_test 1 --\n");
    assert(!(ok = ejss_check(json_str0))); assert(0 == strcmp(ejson_errp(), ""));                 printf("%s \t\t-> %s %s\n", json_str0, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str1))); assert(0 == strcmp(ejson_errp(), "false"));            printf("%s \t\t-> %s %s\n", json_str1, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str2))); assert(0 == strcmp(ejson_errp(), "true"));             printf("%s \t\t-> %s %s\n", json_str2, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str3))); assert(0 == strcmp(ejson_errp(), "null"));             printf("%s \t\t-> %s %s\n", json_str3, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str4))); assert(0 == strcmp(ejson_errp(), "100"));              printf("%s \t\t-> %s %s\n", json_str4, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str5))); assert(0 == strcmp(ejson_errp(), "100.123"));          printf("%s \t\t-> %s %s\n", json_str5, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str6))); assert(0 == strcmp(ejson_errp(), "\"this is a str\""));printf("%s \t-> %s %s\n",   json_str6, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str7))); assert(0 == strcmp(ejson_errp(), "[]"));               printf("%s \t\t-> %s %s\n", json_str7, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert( (ok = ejss_check(json_str8)));                                                        printf("%s \t\t-> %s %s\n", json_str8, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    fflush(stdout);

    json_str0 = "\"key0\":";
    json_str1 = "\"key1\":false";
    json_str2 = "\"key2\":true";
    json_str3 = "\"key3\":null";
    json_str4 = "\"key4\":100";
    json_str5 = "\"key5\":100.123";
    json_str6 = "\"key6\":\"this is a str\"";
    json_str7 = "\"key7\":[]";
    json_str8 = "\"key8\":{}";
    assert(!(ok = ejss_check(json_str0))); assert(0 == strcmp(ejson_errp(), "\"key0\":"));                    printf("%s \t\t-> %s %s\n", json_str0, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str1))); assert(0 == strcmp(ejson_errp(), "\"key1\":false"));               printf("%s \t\t-> %s %s\n", json_str1, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str2))); assert(0 == strcmp(ejson_errp(), "\"key2\":true"));                printf("%s \t\t-> %s %s\n", json_str2, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str3))); assert(0 == strcmp(ejson_errp(), "\"key3\":null"));                printf("%s \t\t-> %s %s\n", json_str3, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str4))); assert(0 == strcmp(ejson_errp(), "\"key4\":100"));                 printf("%s \t\t-> %s %s\n", json_str4, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str5))); assert(0 == strcmp(ejson_errp(), "\"key5\":100.123"));             printf("%s \t\t-> %s %s\n", json_str5, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str6))); assert(0 == strcmp(ejson_errp(), "\"key6\":\"this is a str\""));   printf("%s \t-> %s %s\n",   json_str6, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str7))); assert(0 == strcmp(ejson_errp(), "\"key7\":[]"));                  printf("%s \t\t-> %s %s\n", json_str7, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    assert(!(ok = ejss_check(json_str8))); assert(0 == strcmp(ejson_errp(), "\"key8\":{}"));                  printf("%s \t\t-> %s %s\n", json_str8, ok ? "ok" : "err:",ok ? "" : ejson_errp());
    fflush(stdout);

    cstr json_str = "{"
                       "\"false\":false, #asdasdfdsasdf\n"
                       "\"true\":true ,//asdfasdfasdf\n"
                       "\"null\":null,/*sdfasdgdfsgsdfgsdfgsdfgsdfg*/"
                       "\"int\":100, "
                       "\"double\":100.123, "
                       "\"str\":\"this is a str obj\","
                       "\"arr\":[false, true, null, 100, 100.123, \"this is a str in arr\", {}],"
                       "\"obj\":{"
                           "\"false\":false, "
                           "\"true\":true ,"
                           "\"null\":null, "
                           "\"int\":100, "
                           "\"double\":100.123, "
                           "\"str\":\"this is a str obj\","
                           "\"arr\":[false, true, null, 100, 100.123, \"this is a str in arr\", {}],"
                           "\"obj\":{}"
                       "}"
                   "}";



    printf("\n-- ejson_check_test 2 --\n");
    printf("src: %s\n", json_str);

    assert( (ok = ejss_checkOpts(json_str, 0, CMMT_ON)));  printf("[comment on ] \t-> %s %s\n", ok ? "ok " : "err",ok ? "" : ejson_errp());
    assert(!(ok = ejss_checkOpts(json_str, 0, CMMT_OFF))); assert(0 == strncmp(ejson_errp(), "#asdasdfdsasdf", 10));printf("[comment off] \t-> %s %s\n", ok ? "ok " : "err:",ok ? "" : ejson_errp());

    printf("\n");
    fflush(stdout);
}

void ejson_check_test()
{
    ejson_check_basic_test();
}

int test_check(int argc, char* argv[])
{
    ejson_check_test();

    return ETEST_OK;
}
