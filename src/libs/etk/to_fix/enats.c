/// =====================================================================================
///
///       Filename:  enats.c
///
///    Description:  a easier way to handle cnats
///
///        Version:  1.0
///        Created:  02/28/2017 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///         Needed:  cnats, ejson, estr
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

// -- local
#include <string.h>
#include <time.h>
#include <assert.h>

// -- cnats
#undef   _CRT_SECURE_NO_WARNINGS
#define NATS_CONN_STATUS_NO_PREFIX
#include "stats.h"
#include "conn.h"
#include "opts.h"
#include "natsp.h"

// -- etools
#include "enats.h"
#include "estr.h"
#include "ejson.h"
#include "ecompat.h"
#include "ethread.h"
#include "eutils.h"

/// --------------------- static count record -----------------------------
static int          __cnt_init;
static int          __cnt_conn;
static int          __cnt_pool;
static emutex_t     __cnt_mut;

static inline void __cnt_connInc()
{
    if(!__cnt_init)
    {
        emutex_init(__cnt_mut);
        __cnt_init = 1;
    }

    emutex_lock(__cnt_mut);
    __cnt_conn++;
    emutex_ulck(__cnt_mut);
}

static inline void __cnt_connDec()
{
    emutex_lock(__cnt_mut);
    __cnt_conn--;
    if (__cnt_conn <= 0 && __cnt_pool <= 0) {
        llog("exe nats_Close()");
        nats_Close();
        __cnt_conn = 0;
        __cnt_pool = 0;
    }
    emutex_ulck(__cnt_mut);
}

static inline void __cnt_poolInc()
{
    if(!__cnt_init)
    {
        emutex_init(__cnt_mut);
        __cnt_init = 1;
    }

    emutex_lock(__cnt_mut);
    __cnt_pool++;
    emutex_ulck(__cnt_mut);
}

static inline void __cnt_poolDec()
{
    emutex_lock(__cnt_mut);
    __cnt_pool--;
    if (__cnt_conn <= 0 && __cnt_pool <= 0) {
        llog("exe nats_Close()");
        nats_Close();
        __cnt_conn = 0;
        __cnt_pool = 0;
    }
    emutex_ulck(__cnt_mut);
}

/// ---------------------------- enats  ---------------------
///

typedef struct __CBs_s{
    u8                  closed_setted;
    u8                  disconnected_setted;
    u8                  reconnected_setted;
    u8                  err_setted;
    u8                  connected_setted;

    enats_evtHandler    closed_cb;
    void*               closed_closure;
    enats_evtHandler    disconnected_cb;
    void*               disconnected_closure;
    enats_evtHandler    reconnected_cb;
    void*               reconnected_closure;
    enats_errHandler    err_handler;
    void*               err_closure;
    enats_evtHandler    connected_cb;
    void*               connected_closure;
}__CBs_t;

typedef struct __conn_s{
    natsConnection*     nc;             // connect handler of cnats
    natsOptions*        opts;           // back up of opts in t
    natsStatus          s;              // last status

    __CBs_t             CBs;            // callbacks to call
}__conn_t, * __conn;

typedef struct __subs_s{
    natsSubscription*  sub;

    enats              e;               // enats who handle this subs
    enats_msgHandler   msg_handler;
    void*              msg_closure;
}__subs_t, * __subs;

#pragma pack(push, 1)
typedef struct enats_s{
    char            name[64];
    enatp           p;              // point to the enatp who handles this enats

    // -- connection
    __conn_t        conn;           // only one connector in natsTrans

    // -- subscriber
    ejson           sub_dic;        // subscriber in hash table
    emutex_t         sub_mu;

    // -- for quit control
    int             quit;
    int             wait_num;
    emutex_t	    wait_mutex;
    econd_t         wait_cond;

    natsStatus      es;             // last err status
    estr            err;            // buf to store err occered last time
    estr            statss;         // buf to store statistic info
    estr            connurl;        // buf to store connected urls
    estr            lasturl;        // buf to store last connected url
    estr            lasturlr;       // buf to store last connected url, for API to return
    estr            allurls;        // buf to store all urls
}enats_t;

/// ------------------ enats Pool ---------------------------
typedef struct enatp_s{

    natsStatus      s;                  // last status

    // -- for enats manager
    ejson           name_groups;        // to handle all corresponding-relationship between names and enats added to this enatp, including defauld name "conn"
    ejson           conn_transs;        // transs connected to server
    ejson           lazy_transs;        // transs have not connected to server yet
    int             record;

    ejson           polling_itr;        // point to the ejson obj in poll_transs who is polling now
    enats           polling_now;        // the enats in polling_itr

    ethread_t       lazy_thread;
    emutex_t        mutex;

    // -- default CBs
    __CBs_t         CBs;

    // -- limits
    u64             cnt;
    u64             max;

    // -- for quit control
    int             quit;
    int             conn_num;           // the num of connected trans
    int             wait_num;
    econd_t         wait_cond;
    emutex_t        wait_mutex;

    // -- other
    natsStatus      es;                 // last err status
    estr            err;                // last err
    estr            statss;             // buf to store statistic info
    estr            connurls;           // buf to store connected url
    estr            lazyurls;           // buf to store lazy      url
    estr*           connurls_l;         // buf to store connected url list
    estr*           lazyurls_l;         // buf to store lazy      url list
}enatp_t;

#pragma pack(pop)

static constr _err;
static char   _err_buf[1024];

// -- helpler
typedef natsOptions* natsOptions_p;
static estr        __enats_makeUrls(constr user, constr pass, constr url, int *c);
static natsStatus  __enats_processUrlString(natsOptions *opts, constr urls);        // parse cnats url, rebuild from cnats src

// -- callbacks for events
static void __on_closed      (natsConnection* nc, void* trans);         // handler connection closed
static void __on_disconnected(natsConnection* nc, void* trans);         // handler connection lost
static void __on_reconnected (natsConnection* nc, void* trans);         // handler connection reconnected
static void __on_erroccurred (natsConnection* nc, natsSubscription* subscription, natsStatus err, void* closure);      // errors encountered while processing inbound messages

// -- callbacks for subscriber
static void __on_msg(natsConnection* nc, natsSubscription* sub, natsMsg* msg, void* subs)
{
    __subs s = (__subs)subs;

    s->msg_handler(s->e, sub, (eMsg)msg, s->msg_closure);
}

// -- static err handle for enats
#define G ((enats)0)     // globel err
#define errset(h, str) do{if(h) {estr_wrtS(h->err, str);}else{_err = str;}}while(0)
#define errfmt(h, ...) do{if(h) {estr_wrtP(h->err, ##__VA_ARGS__);}else{sprintf(_err_buf, ##__VA_ARGS__); _err = _err_buf;}}while(0)

/// -------------------------- enats helper -----------------------
#define USERPASS_ERR    0
#define USERPASS_OK     1
#define USER_ERR        2
#define PASS_ERR        3
static inline int __enats_CheckUserPass(constr user, constr pass)
{
    int _out = USERPASS_ERR;
    if(user && *user)
    {
        _out = PASS_ERR;
    }
    if(pass && *pass)
    {
        if(_out == PASS_ERR)
            _out = USERPASS_OK;
        else
        {
            _out = USER_ERR;
            _err = "user is empty or NULL";
        }
    }

    return _out;
}

static inline estr __enats_makeUrls(constr user, constr pass, constr urls, int* cnt)
{
    int s; cstr next_url, urls_dump, url, c; int count; estr o_urls;

    count = 0;
    cnt ? *cnt = 0 : 0;

    is1_exeret(!urls || !*urls, errset(G, "invalid urls(null or empty)"), NULL);
    is1_ret((s = __enats_CheckUserPass(user, pass)) == USER_ERR, NULL);

    is0_exeret(o_urls = estr_newLen(0, strlen(urls) * 2), errset(G, "alloc for urls faild");, NULL);

    while(*urls == ',') urls++;

    urls_dump = strdup(urls);
    url       = urls_dump;
    next_url  = strchr(url, ',');

    do{
        next_url ? (*next_url = '\0') : (0);

        if(0 == strncmp(urls, "nats://", 7))
            url += 7;

        if((c = strchr(url, '@')) && c[-1] != '/')
        {
            estr_catF(o_urls, "nats://%s,", url);
        }
        else
        {
            if(c) url = c + 1;

            if     (s == USERPASS_OK )  estr_catF(o_urls, "nats://%s:%s@%s,", user, pass, url);
            else if(s == PASS_ERR    )  estr_catF(o_urls, "nats://%s@%s,"   , user,       url);
            else if(s == USERPASS_ERR)  estr_catF(o_urls, "nats://%s,"      ,             url);
        }

        count ++;

        url      = next_url ? next_url + 1     : 0;
        next_url = next_url ? strchr(url, ',') : 0;
    }while(url);

    estr_setT(o_urls, '\0');

    free(urls_dump);

    if(count == 0)
    {
        errfmt(G, "can not parse any url from urls: %s", urls);
        estr_free(o_urls);
        o_urls = NULL;
    }

    cnt ? *cnt = count : 0;
    return o_urls;
}

