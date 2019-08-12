/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "etime.h"

static int test_strfstr()
{
    char dest[64];

    typedef struct {
        constr from;
        constr ffmt;
        constr dfmt;
        constr expect;
    }Case;

    Case cases[] =
    {
        { "17:43:03"            , "%T"          , "%F %T", "1900-01-01 17:43:03" },
        { "17:43:03 2018"       , "%T %Y"       , "%F %T", "2018-01-01 17:43:03" },
        { "Jun 17:43:03 2018"   , "%b %T %Y"    , "%F %T", "2018-06-01 17:43:03" },
        { "Jun 27 17:43:03 2018", "%b %e %T %Y" , "%F %T", "2018-06-27 17:43:03" },

        {0,0,0,0}
    };

    for(int i = 0; i < 100; i++)
    {
        Case* c = &cases[i];

        if(!c->from)
            break;

        e_strfstr(dest, 64, c->dfmt, c->from, c->ffmt);
        eexpect_str(dest, c->expect);
    }

    return ETEST_OK;
}

static int test_strfsec()
{
    char dest[64];

    typedef struct {
        int    from;
        constr dfmt;
        constr expect;
    }Case;

    Case cases[] =
    {
        { 31723506  , "%F %T", "1971-01-03 12:05:06" },
        { 1530092583, "%F %T", "2018-06-27 17:43:03" },
        { 1         , "%F %T", "1970-01-01 08:00:01" },

        { 1204412642, "%a"   , "Sun"                 },
        { 1204412642, "%A"   , "Sunday" },
        { 1204412642, "%b"   , "Mar" },
        { 1204412642, "%B"   , "March" },
        { 1204412642, "%c"   , "Sun Mar  2 07:04:02 2008"},
        { 1204412642, "%C"   , "20"},
        { 1204412642, "%d"   , "02"},
        { 1204412642, "%D"   , "03/02/08"},
        { 1204412642, "%e"   , " 2"},
        { 1204412642, "%F"   , "2008-03-02"},
        { 1204412642, "%g"   , "08"},
        { 1204412642, "%h"   , "Mar"},
        { 1204412642, "%H"   , "07"},
        { 1204412642, "%I"   , "07"},
        { 1204412642, "%j"   , "062"},
        { 1204412642, "%m"   , "03"},
        { 1204412642, "%M"   , "04"},
        { 1204412642, "%n"   , "\n"},
        { 1204412642, "%p"   , "AM"},
        { 1204412642, "%r"   , "07:04:02 AM"},
        { 1204412642, "%R"   , "07:04"},
        { 1204412642, "%S"   , "02"},
        { 1204412642, "%t"   , "\t"},
        { 1204412642, "%T"   , "07:04:02"},
        { 1204412642, "%u"   , "7"},
        { 1204412642, "%U"   , "09"},
        { 1204412642, "%V"   , "09"},
        { 1204412642, "%w"   , "0"},
        { 1204412642, "%W"   , "08"},
        { 1204412642, "%x"   , "03/02/08"},
        { 1204412642, "%X"   , "07:04:02"},
        { 1204412642, "%y"   , "08"},
        { 1204412642, "%Y"   , "2008"},
        { 1204412642, "%z"   , "+0800"},
    #ifndef _MSC_VER
        { 1204412642, "%Z"   , "CST"},
    #endif
        { 1204412642, "%%"   , "%"},

        {0,0,0}
    };

    for(int i = 0; i < 100; i++)
    {
        Case* c = &cases[i];

        if(!c->from)
            break;

        e_strfsec(dest, 64, c->dfmt, c->from);
        eexpect_str(dest, c->expect);
    }

    return ETEST_OK;
}

static int test_strpsec()
{
    typedef struct {
        constr from;
        constr ffmt;
        i64    expect;
    }Case;

    Case cases[] =
    {
        { "Jun 27 17:43:03 2018", "%b %e %T %Y", 1530092583},
        { "2008-03-02 07:04:02" , "%F %T"      , 1204412642},
        { "1970-01-01 08:00:00" , "%F %T"      , 0},
        { "1970-01-01 08:00:01" , "%F %T"      , 1},
        { "1970-01-01 07:59:59" , "%F %T"      , -1},
        { "1970-01-01 07:59:58" , "%F %T"      , -1},
        {0,0,0}
    };

    for(int i = 0; i < 100; i++)
    {
        Case* c = &cases[i];

        if(!c->from)
            break;

        i64 s = e_strpsec(c->from, c->ffmt);
        eexpect_num(s, c->expect);
    }

    return ETEST_OK;
}

