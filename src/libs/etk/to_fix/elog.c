/// =====================================================================================
///
///       Filename:  elog.c
///
///    Description:  a easier way to handle log
///
///        Version:  1.1
///        Created:  04/23/2017 11:30:34 AM
///       Revision:  none
///       Compiler:  gcc
///         Needed:  estr
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>

#include "elog.h"
#include "estr.h"
#include "ejson.h"
#include "ecompat.h"
#include "ethread.h"
#include "eutils.h"

/// -------------------------- micros helper ---------------------------------
#ifndef _WIN32
#define _path_gap '/'
#else
#define _path_gap '\\'
#endif

#if (!ELOG_MUTE)
static constr _llog_basename(constr path){static constr slash; if (slash) {return slash;}else{slash = strrchr(path, _path_gap);}if (slash) {return ++slash;}return 0;}
#ifndef _WIN32
#define _llog_err(fmt, ...)  fprintf(stderr, "%s(%d):" fmt "%s", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stderr)
#define _elog_err(...)       _llog_err(__VA_ARGS__, "\n")
#else
#if defined(_MSC_VER)
#define _elog_err(fmt, ...)  fprintf(stderr, "%s(%d):" fmt "\n", _llog_basename(__FILE__), __LINE__, __VA_ARGS__);fflush(stderr)
#else
#define _elog_err(fmt, ...)  fprintf(stderr, "%s(%d):" fmt "\n", _llog_basename(__FILE__), __LINE__, #__VA_ARGS__);fflush(stderr)
#endif
#endif
#else
#define _elog_err(...)
#endif

/// ------------------------------ helper -----------------------------------

static inline constr _file_basename(constr path){path = strrchr(path, _path_gap); return path ? ++path : 0;}

/// ------------------------------ elog -------------------------------------

typedef struct _elog_handle_s * _elogh;

#define ELOG_MAX_TAGS  16

#pragma pack(push, 1)
typedef union _elog_s {
    uint out_id;
    struct {
        uint id     :   16;
        uint level  :    4;
        uint grade  :    4;
        uint opts   :    8;
    }t;
}_elog_t, _elog;

typedef int (*__flush_cb)(_elogh h, _elog_t _e);
static int  __elog_fflush_nobuffer(_elogh h, _elog_t _e);
static int  __elog_fflush_buffered(_elogh h, _elog_t _e);

typedef struct _elog_handle_s{

    char    tag[ELOG_TAG_MAXLEN];
    elog    id;

    char    _elog_fmt_buf[sstr_needLen(ELOG_FORMAT_MAXLEN)];       // format str for log
    char    _time_fmt_buf[sstr_needLen(ELOG_FORMAT_MAXLEN)];       // format str for timezone

    sstr    elog_fmt;
    sstr    time_fmt;

    u8      level;          // log level, only the passed_in level > .level will execute
    u8      grade;          // log grade, only the passed_in grade > .grade will execute

    estr    path;                   // path to write
    int     fd;                     // opened fd for path

    emutex_t mu;                     // write mutex

    estr    msg;                    // msg to print out
    estr    msgs;                   // full msgs buffer
    uint    last_msg_pos;           // the position of last msg in .msgs
    uint    tag_pos[ELOG_MAX_TAGS]; // the position of level tag to output in msg
    uint    tag_poss;               // how many tag_poss in .tag_pos
    uint    cnt;                    // how many msgs now int .msgs
    uint    max_cnt;                // the max msgs we retain in .msgs, if is 0, we do not buffer msg, write the msg to file directly
    uint    max_len;                // the max leng of buffer, if is 0, we do not buffer msg, write the msg to file directly

    __flush_cb flush;

    struct {
        int  enable;
        int  last_rtt;
        int  period;
        int  terms;
        int  files;
        int  suffix;
        estr path;
    }rtt;

}_elog_handle_t;
#pragma pack(pop)

typedef struct _elogh_set_s
{
    _elogh           _set[ELOG_MAX_NUM];
    emutex_t         _mu;
    int              _fmtlen;
#ifdef _WIN32
    CRITICAL_SECTION _print_cs;
#endif
}_elogh_set_t;

static _elogh_set_t _elogh_set_buf;
static _elogh_set_t*_elogh_set = &_elogh_set_buf;

