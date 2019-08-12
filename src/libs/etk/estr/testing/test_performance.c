/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>
#include <ebench.h>

#include "etype.h"
#include "estr.h"
#include "stb_sprintf.h"

static char bigstr[4096] = {0};

static void init_bigstr()
{
    if(bigstr[0] == '\0')
    {
        for(int i = 0; i < 4095 - 6; i++)
        {
            bigstr[i] = '0';
        }
    }
}

static struct __ {
    cstr fmt;
    eval   v;
}cases[] = {                                       // cid
    {"%s(10)"  , {.s   ="1234567890"}},            // 0
    {"%s(4096)", {.s   = bigstr     }},            // 1
    {"%d"      , {.i32 = 1234567890 }},            // 2
    {"%ld(10)" , {.i32 = 1234567890l}},            // 3
    {"%ld(19)" , {.i64 = 1234567890123456789l}},   // 4
    {"%.2f"    , {.f64 = 12.3456789}},             // 5
    {"%.32f"   , {.f64 = 12.3456789e123}},         // 6

    {"%S(10)"  , {.s   ="1234567890"}},            // 7
    {"%S(4096)", {.s   = bigstr     }},            // 8
    {"%i(10)"  , {.i32 = 1234567890l}},            // 9
    {"%I(19)"  , {.i64 = 1234567890123456789l}},   // 10
};

static void estr_wrtP_fmt_s(ebench_p b);
static void estr_wrtP_fmt_d(ebench_p b);
static void estr_wrtP_fmt_ld(ebench_p b);
static void estr_wrtP_fmt_f(ebench_p b);

static void sstr_wrtP_fmt_s(ebench_p b);
static void sstr_wrtP_fmt_d(ebench_p b);
static void sstr_wrtP_fmt_ld(ebench_p b);
static void sstr_wrtP_fmt_f(ebench_p b);

static void estr_wrtF_fmt_s(ebench_p b);
static void estr_wrtF_fmt_S(ebench_p b);
static void estr_wrtF_fmt_i(ebench_p b);
static void estr_wrtF_fmt_I(ebench_p b);

static void sprintf_fmt_s(ebench_p b);
static void sprintf_fmt_d(ebench_p b);
static void sprintf_fmt_ld(ebench_p b);
static void sprintf_fmt_f(ebench_p b);

static void snprintf_fmt_s(ebench_p b);
static void snprintf_fmt_d(ebench_p b);
static void snprintf_fmt_ld(ebench_p b);
static void snprintf_fmt_f(ebench_p b);

static void stbsp_sprintf_fmt_s(ebench_p b);
static void stbsp_sprintf_fmt_d(ebench_p b);
static void stbsp_sprintf_fmt_ld(ebench_p b);
static void stbsp_sprintf_fmt_f(ebench_p b);

static void stbsp_snprintf_fmt_s(ebench_p b);
static void stbsp_snprintf_fmt_d(ebench_p b);
static void stbsp_snprintf_fmt_ld(ebench_p b);
static void stbsp_snprintf_fmt_f(ebench_p b);

