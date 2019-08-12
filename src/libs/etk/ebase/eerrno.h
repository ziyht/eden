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

#ifndef CO_ERRNO_H_
#define CO_ERRNO_H_

#include <errno.h>

#define CO__EOF     (-4095)
#define CO__UNKNOWN (-4094)

#define CO__EAI_ADDRFAMILY  (-3000)
#define CO__EAI_AGAIN       (-3001)
#define CO__EAI_BADFLAGS    (-3002)
#define CO__EAI_CANCELED    (-3003)
#define CO__EAI_FAIL        (-3004)
#define CO__EAI_FAMILY      (-3005)
#define CO__EAI_MEMORY      (-3006)
#define CO__EAI_NODATA      (-3007)
#define CO__EAI_NONAME      (-3008)
#define CO__EAI_OVERFLOW    (-3009)
#define CO__EAI_SERVICE     (-3010)
#define CO__EAI_SOCKTYPE    (-3011)
#define CO__EAI_BADHINTS    (-3013)
#define CO__EAI_PROTOCOL    (-3014)

/* Only map to the system errno on non-Windows platforms. It's apparently
 * a fairly common practice for Windows programmers to redefine errno codes.
 */
#if defined(E2BIG) && !defined(_WIN32)
# define CO__E2BIG (-E2BIG)
#else
# define CO__E2BIG (-4093)
#endif

#if defined(EACCES) && !defined(_WIN32)
# define CO__EACCES (-EACCES)
#else
# define CO__EACCES (-4092)
#endif

#if defined(EADDRINUSE) && !defined(_WIN32)
# define CO__EADDRINUSE (-EADDRINUSE)
#else
# define CO__EADDRINUSE (-4091)
#endif

#if defined(EADDRNOTAVAIL) && !defined(_WIN32)
# define CO__EADDRNOTAVAIL (-EADDRNOTAVAIL)
#else
# define CO__EADDRNOTAVAIL (-4090)
#endif

#if defined(EAFNOSUPPORT) && !defined(_WIN32)
# define CO__EAFNOSUPPORT (-EAFNOSUPPORT)
#else
# define CO__EAFNOSUPPORT (-4089)
#endif

#if defined(EAGAIN) && !defined(_WIN32)
# define CO__EAGAIN (-EAGAIN)
#else
# define CO__EAGAIN (-4088)
#endif

#if defined(EALREADY) && !defined(_WIN32)
# define CO__EALREADY (-EALREADY)
#else
# define CO__EALREADY (-4084)
#endif

#if defined(EBADF) && !defined(_WIN32)
# define CO__EBADF (-EBADF)
#else
# define CO__EBADF (-4083)
#endif

#if defined(EBUSY) && !defined(_WIN32)
# define CO__EBUSY (-EBUSY)
#else
# define CO__EBUSY (-4082)
#endif

#if defined(ECANCELED) && !defined(_WIN32)
# define CO__ECANCELED (-ECANCELED)
#else
# define CO__ECANCELED (-4081)
#endif

#if defined(ECHARSET) && !defined(_WIN32)
# define CO__ECHARSET (-ECHARSET)
#else
# define CO__ECHARSET (-4080)
#endif

#if defined(ECONNABORTED) && !defined(_WIN32)
# define CO__ECONNABORTED (-ECONNABORTED)
#else
# define CO__ECONNABORTED (-4079)
#endif

#if defined(ECONNREFUSED) && !defined(_WIN32)
# define CO__ECONNREFUSED (-ECONNREFUSED)
#else
# define CO__ECONNREFUSED (-4078)
#endif

#if defined(ECONNRESET) && !defined(_WIN32)
# define CO__ECONNRESET (-ECONNRESET)
#else
# define CO__ECONNRESET (-4077)
#endif

#if defined(EDESTADDRREQ) && !defined(_WIN32)
# define CO__EDESTADDRREQ (-EDESTADDRREQ)
#else
# define CO__EDESTADDRREQ (-4076)
#endif

#if defined(EEXIST) && !defined(_WIN32)
# define CO__EEXIST (-EEXIST)
#else
# define CO__EEXIST (-4075)
#endif

#if defined(EFAULT) && !defined(_WIN32)
# define CO__EFAULT (-EFAULT)
#else
# define CO__EFAULT (-4074)
#endif

