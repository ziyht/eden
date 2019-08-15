#include <string.h>

#include "estr.h"
#include "etest.h"

int estr_split_str_test()
{
    char* s; int i;

    esplt split1 = esplt_new(12, 0, 0); // cnt, keep, trim
    esplt split2 = esplt_new(12, 1, 0);
    esplt split3 = esplt_new(12, 0, 1);
    esplt split4 = esplt_new(12, 1, 1);

    esplt split;

    /// ----------------------------------- test 0
    s = "";

    esplt_splitS(split1, s, "|");
    esplt_splitS(split2, s, "|");
    esplt_splitS(split3, s, "|");
    esplt_splitS(split4, s, "|");

    eexpect_num(esplt_cnt(split1), 0);
    eexpect_num(esplt_cnt(split2), 0);
    eexpect_num(esplt_cnt(split3), 0);
    eexpect_num(esplt_cnt(split4), 0);

    /// ----------------------------------- trim test 1
    s = "    ";                     // 4 space

    esplt_splitS(split1, s, "|");   // expect: "    "
    esplt_splitS(split2, s, "|");   // expect: "    "
    esplt_splitS(split3, s, "|");   // expect: ""
    esplt_splitS(split4, s, "|");   // expect: ""

    split = split1;
    eexpect_num(esplt_cnt(split), 1);
    eexpect_str(split[ 0], s);

    split = split2;
    eexpect_num(esplt_cnt(split), 1);
    eexpect_str(split[ 0], s);

    split = split3;
    eexpect_num(esplt_cnt(split), 1);
    eexpect_str(split[ 0], "");

    split = split4;
    eexpect_num(esplt_cnt(split4), 1);
    eexpect_str(split[ 0], "");

//    esplt_show(split1, -1);
//    esplt_show(split2, -1);
//    esplt_show(split3, -1);
//    esplt_show(split4, -1);

    /// ----------------------------------- trim test 2
    s = " a   ";

    esplt_splitS(split1, s, " ");   // expect: ""
    esplt_splitS(split2, s, " ");   // expect: "" "" "" "" ""
    esplt_splitS(split3, s, " ");   // expect: ""
    esplt_splitS(split4, s, " ");   // expect: "" "" "" "" ""

#if 0
    split = split1;
    eexpect_num(esplt_cnt(split), 1);
    eexpect_str(split[ 0], "");

    split = split2;
    eexpect_num(esplt_cnt(split), 5);
    eexpect_str(split[ 0], "");
    eexpect_str(split[ 1], "");
    eexpect_str(split[ 2], "");
    eexpect_str(split[ 3], "");
    eexpect_str(split[ 4], "");

    split = split3;
    eexpect_num(esplt_cnt(split), 1);
    eexpect_str(split[ 0], "");

    split = split4;
    eexpect_num(esplt_cnt(split4), 5);
    eexpect_str(split[ 0], "");
    eexpect_str(split[ 1], "");
    eexpect_str(split[ 2], "");
    eexpect_str(split[ 3], "");
    eexpect_str(split[ 4], "");
#else
    esplt_show(split1, -1);
    esplt_show(split2, -1);
    esplt_show(split3, -1);
    esplt_show(split4, -1);
#endif

    /// ----------------------------------- trim test 3
    s = "    ";

    esplt_splitS(split1, s, "  ");  // expect: ""
    esplt_splitS(split2, s, "  ");  // expect: "" "" ""
    esplt_splitS(split3, s, "  ");  // expect: ""
    esplt_splitS(split4, s, "  ");  // expect: "" "" ""

    esplt_show(split1, -1);
    esplt_show(split2, -1);
    esplt_show(split3, -1);
    esplt_show(split4, -1);

    /// ----------------------------------- trim test 4
    s = "|  bcs | b   | bdfh|sdjflkjkls  |dfjkl |sdjkfl | s df |||| \nsd \t | sdfjks | df ||  |  ab \n";

    esplt_splitS(split1, s, "|");
    esplt_splitS(split2, s, "|");
    esplt_splitS(split3, s, "|");
    esplt_splitS(split4, s, "|");

#if 1
    split = split1; i = 0;
    eexpect_num(esplt_cnt(split), 12);
    eexpect_str(split[i++], "  bcs ");
    eexpect_str(split[i++], " b   ");
    eexpect_str(split[i++], " bdfh");
    eexpect_str(split[i++], "sdjflkjkls  ");
    eexpect_str(split[i++], "dfjkl ");
    eexpect_str(split[i++], "sdjkfl ");
    eexpect_str(split[i++], " s df ");
    eexpect_str(split[i++], " \nsd \t ");
    eexpect_str(split[i++], " sdfjks ");
    eexpect_str(split[i++], " df ");
    eexpect_str(split[i++], "  ");
    eexpect_str(split[i++], "  ab \n");

    split = split2; i = 0;
    eexpect_num(esplt_cnt(split), 17);
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "  bcs ");
    eexpect_str(split[i++], " b   ");
    eexpect_str(split[i++], " bdfh");
    eexpect_str(split[i++], "sdjflkjkls  ");
    eexpect_str(split[i++], "dfjkl ");
    eexpect_str(split[i++], "sdjkfl ");
    eexpect_str(split[i++], " s df ");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], " \nsd \t ");
    eexpect_str(split[i++], " sdfjks ");
    eexpect_str(split[i++], " df ");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "  ");
    eexpect_str(split[i++], "  ab \n");

    split = split3; i = 0;
    eexpect_num(esplt_cnt(split), 12);
    eexpect_str(split[i++], "bcs");
    eexpect_str(split[i++], "b");
    eexpect_str(split[i++], "bdfh");
    eexpect_str(split[i++], "sdjflkjkls");
    eexpect_str(split[i++], "dfjkl");
    eexpect_str(split[i++], "sdjkfl");
    eexpect_str(split[i++], "s df");
    eexpect_str(split[i++], "sd");
    eexpect_str(split[i++], "sdfjks");
    eexpect_str(split[i++], "df");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "ab");

    split = split4; i = 0;
    eexpect_num(esplt_cnt(split), 17);
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "bcs");
    eexpect_str(split[i++], "b");
    eexpect_str(split[i++], "bdfh");
    eexpect_str(split[i++], "sdjflkjkls");
    eexpect_str(split[i++], "dfjkl");
    eexpect_str(split[i++], "sdjkfl");
    eexpect_str(split[i++], "s df");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "sd");
    eexpect_str(split[i++], "sdfjks");
    eexpect_str(split[i++], "df");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "");
    eexpect_str(split[i++], "ab");
