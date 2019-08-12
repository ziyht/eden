#include "test_main.h"


void ejson_obj_get_basic_test()
{
    cstr json_str = "{"
                       "\"key1\":false, "
                       "\"key2\":true ,"
                       "\"key3\":null, "
                       "\"key4\":100, "
                       "\"key5\":100.123, "
                       "\"key6\":\"this is a str obj\","
                       "\"key7\":[false, true, null, 100, 100.123, \"this is a str in arr\", [], {}],"
                       "\"key8\":{}"
                   "}";

    ejson e = ejss_eval(json_str);
    cstr us = ejso_toFStr(e, 0);
    printf("test json(%d):\n%s\n", ejss_len(us), us);

    printf("\n-- ejson_obj_get_basic_test 1 --\n");
    printf("Keys      Type\tValToUStr\n------------------------------\n");
    printf(".key1   : %s\t%s\n", ejso_typeS(ejsk(e, "key1" )),                us = ejsk_toUStr(e, "key1", us));
    printf(".key2   : %s\t%s\n", ejso_typeS(ejsk(e, "key2" )),                us = ejsk_toUStr(e, "key2", us));
    printf(".key3   : %s\t%s\n", ejso_typeS(ejsk(e, "key3" )),                us = ejsk_toUStr(e, "key3", us));
    printf(".key4   : %s\t%s\n", ejso_typeS(ejsk(e, "key4" )),                us = ejsk_toUStr(e, "key4", us));
    printf(".key5   : %s\t%s\n", ejso_typeS(ejsk(e, "key5" )),                us = ejsk_toUStr(e, "key5", us));
    printf(".key6   : %s\t%s\n", ejso_typeS(ejsk(e, "key6" )),                us = ejsk_toUStr(e, "key6", us));
    printf(".key7(%d): %s\t%s\n", ejsk_len(e, "key7"), ejsk_typeS(e, "key7"), us = ejsk_toUStr(e, "key7", us));
    printf(".key8(%d): %s\t%s\n", ejsk_len(e, "key8"), ejsk_typeS(e, "key8"), us = ejsk_toUStr(e, "key8", us));

    printf("\n-- ejson_obj_get_basic_test 2 --\n");
    printf("Keys      Type\tValToUStr\n------------------------------\n");
    printf(".key7[0]: %s\t%s\n", ejsk_typeS(e, "key7[0]" ), us = ejsk_toUStr(e, "key7[0]", us));
    printf(".key7[1]: %s\t%s\n", ejsk_typeS(e, "key7[1]" ), us = ejsk_toUStr(e, "key7[1]", us));
    printf(".key7[2]: %s\t%s\n", ejsk_typeS(e, "key7[2]" ), us = ejsk_toUStr(e, "key7[2]", us));
    printf(".key7[3]: %s\t%s\n", ejsk_typeS(e, "key7[3]" ), us = ejsk_toUStr(e, "key7[3]", us));
    printf(".key7[4]: %s\t%s\n", ejsk_typeS(e, "key7[4]" ), us = ejsk_toUStr(e, "key7[4]", us));
    printf(".key7[5]: %s\t%s\n", ejsk_typeS(e, "key7[5]" ), us = ejsk_toUStr(e, "key7[5]", us));
    printf(".key7[6]: %s\t%s\n", ejsk_typeS(e, "key7[6]" ), us = ejsk_toUStr(e, "key7[6]", us));
    printf(".key7[7]: %s\t%s\n", ejsk_typeS(e, "key7[7]" ), us = ejsk_toUStr(e, "key7[7]", us));

    ejson itr;
    printf("\n-- ejson_obj_get_basic_test 3 --\n");
    printf("Keys   Type\tValToUStr\n------------------------------\n");
    for(itr = ejso_first(e); itr; itr = ejso_next(itr))
    {
        printf(".%s: %s\t%s\n", ejso_keyS(itr), ejso_typeS(itr), us = ejso_toUStr(itr, us));
    }
    for(itr = ejso_last(e); itr; itr = ejso_prev(itr))
    {
        printf(".%s: %s\t%s\n", ejso_keyS(itr), ejso_typeS(itr), us = ejso_toUStr(itr, us));
    }

    printf("\n-- ejson_obj_get_basic_test 4 --\n");
    printf("Keys     Type\ttrue or false\n----------------------------------\n");
    printf(".key1 is false:\t%s\n", ejsk_is(e, "key1", _FALSE_)? "true" : "false");
    printf(".key2 is true :\t%s\n", ejsk_is(e, "key2", _TRUE_) ? "true" : "false");
    printf(".key3 is null :\t%s\n", ejsk_is(e, "key3", _NULL_) ? "true" : "false");
    printf(".key4 is num  :\t%s\n", ejsk_is(e, "key4", _NUM_)  ? "true" : "false");
    printf(".key5 is num  :\t%s\n", ejsk_is(e, "key5", _NUM_)  ? "true" : "false");
    printf(".key6 is str  :\t%s\n", ejsk_is(e, "key6", _STR_)  ? "true" : "false");
    printf(".key7 is arr  :\t%s\n", ejsk_is(e, "key7", _ARR_)  ? "true" : "false");
    printf(".key8 is obj  :\t%s\n", ejsk_is(e, "key8", _OBJ_)  ? "true" : "false");

    printf("\n-- ejson_obj_get_basic_test 5 --\n");
    printf("Keys     Type\ttrue or false\n----------------------------------\n");
    printf(".key1 is false:\t%s\n", ejsk_is(e, "key1", _FALSE_)? "true" : "false");
    printf(".key2 is true :\t%s\n", ejsk_is(e, "key2", _TRUE_) ? "true" : "false");
    printf(".key3 is null :\t%s\n", ejsk_is(e, "key3", _NULL_) ? "true" : "false");
    printf(".key4 is num  :\t%s\n", ejsk_is(e, "key4", _NUM_)  ? "true" : "false");
    printf(".key5 is num  :\t%s\n", ejsk_is(e, "key5", _NUM_)  ? "true" : "false");
    printf(".key6 is str  :\t%s\n", ejsk_is(e, "key6", _STR_)  ? "true" : "false");
    printf(".key7 is arr  :\t%s\n", ejsk_is(e, "key7", _ARR_)  ? "true" : "false");
    printf(".key8 is obj  :\t%s\n", ejsk_is(e, "key8", _OBJ_)  ? "true" : "false");

    ejso_free(e);
    ejss_free(us);
    printf("\n"); fflush(stdout);
}