static inline natsStatus __enats_processUrlString(natsOptions *opts, constr urls)
{
    int count, len; cstr commaPos, ptr, *serverUrls; natsStatus s = NATS_OK; cstr urlsCopy = 0;

    count = 0;

    while(*urls == ',') urls++;

    ptr = (char*) urls;
    while ((ptr = strchr(ptr, ',')) != NULL)
    {
        ptr++;
        count++;
    }
    if (count == 0)
        return natsOptions_SetURL(opts, urls);

    serverUrls = (char**) calloc(count + 1, sizeof(char*));
    if (serverUrls == NULL)
        s = NATS_NO_MEMORY;
    if (s == NATS_OK)
    {
        urlsCopy = strdup(urls);
        if (urlsCopy == NULL)
        {
            free(serverUrls);
            return NATS_NO_MEMORY;
        }
    }

    count = 0;
    ptr = urlsCopy;

    do
    {
        while (*ptr == ' ')
            ptr++;
        serverUrls[count++] = ptr;

        commaPos = strchr(ptr, ',');
        if (commaPos != NULL)
        {
            ptr = (char*)(commaPos + 1);
            *(commaPos) = '\0';
        }

        len = (int) strlen(ptr);
        while ((len > 0) && (ptr[len-1] == ' '))
            ptr[--len] = '\0';

    } while (commaPos != NULL);

    if (s == NATS_OK)
        s = natsOptions_SetServers(opts, (const char**) serverUrls, count);

    free(urlsCopy);
    free(serverUrls);

    return s;
}

static inline enats __enats_newHandle(constr urls, enats_opts e_opts)
{
    enats e; natsOptions* opts; estr nurls; natsStatus s = NATS_OK;

    opts  = NULL;
    nurls = NULL;

    if(e_opts && e_opts->tls.enanle)
    {
       // is1_exeret(access(_opts->tls.ca  , F_OK), errfmt("ca file: %s not exist", _opts->tls.ca), NULL);
        is1_exeret(access(e_opts->tls.key , F_OK), errfmt(G, "tls key  file: %s not exist", e_opts->tls.key ), NULL);
        is1_exeret(access(e_opts->tls.cert, F_OK), errfmt(G, "tls cert file: %s not exist", e_opts->tls.cert), NULL);
    }

    // -- for urls parser
    if(!urls)
    {
        cstr auth, user, pass;

        urls = e_opts->urls;
        auth = e_opts->auth;
        user = e_opts->user;
        pass = e_opts->pass;

        if((user && *user) && (pass && *pass)) nurls = __enats_makeUrls(user, pass, urls, 0);
        else if((auth && *auth))               nurls = __enats_makeUrls(auth, 0   , urls, 0);
        else                                   nurls = __enats_makeUrls(0   , 0   , urls, 0);

        is0_ret(nurls, 0);

        urls  = nurls;
    }

    is1_exe((s = natsOptions_Create(&opts)           ) != NATS_OK, errset(G, "alloc for new nats opts faild"    ); goto err_ret;);
    is1_exe((s = __enats_processUrlString(opts, urls)) != NATS_OK, errset(G, "__processUrlString for opts faild"); goto err_ret;);
    is1_exe((e = calloc(1, sizeof(*e))               ) == 0      , errset(G, "alloc for new enats faild"        ); goto err_ret;);

    e->conn.opts = opts;
    natsOptions_SetClosedCB      (opts, __on_closed,       e);
    natsOptions_SetDisconnectedCB(opts, __on_disconnected, e);
    natsOptions_SetReconnectedCB (opts, __on_reconnected,  e);
    natsOptions_SetErrorHandler  (opts, __on_erroccurred,  e);
    natsOptions_SetMaxReconnect  (opts, -1                  );
    //natsOptions_SetNoRandomize   (opts, true                );

    if(e_opts && e_opts->timeout)
        natsOptions_SetTimeout(opts, e_opts->timeout);

    if(e_opts && e_opts->tls.enanle)
    {
        is1_exe((s = natsOptions_SetSecure(opts, true))!= NATS_OK, errset(G, nats_GetLastError(&s)); goto err_ret;);

        if(e_opts->tls.ca)
        {
            is1_exe((s = natsOptions_LoadCATrustedCertificates(opts, e_opts->tls.ca))           != NATS_OK, errset(G, nats_GetLastError(&s)); goto err_ret;);
        }
        is1_exe((s = natsOptions_LoadCertificatesChain(opts, e_opts->tls.cert, e_opts->tls.key))!= NATS_OK, errset(G, nats_GetLastError(&s)); goto err_ret;);

    }

    emutex_init(e->sub_mu);
    assert(e->sub_dic = ejson_new(EOBJ, EVAL_0));

    e->conn.s = NATS_OK;

    __cnt_connInc();

    return e;

err_ret:
    estr_free(nurls);
    natsOptions_Destroy(opts);

    return NULL;
}

static inline int   __enats_tryConnect(enats e)
{
    natsConnection* nc   = NULL;
    natsStatus      s    = NATS_OK;

    is1_ret(e->conn.nc, 1);

    is1_exeret((s = natsConnection_Connect(&nc, e->conn.opts)) != NATS_OK, e->es =s, 0);

    e->conn.s  = s;
    e->conn.nc = nc;
    estr_wrtS(e->lasturl, nc->opts->url);

    return 1;
}

static inline ejson __enats_makeRoomForSubs(enats e, constr subj, enats_msgHandler onMsg, void* closure)
{
    ejson sroom; __subs subs;

    emutex_lock(e->sub_mu);
    if(!e->sub_dic) e->sub_dic = ejson_new(EOBJ, EVAL_0);
    subs = (__subs)ejson_addR(e->sub_dic, subj, sizeof(*subs));
    emutex_ulck(e->sub_mu);
    is0_exeret(subs, errfmt(e, "subs of \"%s\" already exist", subj), 0);

    assert(sroom = ejson_r(e->sub_dic, subj));

    subs->e           = e;
    subs->msg_handler = onMsg;
    subs->msg_closure = closure;

    return sroom;
}

static inline void __enats_freeSubsRoom(ejson sroom)
{
    enats e;
    e = ((__subs)EOBJ_VALR(sroom))->e;

    emutex_lock(e->sub_mu);
    ejson_freeO(e->sub_dic, sroom);
    emutex_ulck(e->sub_mu);
}

static inline int __enats_trySub(ejson sroom)
{
    __subs subs; enats e;

    subs = EOBJ_VALR(sroom);
    e    = subs->e;

    is1_ret(subs->sub, 0);

    e->conn.s = natsConnection_Subscribe(&subs->sub, e->conn.nc, eobj_keyS(sroom), __on_msg, subs);

    if(e->conn.s == NATS_OK)
        e->conn.s = natsSubscription_SetPendingLimits(subs->sub, -1, -1);
    else
    {
        return 0;
    }

    if(e->conn.s != NATS_OK)
    {
        natsSubscription_Unsubscribe(subs->sub);
        natsSubscription_Destroy(subs->sub);
        subs->sub = 0;
        return 0;
    }

    return 1;
}

/**
 * @note:
 *      in enatp, the connection will never closed when e->conn.nc != NULL, else
 *  e->conn.nc will be NULL, so this call will alwnays make room for subj when call
 *  in enatp, even this is a lazy enats
 */
static inline int __enats_sub(enats e, constr subj, enats_msgHandler onMsg, void* closure)
{
    ejson sroom;

    if(e->conn.nc)
    {
        if(natsConn_isClosed(e->conn.nc))
        {
            e->conn.s = NATS_CONNECTION_CLOSED;
            return 0;
        }
    }

    sroom = __enats_makeRoomForSubs(e, subj, onMsg, closure);
    is0_ret(sroom, e->conn.s = NATS_NO_MEMORY);

    if(!__enats_trySub(sroom) && e->conn.nc)
    {
        __enats_freeSubsRoom(sroom);
        return 0;
    }

    return 1;
}

static inline void __enats_unSub(enats e, constr subj)
{
    ejson sroom; __subs subs;

    emutex_lock(e->sub_mu);

    if((sroom = ejson_takeR(e->sub_dic, subj)))
    {
        subs = EOBJ_VALR(sroom);

        e->conn.s = natsSubscription_Unsubscribe(subs->sub);
        natsSubscription_Destroy(subs->sub);

        ejson_free(sroom);
    }

    emutex_ulck(e->sub_mu);
}