int test_performance(int argc, char* argv[])
{
    (void)argc; (void)argv;

    init_bigstr();

    ebench b;

    int oprts = 500000;
    int cid   = 0;

    b = ebench_new("estr_wrtP");
    cid = 0; ebench_addOprt(b, cases[cid].fmt, estr_wrtP_fmt_s , EVAL_U32(cid));
    cid = 1; ebench_addOprt(b, cases[cid].fmt, estr_wrtP_fmt_s , EVAL_U32(cid));
    cid = 2; ebench_addOprt(b, cases[cid].fmt, estr_wrtP_fmt_d , EVAL_U32(cid));
    cid = 3; ebench_addOprt(b, cases[cid].fmt, estr_wrtP_fmt_ld, EVAL_U32(cid));
    cid = 4; ebench_addOprt(b, cases[cid].fmt, estr_wrtP_fmt_ld, EVAL_U32(cid));
    cid = 5; ebench_addOprt(b, cases[cid].fmt, estr_wrtP_fmt_f , EVAL_U32(cid));
    cid = 6; ebench_addOprt(b, cases[cid].fmt, estr_wrtP_fmt_f , EVAL_U32(cid));
    ebench_addCase(b,  oprts, EVAL_0);

    b = ebench_new("sstr_wrtP");
    cid = 0; ebench_addOprt(b, cases[cid].fmt, sstr_wrtP_fmt_s , EVAL_U32(cid));
    cid = 1; ebench_addOprt(b, cases[cid].fmt, sstr_wrtP_fmt_s , EVAL_U32(cid));
    cid = 2; ebench_addOprt(b, cases[cid].fmt, sstr_wrtP_fmt_d , EVAL_U32(cid));
    cid = 3; ebench_addOprt(b, cases[cid].fmt, sstr_wrtP_fmt_ld, EVAL_U32(cid));
    cid = 4; ebench_addOprt(b, cases[cid].fmt, sstr_wrtP_fmt_ld, EVAL_U32(cid));
    cid = 5; ebench_addOprt(b, cases[cid].fmt, sstr_wrtP_fmt_f , EVAL_U32(cid));
    cid = 6; ebench_addOprt(b, cases[cid].fmt, sstr_wrtP_fmt_f , EVAL_U32(cid));
    ebench_addCase(b,  oprts, EVAL_0);

    b = ebench_new("estr_wrtF");
    cid = 0; ebench_addOprt(b, cases[cid].fmt, estr_wrtF_fmt_s , EVAL_U32(cid));
    cid = 1; ebench_addOprt(b, cases[cid].fmt, estr_wrtF_fmt_s , EVAL_U32(cid));
    cid = 7; ebench_addOprt(b, cases[cid].fmt, estr_wrtF_fmt_S , EVAL_U32(cid));
    cid = 8; ebench_addOprt(b, cases[cid].fmt, estr_wrtF_fmt_S , EVAL_U32(cid));
    cid = 9; ebench_addOprt(b, cases[cid].fmt, estr_wrtF_fmt_i , EVAL_U32(cid));
    cid =10; ebench_addOprt(b, cases[cid].fmt, estr_wrtF_fmt_I , EVAL_U32(cid));
    ebench_addCase(b,  oprts, EVAL_0);

    b = ebench_new("sprintf");
    cid = 0; ebench_addOprt(b, cases[cid].fmt, sprintf_fmt_s , EVAL_U32(cid));
    cid = 1; ebench_addOprt(b, cases[cid].fmt, sprintf_fmt_s , EVAL_U32(cid));
    cid = 2; ebench_addOprt(b, cases[cid].fmt, sprintf_fmt_d , EVAL_U32(cid));
    cid = 3; ebench_addOprt(b, cases[cid].fmt, sprintf_fmt_ld, EVAL_U32(cid));
    cid = 4; ebench_addOprt(b, cases[cid].fmt, sprintf_fmt_ld, EVAL_U32(cid));
    cid = 5; ebench_addOprt(b, cases[cid].fmt, sprintf_fmt_f , EVAL_U32(cid));
    cid = 6; ebench_addOprt(b, cases[cid].fmt, sprintf_fmt_f , EVAL_U32(cid));
    ebench_addCase(b,  oprts, EVAL_0);

    b = ebench_new("snprintf");
    cid = 0; ebench_addOprt(b, cases[cid].fmt, snprintf_fmt_s , EVAL_U32(cid));
    cid = 1; ebench_addOprt(b, cases[cid].fmt, snprintf_fmt_s , EVAL_U32(cid));
    cid = 2; ebench_addOprt(b, cases[cid].fmt, snprintf_fmt_d , EVAL_U32(cid));
    cid = 3; ebench_addOprt(b, cases[cid].fmt, snprintf_fmt_ld, EVAL_U32(cid));
    cid = 4; ebench_addOprt(b, cases[cid].fmt, snprintf_fmt_ld, EVAL_U32(cid));
    cid = 5; ebench_addOprt(b, cases[cid].fmt, snprintf_fmt_f , EVAL_U32(cid));
    cid = 6; ebench_addOprt(b, cases[cid].fmt, snprintf_fmt_f , EVAL_U32(cid));
    ebench_addCase(b,  oprts, EVAL_0);

    b = ebench_new("stbsp_sprintf");
    cid = 0; ebench_addOprt(b, cases[cid].fmt, stbsp_sprintf_fmt_s , EVAL_U32(cid));
    cid = 1; ebench_addOprt(b, cases[cid].fmt, stbsp_sprintf_fmt_s , EVAL_U32(cid));
    cid = 2; ebench_addOprt(b, cases[cid].fmt, stbsp_sprintf_fmt_d , EVAL_U32(cid));
    cid = 3; ebench_addOprt(b, cases[cid].fmt, stbsp_sprintf_fmt_ld, EVAL_U32(cid));
    cid = 4; ebench_addOprt(b, cases[cid].fmt, stbsp_sprintf_fmt_ld, EVAL_U32(cid));
    cid = 5; ebench_addOprt(b, cases[cid].fmt, stbsp_sprintf_fmt_f , EVAL_U32(cid));
    cid = 6; ebench_addOprt(b, cases[cid].fmt, stbsp_sprintf_fmt_f , EVAL_U32(cid));
    ebench_addCase(b,  oprts, EVAL_0);

    b = ebench_new("stbsp_snprintf");
    cid = 0; ebench_addOprt(b, cases[cid].fmt, stbsp_snprintf_fmt_s , EVAL_U32(cid));
    cid = 1; ebench_addOprt(b, cases[cid].fmt, stbsp_snprintf_fmt_s , EVAL_U32(cid));
    cid = 2; ebench_addOprt(b, cases[cid].fmt, stbsp_snprintf_fmt_d , EVAL_U32(cid));
    cid = 3; ebench_addOprt(b, cases[cid].fmt, stbsp_snprintf_fmt_ld, EVAL_U32(cid));
    cid = 4; ebench_addOprt(b, cases[cid].fmt, stbsp_snprintf_fmt_ld, EVAL_U32(cid));
    cid = 5; ebench_addOprt(b, cases[cid].fmt, stbsp_snprintf_fmt_f , EVAL_U32(cid));
    cid = 6; ebench_addOprt(b, cases[cid].fmt, stbsp_snprintf_fmt_f , EVAL_U32(cid));
    ebench_addCase(b,  oprts, EVAL_0);

    ebench_exec();

    ebench_showResult();

    ebench_release();

    return ETEST_OK;
}