void ejson_obj_deep_get_test()
{
    cstr json_str = "{"
                        "\"1\": {\"2\": {\"3\": [[[\"val1\"], {\"4\":\"val2\"}]]}}"
                   "}";

    printf("-- ejson_obj_deep_get_test --\n");
    ejson e = ejss_eval(json_str);

    cstr us = ejso_toFStr(e, 0);
    printf("test json:\n%s\n", us);

    printf("Keys             Type\tValToUStr\n----------------------------------------\n");
    printf(".1             : %s\t%s\n", ejsk_typeS(e, "1"              ), us = ejsk_toUStr(e, "1"             , us));
    printf(".1.2           : %s\t%s\n", ejsk_typeS(e, "1.2"            ), us = ejsk_toUStr(e, "1.2"           , us));
    printf(".1.2.3         : %s\t%s\n", ejsk_typeS(e, "1.2.3"          ), us = ejsk_toUStr(e, "1.2.3"         , us));
    printf(".1.2.3[0]      : %s\t%s\n", ejsk_typeS(e, "1.2.3[0]"       ), us = ejsk_toUStr(e, "1.2.3[0]"      , us));
    printf(".1.2.3[0][0]   : %s\t%s\n", ejsk_typeS(e, "1.2.3[0][0]"    ), us = ejsk_toUStr(e, "1.2.3[0][0]"   , us));
    printf(".1.2.3[0][0][0]: %s\t%s\n", ejsk_typeS(e, "1.2.3[0][0][0]" ), us = ejsk_toUStr(e, "1.2.3[0][0][0]", us));
    printf(".1.2.3[0][1]   : %s\t%s\n", ejsk_typeS(e, "1.2.3[0][1]"    ), us = ejsk_toUStr(e, "1.2.3[0][1]"   , us));
    printf(".1.2.3[0][1].4 : %s\t%s\n", ejsk_typeS(e, "1.2.3[0][1].4"  ), us = ejsk_toUStr(e, "1.2.3[0][1].4" , us));

    ejso_free(e);
    ejss_free(us);
    printf("\n"); fflush(stdout);
}

void ejson_arr_get_test()
{
    cstr json_str = "["
                        "{\"2\": {\"3\": [[[\"val1\"], {\"4\":\"val2\"}]]}}"
                   "]";

    printf("-- ejson_arr_get_test --\n");
    ejson e = ejss_eval(json_str);

    cstr us = ejso_toFStr(e, 0);
    printf("test json:\n%s\n", us);

    printf("Keys             Type\tValToUStr\n----------------------------------------\n");
    printf("[0]             : %s\t%s\n", ejsk_typeS(e, "[0]"             ), us = ejsk_toUStr(e, "[0]"             , us));

    ejso_free(e);
    ejss_free(us);
    printf("\n"); fflush(stdout);
}

void ejson_obj_get_test()
{
    ejson_obj_get_basic_test();
    ejson_obj_deep_get_test();
    ejson_arr_get_test();
}

int test_obj_get(int argc, char* argv[])
{
    ejson_obj_get_test();

    return ETEST_OK;
}