static inline void __enats_destroyAllNatsSubscription(enats e)
{
    ejson itr; __subs s;

    is0_ret(e, )

    emutex_lock(e->sub_mu);
    ejson_foreach(e->sub_dic, itr)
    {
        s = EOBJ_VALR(itr);

        natsSubscription_Unsubscribe(s->sub);
        natsSubscription_Destroy(s->sub);
        s->sub = 0;
    }
    emutex_ulck(e->sub_mu);
}

static void __enats_reSub(enats e)
{
    ejson itr;

    emutex_lock(e->sub_mu);

    ejson_foreach(e->sub_dic, itr)
    {
        if(!__enats_trySub(itr))
        {
            ejson_freeO(e->sub_dic, itr);
        }
    }

    emutex_ulck(e->sub_mu);
}

static inline void __enats_destroySubDic(enats e)
{
    __enats_destroyAllNatsSubscription(e);

    if(e)
    {
        emutex_free(e->sub_mu);
        ejson_free(e->sub_dic);
        e->sub_dic = 0;
    }

}

static inline void __enats_destroyNc(enats e)
{
    is0_ret(e, );

    if(e->conn.nc)
    {
        natsConnection_Destroy(e->conn.nc);
        e->conn.nc = 0;
    }
}

static inline void __enats_destroyOpts(enats e)
{
    is0_ret(e, );

    natsOptions_Destroy(e->conn.opts);
    e->conn.opts = 0;
}

static inline void __enats_freeHandle(enats e)
{
    is0_ret(e, );

    estr_free(e->err);
    estr_free(e->statss);
    estr_free(e->connurl);
    estr_free(e->lasturl);
    estr_free(e->lasturlr);
    estr_free(e->allurls);

    __cnt_connDec();

    free(e);
}

static inline void __enats_release(enats e)
{
    __enats_destroySubDic(e);
    __enats_destroyNc(e);
    __enats_destroyOpts(e);
    __enats_freeHandle(e);
}

static inline void* __enats_waitThread(void* e_)
{
    enats e = (enats)e_;

    e->wait_num++;

    emutex_lock(e->wait_mutex);
    emutex_ulck(e->wait_mutex);

    e->wait_num--;

    return 0;
}

static inline void __enats_exeWait(enats e)
{
    while(!e->quit)
    {
        emutex_lock(e->wait_mutex);
        e->wait_num++;
        econd_wait(e->wait_cond, e->wait_mutex);
        e->wait_num--;
        emutex_ulck(e->wait_mutex);
    }
}

static inline void __enats_quitWait(enats e)
{
    while(e->wait_num)
    {
        econd_all(e->wait_cond);
        usleep(10000);
    }
}

static inline void __enats_hide_passwd(estr urls)
{
    cstr c, ep, endp; int off;

    c    = urls;
    endp = c + strlen(c);

    c = strchr(c, '@');
    while(c)
    {
        off = 0;
        ep  = strchr(c, ','); if(!ep) ep = endp;

        while(*(--c) != ':' && *c != '/')
        {
            *c = ' '; off++;
        }

        c = strchr(ep, '@');
    }

    estr_subc(urls, " ", "*");
}

static inline constr __enats_getConnUrls(enats e, int hidepass)
{
    natsConnection* nc = e->conn.nc;

    is0_ret(nc, NULL);

    natsConn_Lock(nc);
    if ((nc->status == CONNECTED) && (nc->opts->url != NULL))
        estr_wrtS(e->connurl, nc->opts->url);
    else
        estr_wrtB(e->connurl, "", 0);

    natsConn_Unlock(nc);

    if(hidepass) __enats_hide_passwd(e->connurl);

    return e->connurl;
}

static inline constr __enats_getLastUrl(enats e, int hidepass)
{
    estr_wrtE(e->lasturlr, e->lasturl);

    if(hidepass) __enats_hide_passwd(e->lasturlr);

    return e->lasturlr;
}

static inline constr __enats_getAllUrls(enats e, int hidepass)
{
    int cnt, i; natsOptions* opts = e->conn.opts;

    estr_clear(e->allurls);
    cnt = opts->url ? 1 : opts->serversCount;
    for(i = 0; i < cnt; i++)
    {
        estr_catF(e->allurls, "%s,", cnt == 1 ? opts->url : opts->servers[i]);
    }
    estr_setT(e->allurls, '\0');

    if(hidepass) __enats_hide_passwd(e->allurls);

    return e->allurls;
}

static inline natsStatus
__enats_request(eMsg* replyMsg, natsConnection *nc, const char *subj,
               const void *data, int dataLen, int64_t timeout, constr reply)
{
    natsStatus          s       = NATS_OK;
    natsSubscription    *sub    = NULL;

    if (replyMsg == NULL)
        return nats_setDefaultError(NATS_INVALID_ARG);
    s = natsConn_queueSubscribeWithTimeout(&sub, nc, reply, NULL, timeout, NULL, NULL);
    if (s == NATS_OK)
        s = natsSubscription_AutoUnsubscribe(sub, 1);
    if (s == NATS_OK)
        s = natsConnection_PublishRequest(nc, subj, reply, data, dataLen);
    if (s == NATS_OK)
        s = natsSubscription_NextMsg((natsMsg**)replyMsg, sub, timeout);

    natsSubscription_Destroy(sub);

    return NATS_UPDATE_ERR_STACK(s);
}

static inline void __enats_stats(enats trans, constr subj, enats_stats_t* stats, int format)
{
    __subs ntSub; natsStatus s = NATS_OK;

    memset(stats, 0, sizeof(*stats));
    s = natsConnection_GetStats(trans->conn.nc, (natsStatistics*)stats);

    emutex_lock(trans->sub_mu);
    ntSub = ejson_rValR(trans->sub_dic, subj);
    emutex_ulck(trans->sub_mu);
    if ((s == NATS_OK) && (ntSub != NULL))
    {
        s = natsSubscription_GetStats(ntSub->sub,
                                      &stats->pendingMsgs,
                                      &stats->pendingBytes,
                                      &stats->maxPendingMsgs,
                                      &stats->maxPendingBytes,
                                      &stats->deliveredMsgs,
                                      &stats->droppedMsgs);

        // if use AutoUnsubscribe(), when the max has been reached,
        // the subscription is automatically closed, so this call would
        // return "Invalid Subscription". Ignore this error.
        if (s == NATS_INVALID_SUBSCRIPTION)
        {
            s = NATS_OK;
            stats->pendingMsgs = 0;
        }
    }

    if(format) estr_wrtP(trans->statss,
                        "In Msgs: %9" PRIu64 " - In Bytes: %9" PRIu64 " - Out Msgs: %9" PRIu64 " - Out Bytes: %9" PRIu64 " - "
                        "Delivered: %9" PRId64 " - Pending: %5d - Dropped: %5" PRId64 " - Reconnected: %3" PRIu64 "",
                        stats->inMsgs,        stats->inBytes,     stats->outMsgs,     stats->outBytes,
                        stats->deliveredMsgs, stats->pendingMsgs, stats->droppedMsgs, stats->reconnects);
    else       estr_clear(trans->statss);

    trans->conn.s = s;
}

/// ------------------ win32 API setting --------------------
#if (_WIN32)
#define inline
#endif

/// ----------------------- enats  --------------------------
enats enats_newUrl1(constr urls)
{
    enats e = NULL;

    is1_exeret(!urls || !*urls, errset(G, "urls is null or empty");, NULL);

    is0_ret(e = __enats_newHandle(urls, 0), 0);

    if(!__enats_tryConnect(e))
    {
        errset(G, nats_GetLastError(&e->es));
        __enats_release(e);
        e = NULL;
    }

    return e;
}

enats enats_newUrl2(constr auth, constr urls)
{
    estr _urls; enats e = 0;

    is1_exe(_urls = __enats_makeUrls(auth, 0, urls, 0), e = enats_newUrl1(_urls););
    estr_free(_urls);

    return e;
}

enats enats_newUrl3(constr user, constr pass, constr urls)
{
    estr _urls; enats e = 0;

    is1_exe(_urls = __enats_makeUrls(user, pass, urls, 0), e = enats_newUrl1(_urls););
    estr_free(_urls);

    return e;
}

enats enats_newUrl4(constr user, constr pass, constr server, int port)
{
    char url[1024]; estr urls; enats e = 0;

    is1_exeret(!server || !*server, errset(G, "server is null or empty");, NULL);

    is1_exeret(snprintf(url, 1024, "%s:%d", server, port) >= 1024, errset(G, "url buf of 1024 overflow");, NULL);

    is1_exe(urls = __enats_makeUrls(user, pass, url, 0), e = enats_newUrl1(urls););
    estr_free(urls);

    return e;
}