#if defined(EHOSTUNREACH) && !defined(_WIN32)
# define CO__EHOSTUNREACH (-EHOSTUNREACH)
#else
# define CO__EHOSTUNREACH (-4073)
#endif

#if defined(EINTR) && !defined(_WIN32)
# define CO__EINTR (-EINTR)
#else
# define CO__EINTR (-4072)
#endif

#if defined(EINVAL) && !defined(_WIN32)
# define CO__EINVAL (-EINVAL)
#else
# define CO__EINVAL (-4071)
#endif

#if defined(EIO) && !defined(_WIN32)
# define CO__EIO (-EIO)
#else
# define CO__EIO (-4070)
#endif

#if defined(EISCONN) && !defined(_WIN32)
# define CO__EISCONN (-EISCONN)
#else
# define CO__EISCONN (-4069)
#endif

#if defined(EISDIR) && !defined(_WIN32)
# define CO__EISDIR (-EISDIR)
#else
# define CO__EISDIR (-4068)
#endif

#if defined(ELOOP) && !defined(_WIN32)
# define CO__ELOOP (-ELOOP)
#else
# define CO__ELOOP (-4067)
#endif

#if defined(EMFILE) && !defined(_WIN32)
# define CO__EMFILE (-EMFILE)
#else
# define CO__EMFILE (-4066)
#endif

#if defined(EMSGSIZE) && !defined(_WIN32)
# define CO__EMSGSIZE (-EMSGSIZE)
#else
# define CO__EMSGSIZE (-4065)
#endif

#if defined(ENAMETOOLONG) && !defined(_WIN32)
# define CO__ENAMETOOLONG (-ENAMETOOLONG)
#else
# define CO__ENAMETOOLONG (-4064)
#endif

#if defined(ENETDOWN) && !defined(_WIN32)
# define CO__ENETDOWN (-ENETDOWN)
#else
# define CO__ENETDOWN (-4063)
#endif

#if defined(ENETUNREACH) && !defined(_WIN32)
# define CO__ENETUNREACH (-ENETUNREACH)
#else
# define CO__ENETUNREACH (-4062)
#endif

#if defined(ENFILE) && !defined(_WIN32)
# define CO__ENFILE (-ENFILE)
#else
# define CO__ENFILE (-4061)
#endif

#if defined(ENOBUFS) && !defined(_WIN32)
# define CO__ENOBUFS (-ENOBUFS)
#else
# define CO__ENOBUFS (-4060)
#endif

#if defined(ENODEV) && !defined(_WIN32)
# define CO__ENODEV (-ENODEV)
#else
# define CO__ENODEV (-4059)
#endif

#if defined(ENOENT) && !defined(_WIN32)
# define CO__ENOENT (-ENOENT)
#else
# define CO__ENOENT (-4058)
#endif

#if defined(ENOMEM) && !defined(_WIN32)
# define CO__ENOMEM (-ENOMEM)
#else
# define CO__ENOMEM (-4057)
#endif

#if defined(ENONET) && !defined(_WIN32)
# define CO__ENONET (-ENONET)
#else
# define CO__ENONET (-4056)
#endif

#if defined(ENOSPC) && !defined(_WIN32)
# define CO__ENOSPC (-ENOSPC)
#else
# define CO__ENOSPC (-4055)
#endif

#if defined(ENOSYS) && !defined(_WIN32)
# define CO__ENOSYS (-ENOSYS)
#else
# define CO__ENOSYS (-4054)
#endif

#if defined(ENOTCONN) && !defined(_WIN32)
# define CO__ENOTCONN (-ENOTCONN)
#else
# define CO__ENOTCONN (-4053)
#endif

#if defined(ENOTDIR) && !defined(_WIN32)
# define CO__ENOTDIR (-ENOTDIR)
#else
# define CO__ENOTDIR (-4052)
#endif

#if defined(ENOTEMPTY) && !defined(_WIN32)
# define CO__ENOTEMPTY (-ENOTEMPTY)
#else
# define CO__ENOTEMPTY (-4051)
#endif

#if defined(ENOTSOCK) && !defined(_WIN32)
# define CO__ENOTSOCK (-ENOTSOCK)
#else
# define CO__ENOTSOCK (-4050)
#endif

#if defined(ENOTSUP) && !defined(_WIN32)
# define CO__ENOTSUP (-ENOTSUP)
#else
# define CO__ENOTSUP (-4049)
#endif