#define _LEVEL_TAG_DBG "DBG"
#define _LEVEL_TAG_INF "INF"
#define _LEVEL_TAG_WRN "WRN"
#define _LEVEL_TAG_ERR "ERR"
#define _LEVEL_TAG_LEN 3

static constr _LEVEL_TAG_MAP[] =
{
    _LEVEL_TAG_DBG,
    _LEVEL_TAG_INF,
    _LEVEL_TAG_WRN,
    _LEVEL_TAG_ERR
};

#ifndef _WIN32
static constr _LEVEL_CORLER_MAP[] = {
    "\033[36m" _LEVEL_TAG_DBG "\033[0m",
    "\033[32m" _LEVEL_TAG_INF "\033[0m",
    "\033[33m" _LEVEL_TAG_WRN "\033[0m",
    "\033[31m" _LEVEL_TAG_ERR "\033[0m"
};
#define _LEVEL_CORLER_LEN (sizeof("\033[36m" _LEVEL_TAG_DBG "\033[0m") - 1)
#else
static int _LEVEL_CORLER_MAP[] = {
    0xB,    // DBG
    0xA,    // INF
    0xE,    // WRN
    0xc     // ERR: RED
};
static HANDLE           hconsole;
#endif

static inline void __elog_env_init()
{
    static int _inited;

    is1_ret(_inited, );

    _inited = 1;

    emutex_init(_elogh_set->_mu);

#ifdef _WIN32
    InitializeCriticalSection(&_elogh_set->_print_cs);
    hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

#define __DFT_HANDLE  0
#define __NEW_HANDLE  1

static inline _elogh __elog_newHandle(int dft, int wanted_id)
{
    int id; _elogh h;

    __elog_env_init();

    emutex_lock(_elogh_set->_mu);
    if(dft == __DFT_HANDLE)
    {
        if(_elogh_set->_set[0]) return _elogh_set->_set[0];
        else
        {
            id = 0;
            goto make_new_handle;
        }
    }

    if(wanted_id)
    {
        is1_exeret(wanted_id >= ELOG_MAX_NUM, _elog_err("create new elog handle faild: wanted id(%d) out of range", wanted_id); , 0);
        is1_exeret(_elogh_set->_set[wanted_id]    , _elog_err("create new elog handle faild: wanted id(%d) has been used for \"%s\"", _elogh_set->_set[wanted_id]->tag);, 0);

        id = wanted_id;
    }
    else
    {
        for(id = 1; id < ELOG_MAX_NUM; id++)
        {
            if(0 == _elogh_set->_set[id])
                break;
        }
    }

    if(id == ELOG_MAX_NUM)
    {
        _elog_err("create new elog handle faild: full");
        emutex_ulck(_elogh_set->_mu);
        return 0;
    }

make_new_handle:
    // -- make a new elog_handle
    {
        h = calloc(1, sizeof(_elog_handle_t));
        if(!h)
        {
            _elog_err("alloc for new elog handle faild: %s", strerror(errno));
            emutex_ulck(_elogh_set->_mu);
            return 0;
        }

        h->fd       = -1;
        h->elog_fmt = sstr_init(h->_elog_fmt_buf, sstr_needLen(ELOG_FORMAT_MAXLEN));
        h->time_fmt = sstr_init(h->_time_fmt_buf, sstr_needLen(ELOG_FORMAT_MAXLEN));
        h->level    = ELOG_DBG;
        h->msg      = estr_newLen(0, 1024);
        h->msgs     = estr_newLen(0, 1024);
        h->id       = id;
        h->flush    = __elog_fflush_nobuffer;

        assert(sstr_wrtS(h->elog_fmt, ELOG_DF_ELOG_FORMAT));
        assert(sstr_wrtS(h->time_fmt, ELOG_DF_TIME_FORMAT));

        emutex_init(h->mu);
    }

    _elogh_set->_set[id] = h;
    emutex_ulck(_elogh_set->_mu);

    return h;
}

static inline _elogh __elog_getHandle(elog e)
{
    _elogh h;

    if(e == 0)
    {
        h = _elogh_set->_set[0] ? _elogh_set->_set[0] : __elog_newHandle(__DFT_HANDLE, 0);
        return h;
    }

    h = (e >= ELOG_MAX_NUM) ? NULL : _elogh_set->_set[e];

    return h;
}

static inline void __elog_setTag(_elogh h, constr name)
{
    emutex_lock(h->mu);
    if(name)
    {
        int cpylen = strlen(name);

        if(cpylen > (ELOG_TAG_MAXLEN - 1)) cpylen = ELOG_TAG_MAXLEN - 1;

        memset(h->tag, ' ', ELOG_TAG_MAXLEN - 1);
        memcpy(h->tag, name, cpylen);

        if(_elogh_set->_fmtlen < cpylen) _elogh_set->_fmtlen = cpylen;
    }
    emutex_ulck(h->mu);
}

static int  __elog_setPath(_elogh h, constr path)
{
    int fd, fd_h;

    if(path)
    {
        fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0666);
        if (fd == -1) {
            _elog_err("Unable to open %s: %s\n", path, strerror(errno));
            return 0;
        }

        fd_h = h->fd;

        emutex_lock(h->mu);

        h->fd   = fd;
        estr_wrtS(h->path, path);

        emutex_ulck(h->mu);

        if(fd_h >= 0) close(fd_h);
    }

    return 1;
}

