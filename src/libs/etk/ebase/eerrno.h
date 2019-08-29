/// =====================================================================================
///
///       Filename:  eerrno.h
///
///    Description:  rebuild from libuv
///
///        Version:  1.0
///        Created:  04/10/2017 15:30:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

/**********************************************************
* CO_ERRNO                                                *
**********************************************************/

#ifndef E_ERRNO_H_
#define E_ERRNO_H_

#include <errno.h>
#if EDOM > 0
# define __ERR(x) (-(x))
#else
# define __ERR(x) (x)
#endif

#define E__EOF     (-4095)
#define E__UNKNOWN (-4094)

#define E__EAI_ADDRFAMILY  (-3000)
#define E__EAI_AGAIN       (-3001)
#define E__EAI_BADFLAGS    (-3002)
#define E__EAI_CANCELED    (-3003)
#define E__EAI_FAIL        (-3004)
#define E__EAI_FAMILY      (-3005)
#define E__EAI_MEMORY      (-3006)
#define E__EAI_NODATA      (-3007)
#define E__EAI_NONAME      (-3008)
#define E__EAI_OVERFLOW    (-3009)
#define E__EAI_SERVICE     (-3010)
#define E__EAI_SOCKTYPE    (-3011)
#define E__EAI_BADHINTS    (-3013)
#define E__EAI_PROTOCOL    (-3014)

/* Only map to the system errno on non-Windows platforms. It's apparently
 * a fairly common practice for Windows programmers to redefine errno codes.
 */
#if defined(E2BIG) && !defined(_WIN32)
# define E__E2BIG __ERR(E2BIG)
#else
# define E__E2BIG (-4093)
#endif

#if defined(EACCES) && !defined(_WIN32)
# define E__EACCES __ERR(EACCES)
#else
# define E__EACCES (-4092)
#endif

#if defined(EADDRINUSE) && !defined(_WIN32)
# define E__EADDRINUSE __ERR(EADDRINUSE)
#else
# define E__EADDRINUSE (-4091)
#endif

#if defined(EADDRNOTAVAIL) && !defined(_WIN32)
# define E__EADDRNOTAVAIL __ERR(EADDRNOTAVAIL)
#else
# define E__EADDRNOTAVAIL (-4090)
#endif

#if defined(EAFNOSUPPORT) && !defined(_WIN32)
# define E__EAFNOSUPPORT __ERR(EAFNOSUPPORT)
#else
# define E__EAFNOSUPPORT (-4089)
#endif

#if defined(EAGAIN) && !defined(_WIN32)
# define E__EAGAIN __ERR(EAGAIN)
#else
# define E__EAGAIN (-4088)
#endif

#if defined(EALREADY) && !defined(_WIN32)
# define E__EALREADY __ERR(EALREADY)
#else
# define E__EALREADY (-4084)
#endif

#if defined(EBADF) && !defined(_WIN32)
# define E__EBADF __ERR(EBADF)
#else
# define E__EBADF (-4083)
#endif

#if defined(EBUSY) && !defined(_WIN32)
# define E__EBUSY __ERR(EBUSY)
#else
# define E__EBUSY (-4082)
#endif

#if defined(ECANCELED) && !defined(_WIN32)
# define E__ECANCELED __ERR(ECANCELED)
#else
# define E__ECANCELED (-4081)
#endif

#if defined(ECHARSET) && !defined(_WIN32)
# define E__ECHARSET __ERR(ECHARSET)
#else
# define E__ECHARSET (-4080)
#endif

#if defined(ECONNABORTED) && !defined(_WIN32)
# define E__ECONNABORTED __ERR(ECONNABORTED)
#else
# define E__ECONNABORTED (-4079)
#endif

#if defined(ECONNREFUSED) && !defined(_WIN32)
# define E__ECONNREFUSED __ERR(ECONNREFUSED)
#else
# define E__ECONNREFUSED (-4078)
#endif

#if defined(ECONNRESET) && !defined(_WIN32)
# define E__ECONNRESET __ERR(ECONNRESET)
#else
# define E__ECONNRESET (-4077)
#endif

#if defined(EDESTADDRREQ) && !defined(_WIN32)
# define E__EDESTADDRREQ __ERR(EDESTADDRREQ)
#else
# define E__EDESTADDRREQ (-4076)
#endif

#if defined(EEXIST) && !defined(_WIN32)
# define E__EEXIST __ERR(EEXIST)
#else
# define E__EEXIST (-4075)
#endif

#if defined(EFAULT) && !defined(_WIN32)
# define E__EFAULT __ERR(EFAULT)
#else
# define E__EFAULT (-4074)
#endif