#if defined(EPERM) && !defined(_WIN32)
# define CO__EPERM (-EPERM)
#else
# define CO__EPERM (-4048)
#endif

#if defined(EPIPE) && !defined(_WIN32)
# define CO__EPIPE (-EPIPE)
#else
# define CO__EPIPE (-4047)
#endif

#if defined(EPROTO) && !defined(_WIN32)
# define CO__EPROTO (-EPROTO)
#else
# define CO__EPROTO (-4046)
#endif

#if defined(EPROTONOSUPPORT) && !defined(_WIN32)
# define CO__EPROTONOSUPPORT (-EPROTONOSUPPORT)
#else
# define CO__EPROTONOSUPPORT (-4045)
#endif

#if defined(EPROTOTYPE) && !defined(_WIN32)
# define CO__EPROTOTYPE (-EPROTOTYPE)
#else
# define CO__EPROTOTYPE (-4044)
#endif

#if defined(EROFS) && !defined(_WIN32)
# define CO__EROFS (-EROFS)
#else
# define CO__EROFS (-4043)
#endif

#if defined(ESHUTDOWN) && !defined(_WIN32)
# define CO__ESHUTDOWN (-ESHUTDOWN)
#else
# define CO__ESHUTDOWN (-4042)
#endif

#if defined(ESPIPE) && !defined(_WIN32)
# define CO__ESPIPE (-ESPIPE)
#else
# define CO__ESPIPE (-4041)
#endif

#if defined(ESRCH) && !defined(_WIN32)
# define CO__ESRCH (-ESRCH)
#else
# define CO__ESRCH (-4040)
#endif

#if defined(ETIMEDOUT) && !defined(_WIN32)
# define CO__ETIMEDOUT (-ETIMEDOUT)
#else
# define CO__ETIMEDOUT (-4039)
#endif

#if defined(ETXTBSY) && !defined(_WIN32)
# define CO__ETXTBSY (-ETXTBSY)
#else
# define CO__ETXTBSY (-4038)
#endif

#if defined(EXDEV) && !defined(_WIN32)
# define CO__EXDEV (-EXDEV)
#else
# define CO__EXDEV (-4037)
#endif

#if defined(EFBIG) && !defined(_WIN32)
# define CO__EFBIG (-EFBIG)
#else
# define CO__EFBIG (-4036)
#endif

#if defined(ENOPROTOOPT) && !defined(_WIN32)
# define CO__ENOPROTOOPT (-ENOPROTOOPT)
#else
# define CO__ENOPROTOOPT (-4035)
#endif

#if defined(ERANGE) && !defined(_WIN32)
# define CO__ERANGE (-ERANGE)
#else
# define CO__ERANGE (-4034)
#endif

#if defined(ENXIO) && !defined(_WIN32)
# define CO__ENXIO (-ENXIO)
#else
# define CO__ENXIO (-4033)
#endif

#if defined(EMLINK) && !defined(_WIN32)
# define CO__EMLINK (-EMLINK)
#else
# define CO__EMLINK (-4032)
#endif

/* EHOSTDOWN is not visible on BSD-like systems when _POSIX_C_SOURCE is
 * defined. Fortunately, its value is always 64 so it's possible albeit
 * icky to hard-code it.
 */
#if defined(EHOSTDOWN) && !defined(_WIN32)
# define CO__EHOSTDOWN (-EHOSTDOWN)
#elif defined(__APPLE__) || \
      defined(__DragonFly__) || \
      defined(__FreeBSD__) || \
      defined(__FreeBSD_kernel__) || \
      defined(__NetBSD__) || \
      defined(__OpenBSD__)
# define CO__EHOSTDOWN (-64)
#else
# define CO__EHOSTDOWN (-4031)
#endif

#if (!_WIN32)
#include <netdb.h>
#endif