static int test_elpstrfstr()
{
    char dest[64];

    typedef struct {
        constr from;
        constr ffmt;
        constr dfmt;
        constr expect;
    }Case;

    Case cases[] =
    {
        { "367 04:05:06 1", "%j %T %Y", "%Yy %jd %T", "2y 002d 04:05:06" },
        { "367 04:05:06 1", "%j %T %Y", "%jd %T"    , "732d 04:05:06" },
        { "367 04:05:06 1", "%j %T %Y", "%Yy %T"    , "2y 52:05:06" },
        { "367 04:05:06 1", "%j %T %Y", "%T"        , "17572:05:06" },

        {0,0,0,0}
    };

    for(int i = 0; i < 100; i++)
    {
        Case* c = &cases[i];

        if(!c->from)
            break;

        e_elpstrfstr(dest, 64, c->dfmt, c->from, c->ffmt);
        eexpect_str(dest, c->expect);
    }

    return ETEST_OK;

}

static int test_elpstrfsec()
{
    char dest[64];

    typedef struct {
        int    from;
        constr dfmt;
        constr expect;
    }Case;

    Case cases[] =
    {
        { 31723506, "%Yy %jd %T", "1y 002d 04:05:06" },
        { 31723506, "%jd %T"    , "367d 04:05:06"    },
        { 31723506, "%Yy %T"    , "1y 52:05:06"      },
        { 31723506, "%T"        , "8812:05:06"       },
        { 1       , "%Yy %jd %T", "0y 000d 00:00:01" },

        {0,0,0}
    };

    for(int i = 0; i < 100; i++)
    {
        Case* c = &cases[i];

        if(!c->from)
            break;

        e_elpstrfsec(dest, 64, c->dfmt, c->from);
        eexpect_str(dest, c->expect);
    }

    return ETEST_OK;
}

static int test_elpstrpsec()
{
    typedef struct {
        constr from;
        constr ffmt;
        i64    expect;
    }Case;

    Case cases[] =
    {
        { "2 04:05:06 1"        , "%j %T %Y"   , 31723506},
        { "Jan 0 00:00:03 0000" , "%b %d %T %Y",       -1},
        { "Jan 1 00:00:03 0000" , "%b %d %T %Y",        3},
        { "Jan 2 02:00:03 0000" , "%b %d %T %Y",    93603},
        { "00:01:01"            , "%T"         ,       61},
        { "00:01:01 1"          , "%T %d"      ,       61},
        { "00:01:01 2"          , "%T %d"      ,       61 + 1 * 24 * 3600},
        { "00:01:01 1"          , "%T %j"      ,       61 + 1 * 24 * 3600},

        {0,0,0}
    };

    for(int i = 0; i < 100; i++)
    {
        Case* c = &cases[i];

        if(!c->from)
            break;

        i64 s = e_elpstrpsec(c->from, c->ffmt);
        eexpect_num(s, c->expect);
    }

    return ETEST_OK;
}

int test_e_now()
{
    i64 t1, t2, t3;

    t1 = time(0);
    t2 = e_nows();
    t3 = e_ticks();

    printf("t1: %"PRIi64" t2: %"PRIi64" t3: %"PRIi64"\n", t1, t2, t3);

    eexpect_1(t2 - t1 >= 0 && t2 - t1 <= 1);
    eexpect_1(t3 - t1 >= 0 && t2 - t1 <= 1);

    return ETEST_OK;
}

int test_etime(int argc, char* argv[])
{
    (void)argc; (void)argv;

    ETEST_RUN( test_strfstr() );
    ETEST_RUN( test_strfsec() );
    ETEST_RUN( test_strpsec() );

    ETEST_RUN( test_elpstrfstr() );
    ETEST_RUN( test_elpstrfsec() );
    ETEST_RUN( test_elpstrpsec() );

    ETEST_RUN( test_e_now() );

    return ETEST_OK;
}