#if defined(EHOSTUNREACH) && !defined(_WIN32)
# define E__EHOSTUNREACH __ERR(EHOSTUNREACH)
#else
# define E__EHOSTUNREACH (-4073)
#endif

#if defined(EINTR) && !defined(_WIN32)
# define E__EINTR __ERR(EINTR)
#else
# define E__EINTR (-4072)
#endif

#if defined(EINVAL) && !defined(_WIN32)
# define E__EINVAL __ERR(EINVAL)
#else
# define E__EINVAL (-4071)
#endif

#if defined(EIO) && !defined(_WIN32)
# define E__EIO __ERR(EIO)
#else
# define E__EIO (-4070)
#endif

#if defined(EISCONN) && !defined(_WIN32)
# define E__EISCONN __ERR(EISCONN)
#else
# define E__EISCONN (-4069)
#endif

#if defined(EISDIR) && !defined(_WIN32)
# define E__EISDIR __ERR(EISDIR)
#else
# define E__EISDIR (-4068)
#endif

#if defined(ELOOP) && !defined(_WIN32)
# define E__ELOOP __ERR(ELOOP)
#else
# define E__ELOOP (-4067)
#endif

#if defined(EMFILE) && !defined(_WIN32)
# define E__EMFILE __ERR(EMFILE)
#else
# define E__EMFILE (-4066)
#endif

#if defined(EMSGSIZE) && !defined(_WIN32)
# define E__EMSGSIZE __ERR(EMSGSIZE)
#else
# define E__EMSGSIZE (-4065)
#endif

#if defined(ENAMETOOLONG) && !defined(_WIN32)
# define E__ENAMETOOLONG __ERR(ENAMETOOLONG)
#else
# define E__ENAMETOOLONG (-4064)
#endif

#if defined(ENETDOWN) && !defined(_WIN32)
# define E__ENETDOWN __ERR(ENETDOWN)
#else
# define E__ENETDOWN (-4063)
#endif

#if defined(ENETUNREACH) && !defined(_WIN32)
# define E__ENETUNREACH __ERR(ENETUNREACH)
#else
# define E__ENETUNREACH (-4062)
#endif

#if defined(ENFILE) && !defined(_WIN32)
# define E__ENFILE __ERR(ENFILE)
#else
# define E__ENFILE (-4061)
#endif

#if defined(ENOBUFS) && !defined(_WIN32)
# define E__ENOBUFS __ERR(ENOBUFS)
#else
# define E__ENOBUFS (-4060)
#endif

#if defined(ENODEV) && !defined(_WIN32)
# define E__ENODEV __ERR(ENODEV)
#else
# define E__ENODEV (-4059)
#endif

#if defined(ENOENT) && !defined(_WIN32)
# define E__ENOENT __ERR(ENOENT)
#else
# define E__ENOENT (-4058)
#endif

#if defined(ENOMEM) && !defined(_WIN32)
# define E__ENOMEM __ERR(ENOMEM)
#else
# define E__ENOMEM (-4057)
#endif

#if defined(ENONET) && !defined(_WIN32)
# define E__ENONET __ERR(ENONET)
#else
# define E__ENONET (-4056)
#endif

#if defined(ENOSPC) && !defined(_WIN32)
# define E__ENOSPC __ERR(ENOSPC)
#else
# define E__ENOSPC (-4055)
#endif

#if defined(ENOSYS) && !defined(_WIN32)
# define E__ENOSYS __ERR(ENOSYS)
#else
# define E__ENOSYS (-4054)
#endif

#if defined(ENOTCONN) && !defined(_WIN32)
# define E__ENOTCONN __ERR(ENOTCONN)
#else
# define E__ENOTCONN (-4053)
#endif

#if defined(ENOTDIR) && !defined(_WIN32)
# define E__ENOTDIR __ERR(ENOTDIR)
#else
# define E__ENOTDIR (-4052)
#endif

#if defined(ENOTEMPTY) && !defined(_WIN32)
# define E__ENOTEMPTY __ERR(ENOTEMPTY)
#else
# define E__ENOTEMPTY (-4051)
#endif

#if defined(ENOTSOCK) && !defined(_WIN32)
# define E__ENOTSOCK __ERR(ENOTSOCK)
#else
# define E__ENOTSOCK (-4050)
#endif

#if defined(ENOTSUP) && !defined(_WIN32)
# define E__ENOTSUP __ERR(ENOTSUP)
#else
# define E__ENOTSUP (-4049)
#endif

#if defined(EPERM) && !defined(_WIN32)
# define E__EPERM __ERR(EPERM)
#else
# define E__EPERM (-4048)
#endif

#if defined(EPIPE) && !defined(_WIN32)
# define E__EPIPE __ERR(EPIPE)
#else
# define E__EPIPE (-4047)
#endif