/* Expand this list if necessary. */
#define CO_ERRNO_MAP(XX)                                                      \
  XX(E2BIG, "argument list too long")                                         \
  XX(EACCES, "permission denied")                                             \
  XX(EADDRINUSE, "address already in use")                                    \
  XX(EADDRNOTAVAIL, "address not available")                                  \
  XX(EAFNOSUPPORT, "address family not supported")                            \
  XX(EAGAIN, "resource temporarily unavailable")                              \
  XX(EAI_ADDRFAMILY, "address family not supported")                          \
  XX(EAI_AGAIN, "temporary failure")                                          \
  XX(EAI_BADFLAGS, "bad ai_flags value")                                      \
  XX(EAI_BADHINTS, "invalid value for hints")                                 \
  XX(EAI_CANCELED, "request canceled")                                        \
  XX(EAI_FAIL, "permanent failure")                                           \
  XX(EAI_FAMILY, "ai_family not supported")                                   \
  XX(EAI_MEMORY, "out of memory")                                             \
  XX(EAI_NODATA, "no address")                                                \
  XX(EAI_NONAME, "unknown node or service")                                   \
  XX(EAI_OVERFLOW, "argument buffer overflow")                                \
  XX(EAI_PROTOCOL, "resolved protocol is unknown")                            \
  XX(EAI_SERVICE, "service not available for socket type")                    \
  XX(EAI_SOCKTYPE, "socket type not supported")                               \
  XX(EALREADY, "connection already in progress")                              \
  XX(EBADF, "bad file descriptor")                                            \
  XX(EBUSY, "resource busy or locked")                                        \
  XX(ECANCELED, "operation canceled")                                         \
  XX(ECHARSET, "invalid Unicode character")                                   \
  XX(ECONNABORTED, "software caused connection abort")                        \
  XX(ECONNREFUSED, "connection refused")                                      \
  XX(ECONNRESET, "connection reset by peer")                                  \
  XX(EDESTADDRREQ, "destination address required")                            \
  XX(EEXIST, "file already exists")                                           \
  XX(EFAULT, "bad address in system call argument")                           \
  XX(EFBIG, "file too large")                                                 \
  XX(EHOSTUNREACH, "host is unreachable")                                     \
  XX(EINTR, "interrupted system call")                                        \
  XX(EINVAL, "invalid argument")                                              \
  XX(EIO, "i/o error")                                                        \
  XX(EISCONN, "socket is already connected")                                  \
  XX(EISDIR, "illegal operation on a directory")                              \
  XX(ELOOP, "too many symbolic links encountered")                            \
  XX(EMFILE, "too many open files")                                           \
  XX(EMSGSIZE, "message too long")                                            \
  XX(ENAMETOOLONG, "name too long")                                           \
  XX(ENETDOWN, "network is down")                                             \
  XX(ENETUNREACH, "network is unreachable")                                   \
  XX(ENFILE, "file table overflow")                                           \
  XX(ENOBUFS, "no buffer space available")                                    \
  XX(ENODEV, "no such device")                                                \
  XX(ENOENT, "no such file or directory")                                     \
  XX(ENOMEM, "not enough memory")                                             \
  XX(ENONET, "machine is not on the network")                                 \
  XX(ENOPROTOOPT, "protocol not available")                                   \
  XX(ENOSPC, "no space left on device")                                       \
  XX(ENOSYS, "function not implemented")                                      \
  XX(ENOTCONN, "socket is not connected")                                     \
  XX(ENOTDIR, "not a directory")                                              \
  XX(ENOTEMPTY, "directory not empty")                                        \
  XX(ENOTSOCK, "socket operation on non-socket")                              \
  XX(ENOTSUP, "operation not supported on socket")                            \
  XX(EPERM, "operation not permitted")                                        \
  XX(EPIPE, "broken pipe")                                                    \
  XX(EPROTO, "protocol error")                                                \
  XX(EPROTONOSUPPORT, "protocol not supported")                               \
  XX(EPROTOTYPE, "protocol wrong type for socket")                            \
  XX(ERANGE, "result too large")                                              \
  XX(EROFS, "read-only file system")                                          \
  XX(ESHUTDOWN, "cannot send after transport endpoint shutdown")              \
  XX(ESPIPE, "invalid seek")                                                  \
  XX(ESRCH, "no such process")                                                \
  XX(ETIMEDOUT, "connection timed out")                                       \
  XX(ETXTBSY, "text file is busy")                                            \
  XX(EXDEV, "cross-device link not permitted")                                \
  XX(UNKNOWN, "unknown error")                                                \
  XX(EOF, "end of file")                                                      \
  XX(ENXIO, "no such device or address")                                      \
  XX(EMLINK, "too many links")                                                \
  XX(EHOSTDOWN, "host is down")                                               \


typedef enum {
#define XX(code, _) CO_ ## code = CO__ ## code,
    CO_ERRNO_MAP(XX)
#undef XX
    CO_ERRNO_MAX = CO__EOF - 1
}co_errno_t;

#endif /* CO_ERRNO_H_ */