static void estr_wrtP_fmt_s(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtP(s, cases[caseid].fmt, cases[caseid].v.s);

    b->scale = (uint)estr_len(s);
    estr_free(s);
}

static void estr_wrtP_fmt_d(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtP(s, cases[caseid].fmt, cases[caseid].v.i32);

    b->scale = (uint)estr_len(s);
    estr_free(s);
}

static void estr_wrtP_fmt_ld(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtP(s, cases[caseid].fmt, cases[caseid].v.i64);

    b->scale = (uint)estr_len(s);
    estr_free(s);
}

static void estr_wrtP_fmt_f(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtP(s, cases[caseid].fmt, cases[caseid].v.f64);

    b->scale = (uint)estr_len(s);
    estr_free(s);
}

static void sstr_wrtP_fmt_s(ebench_p b)
{
    char buf[5000];

    sstr s = sstr_init(buf, 5000);

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sstr_wrtP(s, cases[caseid].fmt, cases[caseid].v.s);

    b->scale = (uint)sstr_len(s);
}

static void sstr_wrtP_fmt_d(ebench_p b)
{
    char buf[5000];

    sstr s = sstr_init(buf, 5000);

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sstr_wrtP(s, cases[caseid].fmt, cases[caseid].v.i32);

    b->scale = (uint)estr_len(s);
}

static void sstr_wrtP_fmt_ld(ebench_p b)
{
    char buf[5000];

    sstr s = sstr_init(buf, 5000);

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sstr_wrtP(s, cases[caseid].fmt, cases[caseid].v.i64);

    b->scale = (uint)estr_len(s);
}