static inline void __elog_setLevel(_elogh h, uint level, uint grade)
{
    if(level > ELOG_ERR) level = ELOG_ERR;

    h->level = level;
    h->grade = grade;
}

static inline void __elog_setLogFmt(_elogh h, constr fmt)
{
    emutex_lock(h->mu);
    sstr_wrtS(h->elog_fmt, fmt);
    emutex_ulck(h->mu);
}

static inline void __elog_setTimeFmt(_elogh h, constr fmt)
{
    emutex_lock(h->mu);
    sstr_wrtS(h->time_fmt, fmt);
    emutex_ulck(h->mu);
}

static inline int __elog_freeHandle(_elogh h)
{
    is0_ret(h, 0);

    _elogh_set->_set[h->id] = 0;

    estr_free(h->msg );
    estr_free(h->msgs);
    estr_free(h->path);
    estr_free(h->rtt.path);

    if(h->fd >= 0) close(h->fd);

    emutex_lock(h->mu);
    emutex_ulck(h->mu);
    emutex_free(h->mu);

    free(h);

    return 1;
}

elog*elog_getSet()
{
    __elog_env_init();

    return (elog*)_elogh_set;
}

static int __elog_fflush_to_file(_elogh h);
int  elog_setSet(elog* set)
{
    int i; _elogh h; _elogh_set_t* old_set;

    is1_ret(!set || set == (elog*)_elogh_set, 0);

    __elog_env_init();

    old_set    = _elogh_set;
    _elogh_set = (_elogh_set_t*)set;

    emutex_lock(old_set->_mu);

    for(i = 0; i < ELOG_MAX_NUM; i++)
    {
        h = old_set->_set[i];

        if(h)
        {
            __elog_fflush_to_file(h);
            __elog_freeHandle(h);
        }
    }
#ifdef _WIN32
    DeleteCriticalSection(&old_set->_print_cs);
#endif
    emutex_ulck(old_set->_mu);

    return 1;
}

elog elog_new(constr tag, constr path)
{
    elog_opts_t opts;

    memset(&opts, 0, sizeof(opts));

    opts.path        = path;

    return elog_newOpts(tag, &opts);
}

elog elog_newOpts(constr tag, elog_opts opts)
{
    _elogh h;

    h = __elog_newHandle(__NEW_HANDLE, opts->id);

    is0_ret(h, 0);

    __elog_setTag(h, tag);

    if(!__elog_setPath(h, opts->path))
    {
        __elog_freeHandle(h);
        return 0;
    }

    if(opts->buf.max_logs || opts->buf.max_size)
    {
        h->max_cnt = opts->buf.max_logs ? opts->buf.max_logs : UINT_MAX;
        h->max_len = opts->buf.max_size ? opts->buf.max_size : UINT_MAX;

        h->flush = __elog_fflush_buffered;
    }

    if(opts->rtt.enable)
    {
        h->rtt.enable = 1;

        h->rtt.period = opts->rtt.period ? opts->rtt.period : 3600 * 24;
        h->rtt.terms  = opts->rtt.terms;
        h->rtt.files  = opts->rtt.files;
    }

    return h->id;
}

