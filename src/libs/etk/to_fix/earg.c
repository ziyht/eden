/// =====================================================================================
///
///       Filename:  earg.c
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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "earg.h"
#include "estr.h"
#include "ejson.h"
#include "eutils.h"

/// ----------------------- earg --------------------------

typedef struct option option_t;

static option_t  __end_opt;

typedef union {cptr p; u64 u; i64 i; constr s;}eptr;
#define __INT__ 0
#define __STR__ 1

typedef struct _arg_s{
    int      given;
    i64      valI;
    estr     valS;
    estr     desc;
    int      __type;
    option_t __option;
}_arg_t, * _arg;

typedef struct earg_s{
    estr  desc;
    estr  info;
    eobj  args_d;
    estr  __shortopts;
    estr  __longopts;
}earg_t;

earg   earg_new (constr desc)
{
    earg h = calloc(1, sizeof(earg_t));

    h->args_d = ejson_new(EOBJ, EVAL_0);

    estr_wrtS(h->desc, desc);
    estr_catB(h->desc, "\n", 1);
    estr_wrtS(h->info, desc);
    estr_catB(h->info, "\n", 1);

    estr_wrtB(h->__shortopts, 0, 0);
    estr_wrtB(h->__longopts, &__end_opt, sizeof(option_t));

    return h;
}

int    earg_free(earg   h)
{
    _arg arg;

    is0_ret(h, 0);

    estr_free(h->desc);         h->desc        = 0;
    estr_free(h->info);         h->info        = 0;
    estr_free(h->__shortopts);  h->__shortopts = 0;
    estr_free(h->__longopts);   h->__longopts  = 0;

    ejson_foreach_s(h->args_d, itr)
    {
        arg = EOBJ_VALR(itr);

        estr_free(arg->desc);                arg->desc          = 0;
        estr_free(arg->valS);                arg->valS          = 0;
        estr_free((estr)arg->__option.name); arg->__option.name = 0;
    }

    ejson_free(h->args_d); h->args_d = 0;

    free(h);

    return 1;
}

static _arg __earg_addArg(earg h, constr tag, constr name, eptr df, constr desc, int type)
{
    _arg arg; int len; char short_arg[4] = {0};

    is0_ret(len = strlen(tag), 0);
    short_arg[0] = tag[0];

    arg = (_arg)ejson_addR(h->args_d, short_arg, sizeof(_arg_t));
    is0_ret(arg, 0);

    {
        estr_wrtS(arg->desc, desc);
        estr_catB(arg->desc, "\n", 1);

        if(type == __STR__)
        {
            arg->__type = __STR__;
            estr_wrtS(arg->valS, df.s);
        }
        else if(type == __INT__)
        {
            arg->__type = __INT__;
            arg->valI = df.i;
        }

        estr_wrtS(*((estr*)&arg->__option.name), name);
        arg->__option.val     = tag[0];
        arg->__option.has_arg = no_argument;

        short_arg[0] = tag[0];
        switch (len) {
            case 1 :    break;
            case 2 :    if(tag[1] == ':')
                        {
                            arg->__option.has_arg = required_argument;
                            short_arg[1] = ':';
                        }
                        break;
            case 3 :
            default:    if(tag[1] == ':')
                        {
                            arg->__option.has_arg = required_argument;
                            short_arg[1] = ':';
                        }
                        if(tag[1] == ':' && tag[2] == ':')
                        {
                            arg->__option.has_arg = optional_argument;
                            short_arg[1] = ':';
                            short_arg[2] = ':';
                        }
                        break;
        }
    }

    {
        estr_catB(h->__shortopts, short_arg, strlen(short_arg));

        estr_catB(h->info, "    -", 5);
        estr_catB(h->info, tag, 1);
        estr_catB(h->info, "\t", 1);
        estr_catE(h->info, arg->desc);
    }

    return arg;
}

arg    earg_addArgS(earg h, constr tag, constr name, constr df, constr desc)
{
    is0_ret(h, 0); is0_ret(tag, 0);

    eptr ptr; ptr.s = df;
    return (arg)__earg_addArg(h, tag, name, ptr, desc, __STR__);
}


arg    earg_addArgI(earg h, constr tag, constr name, i64    df, constr desc)
{
    is0_ret(h, 0); is0_ret(tag, 0);

    eptr ptr; ptr.i = df;
    return (arg)__earg_addArg(h, tag, name, ptr, desc, __INT__);
}

int    earg_parse(earg h, int argc, cstr argv[])
{
    int opt; int __longid; _arg arg; option_t* __longopts; char short_arg[4] = {0};

    is0_ret(h, 0);
    is0_ret(eobj_len(h->args_d), 0);

    estr_clear(h->__longopts);
    ejson_foreach_s(h->args_d, itr)
    {
        arg = EOBJ_VALR(itr);

        estr_catB(h->__longopts, &arg->__option, sizeof(option_t));
    }
    estr_catB(h->__longopts, &__end_opt, sizeof(option_t));
    __longopts = (option_t*)h->__longopts;

    while( (opt = getopt_long(argc, argv, h->__shortopts, __longopts, &__longid)) != -1 )
    {
        switch (opt) {
            case  0  :
            case '?' : return 0;    // getopt_long' already printed an error message
        }

        short_arg[0] = (u8)opt;
        arg = ejson_rValR(h->args_d, short_arg);
        if(!arg)
        {
            fprintf (stderr, "%s    option unknown: %c\n", h->desc, *short_arg);
            fflush(stderr);
            exit(2);
        }

        arg->given = 1;

        switch (arg->__option.has_arg) {
            case optional_argument: if(!optarg)
                                    break;
            case required_argument: if(arg->__type == __STR__) estr_wrtS(arg->valS, optarg);
                                    else
                                    {
                                        cstr endptr;
                                        arg->valI = strtoll(optarg, &endptr, 10);

                                        if(*endptr)
                                        {
                                            fprintf(stderr, "%s    option -%c need an int value\n", h->desc, *short_arg);
                                            fflush(stderr);
                                            exit(3);
                                        }
                                    }
                                    break;
        }
    }

    return 1;
}

constr earg_info(earg h)
{
    return h ? h->info : 0;
}

arg    earg_find(earg h, char tag)
{
    char short_arg[4] = {0};
    short_arg[0] = tag;

    return h ? ejson_rValR(h->args_d, short_arg) : 0;
}
