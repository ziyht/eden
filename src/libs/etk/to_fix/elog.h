/// =====================================================================================
///
///       Filename:  elog.h
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


#ifndef __ELOG_H__
#define __ELOG_H__

#define ELOG_VERSION "elog 1.1.7"      // fixed coredump when the passed in fmt is NULL in elog_log()

#include "etype.h"

/// ---------------------- setting ---------------------

#define ELOG_SYS_MUTE           0        // if this set to 1, elog will not output internal err
#define ELOG_TAG_COLOR          1        // if this set to 1, the tag for DBG,INF,WRN,ERR will have color in console
#define ELOG_MAX_NUM            1024     // the max elog you can create, the max num is (ELOG_MAX_NUM - 1) in actually

#define ELOG_TAG_MAXLEN         16
#define ELOG_FORMAT_MAXLEN      128
#define ELOG_TIMESTR_MAXLEN     256

typedef int elog;

/// ---------------------- control ----------------------
#ifdef  ELOG_MUTE
#if     ELOG_MUTE           // if you set to 1 on outside, the elog APIs in the specific file are whole mute
#define ELOG_MUTE_SETTED
#endif
#endif
#undef  ELOG_MUTE

/// ---------------------- params -----------------------

#define ELOG_DISABLE    0
#define ELOG_ENABLE     1

/*--------------------------------

    log format str statement:

    %% - %

    %T - timestr            you can using a str to set the timestr format, see ELOG_DF_TIME_FORMAT
    %S - second detail      you can output ns(%9S), us(%6S), ms(%3S, default) by setting this
    %F - src file           only the filename, not the whole path
    %L - line

    %l - level
    %g - grade
    %t - elog tag

    %f - formated msg

---------------------------------*/

#define ELOG_DF_ELOG_FORMAT "%T.%6S [%t:%l] %f\n"
#define ELOG_DF_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

// -- elog level
#define ELOG_DBG  0
#define ELOG_INF  1
#define ELOG_WRN  2
#define ELOG_ERR  3
#define ELOG_OFF  4

// -- elog opts
#define ELOG_MUTE 1         // the log will not print to console

/*--------------------------------

    elog handle builder macro:

    e - elog  : returned by elog_new() or elog_newOpts()
    l - level : ELOG_DBG, ELOG_INF, ...
    g - grade : 0~15
    o - opts  : ELOG_MUTE
---------------------------------*/
#define ELOG(e, l, g, o) ((e & 0xffff) | ((l & 0x03) << 16) | ((g & 0x0f) << 20) | (o << 24))
#define ELOG_L(e, l)     ((e & 0xffff) | ((l & 0x03) << 16))
#define ELOG_G(e, g)     ((e & 0xffff) | ((g & 0x0f) << 20))
#define ELOG_O(e, o)     ((e & 0xffff) | (o << 24))

typedef struct elog_opts_s{

    constr path;

    elog   id;                  // set the elog id(1 ~ ELOG_MAX_NUM - 1) you want to set, so you can use the id in other file directly
                                // if is 0, this function disabled, get an unused id if have
                                // if out of range or the id has been used,  elog_newOpts() return 0

    // -- buffered opts
    struct {
        uint   max_logs;        // max cnt of logs buffered in buffer
        uint   max_size;        // max size of buffer for all logs, unit: byte
    }buf;                       // if max_logs and max_size are all 0, internal buffer function disbled

    struct {
        int    enable;          // enable or not
        int    period;          // rotate period, unit: s, default is daily (3600s * 24 when the passed in .period == 0), rotate in 00:00:00
        int    files;           // how many rotation files you want to reserved
        int    terms;           // how earlist the file in terms of period you want to reserved
    }rtt;                       // rotate settings

}elog_opts_t, * elog_opts;

#ifdef __cplusplus
extern "C" {
#endif

elog*elog_getSet();             // get internal elog set
int  elog_setSet(elog* set);    // set internal elog set

elog elog_new    (constr tag, constr    path);
elog elog_newOpts(constr tag, elog_opts opts);

int  elog_free(elog e);

int  elog_setLevel (elog e);                // lg: elog_setLevel(ELOG(e, l, g, 0)); // default is ELOG_DBG(level) and 0(grade)
int  elog_setBuffer(elog e, int enable);    // set internal buffer enable or disable
int  elog_setRotate(elog e, int enable);    // set internal rotate enable or disable

int  elog_setElogFmt(elog e, constr fmt);
int  elog_setTimeFmt(elog e, constr fmt);

int  elog_log(elog e, constr sfile, int sline, constr fmt, ...);

#define  elog_dbg(e, ...) _elog_log_dbg((e | (ELOG_DBG << 16)), __VA_ARGS__)
#define  elog_inf(e, ...) _elog_log_inf((e | (ELOG_INF << 16)), __VA_ARGS__)
#define  elog_wrn(e, ...) _elog_log_wrn((e | (ELOG_WRN << 16)), __VA_ARGS__)
#define  elog_err(e, ...) _elog_log_err((e | (ELOG_ERR << 16)), __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#ifdef  ELOG_MUTE_SETTED
#define ELOG_PARAM(e) (e|ELOG_MUTE<<24), __FILE__, __LINE__
#else
#define ELOG_PARAM(e) (e), __FILE__, __LINE__
#endif

#define _elog_log_dbg(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)
#define _elog_log_inf(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)
#define _elog_log_wrn(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)
#define _elog_log_err(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)

#endif      // __ELOG_H__

#ifdef  ELOG_LEVEL_RESET
#undef  _elog_log_dbg
#undef  _elog_log_inf
#undef  _elog_log_wrn
#undef  _elog_log_err
#define _elog_log_dbg(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)
#define _elog_log_inf(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)
#define _elog_log_wrn(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)
#define _elog_log_err(e, ...) elog_log(ELOG_PARAM(e), __VA_ARGS__)
#define ELOG_LEVEL  ELOG_LEVEL_RESET
#endif

#ifdef ELOG_LEVEL

#if ELOG_LEVEL + 0 > ELOG_DBG           // ELOG_LEVEL_RESET need a value
#undef  _elog_log_dbg
#define _elog_log_dbg(e, ...)
#endif

#if ELOG_LEVEL + 0 > ELOG_INF
#undef  _elog_log_inf
#define _elog_log_inf(e, ...)
#endif

#if ELOG_LEVEL + 0 > ELOG_WRN
#undef  _elog_log_wrn
#define _elog_log_wrn(e, ...)
#endif

#if ELOG_LEVEL + 0 > ELOG_ERR
#undef  _elog_log_err
#define _elog_log_err(e, ...)
#endif

#undef ELOG_LEVEL
#undef ELOG_LEVEL_RESET
#endif