#else
    esplt_show(split1, -1);
    esplt_show(split2, -1);
    esplt_show(split3, -1);
    esplt_show(split4, -1);
#endif

    esplt_free(split1);
    esplt_free(split2);
    esplt_free(split3);
    esplt_free(split4);

    return ETEST_OK;
}

int estr_split_arg_test()
{
    char args[] = "abc  -t \"123\" \'\"second\" three\' -x four -s five six";

    estr* argv = 0;

    esplt_splitCmdl(argv, args);

    esplt_show(argv, -1);

    esplt_free(argv);

    return ETEST_OK;
}

int estr_split_unique_test()
{
    char* s = "|  node1 | node2   | node3|node1  |node4 |node7 | node1 |||| node2 | node3 |node3||  |";

    esplt split1 = esplt_new(12, 0, 1);
    esplt split2 = esplt_new(12, 1, 1);

    esplt_splitS(split1, s, "|");
    esplt_splitS(split2, s, "|");

    esplt_unique(split1); eexpect_num(esplt_cnt(split1), 5);
    esplt_unique(split2); eexpect_num(esplt_cnt(split1), 5);

    esplt_show(split1, -1);
    esplt_show(split2, -1);

    esplt_free(split1);
    esplt_free(split2);

    return ETEST_OK;
}

int estr_split_test()
{
    ETEST_RUN(estr_split_str_test());
    ETEST_RUN(estr_split_arg_test());
    ETEST_RUN(estr_split_unique_test());

    return ETEST_OK;
}

int test_split(int argc, char* argv[])
{
    ETEST_RUN(estr_split_test());

    return ETEST_OK;
}