enats enats_newOpts(enats_opts opts)
{
    enats e;

    is0_exeret(opts, errset(G, "opts is null or empty"), NULL);

    is0_exeret(e = __enats_newHandle(0, opts), errfmt(G, "%s", _err), 0);

    if(!__enats_tryConnect(e))
    {
        errfmt(G, "%s", nats_GetLastError(&e->es));
        __enats_release(e);
        e = NULL;
    }

    return e;
}


void enats_join(enats e)
{
    is0_exeret(e, errset(e, "invalid enatp (nullptr)"), );

    __enats_exeWait(e);
}

void enats_destroy(enats e)
{
    is0_ret(e, )

    e->quit  = 1;

    __enats_quitWait(e);
    __enats_destroyNc(e);           // this call will raise __on_closed, we do __enats_freeHandle() in it, but this is only effect on connected enats
}

inline constr enats_allurls(enats e, int hidepass)
{
    is0_ret(e, NULL);

    return __enats_getAllUrls(e, hidepass);
}

inline constr enats_connurl(enats e, int hidepass)
{
    is0_ret(e, NULL);

    return __enats_getConnUrls(e, hidepass);
}

inline constr enats_lasturl(enats e, int hidepass)
{
    is0_ret(e, NULL);

    return __enats_getLastUrl(e, hidepass);
}

inline constr enats_name(enats e)
{
    return e ? e->name : NULL;
}

inline enatp enats_pool(enats e)
{
    return e ? e->p : NULL;
}

inline void enats_setClosedCB(enats e, enats_evtHandler cb, void* closure)
{
    is0_ret(e, );

    e->conn.CBs.closed_cb       = cb;
    e->conn.CBs.closed_closure  = closure;
    e->conn.CBs.closed_setted   = 1;
}
inline void enats_setDisconnectedCB(enats e, enats_evtHandler cb, void* closure)
{
    is0_ret(e, );

    e->conn.CBs.disconnected_cb       = cb;
    e->conn.CBs.disconnected_closure  = closure;
    e->conn.CBs.disconnected_setted   = 1;
}
inline void enats_setReconnectedCB(enats e, enats_evtHandler cb, void* closure)
{
    is0_ret(e, );

    e->conn.CBs.reconnected_cb       = cb;
    e->conn.CBs.reconnected_closure  = closure;
    e->conn.CBs.reconnected_setted   = 1;
}

inline void enats_setErrHandler(enats e, enats_errHandler cb, void* closure)
{
    is0_ret(e, );

    e->conn.CBs.err_handler = cb;
    e->conn.CBs.err_closure = closure;
    e->conn.CBs.err_setted  = 1;
}


/// @brief -- publish msg through enats, thread safe
///
/// @param trans   : enats transport handle
/// @param subj    : the subject you want to publish to
/// @param data    : the data you want to publish
/// @param dataLen : the lengh of the publish data
/// @param reply   : the msg will set reply
/// @return [ = NAT_OK] if publish ok
///         [!= NAT_OK] if publish faild, using enats_err() to get err info
///
inline natsStatus enats_pub(enats trans, constr subj, conptr data, int dataLen)
{
    is0_exeret(trans, errset(G, "invalid enats (nullptr)");, NATS_ERR);

    trans->conn.s = natsConnection_Publish(trans->conn.nc, subj, data, dataLen);
    return trans->conn.s;
}

inline natsStatus  enats_pubr(enats trans, constr subj, conptr data, int dataLen, constr reply)
{
    is0_exeret(trans, errset(G, "invalid enats (nullptr)");, NATS_ERR);

    trans->conn.s = natsConnection_PublishRequest(trans->conn.nc, subj, reply, data, dataLen);
    return trans->conn.s;
}

/// @brief enats_req -- request a msg through the trans, it will autolly subscribe the reply before publish the data
///                     and autolly unsubscribe it when recieve a msg
///
/// @param e        : enats transport handle
/// @param subj     : the subject you want to publish to
/// @param data     : the data you want to publish
/// @param dataLen  : the lengh of the publish data
/// @param reply    : the reply subject to auto subscribe and unsubscribe
/// @param replyMsg : the msg recieved
/// @param timeout  : timeout setting
/// @return [ = NAT_OK] if requst ok
///         [!= NAT_OK] if requst faild, using enats_err() to get err info
///
/// @note:
///     if timeout = 0, this API will wait forever unless recieve a msg
///
natsStatus  enats_req(enats e, constr subj, conptr data, int dataLen, constr reply, eMsg* replyMsg, int64_t timeout)
{
    is0_exeret(e, errset(G, "invalid enats (nullptr)"), NATS_ERR);

    //trans->conn.s = natsConnection_Request(replyMsg, trans->conn.nc, subj, data, dataLen, timeout);
    e->conn.s = __enats_request(replyMsg, e->conn.nc, subj, data, dataLen, timeout, reply);

    return e->conn.s;
}

/// @brief enats_sub -- sub a subject in one enats
///
/// @param e       : enats transport handle
/// @param subj    : the subject you want to subscribe
/// @param onMsg   : the cb to be called when a msg coming
/// @param closure : the private data of this subj
/// @return [ = NAT_OK] if sub faild
///         - !=NAT_OK if operate success
/// @note:
///     1. The subject can have wildcards (partial:*, full:>).
///     2. Messages will be delivered to the associated enats_msgHandler @param onMsg
///        and using eMsg_free() to free the msg after using it
///
natsStatus  enats_sub(enats e, constr subj, enats_msgHandler onMsg, void* closure)
{
    is0_exeret(e                 , errset(e, "invalid enats (nullptr)");        ,             NATS_ERR);
    is0_exeret(onMsg             , errfmt(e, "invalid callbacks (nullptr)");    , e->conn.s = NATS_ERR);
    is1_exeret(!subj || !*subj   , errset(e, "invalid subj (nullptr or empty)");, e->conn.s = NATS_ERR);
    is1_exeret(strstr(subj, ".."), errfmt(e, "invalid subj (contain \"..\")");  , e->conn.s = NATS_ERR);

    if(!__enats_sub(e, subj, onMsg, closure))
        errset(e, nats_GetLastError(&e->conn.s));

    return e->conn.s;
}

natsStatus  enats_unsub(enats e, constr subj)
{
    is0_exeret(e, errset(G, "invalid enats (nullptr)"), NATS_ERR);

    __enats_unSub(e, subj);

    return e->conn.s;
}

int    enats_stats (enats e, constr subj, enats_stats_t* stats)
{
    is1_exeret(!e || !stats, errset(G, "invalid param (nullptr)");, 0);

    __enats_stats(e, subj, stats, 0);

    return 1;
}

constr enats_statsS(enats e, constr subj)
{
    enats_stats_t stats;

    is0_exeret(e, errset(G, "invalid enats (nullptr)");, NULL);

    __enats_stats(e, subj, &stats, 1);

    return e->statss;
}

constr enats_err(enats trans)
{
    return trans ? trans->err : _err;
}

/// =====================================================================================
///                                natsTrans Pool
/// =====================================================================================

// -- micros
#define enatp_lock(p)           emutex_lock((p)->mutex)
#define enatp_ulck(p)           emutex_ulck((p)->mutex)

#define enatp_add_conntrans(p, add) ejson_addP(p->conn_transs, add->name, add)
#define enatp_get_conntrans(p,name) ejson_valPk(p->conn_transs, name)
#define enatp_del_conntrans(p, del) ejson_freeR(p->conn_transs, del->name)
#define enatp_cnt_conntrans(p     ) eobj_len (p->conn_transs)

#define enatp_add_lazytrans(p, add) ejson_addP(p->lazy_transs, add->name, add)
#define enatp_get_lazytrans(p,name) ejson_valPk(p->lazy_transs, name)
#define enatp_del_lazytrans(p, del) ejson_freeR(p->lazy_transs, del->name)
#define enatp_cnt_lazytrans(p     ) ejson_size (p->lazy_transs)

#define enatp_add_url(p,url,ntname) ejso_addS(p->urls, url, ntname)
#define enatp_get_url(p,url       ) ejsr     (p->urls, url        )
#define enatp_del_url(p,url       ) ejsr_free(p->urls, url        )

// -- static err handle for enatp
#undef  G
#undef  errset
#undef  errfmt
#define G ((enatp)0)
#define errset(h, str) do{if(h) {estr_wrtS(h->err, str);}else{_err = str;}}while(0)
#define errfmt(h, ...) do{if(h) {estr_wrtP(h->err, ##__VA_ARGS__);}else{sprintf(_err_buf, ##__VA_ARGS__); _err = _err_buf;}}while(0)

