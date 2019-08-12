/// =====================================================================================
///
///       Filename:  ebench.h
///
///    Description:  easy benchmark tools
///
///        Version:  1.0
///        Created:  06/18/2019 04:38:34 PM
///       Revision:  none
///       Compiler:  gcc
///        Depends:  ejson, estr
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#include "etype.h"

typedef struct ebench_s{
    constr  name;
    u64     oprts;
    u64     scale;      // you can set this after run a unit Oprt to set extra info
    eval    bprvt;      // private data for ebench handle
    eval    oprvt;      // private data for operation
    eval    cprvt;      // private data for each case
}ebench_t, * ebench, * ebench_p;

typedef void (*ebench_cb)(ebench b);

ebench ebench_new(constr name);    // create a new ebench handle in internal engine, return 0 if already exists
ebench ebench_get(constr name);    // find ebench handle, return 0 if not exists

/**
 *  add a operations to a benchmark handle
 *
 * @note:
 *
 *    step will execute but not statistic in result after exec,
 *  this will be very helpful when you want to add init or
 *  some like oprts.
 *
 */
ebench ebench_addOprt(ebench h, constr name, ebench_cb oprt, eval prvt);
ebench ebench_addStep(ebench h, constr name, ebench_cb oprt, eval prvt);

/**
 *  add a case for a benchmark handle
 *
 * @note:
 *  1. all Callbacks in this handle will be executed for every case
 *  2. if a handle have no case, it will be skipped when executing
 */
ebench ebench_addCase(ebench h, uint oprts, eval prvt);

/**
 *  execute all created benchmarks
 */
void   ebench_exec();
void   ebench_skipCase(uint oprts1, uint oprts2);

void   ebench_showResult();
constr ebench_getResult();

void   ebench_release();
