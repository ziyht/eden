/// =====================================================================================
///
///       Filename:  enats.h
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

#ifndef __ENATS_H__
#define __ENATS_H__

#include "nats/nats.h"
#include "etype.h"

#define ENATS_VERSION     "enats 1.0.16"     // fix ret of enatp_sub

#ifdef __cplusplus
extern "C" {
#endif

typedef struct enats_statistics_s{
    // -- for conn
    u64  inMsgs;
    u64  outMsgs;
    u64  inBytes;
    u64  outBytes;
    u64  reconnects;

    // -- for sub
    int  pendingMsgs;
    int  pendingBytes;
    int  maxPendingMsgs;
    int  maxPendingBytes;
    i64  deliveredMsgs;
    i64  droppedMsgs;
}enats_stats_t;

typedef struct enats_opts_s {
    cstr    urls;               // urls syntax: nats://[[user][:passwd]@]/[auth@]server:port[,nats://...]
    cstr    auth;               // auth for single url in urls, if the url havn't a auth or user:pass, it'll be made up by auth
    cstr    user;               // user for single url in urls, if the url havn't a auth or user:pass, it'll be made up by user:pass, user and pass should exist simultaneously and has higher priority
    cstr    pass;               // pass for single url in urls, if the url havn't a auth or user:pass, it'll be made up by user:pass, user and pass should exist simultaneously and has higher priority

    struct {                    // -- tls support
        int     enanle;
        cstr    ca;                 // client ca   file, lg: ca.pem     , this file can be empty
        cstr    key;                // client key  file, lg: key.pem
        cstr    cert;               // client cert file, lg: cert.pem
    }       tls;

    i64     timeout;            // timeout for connecting
}enats_opts_t, * enats_opts;

typedef struct eMsg_s
{
    cptr    __1, __2;

    constr  subject;
    constr  reply;
    constr  data;
    int     dataLen;
}eMsg_t, * eMsg;

static inline void eMsg_free(eMsg msg){ if(msg->__2) natsMsg_Destroy((natsMsg*)msg); else free(msg); }

typedef struct enats_s* enats;
typedef struct enatp_s* enatp;

/// -- callbacks type
typedef void (*enats_evtHandler) (enats t, void* closure);
typedef void (*enats_errHandler) (enats t, natsSubscription *subscription, natsStatus err, void* closure);
typedef void (*enats_msgHandler) (enats t, natsSubscription *sub, eMsg msg, void *closure);


/// ====================== enats ==================================
///
///     enats - an easier using wrapper for cnats
///


/// --------------------- enats new -------------------------------
enats enats_newUrl1(constr urls);                                           // urls syntax: nats://[user:passwd@]/[auth@]server:port[,nats://...]
enats enats_newUrl2(constr auth, constr urls);                              // if the single url in urls not have user:pass or auth, it'll made up by auth
enats enats_newUrl3(constr user, constr pass, constr urls);                 // if the single url in urls not have user:pass or auth, it'll made up by user:pass, user will be consider as auth if pass not exist
enats enats_newUrl4(constr user, constr pass, constr server, int port);     // the url will be made up by fmt: nats://user:pass@server:port, user will be consider as auth if pass not exist
enats enats_newOpts(enats_opts opts);

void  enats_join(enats e);          // blocking until e is been destoried
void  enats_destroy(enats e);

/// -------------------- enats callbacks --------------------------
void enats_setClosedCB      (enats e, enats_evtHandler cb, void* closure);
void enats_setDisconnectedCB(enats e, enats_evtHandler cb, void* closure);
void enats_setReconnectedCB (enats e, enats_evtHandler cb, void* closure);
void enats_setErrHandler    (enats e, enats_errHandler cb, void* closure);

/// -------------------- enats msg transfer --------------------------
natsStatus  enats_pub (enats e, constr subj, conptr data, int dataLen);
natsStatus  enats_pubr(enats e, constr subj, conptr data, int dataLen, constr reply);

natsStatus  enats_sub  (enats e, constr subj, enats_msgHandler onMsg, void* closure);
natsStatus  enats_unsub(enats e, constr subj);

natsStatus  enats_req(enats e, constr subj, conptr data, int dataLen, constr reply, eMsg* replyMsg, int64_t timeout);

/// ---------------------- enats utils ----------------------------
constr enats_allurls(enats e, int hidepass);              // return all urls linked by this enats, if hidepass == 1, the whole passwd or auth(only user) will be replaced by "*"
constr enats_connurl(enats e, int hidepass);              // return the url  connnected by now
constr enats_lasturl(enats e, int hidepass);              // return the url  conneected at last time

constr enats_name(enats e);                 // return the name of  this enats
enatp  enats_pool(enats e);                 // return the pool who handle this enats

int    enats_stats (enats e, constr subj, enats_stats_t* stats);
constr enats_statsS(enats e, constr subj);

constr enats_err(enats e);



/// ====================== enatp ==================================
///
///     enatp - an enats pool to handle enats more convenient
///

/// --- macro names ---
#define ENATP_DFT        (constr)0    // set default settings
#define ENATP_CONN_TRANS (constr)1    // all connected  trans will be operated
#define ENATP_LAZY_TRANS (constr)2    // all connecting trans will be operated
#define ENATP_ALL_TRANS  (constr)3    // all            trans will be operated

/// --- enatp opts2 settings ---
#define ENATP_LAZY  0x01    // add new urls in lazy mode
#define ENATP_GROUP 0x02    // add new urls in group

/// --------------------- enatp new -------------------------------
enatp enatp_new();

void  enatp_join(enatp p);
void  enatp_destroy(enatp p);

int   enatp_addUrls(enatp p, constr name, constr     urls, int opts2);
int   enatp_addOpts(enatp p, constr name, enats_opts opts, int opts2);

int   enatp_setPubMax(enatp p, u64 cnt);

/// -------------------- enatp callbacks --------------------------

void  enatp_setConnectedCB   (enatp p, constr name, enats_evtHandler cb, void* closure);
void  enatp_setClosedCB      (enatp p, constr name, enats_evtHandler cb, void* closure);
void  enatp_setDisconnectedCB(enatp p, constr name, enats_evtHandler cb, void* closure);
void  enatp_setReconnectedCB (enatp p, constr name, enats_evtHandler cb, void* closure);
void  enatp_setErrHandler    (enatp p, constr name, enats_errHandler cb, void* closure);

/// -------------------- enats msg transfer -----------------------

/// -- normal mode --
///     the msg will be publishing via the first connected enats
///
natsStatus  enatp_pub (enatp p, constr subj, conptr data, int dataLen);
natsStatus  enatp_pubf(enatp p, constr subj, conptr data, int dataLen);     // in force mode

natsStatus  enatp_pubr(enatp p, constr subj, conptr data, int dataLen, constr reply);

natsStatus  enatp_sub  (enatp p, constr name, constr subj, enats_msgHandler onMsg, void* closure);
natsStatus  enatp_unsub(enatp p, constr name, constr subj);

natsStatus  enatp_req(enatp p, constr subj, conptr data, int dataLen, constr reply, eMsg* replyMsg, int64_t timeout);

/// -- poll mode --
///     the msg will be publishing via polling all the connected enats, a msg only publish once
///
natsStatus  enatp_pubPoll (enatp p, constr subj, conptr data, int dataLen);
natsStatus  enatp_pubrPoll(enatp p, constr subj, conptr data, int dataLen, constr reply);

natsStatus  enatp_reqPoll(enatp p, constr subj, conptr data, int dataLen, constr reply, eMsg* replyMsg, int64_t timeout);


/// ---------------------- enatp utils ----------------------------

int    enatp_cntTrans(enatp p, constr name);

constr enatp_connurls(enatp p, int hidepass);
constr enatp_lazyurls(enatp p, int hidepass);

constr*enatp_connurlsL(enatp p, int hidepass, int* cnt);    // connurls list
constr*enatp_lazyurlsL(enatp p, int hidepass, int* cnt);    // connurls list

int    enatp_stats (enatp p, constr subj, enats_stats_t* stats);
constr enatp_statsS(enatp p, constr subj, int detail);

constr enatp_err(enatp p);

#ifdef __cplusplus
}
#endif

#endif