int  elog_setLevel (elog e)
{
    _elogh h = __elog_getHandle(((_elog*)&e)->t.id);

    is0_exeret(h, _elog_err("elog e(%d) has not been set", ((_elog*)&e)->t.id);, 0);

    __elog_setLevel(h, ((_elog*)&e)->t.level, ((_elog*)&e)->t.grade);

    return 1;
}

int  elog_setBuffer(elog e, int enable)
{
    _elogh h = __elog_getHandle(((_elog*)&e)->t.id);

    is0_exeret(h, _elog_err("elog e(%d) has not been set", ((_elog*)&e)->t.id);, 0);

    if(enable)
    {
        if(h->flush == __elog_fflush_buffered) return 1;

        if(h->max_cnt && h->max_len)
        {
            h->flush = __elog_fflush_buffered;
            return 1;
        }
        else
            return 0;
    }

    h->flush = __elog_fflush_nobuffer;

    return 1;
}

int  elog_setRotate(elog e, int enable)
{
    _elogh h = __elog_getHandle(((_elog*)&e)->t.id);

    is0_exeret(h, _elog_err("elog e(%d) has not been set", ((_elog*)&e)->t.id);, 0);

    h->rtt.enable = enable != 0;

    return 1;
}

int  elog_setElogFmt(elog e, constr fmt)
{
    _elogh h = __elog_getHandle(((_elog*)&e)->t.id);

    is1_ret(!fmt || !*fmt, 0);

    is0_exeret(h, _elog_err("elog e(%d) has not been set", ((_elog*)&e)->t.id);, 0);

    __elog_setLogFmt(h, fmt);

    return 1;
}

int  elog_setTimeFmt(elog e, constr fmt)
{
    _elogh h = __elog_getHandle(((_elog*)&e)->t.id);

    is1_ret(!fmt || !*fmt, 0);

    is0_exeret(h, _elog_err("elog e(%d) has not been set", ((_elog*)&e)->t.id);, 0);

    __elog_setTimeFmt(h, fmt);

    return 1;
}