/// -------------------------- enats helper -----------------------
static inline enatp __enatp_newHandle()
{
    enatp p = calloc(1, sizeof(enatp_t));

    is0_exeret(p, errset(G, "alloc for new enatp handle faild"), 0);

    p->conn_transs = ejson_new(EOBJ, EVAL_0);
    p->lazy_transs = ejson_new(EOBJ, EVAL_0);
    p->name_groups = ejson_new(EOBJ, EVAL_0);

    if(!p->conn_transs || !p->lazy_transs || !p->name_groups)
    {
        ejson_free(p->conn_transs);
        ejson_free(p->lazy_transs);
        ejson_free(p->name_groups);

        return 0;
    }

    emutex_init(p->mutex);

    emutex_init(p->wait_mutex); econd_init(p->wait_cond);

    __cnt_poolInc();

    return p;
}

static inline void __enatp_destroyLazy(enatp p)
{
    ejson e, itr;

    is1_ret(!p || !p->quit, );

    e = p->lazy_transs; p->lazy_transs = 0;

    ejson_foreach(e, itr)
    {
        __enats_release(EOBJ_VALP(itr));
    }

    ejson_free(e);
}

static inline int __enatp_destroyConn(enatp p)
{
    ejson e, itr; int can_free;

    is1_ret(!p || !p->quit, 0);

    p->polling_itr = 0;
    p->polling_now = 0;

    p->conn_num = enatp_cnt_conntrans(p);
    can_free = !p->conn_num;

    e = p->conn_transs; p->conn_transs = 0;

    ejson_foreach(e, itr)
    {
        enats_destroy(EOBJ_VALP(itr));
    }

    ejson_free(e);

    return can_free;
}

static inline void __enatp_destroyName(enatp p)
{
    ejson e;

    is1_ret(!p || !p->quit, );

    e = p->name_groups; p->name_groups = 0;

    ejson_free(e);
}

static inline void __enatp_freeHandle(enatp p)
{
    is0_ret(p, );

    estr_free(p->err);
    estr_free(p->statss);
    estr_free(p->connurls);
    estr_free(p->lazyurls);

    esplt_free(p->connurls_l);
    esplt_free(p->lazyurls_l);

    __cnt_poolDec();

    free(p);
}

static inline void __enatp_waitLazy(enatp p)
{
    is1_ret(!p || !p->quit, );

    if(p->lazy_thread)
        ethread_join(p->lazy_thread);

    p->lazy_thread = 0;
}

static inline void __enatp_exeWait(enatp p)
{
    while(!p->quit)
    {
        emutex_lock(p->wait_mutex);
        p->wait_num++;
        econd_wait(p->wait_cond, p->wait_mutex);
        p->wait_num--;
        emutex_ulck(p->wait_mutex);
    }
}

static inline void __enatp_quitWait(enatp p)
{
    while(p->wait_num)
    {
        econd_all(p->wait_cond);
        usleep(1000);
    }

    __enatp_freeHandle(p);
}

// todo: optimizing it
static void  __enatp_pollingNext(enatp p, enats reject)
{
    if(reject)
    {
        if(p->polling_now != reject) return;
        else
        {
            p->polling_itr = ejson_next(p->polling_itr);
            if(!p->polling_itr) p->polling_itr = ejson_first(p->conn_transs);
            p->polling_now = p->polling_itr ? EOBJ_VALP(p->polling_itr)
                                            : 0;

            if(p->polling_now == reject)
            {
                p->polling_itr = 0;
                p->polling_now = 0;
            }
        }
    }

    if(!p->polling_itr)
    {
        p->polling_itr = ejson_first(p->conn_transs);
    }
    else
    {
        p->polling_itr = ejson_next(p->polling_itr);
        if(!p->polling_itr) p->polling_itr = ejson_first(p->conn_transs);
    }

    p->polling_now = p->polling_itr ? EOBJ_VALP(p->polling_itr)
                                    : 0;
}

static void* __enatp_lazy_thread(void* _p)
{
    enatp p; ejson itr; enats e;

    p = (enatp)_p;

    while(!p->quit && enatp_cnt_lazytrans(p))
    {
        sleep(1);

        ejson_foreach(p->lazy_transs, itr)
        {
            e = EOBJ_VALP(itr);

            if(e->conn.nc)         // this should not happen
            {
                enatp_lock(p);

                // -- add to pool
                enatp_del_lazytrans(p, e);
                enatp_add_conntrans(p, e);

                if(!p->polling_now) __enatp_pollingNext(p, 0);

                enatp_ulck(p);

                continue;
            }

            if(__enats_tryConnect(e))      // connect ok
            {
                enatp_lock(p);

                // -- add to pool
                enatp_del_lazytrans(p, e);
                enatp_add_conntrans(p, e);
                if(!p->polling_now) __enatp_pollingNext(p, 0);

                enatp_ulck(p);

                __enats_reSub(e);

                if(e->conn.CBs.connected_cb)
                    e->conn.CBs.connected_cb(e, e->conn.CBs.connected_closure);
            }
        }
    }

    return 0;
}

static void __enatp_exeLazyThread(enatp p)
{
    if(p->quit) return;

    if(p->lazy_thread == 0)
    {
        ethread_init(p->lazy_thread, __enatp_lazy_thread, p);
        return ;
    }
#ifdef _WIN32_THREAD
    DWORD exitCode;
    GetExitCodeThread(p->lazy_thread, &exitCode);
    if(exitCode != STILL_ACTIVE)
#else
    else if(pthread_kill(p->lazy_thread, 0) == ESRCH)
#endif
    {
        ethread_join(p->lazy_thread);        // wo sure the thread is over, so this will not blocking, only release the resource of the quited thread
        ethread_init(p->lazy_thread, __enatp_lazy_thread, p);
    }
}

static inline int __enatp_chkName(enatp p, constr name)
{
    return !ejson_r(p->name_groups, name);
}

static inline ejson __enatp_getGroup(enatp p, constr name)
{
    ejson egroup;

    enatp_lock(p);

    egroup = ejson_r(p->name_groups, name);

    if(!egroup)
    {
        assert(egroup = ejson_addT(p->name_groups, name, EOBJ));
    }

    enatp_ulck(p);

    return egroup;
}

static inline int __enatp_getAvailNameIdForGroup(enatp p, ejson egroup, char ename[64])
{
    enatp_lock(p);

    int id = ejson_size(egroup);

    do
    {
        snprintf(ename, 64, "%s.%d", eobj_keyS(egroup),++id);
    }while(ejson_r(egroup, ename));

    enatp_ulck(p);

    return id;
}

static inline void __enatp_addNameToGroup(enatp p, ejson egroup, constr id, enats e)
{
    enatp_lock(p);

    if(!egroup) egroup = p->name_groups;

    assert(ejson_addP(egroup, id, e));

    enatp_ulck(p);
}

static inline void __enatp_rmEmptyGroup(enatp p, ejson egroup)
{
    enatp_lock(p);

    if(ejson_size(egroup) == 0)
        ejson_freeO(p->name_groups, egroup);

    enatp_ulck(p);
}

static inline enats __enatp_addEnats(enatp p, constr name, constr urls, enats_opts opts, int lazy)
{
    enats e;

    e = __enats_newHandle(urls, opts);
    is0_exeret(e, errset(p, _err);p->s = NATS_ERR;, 0);

    enatp_lock(p);

    strncpy(e->name, name, 64);
    e->p        = p;
    e->conn.CBs = p ->CBs;

    if(__enats_tryConnect(e))
    {
        enatp_add_conntrans(p, e);
        if(!p->polling_now) __enatp_pollingNext(p, 0);

        if(e->conn.CBs.connected_cb)
            e->conn.CBs.connected_cb(e, e->conn.CBs.connected_closure);
    }
    else if(lazy)
    {
        enatp_add_lazytrans(p, e);
        __enatp_exeLazyThread(p);
    }
    else
    {
        __enats_release(e);

        e = 0;
    }

    enatp_ulck(p);

    return e;
}

static inline int __enatp_addConn(enatp p, constr name, constr urls, enats_opts opts, int lazy, int group)
{
    char ename[64]; int i, cnt, added; esplt separate_urls = 0; ejson egroup; enats e;

    added = 0;

    if(!name || name <= ENATP_ALL_TRANS || !*name)
    {
        name = "conn";
        cnt  = group ? esplt_splitS(separate_urls, urls, ",") : esplt_splitS(separate_urls, urls, "");
    }
    else
        cnt  = group ? esplt_splitS(separate_urls, urls, ",") : 0;

    if(separate_urls)
    {
        egroup = __enatp_getGroup(p, name);

        for(i = 0; i < cnt; i++)
        {
            __enatp_getAvailNameIdForGroup(p, egroup, ename);

            if( (e = __enatp_addEnats(p, ename, separate_urls[i], opts, lazy)) )
            {
                __enatp_addNameToGroup(p, egroup, ename, e);
                added++;
            }
        }

        __enatp_rmEmptyGroup(p, egroup);
    }
    else
    {
        is1_exe(!__enatp_chkName(p, name), errfmt(p, "name \"%s\" already exist", name);goto err_ret;);

        if( (e = __enatp_addEnats(p, name, urls, opts, lazy)) )
        {
            __enatp_addNameToGroup(p, 0, name, e);
            added++;
        }
    }

err_ret:

    esplt_free(separate_urls);
    p->s = added ? NATS_OK : NATS_ERR;

    return added;
}