#if defined(EPROTO) && !defined(_WIN32)
# define E__EPROTO __ERR(EPROTO)
#else
# define E__EPROTO __ERR(4046)
#endif

#if defined(EPROTONOSUPPORT) && !defined(_WIN32)
# define E__EPROTONOSUPPORT __ERR(EPROTONOSUPPORT)
#else
# define E__EPROTONOSUPPORT (-4045)
#endif

#if defined(EPROTOTYPE) && !defined(_WIN32)
# define E__EPROTOTYPE __ERR(EPROTOTYPE)
#else
# define E__EPROTOTYPE (-4044)
#endif

#if defined(EROFS) && !defined(_WIN32)
# define E__EROFS __ERR(EROFS)
#else
# define E__EROFS (-4043)
#endif

#if defined(ESHUTDOWN) && !defined(_WIN32)
# define E__ESHUTDOWN __ERR(ESHUTDOWN)
#else
# define E__ESHUTDOWN (-4042)
#endif

#if defined(ESPIPE) && !defined(_WIN32)
# define E__ESPIPE __ERR(ESPIPE)
#else
# define E__ESPIPE (-4041)
#endif

#if defined(ESRCH) && !defined(_WIN32)
# define E__ESRCH __ERR(ESRCH)
#else
# define E__ESRCH (-4040)
#endif

#if defined(ETIMEDOUT) && !defined(_WIN32)
# define E__ETIMEDOUT __ERR(ETIMEDOUT)
#else
# define E__ETIMEDOUT (-4039)
#endif

#if defined(ETXTBSY) && !defined(_WIN32)
# define E__ETXTBSY __ERR(ETXTBSY)
#else
# define E__ETXTBSY (-4038)
#endif

#if defined(EXDEV) && !defined(_WIN32)
# define E__EXDEV __ERR(EXDEV)
#else
# define E__EXDEV (-4037)
#endif

#if defined(EFBIG) && !defined(_WIN32)
# define E__EFBIG __ERR(EFBIG)
#else
# define E__EFBIG (-4036)
#endif

#if defined(ENOPROTOOPT) && !defined(_WIN32)
# define E__ENOPROTOOPT __ERR(ENOPROTOOPT)
#else
# define E__ENOPROTOOPT (-4035)
#endif

#if defined(ERANGE) && !defined(_WIN32)
# define E__ERANGE __ERR(ERANGE)
#else
# define E__ERANGE (-4034)
#endif

#if defined(ENXIO) && !defined(_WIN32)
# define E__ENXIO __ERR(ENXIO)
#else
# define E__ENXIO (-4033)
#endif

#if defined(EMLINK) && !defined(_WIN32)
# define E__EMLINK __ERR(EMLINK)
#else
# define E__EMLINK (-4032)
#endif

/* EHOSTDOWN is not visible on BSD-like systems when _POSIX_C_SOURCE is
 * defined. Fortunately, its value is always 64 so it's possible albeit
 * icky to hard-code it.
 */
#if defined(EHOSTDOWN) && !defined(_WIN32)
# define E__EHOSTDOWN __ERR(EHOSTDOWN)
#elif defined(__APPLE__) || \
      defined(__DragonFly__) || \
      defined(__FreeBSD__) || \
      defined(__FreeBSD_kernel__) || \
      defined(__NetBSD__) || \
      defined(__OpenBSD__)
# define E__EHOSTDOWN (-64)
#else
# define E__EHOSTDOWN (-4031)
#endif

#if defined(EREMOTEIO) && !defined(_WIN32)
# define E__EREMOTEIO __ERR(EREMOTEIO)
#else
# define E__EREMOTEIO (-4030)
#endif

#if defined(ENOTTY) && !defined(_WIN32)
# define E__ENOTTY __ERR(ENOTTY)
#else
# define E__ENOTTY (-4029)
#endif

#if defined(EFTYPE) && !defined(_WIN32)
# define E__EFTYPE __ERR(EFTYPE)
#else
# define E__EFTYPE (-4028)
#endif


#if (!_WIN32)
#include <netdb.h>
#endif