static int __elog_file_cleanup(_elogh h, i64 nows)
{
    struct stat _stat_;
    char    logpath[1024], * c;
    int     pathlen;
    ejson   files;

#ifndef _WIN32
    DIR* logdirfs; struct dirent* logfile;
    char filename[1024];
    int  namelen;
    i64  whole_period;

    files = ejson_new(EOBJ, EVAL_0);

    memccpy(logpath, h->path, '\0', 1024);
    if((c = strrchr(logpath, '/')))
    {
        snprintf(filename, 1024, "%s_", c + 1);
        c++; *c = '\0';
        pathlen = c - logpath;
    }
    else
    {
        snprintf(filename, 1024, "%s_", logpath);
        sprintf(logpath, "./");
        pathlen = 2;
    }

    is0_exeret(logdirfs = opendir(logpath), _elog_err("elog files cleanup err: %s", strerror(errno));, 0);

    namelen = strlen(filename);
    whole_period = (h->rtt.terms - 1) * h->rtt.period;
    while((logfile = readdir(logdirfs)))
    {
        if(0 == strncmp(filename, logfile->d_name, namelen))    // ok, find log file create by this pagentd
        {
            memccpy(logpath + pathlen, logfile->d_name, '\0', 1024 - pathlen);  // get full path

            if(h->rtt.terms == 0)
            {
                if(-1 == remove(logpath)){_elog_err("[err] cleanup logfile: %s [err: %s]", logpath, strerror(errno));}
                else                     {_elog_err("[ok ] cleanup logfile: %s", logpath);}
            }
            else if(h->rtt.files == 0)
            {
                if(-1 == remove(logpath)){_elog_err("[err] cleanup logfile: %s [err: %s]", logpath, strerror(errno));}
                else                     {_elog_err("[ok ] cleanup logfile: %s", logpath);}
            }
            else
            {
                if(0 == stat(logpath, &_stat_))
                {
#ifdef __APPLE__
#define st_ctim st_ctimespec
#endif
                    if((nows - _stat_.st_ctim.tv_sec) > whole_period)    // check time
                    {
                        if(-1 == remove(logpath)){_elog_err("[err] cleanup logfile: %s [err: %s]", logpath, strerror(errno));}
                        else                     {_elog_err("[ok ] cleanup logfile: %s", logpath);}
                    }
                    else
                    {
                        ejson_addI(files, logpath, _stat_.st_ctim.tv_sec);
                    }
                }
            }
        }
    }
    closedir(logdirfs);

#else
    HANDLE hFind; WIN32_FIND_DATA fileData;

    files = ejson_new(EOBJ, EVAL_0);

    // -- get logpath and logfile name
    memccpy(logpath, h->path, '\0', 1024);
    pathlen = strlen(logpath);
    snprintf(logpath + pathlen, 1024 - pathlen, "_*");

    hFind = FindFirstFile(logpath, &fileData);
    if(hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return 0;
    }

    if((c = strrchr(logpath, '/')))
        pathlen = (int)(c - logpath + 1);
    else
    {
        sprintf(logpath, "./");
        pathlen = 2;
    }

    // -- search file
    i64 whole_period = (h->rtt.terms - 1) * h->rtt.period;
    do
    {
        memccpy(logpath + pathlen, fileData.cFileName, '\0', 1024 - pathlen);  // get full path

        if(h->rtt.terms == 0)
        {
            if(-1 == remove(logpath)){_elog_err("[err] cleanup logfile: %s [err: %s]", logpath, strerror(errno));}
            else                     {_elog_err("[ok ] cleanup logfile: %s", logpath);}
        }
        else if(h->rtt.files == 0)
        {
            if(-1 == remove(logpath)){_elog_err("[err] cleanup logfile: %s [err: %s]", logpath, strerror(errno));}
            else                     {_elog_err("[ok ] cleanup logfile: %s", logpath);}
        }
        else
        {
            if(0 == stat(logpath, &_stat_))
            {
                if((nows - _stat_.st_ctime) > whole_period)    // check time
                {
                    if(-1 == remove(logpath)){_elog_err("[err] cleanup logfile: %s [err: %s]", logpath, strerror(errno));}
                    else                     {_elog_err("[ok ] cleanup logfile: %s", logpath);}
                }
                else
                {
                    ejson_addI(files, logpath, _stat_.st_ctime);
                }
            }
        }
    }while( FindNextFile( hFind, &fileData ) );
    FindClose(hFind);
#endif

    // -- delete files if rotations file more then g.logrtt_files
//    int need_deleted = eobj_len(files) - h->rtt.files;
//    if(need_deleted > 0)
//    {
//        ejson itr; int i = 0;

//        // -- sort
//        ejso_sort(files, __VALI_ACS);

//        // -- delete redundant files
//        ejson_foreach_s(files, itr)
//        {
//            if(-1 == remove(eobj_keyS(itr))){_elog_err("[err] cleanup logfile: %s [err: %s]", eobj_keyS(itr), strerror(errno));}
//            else                            {_elog_err("[ok ] cleanup logfile: %s", eobj_keyS(itr));}

//            if(++i == need_deleted)
//                break;
//        }
//    }
//    ejson_free(files);

    return 0;
}

