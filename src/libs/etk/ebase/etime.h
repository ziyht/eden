/// =====================================================================================
///
///       Filename:  etime.h
///
///    Description:  a compat time impletation for different platforms
///
///        Version:  1.1
///        Created:  06/11/2019 11:00:34 AM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __ETIME_H__
#define __ETIME_H__

#include <time.h>

#include "etype.h"

#ifdef __cplusplus
extern "C" {
#endif

/** -----------------------------------------------------
 *
 *  get timestamp or timestr from now
 *
 */
i64  e_nowns();     // REALTIME
i64  e_nowus();
i64  e_nowms();
i64  e_nows ();

i64  e_tickns();    // MONOTONIC, it will init to REALTIME in first call
i64  e_tickus();
i64  e_tickms();
i64  e_ticks ();

i64  e_ticker_ns(i64* tick);
i64  e_ticker_us(i64* tick);
i64  e_ticker_ms(i64* tick);
i64  e_ticker_s (i64* tick);

cstr e_nowstr(cstr desc, int dlen);             // YYYYMMDDhhmmss.[9]ns  20130603150702.352432467
cstr e_secstr(cstr desc, int dlen, time_t sec); // YYYYMMDDhhmmss

/** -----------------------------------------------------
 *
 * timestr parser and formator
 *
 * --
 *
 *  etimer_strfstr()
 *      input                               output [%F %T]
 *      17:43:03             %T             1900-01-01 17:43:03
 *      17:43:03 2018        %T %Y          2018-01-01 17:43:03
 *      Jun 17:43:03 2018    %b %T %Y       2018-06-01 17:43:03
 *      Jun 27 17:43:03 2018 %b %e %T %Y    2018-06-27 17:43:03
 *
 *  etimer_strfsec()
 *      input                               output [%F %T]
 *      31723506(1y2d4h5m6s)                1971-01-03 12:05:06
 *      1530092583                          2018-06-27 17:43:03
 *      1                                   1970-01-01 08:00:01
 *
 *  etimer_strpsec()
 *      input                               output
 *      Jun 27 17:43:03 2018                1530092583
 */
cstr e_strfstr(cstr dest, int dlen, constr dfmt, constr from, constr ffmt); // format to time str from another formated time str, return null if failed
cstr e_strfsec(cstr dest, int dlen, constr dfmt, time_t sec );              // format to time str from a timestamp(sec)         , return null if failed
i64  e_strpsec(constr from, constr ffmt);                                   // parse a time str to timestamp(sec), return -1 if failed
i64  e_strpms (constr from, constr ffmt);                                   // parse a time str to timestamp(ms ), return -1 if failed
/** ---------------------------

符号     e.g.                        说明
--------------------------------------------------------------------
%a      Sun                         星期几的简写
%A      Sunday                      星期几的全称
%b      Mar                         月分的简写
%B      March                       月份的全称
%c      Wed Jun 27 17:43:03 2018    标准的日期的时间串, %a %b %e %T %Y
%C      20                          世纪，(year/100)，2位
%d      02                          十进制表示的每月的第几天, 01 - 31
%D      03/02/08                    %m/%d/%y, 月/天/年
%e       2                          同 %d，但是 0 会被空格代替
%F      2008-03-02                  年-月-日
%g      08                          年分，但是只有后两位(00-99)
%h      Mar                         简写的月份，同 %b
%H      07                          24小时制的小时(00 - 23)
%I      07                          12小时制的小时(01 - 12)
%j      062                         十进制表示的每年的第几天(001 - 366)
%m      03                          十进制表示的月份(01 - 12)
%M      04                          十时制表示的分钟数(00 - 59)
%n      \n                          新行符
%p      AM                          AM/PM
%r      07:04:02 AM                 12小时制的时间
%R      07:04                       显示小时和分钟：hh:mm
%S      02                          十进制的秒数
%t      \t                          水平制表符
%T      07:04:02                    24小时制的时间(%H:%M:%S)
%u      7                           每周的第几天，星期一为第一天（1 - 7）
%U      09                          第年的第几周，星期日为第一天（0 - 53）
%V      09                          每年的第几周，使用基于周的年（0 - 53）ISO 8601, where week 1 is the first week that has at least 4 days in  the  new  year
%w      0                           十进制表示的星期几（0 - 6，星期天为0）
%W      08                          每年的第几周，把星期一做为第一天（值从0到53）
%x      03/02/08                    标准的日期串
%X      07:04:02                    标准的时间串
%y      08                          不带世纪的十进制年份（00 - 99）
%Y      2008                        带世纪部分的十进制年份
%z      +0800                       +hhmm，-hhmm 格式的时区
%Z      CST                         时区名称，如果不能得到时区名称则返回空字符。
%%      %                           百分号

--------------------------- */

/**
 *   e_elpstrfstr
 *      input                       output
 *      367 04:05:06 1 [%j %T %Y]   2y 002d 04:05:06 [%Yy %jd %T]
 *      367 04:05:06 1 [%j %T %Y]   732d 04:05:06    [%jd %T]
 *      367 04:05:06 1 [%j %T %Y]   2y 52:05:06      [%Yy %T]
 *      367 04:05:06 1 [%j %T %Y]   17572:05:06      [%T]
 *
 *  e_elpstrfsec
 *      input                       output
 *      31723506(1y2d4h5m6s)        1y 002d 04:05:06 [%Yy %jd %T]
 *      31723506                    367d 04:05:06    [%jd %T]
 *      31723506                    1y 52:05:06      [%Yy %T]
 *      31723506                    8812:05:06       [%T]
 *      1                           0y 000d 00:00:01 [%Yy %jd %T]
 *
 *  e_elpstrpsec
 *      input                       output
 *   1  2 04:05:06 1 [%j %T %Y]     31723506   use %j as days, not yday likely in tm, this case is specific if only using %Y %j %T in ffmt
 *   2  Jan 0 00:00:03 0000         -1
 *   3  Jan 1 00:00:03 0000         3
 *   4  Jan 2 02:00:03 0000         93603
 */
cstr e_elpstrfstr(cstr dest, int dlen, constr dfmt, constr from, constr ffmt);  // format to elapse str from a time str
cstr e_elpstrfsec(cstr dest, int dlen, constr dfmt, time_t sec);                // format to elapse str from a sec timestamp
i64  e_elpstrpsec(constr from, constr ffmt);                                    // parse a elapse str to passed seconds
i64  e_elpstrpms (constr from, constr ffmt);                                    // parse a elapse str to passed ms

#ifdef __cplusplus
}
#endif

#endif
