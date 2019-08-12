/// =====================================================================================
///
///       Filename:  ealloc.h
///
///    Description:  a alloc compat header
///
///        Version:  1.0
///        Created:  06/09/2017 05:15:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __EALLOC_HEADER_H__
#define __EALLOC_HEADER_H__

#include "ecompat.h"

#define emalloc     malloc
#define ecalloc     calloc
#define erealloc    realloc
#define efree       free

#endif  //! __EALLOC_HEADER_H__