static int __elog_fflush_to_file(_elogh h);
static int __elog_rtt_check(_elogh h, i64 nows)
{
    static int offset_s,    //
               last_day;    // to check and update the backup log file name

    int now_rtt;

    if(0 == offset_s)
    {
        struct tm time; time_t sec;

        sec = (int)(nows / h->rtt.period);
        sec = sec * h->rtt.period;
        localtime_r(&sec, &time);

        offset_s = time.tm_hour * 3600;     // win32 have no field time.tm_gmtoff
        last_day = time.tm_mday;
    }

    // -- check if not need to rotate
    {
        now_rtt = (int)((nows + offset_s) / (h->rtt.period));
        if(h->rtt.last_rtt == 0      ) h->rtt.last_rtt = now_rtt;
        if(h->rtt.last_rtt == now_rtt) return 0;
    }

    // -- execute rotate operation
    {
        struct tm time; time_t sec; uint basic_len;

        sec = h->rtt.last_rtt * h->rtt.period;
        localtime_r(&sec, &time);

        // -- basic path for this day
        estr_wrtP(h->rtt.path, "%s_%4d%02d%02d", h->path, time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
        basic_len  = (uint)estr_len(h->rtt.path);

        // -- get a path for backup rename
        if(h->rtt.suffix)
            estr_catP(h->rtt.path, "_%02d", h->rtt.suffix++);
        while(access(h->rtt.path, F_OK) == 0)
        {
            estr_decrLen(h->rtt.path, estr_len(h->rtt.path) - basic_len);
            estr_catP(h->rtt.path, "_%02d", h->rtt.suffix++);
        }

        __elog_fflush_to_file(h);

        close(h->fd);       // must be sure the fd is ok

        if (rename(h->path, h->rtt.path) != 0)
        {
            fprintf(stderr, "Failed to rename log file \"%s\": %s\n", h->path, strerror(errno));
            if(h->rtt.suffix) h->rtt.suffix--;
        }

        h->fd = open(h->path, O_CREAT | O_WRONLY | O_APPEND, 0666);
        assert(h->fd != -1);

        h->rtt.last_rtt = now_rtt;

        if(last_day != time.tm_mday)
        {
            last_day      = time.tm_mday;
            h->rtt.suffix = 0;
        }
    }

    __elog_file_cleanup(h, nows);

    return 1;
}

static int __elog_print_last_msg(_elogh h, _elog_t _e)
{
    uint len, pre_tag_pos, i;

    is1_ret(_e.t.opts & ELOG_MUTE, 1);

    len = (uint)estr_len(h->msgs);

#if !defined(ELOG_TAG_COLOR) || ELOG_TAG_COLOR == 0
    write(STDOUT_FILENO, h->msgs + h->last_msg_pos, len - h->last_msg_pos);
#else

#ifndef _WIN32
    if(h->tag_poss == 0)
    {
        write(STDOUT_FILENO, h->msgs + h->last_msg_pos, len - h->last_msg_pos);
    }
    else
    {
        pre_tag_pos = h->last_msg_pos;

        estr_clear(h->msg);

        for(i = 0; i < h->tag_poss; i++)
        {
            estr_catB(h->msg, h->msgs + pre_tag_pos, h->tag_pos[i] - pre_tag_pos);
            estr_catB(h->msg, _LEVEL_CORLER_MAP[_e.t.level], _LEVEL_CORLER_LEN);
            pre_tag_pos = h->tag_pos[i] + _LEVEL_TAG_LEN;
        }
        estr_catB(h->msg, h->msgs + pre_tag_pos, len - pre_tag_pos);

        write(STDOUT_FILENO, h->msg, estr_len(h->msg));
    }
#else
    if(h->tag_poss == 0)
    {
        write(STDOUT_FILENO, h->msgs + h->last_msg_pos, len - h->last_msg_pos);
    }
    else
    {
        pre_tag_pos = h->last_msg_pos;

        estr_clear(h->msg);

        EnterCriticalSection(&_elogh_set->_print_cs);
        for(i = 0; i < h->tag_poss; i++)
        {
            write(STDOUT_FILENO, h->msgs + pre_tag_pos, h->tag_pos[i] - pre_tag_pos);
            SetConsoleTextAttribute(hconsole, MAKEWORD(_LEVEL_CORLER_MAP[_e.t.level], 0));
            write(STDOUT_FILENO, _LEVEL_TAG_MAP[_e.t.level], _LEVEL_TAG_LEN);
            SetConsoleTextAttribute(hconsole, MAKEWORD(7, 0));
            pre_tag_pos = h->tag_pos[i] + _LEVEL_TAG_LEN;
        }
        write(STDOUT_FILENO, h->msgs + pre_tag_pos, len - pre_tag_pos);
        LeaveCriticalSection(&_elogh_set->_print_cs);
    }
#endif
#endif // ELOG_TAG_COLOR

    return 1;
}

static int __elog_fflush_to_file(_elogh h)
{
    int ret = 0;

    if(h->fd >= 0)
    {
        ret = write(h->fd, h->msgs, (uint)estr_len(h->msgs));

        if(ret == -1)
        {
            fprintf(stderr, "write faild for path(fd:%d):\"%s\": %s | %"PRIi64"\n%s", h->fd, h->path, strerror(errno), (i64)estr_len(h->msgs), h->msgs);
            fflush(stderr);
            ret = 0;
        }
    }

    estr_clear(h->msgs);
    h->cnt = 0;

    return ret;
}

static int  __elog_fflush_nobuffer(_elogh h, _elog_t _e)
{
    __elog_print_last_msg(h, _e);

    __elog_fflush_to_file(h);

    return 1;
}

static int  __elog_fflush_buffered(_elogh h, _elog_t _e)
{
    h->cnt++;

    __elog_print_last_msg(h, _e);

    if(h->cnt >= h->max_cnt || estr_len(h->msgs) >= h->max_len || _e.t.level == ELOG_ERR)
    {
        __elog_fflush_to_file(h);
        return 1;       // wrote
    }

    return 2;           // buffered
}

/**
 * @brief elog_log
 * @param e      - an elog handle returned by elog_new() and elog_newOpts()
 * @param sfile  - src file name
 * @param sline  - src file line
 * @param fmt    - format string
 * @return 0 - log write faild
 *         1 - log write ok and process a io operation
 *         2 - log write ok and not process a io operation, write to buffer
 */

//TODO: create write thread to write to file
int  elog_log(elog e, constr sfile, int sline, constr fmt, ...)
{
    _elogh h; _elog _e; time_t s; i64 ns; char _us[10]; struct tm lt; uint fmtlen, timelen, uslen, i = 0; char timestr[ELOG_TIMESTR_MAXLEN]; va_list ap;
    enum { NORMAL, SUBST } op;

    is0_ret(fmt, 0);

    _e = *(_elog*)&e;

    h = __elog_getHandle(_e.t.id);

    is0_exeret(h, _elog_err("elog e(%d) has not been set", _e.id);, 0);

    emutex_lock(h->mu);

    if(_e.t.level < h->level || _e.t.grade > h->grade)
        goto log_over;

    {
        ns = e_nowns();
        s  = (time_t)(ns / 1000000000);
        localtime_r(&s, &lt);

        timelen = (uint)strftime(timestr, ELOG_TIMESTR_MAXLEN, h->time_fmt, &lt);

        if(h->rtt.enable) __elog_rtt_check(h, s);
    }

    h->last_msg_pos = (uint)estr_len(h->msgs);
    h->tag_poss     = 0;
    //memset(h->tag_pos, 0, h->tag_poss * sizeof(int));

    op     = NORMAL;
    uslen  = 0;
    fmtlen = (uint)sstr_len(h->elog_fmt);
    for (; i < fmtlen; ++i) {
        if (op == NORMAL) {
            if (h->elog_fmt[i] == '%')
                op = SUBST;
            else
                estr_catB(h->msgs, &h->elog_fmt[i], 1);
        }
        else {
            switch (h->elog_fmt[i]) {
                case '%':  estr_catB(h->msgs, "%", 1);                            break;
                case 'F':  estr_catS(h->msgs, _file_basename(sfile));             break;
                case 'L':  estr_catF(h->msgs, "%i", sline);                       break;
                case 'S':  sprintf(_us, "%09"PRIi64"", ns % 1000000000);
                           if(uslen == 0) uslen = 3;
                           if(uslen >  9) uslen = 9;
                           estr_catB(h->msgs, _us, uslen);                        break;
                case 'T':  estr_catB(h->msgs, timestr, timelen);                  break;
                case 'f':  va_start(ap, fmt);
                           estr_catA(h->msgs, fmt, ap);                           break;
                case 'g':  estr_catF(h->msgs, "%i", _e.t.grade);                  break;
                case 'l':
#ifdef ELOG_TAG_COLOR
                           if(h->tag_poss < ELOG_MAX_TAGS) h->tag_pos[h->tag_poss++] = (uint)estr_len(h->msgs);
#endif
                           estr_catB(h->msgs, _LEVEL_TAG_MAP[_e.t.level], _LEVEL_TAG_LEN);
                                                                                  break;
                case 't':  estr_catB(h->msgs, h->tag, _elogh_set->_fmtlen);       break;
                default :  uslen = atol(&h->elog_fmt[i]);
                           if(uslen)
                           {
                               op = SUBST;
                               continue;
                           }
            }
            op = NORMAL;
        }
    }

    i = h->flush(h, _e);

log_over:
    emutex_ulck(h->mu);

    return i;
}

int elog_free(elog e)
{
    _elogh h; _elog _e; int ret = 2;

    _e = *(_elog*)&e;
    h  = __elog_getHandle(_e.t.id);

    is0_exeret(h, _elog_err("elog e(%d) has not been set", _e.t.id);, 0);

    if(estr_len(h->msgs))
    {
        __elog_fflush_to_file(h);
        ret = 1;
    }

    __elog_freeHandle(h);

    return ret;
}