static void sstr_wrtP_fmt_f(ebench_p b)
{
    char buf[5000];

    sstr s = sstr_init(buf, 5000);

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sstr_wrtP(s, cases[caseid].fmt, cases[caseid].v.f64);

    b->scale = (uint)estr_len(s);
}

static void estr_wrtF_fmt_s(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtF(s, cases[caseid].fmt, cases[caseid].v.s);

    b->scale = (uint)estr_len(s);
    estr_free(s);
}

static void estr_wrtF_fmt_S(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    estr s2 = estr_dupS(cases[caseid].v.s);

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtF(s, cases[caseid].fmt, s2);

    b->scale = (uint)estr_len(s);
    estr_free(s);
    estr_free(s2);
}

static void estr_wrtF_fmt_i(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtF(s, cases[caseid].fmt, cases[caseid].v.i32);

    b->scale = (uint)estr_len(s);
    estr_free(s);
}

static void estr_wrtF_fmt_I(ebench_p b)
{
    estr s = 0;

    int caseid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        estr_wrtF(s, cases[caseid].fmt, cases[caseid].v.i64);

    b->scale = (uint)estr_len(s);
    estr_free(s);
}

static void sprintf_fmt_s(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sprintf(s, cases[cid].fmt, cases[cid].v.s);

    b->scale = strlen(s);
}

static void sprintf_fmt_d(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sprintf(s, cases[cid].fmt, cases[cid].v.i32);

    b->scale = strlen(s);
}

static void sprintf_fmt_ld(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sprintf(s, cases[cid].fmt, cases[cid].v.i64);

    b->scale = strlen(s);
}

static void sprintf_fmt_f(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        sprintf(s, cases[cid].fmt, cases[cid].v.f64);

    b->scale = strlen(s);
}

static void snprintf_fmt_s(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        snprintf(s, 4096, cases[cid].fmt, cases[cid].v.s);

    b->scale = strlen(s);
}

static void snprintf_fmt_d(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        snprintf(s, 4096, cases[cid].fmt, cases[cid].v.i32);

    b->scale = strlen(s);
}

static void snprintf_fmt_ld(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        snprintf(s, 4096, cases[cid].fmt, cases[cid].v.i64);

    b->scale = strlen(s);
}

static void snprintf_fmt_f(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        snprintf(s, 4096, cases[cid].fmt, cases[cid].v.f64);

    b->scale = strlen(s);
}

static void stbsp_sprintf_fmt_s(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_sprintf(s, cases[cid].fmt, cases[cid].v.s);

    b->scale = strlen(s);
}

static void stbsp_sprintf_fmt_d(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_sprintf(s, cases[cid].fmt, cases[cid].v.i32);

    b->scale = strlen(s);
}

static void stbsp_sprintf_fmt_ld(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_sprintf(s, cases[cid].fmt, cases[cid].v.i64);

    b->scale = strlen(s);
}

static void stbsp_sprintf_fmt_f(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_sprintf(s, cases[cid].fmt, cases[cid].v.f64);

    b->scale = strlen(s);
}

static void stbsp_snprintf_fmt_s(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_snprintf(s, 4096, cases[cid].fmt, cases[cid].v.s);

    b->scale = strlen(s);
}

static void stbsp_snprintf_fmt_d(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_snprintf(s, 4096, cases[cid].fmt, cases[cid].v.i32);

    b->scale = strlen(s);
}

static void stbsp_snprintf_fmt_ld(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_snprintf(s, 4096, cases[cid].fmt, cases[cid].v.i64);

    b->scale = strlen(s);
}

static void stbsp_snprintf_fmt_f(ebench_p b)
{
    char s[4096];

    int cid = b->oprvt.u32;

    for(uint i = 0; i < b->oprts; i++)
        stbsp_snprintf(s, 4096, cases[cid].fmt, cases[cid].v.f64);

    b->scale = strlen(s);
}