static inline ejson __enatp_getEnatsRooms(enatp p, constr name)
{
    ejson eroom;

    if(!name || name <= ENATP_ALL_TRANS || !*name)
    {
        name  = "conn";
    }

    // -- search name
    if     (name == ENATP_CONN_TRANS) eroom = p->conn_transs;
    else if(name == ENATP_LAZY_TRANS) eroom = p->lazy_transs;
    else if(name == ENATP_ALL_TRANS)  eroom = p->name_groups;
    else {
        enatp_lock(p);
        eroom = ejson_r(p->name_groups, name);
        enatp_ulck(p);
    }

    is0_exe(eroom, errfmt(p, "can not find enats named \"%s\"", name));

    return eroom;
}

/// -------------------------- enatp ------------------------------
enatp enatp_new()
{
    return __enatp_newHandle();
}

void enatp_destroy(enatp p)
{
    int can_free;

    is1_ret(!p || p->quit, );

    enatp_lock(p);

    p->quit = 1;

    __enatp_waitLazy(p);

    __enatp_destroyLazy(p);
    __enatp_destroyName(p);

    can_free = __enatp_destroyConn(p);

    enatp_ulck(p);

    if(can_free)
    {
        if(p->wait_num)  __enatp_quitWait(p);    // p will be free in __enatp_exeWait()
        else             __enatp_freeHandle(p);
    }
}

void   enatp_join(enatp p)
{
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), );

    __enatp_exeWait(p);
}

int   enatp_addUrls(enatp p, constr name, constr     urls, int opts2)
{
    // -- check args
    is0_exeret(p              , errset(p, "invalid enatp (nullptr)"     );                , 0);
    is1_exeret(!urls || !*urls, errset(p, "invalid urls (null or empty)");p->s = NATS_ERR;, 0);

    return __enatp_addConn(p, name, urls, 0, opts2 & ENATP_LAZY, opts2 & ENATP_GROUP);
}

int   enatp_addOpts(enatp p, constr name, enats_opts opts, int opts2)
{
    estr urls, auth, user, pass; int cnt;

    // -- check args
    is0_exeret(p   , errset(p, "invalid enatp (nullptr)");                , 0);
    is0_exeret(opts, errset(p, "invalid opts (nullptr)" );p->s = NATS_ERR;, 0);

    urls = opts->urls;
    auth = opts->auth;
    user = opts->user;
    pass = opts->pass;

    if((user && *user) && (pass && *pass)) urls = __enats_makeUrls(user, pass, urls, 0);
    else if((auth && *auth))               urls = __enats_makeUrls(auth, 0   , urls, 0);
    else                                   urls = __enats_makeUrls(0   , 0   , urls, 0);

    is0_exeret(urls , errset(p, _err);p->s = NATS_ERR;, 0);

    cnt =  __enatp_addConn(p, name, urls, opts, opts2 & ENATP_LAZY, opts2 & ENATP_GROUP);

    estr_free(urls);

    return cnt;
}

int   enatp_setPubMax(enatp p, u64 cnt)
{
    is0_ret(p, 0);

    p->max = cnt;
    return 1;
}

void enatp_setConnectedCB(enatp p, constr name, enats_evtHandler cb, void* closure)
{
    enats e; ejson eroom;

    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), );

    eroom = (ENATP_DFT == name) ? __enatp_getEnatsRooms(p, ENATP_ALL_TRANS)
                                : __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if((e = EOBJ_VALP(eroom)))
    {
        e->conn.CBs.connected_cb      = cb;
        e->conn.CBs.connected_closure = closure;
        e->conn.CBs.connected_setted  = 1;
    }
    else if(ENATP_DFT == name)
    {
        p->CBs.connected_cb      = cb;
        p->CBs.connected_closure = closure;

        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                if(!e->conn.CBs.connected_setted)      // if not set, set it
                {
                    e->conn.CBs.connected_cb      = cb;
                    e->conn.CBs.connected_closure = closure;
                }

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                e = EOBJ_VALP(itr2);

                if(!e->conn.CBs.connected_setted)
                {
                    e->conn.CBs.connected_cb      = cb;
                    e->conn.CBs.connected_closure = closure;
                }
            }
        }
    }
    else
    {
        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                e->conn.CBs.connected_cb      = cb;
                e->conn.CBs.connected_closure = closure;
                e->conn.CBs.connected_setted  = 1;

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                e = EOBJ_VALP(itr2);

                e->conn.CBs.connected_cb      = cb;
                e->conn.CBs.connected_closure = closure;
                e->conn.CBs.connected_setted  = 1;
            }
        }
    }
    enatp_ulck(p);
}

void enatp_setClosedCB(enatp p, constr name, enats_evtHandler cb, void* closure)
{
    enats e; ejson eroom;

    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), );

    eroom = (ENATP_DFT == name) ? __enatp_getEnatsRooms(p, ENATP_ALL_TRANS)
                                : __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if((e = EOBJ_VALP(eroom)))
    {
        enats_setClosedCB(e, cb, closure);
    }
    else if(ENATP_DFT == name)
    {
        p->CBs.closed_cb      = cb;
        p->CBs.closed_closure = closure;

        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                if(!e->conn.CBs.closed_setted)      // if not set, set it
                {
                    e->conn.CBs.closed_cb      = cb;
                    e->conn.CBs.closed_closure = closure;
                }

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                e = EOBJ_VALP(itr2);

                if(!e->conn.CBs.closed_setted)
                {
                    e->conn.CBs.closed_cb      = cb;
                    e->conn.CBs.closed_closure = closure;
                }
            }
        }
    }
    else
    {
        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                enats_setClosedCB(e, cb, closure);

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                enats_setClosedCB(EOBJ_VALP(itr2), cb, closure);
            }
        }
    }

    enatp_ulck(p);
}
void enatp_setDisconnectedCB(enatp p, constr name, enats_evtHandler cb, void* closure)
{
    enats e; ejson eroom;

    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), );

    eroom = (ENATP_DFT == name) ? __enatp_getEnatsRooms(p, ENATP_ALL_TRANS)
                                : __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if((e = EOBJ_VALP(eroom)))
    {
        enats_setDisconnectedCB(e, cb, closure);
    }
    else if(ENATP_DFT == name)
    {
        p->CBs.disconnected_cb      = cb;
        p->CBs.disconnected_closure = closure;

        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                if(!e->conn.CBs.disconnected_setted)      // if not set, set it
                {
                    e->conn.CBs.disconnected_cb      = cb;
                    e->conn.CBs.disconnected_closure = closure;
                }

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                e = EOBJ_VALP(itr2);

                if(!e->conn.CBs.disconnected_setted)
                {
                    e->conn.CBs.disconnected_cb      = cb;
                    e->conn.CBs.disconnected_closure = closure;
                }
            }
        }
    }
    else
    {
        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                enats_setDisconnectedCB(e, cb, closure);

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                enats_setDisconnectedCB(EOBJ_VALP(itr2), cb, closure);
            }
        }
    }
    enatp_ulck(p);
}

void  enatp_setReconnectedCB(enatp p, constr name, enats_evtHandler cb, void* closure)
{
    enats e; ejson eroom;

    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), );

    eroom = (ENATP_DFT == name) ? __enatp_getEnatsRooms(p, ENATP_ALL_TRANS)
                                : __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if((e = EOBJ_VALP(eroom)))
    {
        enats_setReconnectedCB(e, cb, closure);
    }
    else if(ENATP_DFT == name)
    {
        p->CBs.reconnected_cb      = cb;
        p->CBs.reconnected_closure = closure;

        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                if(!e->conn.CBs.reconnected_setted)      // if not set, set it
                {
                    e->conn.CBs.reconnected_cb      = cb;
                    e->conn.CBs.reconnected_closure = closure;
                }

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                e = EOBJ_VALP(itr2);

                if(!e->conn.CBs.reconnected_setted)
                {
                    e->conn.CBs.reconnected_cb      = cb;
                    e->conn.CBs.reconnected_closure = closure;
                }
            }
        }
    }
    else
    {
        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                enats_setReconnectedCB(e, cb, closure);

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                enats_setReconnectedCB(EOBJ_VALP(itr2), cb, closure);
            }
        }
    }
    enatp_ulck(p);
}