/* Expand this list if necessary. */
#define __E_ERRNO_MAP(XX)                                                       \
    XX(E2BIG            , "argument list too long")                             \
    XX(EACCES           , "permission denied")                                  \
    XX(EADDRINUSE       , "address already in use")                             \
    XX(EADDRNOTAVAIL    , "address not available")                              \
    XX(EAFNOSUPPORT     , "address family not supported")                       \
    XX(EAGAIN           , "resource temporarily unavailable")                   \
    XX(EAI_ADDRFAMILY   , "address family not supported")                       \
    XX(EAI_AGAIN        , "temporary failure")                                  \
    XX(EAI_BADFLAGS     , "bad ai_flags value")                                 \
    XX(EAI_BADHINTS     , "invalid value for hints")                            \
    XX(EAI_CANCELED     , "request canceled")                                   \
    XX(EAI_FAIL         , "permanent failure")                                  \
    XX(EAI_FAMILY       , "ai_family not supported")                            \
    XX(EAI_MEMORY       , "out of memory")                                      \
    XX(EAI_NODATA       , "no address")                                         \
    XX(EAI_NONAME       , "unknown node or service")                            \
    XX(EAI_OVERFLOW     , "argument buffer overflow")                           \
    XX(EAI_PROTOCOL     , "resolved protocol is unknown")                       \
    XX(EAI_SERVICE      , "service not available for socket type")              \
    XX(EAI_SOCKTYPE     , "socket type not supported")                          \
    XX(EALREADY         , "connection already in progress")                     \
    XX(EBADF            , "bad file descriptor")                                \
    XX(EBUSY            , "resource busy or locked")                            \
    XX(ECANCELED        , "operation canceled")                                 \
    XX(ECHARSET         , "invalid Unicode character")                          \
    XX(ECONNABORTED     , "software caused connection abort")                   \
    XX(ECONNREFUSED     , "connection refused")                                 \
    XX(ECONNRESET       , "connection reset by peer")                           \
    XX(EDESTADDRREQ     , "destination address required")                       \
    XX(EEXIST           , "file already exists")                                \
    XX(EFAULT           , "bad address in system call argument")                \
    XX(EFBIG            , "file too large")                                     \
    XX(EHOSTUNREACH     , "host is unreachable")                                \
    XX(EINTR            , "interrupted system call")                            \
    XX(EINVAL           , "invalid argument")                                   \
    XX(EIO              , "i/o error")                                          \
    XX(EISCONN          , "socket is already connected")                        \
    XX(EISDIR           , "illegal operation on a directory")                   \
    XX(ELOOP            , "too many symbolic links encountered")                \
    XX(EMFILE           , "too many open files")                                \
    XX(EMSGSIZE         , "message too long")                                   \
    XX(ENAMETOOLONG     , "name too long")                                      \
    XX(ENETDOWN         , "network is down")                                    \
    XX(ENETUNREACH      , "network is unreachable")                             \
    XX(ENFILE           , "file table overflow")                                \
    XX(ENOBUFS          , "no buffer space available")                          \
    XX(ENODEV           , "no such device")                                     \
    XX(ENOENT           , "no such file or directory")                          \
    XX(ENOMEM           , "not enough memory")                                  \
    XX(ENONET           , "machine is not on the network")                      \
    XX(ENOPROTOOPT      , "protocol not available")                             \
    XX(ENOSPC           , "no space left on device")                            \
    XX(ENOSYS           , "function not implemented")                           \
    XX(ENOTCONN         , "socket is not connected")                            \
    XX(ENOTDIR          , "not a directory")                                    \
    XX(ENOTEMPTY        , "directory not empty")                                \
    XX(ENOTSOCK         , "socket operation on non-socket")                     \
    XX(ENOTSUP          , "operation not supported on socket")                  \
    XX(EPERM            , "operation not permitted")                            \
    XX(EPIPE            , "broken pipe")                                        \
    XX(EPROTO           , "protocol error")                                     \
    XX(EPROTONOSUPPORT  , "protocol not supported")                             \
    XX(EPROTOTYPE       , "protocol wrong type for socket")                     \
    XX(ERANGE           , "result too large")                                   \
    XX(EROFS            , "read-only file system")                              \
    XX(ESHUTDOWN        , "cannot send after transport endpoint shutdown")      \
    XX(ESPIPE           , "invalid seek")                                       \
    XX(ESRCH            , "no such process")                                    \
    XX(ETIMEDOUT        , "connection timed out")                               \
    XX(ETXTBSY          , "text file is busy")                                  \
    XX(EXDEV            , "cross-device link not permitted")                    \
    XX(UNKNOWN          , "unknown error")                                      \
    XX(EOF              , "end of file")                                        \
    XX(ENXIO            , "no such device or address")                          \
    XX(EMLINK           , "too many links")                                     \
    XX(EHOSTDOWN        , "host is down")                                       \
    XX(EREMOTEIO        , "remote I/O error")                                   \
    XX(ENOTTY           , "inappropriate ioctl for device")                     \
    XX(EFTYPE           , "inappropriate file type or format")                  \


typedef enum {
#define XX(code, _) E_ ## code = E__ ## code,
    __E_ERRNO_MAP(XX)
#undef XX
    E_ERRNO_MAX = E__EOF - 1
}e_errno_t, eerrno;

#endif /* E_ERRNO_H_ */
