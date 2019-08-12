/// =====================================================================================
///
///       Filename:  earg.h
///
///    Description:  an easier cmdline arg parser
///
///        Version:  1.0
///        Created:  2017.05.18 04:03:34 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================


#ifndef __EARG_H__
#define __EARG_H__

#define EARG_VERSION "earg 1.0.2"      // adjust return val of earg_addArg()

#ifdef __cplusplus
extern "C" {
#endif

#include "etype.h"

typedef struct earg_s* earg;

typedef struct arg_s{
    int  given;
    i64  valI;
    cstr valS;
    cstr desc;
}arg_t, * arg;

earg   earg_new (constr desc);
int    earg_free(earg   h);

arg    earg_addArgS(earg h, constr tag, constr name, constr df, constr desc);
arg    earg_addArgI(earg h, constr tag, constr name, i64    df, constr desc);

int    earg_parse(earg h, int argc, cstr argv[]);

constr earg_info(earg h);
arg    earg_find(earg h, char tag);

#ifdef __cplusplus
}
#endif

#endif