void enatp_setErrHandler(enatp p, constr name, enats_errHandler cb, void* closure)
{
    enats e; ejson eroom;

    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), );

    eroom = (ENATP_DFT == name) ? __enatp_getEnatsRooms(p, ENATP_ALL_TRANS)
                                : __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if((e = EOBJ_VALP(eroom)))
    {
        enats_setErrHandler(e, cb, closure);
    }
    else if(ENATP_DFT == name)
    {
        p->CBs.err_handler = cb;
        p->CBs.err_closure = closure;

        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                if(!e->conn.CBs.err_setted)      // if not set, set it
                {
                    e->conn.CBs.err_handler = cb;
                    e->conn.CBs.err_closure = closure;
                }

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                e = EOBJ_VALP(itr2);

                if(!e->conn.CBs.err_setted)
                {
                    e->conn.CBs.err_handler = cb;
                    e->conn.CBs.err_closure = closure;
                }
            }
        }
    }
    else
    {
        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                enats_setErrHandler(e, cb, closure);

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                enats_setErrHandler(EOBJ_VALP(itr2), cb, closure);
            }
        }
    }
    enatp_ulck(p);
}

inline natsStatus  enatp_pub(enatp p, constr subj, conptr data, int dataLen)
{
    enats e; ejson itr; natsStatus s = NATS_ERR;

    // -- check args
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);

    enatp_lock(p);

    is1_exeret(p->max && p->cnt >= p->max, enatp_ulck(p);, NATS_ERR);

    // -- traverse
    ejson_foreach(p->conn_transs, itr)
    {
        e = EOBJ_VALP(itr);

        //s = enats_pub(e, subj, data, dataLen);
        s = natsConnection_Publish(e->conn.nc, subj, data, dataLen);
        if(s == NATS_OK)
        {
            p->cnt++;
            break;
        }
    }

    enatp_ulck(p);
    return p->s = s;
}

natsStatus  enatp_pubf(enatp p, constr subj, conptr data, int dataLen)
{
    enats e; ejson itr; natsStatus s = NATS_ERR;

    // -- check args
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);

    enatp_lock(p);

    // -- traverse
    ejson_foreach(p->conn_transs, itr)
    {
        e = EOBJ_VALP(itr);

        //s = enats_pub(e, subj, data, dataLen);
        s = natsConnection_Publish(e->conn.nc, subj, data, dataLen);
        if(s == NATS_OK)
        {
            p->cnt++;
            break;
        }
    }

    enatp_ulck(p);
    return p->s = s;
}

natsStatus  enatp_pubr(enatp p, constr subj, conptr data, int dataLen, constr reply)
{
    enats e; ejson itr; natsStatus s = NATS_ERR;

    // -- check args
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);

    enatp_lock(p);

    is1_exeret(p->max && p->cnt >= p->max, enatp_ulck(p);, NATS_ERR);

    // -- traverse
    ejson_foreach(p->conn_transs, itr)
    {
        e = EOBJ_VALP(itr);

        // s = enats_pubr(e, subj, reply, data, dataLen);
        s = natsConnection_PublishRequest(e->conn.nc, subj, reply, data, dataLen);

        if(s == NATS_OK)
        {
            p->cnt++;
            break;
        }
    }

    enatp_ulck(p);
    return p->s = s;
}

natsStatus enatp_sub(enatp p, constr name, constr subj, enats_msgHandler onMsg, void* closure)
{
    ejson eroom; enats e = 0; int subed = 0;

    is0_exeret(p                 , errset(p, "invalid enatp (nullptr)")         ,        NATS_ERR);
    is0_exeret(onMsg             , errfmt(p, "invalid callbacks (nullptr)");    , p->s = NATS_ERR);
    is1_exeret(!subj || !*subj   , errset(p, "invalid subj (nullptr or empty)");, p->s = NATS_ERR);
    is1_exeret(strstr(subj, ".."), errfmt(p, "invalid subj (contain \"..\")");  , p->s = NATS_ERR);

    eroom = __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if((e = EOBJ_VALP(eroom)))
    {
        if(__enats_sub(e, subj, onMsg, closure)) subed++;
    }
    else
    {
        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                if(__enats_sub(e, subj, onMsg, closure)) subed++;

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                if(__enats_sub(EOBJ_VALP(itr2), subj, onMsg, closure)) subed++;
            }
        }
    }
    enatp_ulck(p);

    return p->s = subed ? NATS_OK : NATS_NO_SERVER;
}

natsStatus  enatp_unsub(enatp p, constr name, constr subj)
{
    ejson eroom; enats e = 0;

    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);
    is1_exeret(!subj || !*subj   , errset(p, "invalid subj (nullptr or empty)");, p->s = NATS_ERR);
    is1_exeret(strstr(subj, ".."), errfmt(p, "invalid subj (contain \"..\")");  , p->s = NATS_ERR);

    eroom = __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if((e = EOBJ_VALP(eroom)))
    {
        __enats_unSub(e, subj);
    }
    else
    {
        ejson_foreach_s(eroom, itr1)
        {
            e = EOBJ_VALP(itr1);
            if(e)
            {
                __enats_unSub(e, subj);

                continue;
            }

            ejson_foreach_s(itr1, itr2)
            {
                __enats_unSub(EOBJ_VALP(itr2), subj);
            }
        }
    }

    enatp_ulck(p);

    return p->s = NATS_OK;
}

natsStatus  enatp_req(enatp p, constr subj, conptr data, int dataLen, constr reply, eMsg* replyMsg, int64_t timeout)
{
    ejson itr; enats e = 0;

    // -- check args
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);

    // -- travles
    enatp_lock(p);

    is1_exeret(p->max && p->cnt >= p->max, enatp_ulck(p);, NATS_ERR);

    ejson_foreach(p->conn_transs, itr)
    {
        e = EOBJ_VALP(itr);

        if(natsConnection_Status(e->conn.nc) == CONNECTED)
        {
            p->cnt++;       // we assume that the next pub will ok
            break;
        }
    }
    enatp_ulck(p);

    // p->s = enats_req(e, subj, data, dataLen, reply, replyMsg, timeout);
    p->s = __enats_request(replyMsg, e->conn.nc, subj, data, dataLen, timeout, reply);

    return p->s;
}

inline natsStatus  enatp_pubPoll(enatp p, constr subj, conptr data, int dataLen)
{
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);

    enatp_lock(p);

    is1_exeret(p->max && p->cnt >= p->max, enatp_ulck(p);, NATS_ERR);

    is0_exeret(p->polling_now, errset(p, "have no connected enats");enatp_ulck(p);, p->s = NATS_ERR);

    p->s = natsConnection_Publish(p->polling_now->conn.nc, subj, data, dataLen);
    if(p->s == NATS_OK) p->cnt++;

    __enatp_pollingNext(p, 0);

    enatp_ulck(p);

    return p->s;
}

natsStatus  enatp_pubrPoll(enatp p, constr subj, conptr data, int dataLen, constr reply)
{
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);

    enatp_lock(p);

    is1_exeret(p->max && p->cnt >= p->max, enatp_ulck(p);, NATS_ERR);

    is0_exeret(p->polling_now, errset(p, "have no connected enats");enatp_ulck(p);, p->s = NATS_ERR);

    p->s = enats_pubr(p->polling_now, subj, data, dataLen, reply);
    if(p->s == NATS_OK) p->cnt++;

    __enatp_pollingNext(p, 0);

    enatp_ulck(p);

    return p->s;
}

natsStatus  enatp_reqPoll(enatp p, constr subj, conptr data, int dataLen, constr reply, eMsg* replyMsg, int64_t timeout)
{
    enats e;
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NATS_ERR);

    enatp_lock(p);

    is1_exeret(p->max && p->cnt >= p->max, enatp_ulck(p);, NATS_ERR);

    is0_exeret(p->polling_now, errset(p, "have no polling enats in enatp");enatp_ulck(p);, p->s);
    e = p->polling_now;
    p->cnt++;

    __enatp_pollingNext(p, 0);

    enatp_ulck(p);

    p->s = __enats_request(replyMsg, e->conn.nc, subj, data, dataLen, timeout, reply);

    return p->s;
}

int    enatp_cntTrans(enatp p, constr name)
{
    ejson eroom; int cnt = 0;

    is0_ret(p, 0); is0_ret(name, 0);

    eroom = __enatp_getEnatsRooms(p, name);

    enatp_lock(p);

    if(name == ENATP_CONN_TRANS || name == ENATP_LAZY_TRANS)
        cnt = ejson_size(eroom);
    else
    {
        if((EPTR == eobj_typeo(eroom)))
        {
            cnt = 1;
        }
        else
        {
            ejson_foreach_s(eroom, itr1)
            {
                cnt += (EPTR == eobj_typeo(itr1)) ? 1 : ejson_size(itr1);
            }
        }
    }

    enatp_ulck(p);

    return cnt;
}

