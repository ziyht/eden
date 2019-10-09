/**
* this file is create by subsystem 'ETest' from EMake framework
*/

#include <etest.h>

#include "ebench.h"
#include "echan.h"
#include "ethread.h"
#include "estr.h"

typedef struct info_s
{
    echan  chan;
    etypev type;
    int    cap;
}info_t, *info;

static void chan_init(ebench b)
{
    info i = b->oprvt.p;

    i->chan = echan_new(i->type, i->cap);
}

static void chan_release(ebench b)
{
    info i = b->oprvt.p;

    echan_free(i->chan);
}

static void* sender(void* d)
{
    ebench b = d;

    info i = b->oprvt.p;

    int cnt = b->oprts;

    while(cnt)
    {
        echan_sendV(i->chan, __EVAR_MK(i->type, 1, 1, EVAL_I64(1)));
        // printf("send: %d\n", 1);

        cnt--;
    }

    return 0;
}

static void* recver(void* d)
{
    ebench b = d;

    info i = b->oprvt.p;

    int cnt = b->oprts;

    while(cnt)
    {
        evar v = echan_recvV(i->chan);
        // printf("recv: %d\n", 1);

        if(v.type == E_NAV && echan_closed(i->chan))
            break;

        evar_free(v);
        cnt--;
    }

    return 0;
}

static void chan_transfer(ebench b)
{
    ethread_t th1, th2;

    ethread_init(th1, sender, b);
    ethread_init(th2, recver, b);

    b->scale = b->oprts;

    ethread_join(th1);
    ethread_join(th2);
}

int echan_benchmark(int argc, char* argv[])
{
    (void)argc; (void)argv;

    char __buf[128];

    sstr name = sstr_init(__buf, 128);

    ebench b; info_t info;

    info.type = E_I8;
    info.cap  = 1;
    sstr_wrtP(name, "echan transfer: type %s, cap: %d", evar_typeS(evar_gen(info.type, 1, 1)), info.cap);
    b = ebench_new(name);
    ebench_addStep(b, "init"    , chan_init    , EVAL_P(&info));
    ebench_addOprt(b, "transfer", chan_transfer, EVAL_P(&info));
    ebench_addStep(b, "release" , chan_release , EVAL_P(&info));
    ebench_addCase(b, 100000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    info.type = E_I8;
    info.cap  = 2;
    sstr_wrtP(name, "echan transfer: type %s, cap: %d", evar_typeS(evar_gen(info.type, 1, 1)), info.cap);
    b = ebench_new(name);
    ebench_addStep(b, "init"    , chan_init    , EVAL_P(&info));
    ebench_addOprt(b, "transfer", chan_transfer, EVAL_P(&info));
    ebench_addStep(b, "release" , chan_release , EVAL_P(&info));
    ebench_addCase(b, 100000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    info.type = E_I8;
    info.cap  = 4;
    sstr_wrtP(name, "echan transfer: type %s, cap: %d", evar_typeS(evar_gen(info.type, 1, 1)), info.cap);
    b = ebench_new(name);
    ebench_addStep(b, "init"    , chan_init    , EVAL_P(&info));
    ebench_addOprt(b, "transfer", chan_transfer, EVAL_P(&info));
    ebench_addStep(b, "release" , chan_release , EVAL_P(&info));
    ebench_addCase(b, 100000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    info.type = E_I8;
    info.cap  = 16;
    sstr_wrtP(name, "echan transfer: type %s, cap: %d", evar_typeS(evar_gen(info.type, 1, 1)), info.cap);
    b = ebench_new(name);
    ebench_addStep(b, "init"    , chan_init    , EVAL_P(&info));
    ebench_addOprt(b, "transfer", chan_transfer, EVAL_P(&info));
    ebench_addStep(b, "release" , chan_release , EVAL_P(&info));
    ebench_addCase(b, 100000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    info.type = E_I8;
    info.cap  = 32;
    sstr_wrtP(name, "echan transfer: type %s, cap: %d", evar_typeS(evar_gen(info.type, 1, 1)), info.cap);
    b = ebench_new(name);
    ebench_addStep(b, "init"    , chan_init    , EVAL_P(&info));
    ebench_addOprt(b, "transfer", chan_transfer, EVAL_P(&info));
    ebench_addStep(b, "release" , chan_release , EVAL_P(&info));
    ebench_addCase(b, 100000, EVAL_0);
    ebench_addCase(b, 5000000, EVAL_0);

    ebench_exec();
    ebench_showResult();
    ebench_release();

    return ETEST_OK;
}

