/// =====================================================================================
///
///       Filename:  ecompat.h
///
///    Description:  a header file to compat different platform, especially between linux
///                  and windows
///
///                  the thread compat are rebuild from libuv
///
///        Version:  1.0
///        Created:  12/18/2016 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __ECOMPAT_H__
#define __ECOMPAT_H__

#define ECOMPAT_VERSION  "ecompat 1.0.1"        // #include <fcntl.h> for all platform

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdio.h>

/// ---------------------- inline ---------------------
///
///
#if defined(_WIN32) && defined(_MSC_VER)
#define __always_inline __forceinline
#define __attribute__(...)
#else
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION < 40300
#define __always_inline
#define inline
#endif
#endif

/// ---------------------- unistd.h ---------------------
///
///
#ifndef __ECOMPAT_UNISTD_DECLARATION__
#define __ECOMPAT_UNISTD_DECLARATION__

#include <fcntl.h>

#if defined(_WIN32)
#include <stdint.h>
#include <Windows.h>
//void usleep(int64_t microsecond);
//void sleep(int64_t second);

#ifndef __IMPORT_KERNEL32_LIB
#define __IMPORT_KERNEL32_LIB
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "msvcrtd.lib")
#endif

#define sleep(s)	Sleep((s) * 1000)
#define usleep(us)  Sleep((us) / 1000 + (us) % 1000 > 500 ? 1 : 0)

#include <io.h>

#define F_OK 0
#define W_OK 2
#define R_OK 4

#define open        _open
#define close       _close
#define lseek       _lseek
#define read        _read
#define write       _write
#define access      _access

#define snprintf    _snprintf

#define	STDIN_FILENO	0	/* Standard input.  */
#define	STDOUT_FILENO	1	/* Standard output.  */
#define	STDERR_FILENO	2	/* Standard error output.  */

#else   // ------------- UNIX

#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#endif  // _WIN32

#endif  // !__ECOMPAT_UNISTD_DECLARATION__


/// ---------------------- string.h ---------------------
///
///
#if defined(_MSC_VER)

#define memccpy     _memccpy
#define strdup      _strdup
#define strtoll     _strtoi64
#define strtoull    _strtoui64
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp

#define strlen(s)   ((uint)(strchr(s, '\0') - s))   // higher performance, maybe not check the character encoding

#define __MEMMEM_DECLARED
void* memmem(const void *l, size_t l_len, const void *s, size_t s_len);

#endif

#ifndef __linux__
#define __STRCHRNUL_DECLARED
char* strchrnul (const char* s, int c_in);
#endif


/// ---------------------- time.h ---------------------
///
///
#if defined(_WIN32)
#if defined(_MSC_VER) && defined(_INC_TIME)
#define localtime_r(sec, time) localtime_s(time, sec)
#elif defined(_TIME_H_)
#define localtime_r(sec, time) localtime_s(time, sec)
#endif
#endif // _INC_TIME

/// ---------------------- stdlib.h ---------------------
///
///
#if defined(_WIN32) && defined(_INC_STDLIB)
#ifndef _MSC_VER
#define _In_
#endif
typedef _In_ int (__cdecl* __compar_fn_t)(void const*, void const*);

#endif


/// ---------------------- assert.h ---------------------
///
///

#if defined(_WIN32)

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>

#endif

#ifdef __cplusplus
}
#endif

#endif // ! __ECOMPAT_H__