constr enatp_connurls(enatp p, int hidepass)
{
    enats e; ejson itr;

    // -- check args
    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NULL); estr_clear(p->connurls);
    is0_ret(ejson_size(p->conn_transs), NULL);

    // -- travles
    enatp_lock(p);
    p->s = NATS_OK;

    ejson_foreach(p->conn_transs, itr)
    {
        e = EOBJ_VALP(itr);

        __enats_getConnUrls(e, hidepass);

        estr_catF(p->connurls, "[%s]%S ", e->name, e->connurl);
    }
    enatp_ulck(p);
    estr_setT(p->connurls, '\0');

    return p->connurls;
}

constr enatp_lazyurls(enatp p, int hidepass)
{
    enats e; natsOptions* opts; ejson itr; cstr url; int j, c;

    // -- check args
    is0_exeret(p, errset(p, "invalid enatp (nullptr)");, NULL); estr_clear(p->lazyurls);
    is0_ret(ejson_size(p->lazy_transs), NULL);

    // -- travles
    enatp_lock(p);
    p->s = NATS_OK;

    ejson_foreach(p->lazy_transs, itr)
    {
        e = EOBJ_VALP(itr);
        opts = e->conn.opts;

        estr_catF(p->lazyurls, "[%s]", e->name);

        c = opts->url ? 1 : opts->serversCount;
        for(j = 0; j < c; j++)
        {
            url  = c == 1 ? opts->url : opts->servers[j];
            estr_catF(p->lazyurls, "%s,", url);
        }
        estr_setT(p->lazyurls, ' ');
    }
    estr_setT(p->lazyurls, '\0');
    enatp_ulck(p);

    if(hidepass) __enats_hide_passwd(p->lazyurls);

    return p->lazyurls;
}

constr*enatp_connurlsL(enatp p, int hidepass, int* cnt)
{
    int _cnt; estr connurls;

    cnt ? *cnt = 0 : 0;

    connurls = (estr)enatp_connurls(p, hidepass);
    is0_ret(connurls, 0);

    _cnt = esplt_splitE(p->connurls_l, connurls, " ");

    cnt ? *cnt = _cnt : 0;

    return (constr*)p->connurls_l;
}

constr*enatp_lazyurlsL(enatp p, int hidepass, int* cnt)
{
    int _cnt; estr lazyurls;

    cnt ? *cnt = 0 : 0;

    lazyurls = (estr)enatp_lazyurls(p, hidepass);
    is0_ret(lazyurls, 0);

    _cnt = esplt_splitE(p->lazyurls_l, lazyurls, " ");

    cnt ? *cnt = _cnt : 0;

    return (constr*)p->lazyurls_l;
}

static inline void __enatp_stats(enatp p, constr subj, enats_stats_t* _stats, int format, int detail)
{
    enats e; ejson itr; enats_stats_t stats;

    memset(_stats, 0, sizeof(*_stats));

    is0_exeret(enatp_cnt_conntrans(p), estr_clear(p->statss);, );

    enatp_lock(p);

    p->s = NATS_OK;
    ejson_foreach(p->conn_transs, itr)
    {
        e = EOBJ_VALP(itr);

        __enats_stats(e, subj, &stats, 0);

        if (e->conn.s == NATS_OK)
        {
            _stats->inMsgs          += stats.inMsgs;
            _stats->outMsgs         += stats.outMsgs;
            _stats->inBytes         += stats.inBytes;
            _stats->outBytes        += stats.outBytes;
            _stats->reconnects      += stats.reconnects;
            _stats->pendingMsgs     += stats.pendingMsgs;
            _stats->pendingBytes    += stats.pendingBytes;
            _stats->maxPendingMsgs  += stats.maxPendingMsgs;
            _stats->maxPendingBytes += stats.maxPendingBytes;
            _stats->deliveredMsgs   += stats.deliveredMsgs;
            _stats->droppedMsgs     += stats.droppedMsgs;

            if(format && detail)
                estr_catP(p->statss, "[%s]\t"
                                 "In Msgs: %9" PRIu64 " - In Bytes: %9" PRIu64 " - Out Msgs: %9" PRIu64 " - Out Bytes: %9" PRIu64 " - "
                                 "Delivered: %9" PRId64 " - Pending: %5d - Dropped: %5" PRId64 " - Reconnected: %3" PRIu64 "\n",
                                 e->name,
                                 stats.inMsgs       , stats.inBytes    , stats.outMsgs    , stats.outBytes,
                                 stats.deliveredMsgs, stats.pendingMsgs, stats.droppedMsgs, stats.reconnects);
        }
        else
        {
            p->s = NATS_ERR;
            errfmt(p, "faild when get statistics for trans [%s]", e->name);
            continue;
        }
    }

    if(format)
        estr_catP(p->statss, "[SUM]\t"
                         "In Msgs: %9" PRIu64 " - In Bytes: %9" PRIu64 " - Out Msgs: %9" PRIu64 " - Out Bytes: %9" PRIu64 " - "
                         "Delivered: %9" PRId64 " - Pending: %5d - Dropped: %5" PRId64 " - Reconnected: %3" PRIu64 "",
                         _stats->inMsgs,        _stats->inBytes,     _stats->outMsgs,     _stats->outBytes,
                         _stats->deliveredMsgs, _stats->pendingMsgs, _stats->droppedMsgs, _stats->reconnects);
    else
        estr_clear(p->statss);

    enatp_ulck(p);
}

int enatp_stats (enatp p, constr subj, enats_stats_t* stats)
{
    is1_exeret(!p || !stats, errset(p, "invalid param (nullptr)"), 0);

    __enatp_stats(p, subj, stats, 0, 0);

    return 1;
}

constr enatp_statsS(enatp p, constr subj, int detail)
{
    enats_stats_t sum;

    is0_exeret(p, errset(p, "invalid enatp (nullptr)"), NULL);

    __enatp_stats(p, subj, &sum, 1, detail);

    return p->statss;
}


inline constr enatp_err(enatp p)
{
    return p ? p->err : _err;
}

static void __on_closed(natsConnection* nc, void* trans)
{
    enatp p; enats e; int quit_wait = 0;

    p = NULL;
    e = (enats)trans;

    if(e->p)
    {
        p = e->p;
        enatp_lock(p);

        if(!p->quit)
        {
            __enatp_pollingNext(p, e);

            enatp_del_conntrans(p, e);
            enatp_add_lazytrans(p, e);

            __enats_destroyAllNatsSubscription(e);

            e->conn.nc = 0;
            natsConnection_Destroy(nc);

            __enatp_exeLazyThread(p);
        }

        enatp_ulck(p);
    }

    if(e->conn.CBs.closed_cb)
        e->conn.CBs.closed_cb(e, e->conn.CBs.closed_closure);

    if(e->quit)
        __enats_release(e);

    if(p && p->quit)
    {
        emutex_lock(p->mutex);
        if(p->conn_num > 0)     // p->conn_num will be set only when call nTPool_destroy()
            p->conn_num --;

        if(p->conn_num == 0)
        {
            if(p->wait_num)  quit_wait = 1;
            else             __enatp_freeHandle(p);
        }
        else
            emutex_ulck(p->mutex);
    }

    if(quit_wait)
    {
        __enatp_quitWait(p);
    }
}

static void __on_disconnected(natsConnection* nc, void* trans)
{
    enatp p; enats e = (enats)trans;

    if(e->p)
    {
        p = e->p;

        enatp_lock(p);

        __enatp_pollingNext(p, e);
        enatp_del_conntrans(p, e);

        enatp_ulck(p);
    }

    if(e->conn.CBs.disconnected_cb)
        e->conn.CBs.disconnected_cb(e, e->conn.CBs.disconnected_closure);
}

static void __on_reconnected(natsConnection* nc, void* trans)  // handler connect reconnected
{
    enatp p; enats e = (enats)trans;

    if(e->p)
    {
        p = e->p;

        enatp_lock(p);
        enatp_add_conntrans(p, e);
        if(!p->polling_now) __enatp_pollingNext(p, 0);
        enatp_ulck(p);
    }

    estr_wrtS(e->lasturl, nc->opts->url);

    if(e->conn.CBs.reconnected_cb)
        e->conn.CBs.reconnected_cb(e, e->conn.CBs.reconnected_closure);
}

static void __on_erroccurred(natsConnection *nc, natsSubscription *subscription, natsStatus err, void *trans)
{
    enats e = (enats)trans;

    if(e->conn.CBs.err_handler)
        e->conn.CBs.err_handler(e, subscription, err, e->conn.CBs.err_closure);
}
