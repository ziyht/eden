/// =====================================================================================
///
///       Filename:  ejson.c
///
///    Description:  a easier way to handle json, you can also using it as a simple dic
///
///        Version:  1.1.0
///        Created:  12/18/2016 08:51:34 PM
///       Revision:  none
///       Compiler:  gcc
///       Depeneds:  estr
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#undef  EJSON_VERSION
#define EJSON_VERSION "ejson 1.1.8"     // adjust api and fix sort logic

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <fcntl.h>

#include "ecompat.h"
#include "eerr.h"
#include "etype.h"
#include "eutils.h"
#include "estr.h"

#include "eobj_p.h"
#include "libs/estr_p.h"

#include "ejson.h"

typedef struct _ejson_node_s* _ejsn;
typedef struct _ejson_root_s* _ejsr;

/** -----------------------------------------------------
 *
 *  ejson dict
 *
 *      rebuild from redis, is a more simple version vs
 *  edict
 *
 *  -----------------------------------------------------
 */
#pragma pack(1)
typedef struct dictht_s{
    _ejsn*  table;       // to save data
    uld     size;
    uld     sizemask;
    uld     used;
}dictht_t, dictht;

typedef struct dict_s {
    dictht_t ht[2];
    long     rehashidx;
}dict_t, * dict;

typedef struct dictLink_s{
    _ejsn* _prev;   // addr of prev addr point to found node
    _ejsn* _pos;    // addr of addr in table witch contains the node
    uld*   _used;
}dictLink_t, *L;
#pragma pack()

// -- pre definitions
#define DICT_OK                   0
#define DICT_ERR                  1
#define DICT_HT_INITIAL_SIZE      4

#define _dict_can_resize          1
#define _dict_force_resize_ratio  5

// -- API
static inline dict  _dict_new();
static inline void  _dict_clear   (dict d);
static inline void  _dict_free    (dict d);
static inline _ejsn _dict_addB    (dict d, constr k, int k_len, _ejsn n);
static inline _ejsn _dict_addS    (dict d, constr k, _ejsn n);
static inline _ejsn _dict_findB   (dict d, constr k, int k_len);
static inline _ejsn _dict_findB_ex(dict d, constr k, int k_len, bool rm);
static inline _ejsn _dict_findS   (dict d, constr k);
static inline _ejsn _dict_findS_ex(dict d, constr k, bool rm);
static inline _ejsn _dict_del     (dict d, _ejsn del);
static inline int   _dict_getBL   (dict d, constr k, int k_len, L l);
static inline int   _dict_getBL_ex(dict d, constr k, int k_len, L l);
static inline int   _dict_getSL   (dict d, constr k, L l);
static inline int   _dict_getSL_ex(dict d, constr k, L l);

#define _dict_link(l, n)        do{ _n_dnext(n) = *((l)._pos); *((l)._pos) = n; *((l)._used) += 1;}while(0)
#define _dictHashKeyS(k, l)     __djbHashS(k)
#define _dictHashKeyB(k, l)     __djbHashB(k, l)

// -- micros
#define _dict_htreset(ht)       memset(ht, 0, sizeof(dictht_t));
#define _dict_resetHt(d)        memset(d->ht, 0, sizeof(dictht_t) * 2);
#define _dictIsRehashing(d)     ((d)->rehashidx != -1)
#define _dictSize(d)            ((d)->ht[0].used+(d)->ht[1].used)


/** -----------------------------------------------------
 *
 *  ejson data struct definitions
 *
 *  -----------------------------------------------------
 */
#pragma pack(1)

typedef struct { _ejsn o; uint  i;} list_t;

typedef struct _ejson_link_s{
    _ejsn           lp,     // prev node, used for double link
                    ln,     // next node, used for double link
                    dn;     // next node, used for obj's dic
}_ejsl_t, * _ejsl;

typedef struct _ejson_node_s{
    _ejsl_t         link;
    ekey_t          key;
    _ehdr_t         hdr;

    eobj_t          obj;
}_ejsn_t;

typedef struct _ejson_s{

    _ejsn h,      // head
          t;      // tail

    union{
        dict_t*     dict;
        list_t*     list;
    }hd;
}_ejson_t;

#define _R_OLEN (sizeof(cptr) * 3)

typedef struct _ejson_root_s{
    _ejsl_t         link;
    ekey_t          key;
    _ehdr_t         hdr;

    _ejson_t        ejson;
}_ejsr_t;

#pragma pack()

static constr err_p;

/// ---------------------- eobj adaptor ---------------------------

#define _CUR_C_TYPE             EJSON

#define _DNODE_TYPE             _ejsn_t
#define _DNODE_LNK_FIELD        link
#define _DNODE_KEY_FIELD        key
#define _DNODE_HDR_FIELD        hdr
#define _DNODE_OBJ_FIELD        obj

#define _RNODE_TYPE             _ejsr_t
#define _RNODE_OBJ_FIELD        ejson

#define _n_newO(n)      n = _n_newc(sizeof(_RNODE_TYPE) - sizeof(void*)); _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_OBJ );
#define _n_newA(n)      n = _n_newc(sizeof(_RNODE_TYPE));                 _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_ARR );

#define _n_lprev(n)             _n_l(n).lp
#define _n_lnext(n)             _n_l(n).ln
#define _n_dnext(n)             _n_l(n).dn

#define _r_dict(r)              (*_r_o(r)).hd.dict
#define _r_list(r)              (*_r_o(r)).hd.list
#define _r_head(r)              (*_r_o(r)).h
#define _r_tail(r)              (*_r_o(r)).t

#define _r_keyS(r)              (r)->key.s
#define _r_freekeyS(r)          _cur_freekeyS(_r_keyS(r))

#define _cur_newkeyS(l)         emalloc(l + 1)
#define _cur_dupkeyS            strdup
#define _cur_cmpkeyS            strcmp
#define _cur_ncmpkeyS           strncmp
#define _cur_freekeyS           efree
#define _cur_lenkeyS            strlen

/// -------------------------- managers ---------------------------

// -- obj's manager
#define             _obj_hd(     r)          _r_dict(r)
#define             _obj_init(  r)           _obj_hd((_ejsr)(r)) = _dict_new()
#define             _obj_clear(  r)          _dict_clear(_obj_hd(r))
#define             _obj_bzero( r)           memset(_r_o(r), 0, _R_OLEN)
#define             _obj_findS(   r,k  )     _dict_findS(_obj_hd(r), k)
//#define             _obj_findS_ex(r,k,  rm)  _dict_findS_ex(_obj_hd(r), k, rm)
#define             _obj_findB(   r,k,l)     _dict_findB(_obj_hd(r), k, l)
#define             _obj_findB_ex(r,k,l,rm)  _dict_findB_ex(_obj_hd(r), k, l, rm)
#define             _obj_getSL(   r,k  ,lp)  _dict_getSL   (_obj_hd(r), k,    lp)
#define             _obj_getSL_ex(r,k  ,lp)  _dict_getSL_ex(_obj_hd(r), k,    lp)
#define             _obj_getBL(   r,k,l,lp)  _dict_getBL   (_obj_hd(r), k, l, lp)
#define             _obj_getBL_ex(r,k,l,lp)  _dict_getBL_ex(_obj_hd(r), k, l, lp)
#define             _obj_free(   r)          _dict_free(_obj_hd(r))
static inline _ejsn _obj_add  (_ejsr r, cstr key, _ejsn n);
static inline void  _obj_link (_ejsr r, _ejsn n, L l);
static inline eobj  _obj_popH (_ejsr r);
static inline eobj  _obj_popT (_ejsr r);
static inline eobj  _obj_takeN(_ejsr r, _ejsn n);
static inline eobj  _obj_findS_ex(_ejsr r, constr k, bool rm);

// -- arr's manager
#define             _arr_hd(    r)   _r_list(r)
#define             _arr_init(  r)   _arr_hd((_ejsr)(r)) = ecalloc(3, sizeof(uint))
#define             _arr_clear( r)   memset( _arr_hd(r), 0, sizeof(list_t))
#define             _arr_bzero( r)   memset(_r_o(r), 0, _R_OLEN)
#define             _arr_free(  r)   efree(_arr_hd(r))
static inline void  _arr_appd   (_ejsr r, _ejsn n);
static inline _ejsn _arr_push   (_ejsr r, _ejsn n);
static inline eobj  _arr_popH   (_ejsr r);
static inline eobj  _arr_popT   (_ejsr r);
static inline _ejsn _arr_find   (_ejsr r, uint  idx);
static inline _ejsn _arr_find_ex(_ejsr r, uint  idx, bool rm);
static inline eobj  _arr_takeI  (_ejsr r, uint  idx);
static inline eobj  _arr_takeN  (_ejsr r, _ejsn n);

/// -------------------- check settings ---------------------------

#define _key_is_valid(  k) (k)
#define _key_is_invalid(k) (!k)

#define _getObjByPath(r, path) __objByPath(r, path, 0)
#define _rmObjByPath( r, path) __objByPath(r, path, 1)
#define _getObjByIdx( r, idx ) __objByIdx( r, idx , 0)
#define _rmObjByIdx(  r, idx ) __objByIdx( r, idx , 1)
#define _getObjByRawk(r, rawk) __objByRawk(r, rawk, 0)
#define _rmObjByRawk( r, rawk) __objByRawk(r, rawk, 1)

static inline eobj __objByRawk(_ejsr r, constr rawk, bool rm)
{
    _ejsn n;

    is1_ret(!_r_o(r) || _r_typec(r) != EJSON || !_key_is_valid(rawk), 0);

    switch (_r_typeo(r))
    {
        case EOBJ:  return _obj_findS_ex(r, rawk, rm);

        case EARR:  {
                        cstr endp; int id;

                        id = strtol(rawk, &endp, 10);

                        is1_ret(*endp, NULL);

                        n = _arr_find_ex(r, id, rm);
                    }

                    return n ? _n_o(n) : 0;
    }

    return 0;
}

static inline eobj __objByIdx(_ejsr r, int idx, bool rm)
{
    _ejsn n;

    is1_ret(!_r_o(r) || _r_typeco(r) != _EJSON_CO_ARR, 0);

    n = _arr_find_ex(r, idx, rm);

    return n ? _n_o(n) : 0;
}

static __always_inline int __getAKey(constr p, constr* _key, constr* _p)
{
    if(*p == '.')
    {
        p++; *_key = p;

        while(*p && *p != '.' && *p != '[')
        {
            p++;
        }

        *_p = p;
    }
    else if(*p == '[')
    {
        p++; *_key = p;

        while(*p && *p != ']')
        {
            p++;
        }

        if(*p != ']')
            return -1;

        *_p = p + 1;

        return (int)(*_p - *_key) - 1;
    }
    else
    {
        *_key = p;

        while(*p && *p != '.' && *p != '[')
        {
            p++;
        }

        *_p = p;
    }

    return (int)(*_p - *_key);
}

static eobj __objByPath(_ejsr r, constr path, bool rm)
{
    constr key; constr p; int len; int id;

    _ejsn n;

    is1_ret(!_r_o(r) || _r_typec(r) != EJSON || !_key_is_valid(path) , 0);

    p = path;

    n = (_ejsn)r;

    do{

        len = __getAKey(p, &key, &p);

        is1_ret(len < 0, 0);

        r = (_ejsr)n;

        switch (_r_typeo(r))
        {
            case EOBJ:  is0_ret(n = _obj_findB(r, key, len), 0);
                        continue;

            case EARR:  {
                            cstr endp;

                            is1_ret(len == 0, 0);

                            id = strtol(key, &endp, 10);

                            //! must parse over then is a valid number
                            if(*endp && *endp != ']' && *endp != '.')
                                return 0;
                        }

                        is0_ret(n = _arr_find(r, id), 0);

                        continue;

            default:    return 0;
        }

    }while(*p);

    is1_exe(rm, switch (_r_typeo(r)) {
                                          case EOBJ: _obj_takeN(r, n); break;
                                          case EARR: _arr_takeN(r, n); break;
                                          default  : return 0;              })

    return _n_o(n);
}

/// -------------------- str strip helper -------------------------

#define _lstrip1(s) do{ while(*s && (unsigned char)*s <= 32) s++;}while(0)

#define NEW 0

#if NEW
#else

typedef constr (*__lstrip_cb)(constr);

static __always_inline constr __lstrip1(constr str)
{
    _lstrip1(str);
    return str;
}

static __always_inline constr __lstrip2(constr str)
{
    do{
        if(!*str || (*str > 32 && (unsigned char)*str != '/' && (unsigned char)*str != '#'))
            break;

        while (*str && ((unsigned char)*str <= 32 || (unsigned char)*str >= 128))
            str++;

        switch (*str) {
            case '/': if     (*(str+1) == '*') {while (*str && !(*str=='*' && str[1]=='/')) str++; if(*str) str += 2; else { goto return_;}}
                      else if(*(str+1) == '/') {while (*str && *str!='\n'                 ) str++; if(*str) str += 1; else { goto return_;}}
                      break;
            case '#':                          {while (*str && *str!='\n'                 ) str++; if(*str) str += 1; else { goto return_;}}
                      break;
        }
    }while(str);

return_:
    return str;
}

#endif


/** -----------------------------------------------------
 *
 *  ejson newer
 *
 *  -----------------------------------------------------
 */
ejson  ejson_new(etypeo type, eval val)
{
    _ejsn n;

    switch (type) {
        case EFALSE :  _n_newTF(n);              break;
        case ETRUE  :  _n_newTT(n);              break;
        case ENULL  :  _n_newTN(n);              break;
        case ENUM   :  _n_newI (n, val);         break;
        case EPTR   :  _n_newP (n, val.p);       break;
        case ESTR   :  _n_newS (n, val.s);       break;
        case ERAW   :  _n_newRc(n, val.u32);     break;
        case EOBJ   :  _n_newO(n); _obj_init(n); break;
        case EARR   :  _n_newA(n); _arr_init(n); break;
        case ENUM_F :  _n_newF (n, val);         break;

        default     :  eerrset("invalid type"); return 0;
    }

    return _n_o(n);
}

/** -----------------------------------------------------
 *
 *  ejson parsing
 *
 *  -----------------------------------------------------
 */
#define KEY 1

static __always_inline int __scan_str_len(constr s);

static cstr  __parse_str(constr s, int len, constr* _err, cstr out);
static cstr  __parse_KEY(constr* _src, constr* _err, __lstrip_cb lstrip);

static _ejsn __parse_NUM(cstr   key, constr* _src, constr* _err, __lstrip_cb lstrip);
static _ejsn __parse_STR(cstr* _key, constr* _src, constr* _err, __lstrip_cb lstrip);
static _ejsn __parse_OBJ(cstr   key, constr* _src, constr* _err, __lstrip_cb lstrip);
static _ejsn __parse_ARR(cstr   key, constr* _src, constr* _err, __lstrip_cb lstrip);

static _ejsn __parse_obj(cstr* _key, constr* _src, constr* _err, __lstrip_cb lstrip);

static int   __ejson_free(_ejsr r);

ejson ejson_parseS  (constr json) { return ejson_parseSEx(json, &eerrget(), ENDCHECK);}
ejson ejson_parseSEx(constr json, constr* _err, eopts opts)
{
    _ejsn n; __lstrip_cb lstrip; constr err;

    is0_exeret(json, err = "null", 0);

    lstrip = opts & COMMENT ? __lstrip2 : __lstrip1;

    json = lstrip(json);

    if(is_eq(*json, '\"'))
    {
        int len = __scan_str_len(json);

        is1_ret(len < 0, 0);

        constr obj_p = lstrip(json + len + 2);

        if(*obj_p == ':')
        {
            cstr key = _cur_newkeyS(len);

            if(!__parse_str(json, len, &err, key))
            {
                goto failed;
            }

            obj_p++;

            n = __parse_obj(&key, &obj_p, &err, lstrip);

            is0_exe(n, _cur_freekeyS(key); goto failed);

            json = obj_p;
        }
        else
        {
            _n_newSc(n, len);

            if(!__parse_str(json, len, _err, _n_valS(n)))
            {
                _n_free(n);
                goto failed;
            }

            json = lstrip(json + len +2);
        }
    }
    else
        n = __parse_obj(0, &json, &err, lstrip);

    if(n && opts & ENDCHECK)
    {
        is1_exe(*json, ejson_free(_n_o(n));
                       goto failed);
    }

    return n ? _n_o(n) : 0;

failed:
    if(_err) *_err = err;

    return 0;
}

ejson ejson_parseF  (constr path) { return ejson_parseFEx(path, &eerrget(), COMMENT);}
ejson ejson_parseFEx(constr path, constr* _err, eopts opts)
{
    estr s; constr err;

    s = estr_fromFile(path, 10 * 1024 * 1024 );  // 10M
    if(s)
    {
        __lstrip_cb lstrip = opts & COMMENT ? __lstrip2 : __lstrip1;

        constr json = lstrip(s);

        _ejsn n = __parse_obj(0, &json, &err, lstrip);

        if(n)
        {
            if(opts & ENDCHECK)
            {
                json = lstrip(json);

                is1_exe(*json,  ejson_free(_n_o(n));
                                err = "null-terminated check failed";
                                goto failed);
            }

            estr_free(s);

            return _n_o(n);
        }
    }

failed:
    if(_err) *_err = err;

    estr_free(s);

    return 0;
}

// todo : check s if have effect
static _ejsn __parse_obj(cstr* _key,  constr* _src, constr* _err, __lstrip_cb lstrip)
{
    _ejsn n;

    switch (**_src)
    {
        case 'n' :  is0_exeret(strncmp(*_src, "null" , 4), _n_newTN(n); is1_exeret(n, if(_key) _n_setKeyS(n, *_key); *_src = lstrip(*_src + 4), n), 0); break;
        case 'f' :  is0_exeret(strncmp(*_src, "false", 5), _n_newTF(n); is1_exeret(n, if(_key) _n_setKeyS(n, *_key); *_src = lstrip(*_src + 5), n), 0); break;
        case 't' :  is0_exeret(strncmp(*_src, "true" , 4), _n_newTT(n); is1_exeret(n, if(_key) _n_setKeyS(n, *_key); *_src = lstrip(*_src + 4), n), 0); break;
        case '\"':  return __parse_STR(_key            , _src, _err, lstrip);
        case '[' :  return __parse_ARR(_key ? *_key : 0, _src, _err, lstrip);
        case '{' :  return __parse_OBJ(_key ? *_key : 0, _src, _err, lstrip);
        default  :  if(**_src == '-' || (**_src >= '0' && **_src <= '9'))
                    {
                        return __parse_NUM(_key ? *_key : 0, _src, _err, lstrip);
                    }
    }

    *_err = *_src;

    return 0;
}

static cstr __parse_KEY(constr* _src, constr* _err, __lstrip_cb lstrip)
{
    int len = __scan_str_len(*_src);

    if(len >= 0)
    {
        cstr s = _cur_newkeyS(len);

        if(__parse_str(*_src, len, _err, s))
        {
            *_src = lstrip(*_src + len + 2);   // len + two quotation '\"'
            return s;
        }

        _cur_freekeyS(s);

        return 0;
    }

    *_err = *_src - len;

    return 0;
}

static _ejsn __parse_NUM(cstr key, constr* _src, constr* _err, __lstrip_cb lstrip)
{
    double v = 0, sign = 1, scale = 0;  int subscale = 0, signsubscale = 1; bool is_float = 0; constr s  = *_src; _ejsn n;

    E_UNUSED(_err);

    _n_newNc(n); is1_elsret(n, _n_setKeyS(n, key);, 0);

    if (*s == '-')                     sign = -1, s++;      // Has sign?
    if (*s == '0')                                s++;      // is zero
    if (*s >= '1' && *s   <= '9')                {      do v = (v * 10.0) + (*s++ - '0');          while (*s >= '0' && *s <= '9'); }    // Number?
    if (*s == '.' && s[1] >= '0' && s[1] <= '9') { s++; do v = (v * 10.0) + (*s++ - '0'), scale--; while (*s >= '0' && *s <= '9'); is_float = 1;}	// Fractional part?
    if (*s == 'e' || *s   == 'E')                           // Exponent?
    {
        s++;

        if      (*s == '+') s++;
        else if (*s == '-') signsubscale = -1, s++;     // With sign?

        while (*s >= '0' && *s <= '9') subscale = (subscale * 10) + (*s++ - '0');	// Number?
    }

    v = sign * v * pow(10.0, (scale + subscale * signsubscale));     // number = +/- number.fraction * 10^+/- exponent

    if(is_float) { _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_NUM_F); _n_valF(n) =      v; }
    else         { _n_typecoe(n) = _cur_type(_CUR_C_TYPE, COE_NUM_I); _n_valI(n) = (i64)v; }

    *_src       = lstrip(s);         // save new pos

    return n;
}

/**
 * @brief __scan_str_len
 * @param _src
 * @return >= 0 ok, the return value is the str len scaned
 *         <  0 err occured, abs() to get the err pos
 */
static __always_inline int __scan_str_len(constr s)
{
    constr end_p = s + 1;

// todo

#if 1
    //int len = 0;
    while (*end_p !='\"' && *end_p) if (*end_p++ == '\\') end_p++;
#else
    while (*end_p) if(*end_p !='\"') end_p++; else if (end_p[-1] != '\\') break;
#endif

    return *end_p == '\"' ? (int)(end_p - s     - 1)
                          : (int)(s     - end_p - 1);
}

static __always_inline unsigned __parse_hex4(constr str)
{
    unsigned h=0;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    return h;
}

static cstr  __parse_str(constr s, int len, constr* _err, cstr out)
{
#if 0
    memcpy(out, s + 1, len); out[len] = '\0';
#else
    static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    constr   end_p = s + 1 +len,
             ptr1  = s + 1;
    cstr     ptr2  = out;
    unsigned uc, uc2;

    while (ptr1 < end_p)
    {
        if (*ptr1!='\\') *ptr2++ = *ptr1++;
        else
        {
            ptr1++;
            switch (*ptr1)
            {
                case 'b': *ptr2++='\b';	break;
                case 'f': *ptr2++='\f';	break;
                case 'n': *ptr2++='\n';	break;
                case 'r': *ptr2++='\r';	break;
                case 't': *ptr2++='\t';	break;
                case 'u':	 // transcode utf16 to utf8
                    uc = __parse_hex4(ptr1 + 1); ptr1 += 4;	// get the unicode char
                    is1_exeret(ptr1 >= end_p,                       *_err = s, 0);     // invalid
                    is1_exeret((uc>=0xDC00 && uc<=0xDFFF) || uc==0, *_err = s, 0);     // check for invalid

                    if (uc>=0xD800 && uc<=0xDBFF)       // UTF16 surrogate pairs
                    {
                        is1_exeret(ptr1 + 6 > end_p,               *_err = s, 0);   // invalid
                        is1_exeret(ptr1[1]!='\\' || ptr1[2]!='u',  *_err = s, 0);	// missing second-half of surrogate
                        uc2 = __parse_hex4(ptr1 + 3); ptr1 += 6;
                        is1_exeret(uc2 < 0xDC00  || uc2 > 0xDFFF,  *_err = s, 0);	// invalid second-half of surrogate
                        uc = 0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                    }

                    if      (uc < 0x80   ) len = 1;
                    else if (uc < 0x800  ) len = 2;
                    else if (uc < 0x10000) len = 3;
                    else                   len = 4;
                    ptr2 += len;

                    switch (len) {
                        case 4: *--ptr2 =((uc | 0x80              ) & 0xBF); uc >>= 6;
                        case 3: *--ptr2 =((uc | 0x80              ) & 0xBF); uc >>= 6;
                        case 2: *--ptr2 =((uc | 0x80              ) & 0xBF); uc >>= 6;
                        case 1: *--ptr2 =( uc | firstByteMark[len]);
                        default:;
                    }
                    ptr2+=len;
                    break;
                default:  *ptr2++ = *ptr1; break;
            }
            ptr1++;
        }
    }
    *ptr2 = 0;
#endif

    return out;
}

/**
 * @brief __parse_STR
 * @param _key : a pointer to the name of obj
 *                   if  _key == NULL, create new obj and parse str to obj.v.s
 *                   if *_key == NULL, parse str to *_key;
 *                   else              create new obj, set *_key to obj.k and parse str to obj.v.s
 * @param _src : src string pointer
 * @param _err : error str pointer
 * @return
 */
static _ejsn __parse_STR(cstr* _key, constr* _src, constr* _err, __lstrip_cb lstrip)
{
    static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    int len; cstr ptr2, k_v; unsigned uc, uc2;

    constr end_p = *_src + 1, ptr1 = end_p;

// todo
#if 1
    while (*end_p !='\"' && *end_p) if (*end_p++ == '\\') end_p++;
#else
    while (*end_p) if(*end_p !='\"') end_p++; else if (end_p[-1] != '\\') break;
#endif

    len = (int)(end_p - *_src - 1);

    is0_ret(k_v = _cur_newkeyS(len), 0);   // mem fail

#if 0
    memcpy(k_v, s + 1, len);
    ptr = end_p;
#else

    ptr2 = k_v;
    while (ptr1 < end_p)
    {
        if (*ptr1!='\\') *ptr2++ = *ptr1++;
        else
        {
            ptr1++;
            switch (*ptr1)
            {
                case 'b': *ptr2++='\b';	break;
                case 'f': *ptr2++='\f';	break;
                case 'n': *ptr2++='\n';	break;
                case 'r': *ptr2++='\r';	break;
                case 't': *ptr2++='\t';	break;
                case 'u':	 // transcode utf16 to utf8
                    uc = __parse_hex4(ptr1 + 1); ptr1 += 4;	// get the unicode char
                    is1_exeret(ptr1 >= end_p,                       *_err = ptr1, 0);     // invalid
                    is1_exeret((uc>=0xDC00 && uc<=0xDFFF) || uc==0, *_err = ptr1, 0);     // check for invalid

                    if (uc>=0xD800 && uc<=0xDBFF)       // UTF16 surrogate pairs
                    {
                        is1_exeret(ptr1 + 6 > end_p,               *_err = ptr1, 0);   // invalid
                        is1_exeret(ptr1[1]!='\\' || ptr1[2]!='u',  *_err = ptr1, 0);	// missing second-half of surrogate
                        uc2 = __parse_hex4(ptr1 + 3); ptr1 += 6;
                        is1_exeret(uc2 < 0xDC00  || uc2 > 0xDFFF,  *_err = ptr1, 0);	// invalid second-half of surrogate
                        uc = 0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                    }

                    if      (uc < 0x80   ) len = 1;
                    else if (uc < 0x800  ) len = 2;
                    else if (uc < 0x10000) len = 3;
                    else                   len = 4;
                    ptr2 += len;

                    switch (len) {
                        case 4: *--ptr2 =((uc | 0x80              ) & 0xBF); uc >>= 6;
                        case 3: *--ptr2 =((uc | 0x80              ) & 0xBF); uc >>= 6;
                        case 2: *--ptr2 =((uc | 0x80              ) & 0xBF); uc >>= 6;
                        case 1: *--ptr2 =( uc | firstByteMark[len]);
                        default:;
                    }
                    ptr2+=len;
                    break;
                default:  *ptr2++ = *ptr1; break;
            }
            ptr1++;
        }
    }
    *ptr2 = 0;
#endif

    if (*ptr1 =='\"') *_src = lstrip(ptr1 + 1);

    if(!_key || *_key != NULL)
    {
         _ejsn n;

        _n_newSl(n, k_v, len); _cur_freekeyS(k_v);
        is0_ret(n, 0);

        if(_key)
            _n_setKeyS(n, *_key);

        return n;
    }
    else
        *_key = k_v;

    return 0;
}

static int __ejson_free(_ejsr r);
static _ejsn __parse_OBJ(cstr key, constr* _src, constr* _err, __lstrip_cb lstrip)
{
    _ejsr r; _ejsn c_n; cstr c_key;

    //! check err format
    *_src = lstrip(*_src + 1);
    if(**_src != '\"' && **_src != '}')
    {
        *_err = *_src;
        return 0;
    }

    //! creat obj
    {
        _n_newO(r);
        _n_setKeyS(r, key);
        _obj_init(r);

        is1_exeret(**_src == '}', *_src = lstrip(*_src + 1), (_ejsn)r);
    }

    //! parse first child
    {
        // -- parse key
        c_key = 0;
        __parse_STR(&c_key, _src, _err, lstrip);
        is0_exe(c_key    , goto rls_ret);

        // -- parse child
        is1_exe(**_src != ':', goto err_set);
        *_src = lstrip(*_src + 1);
        c_n   = __parse_obj(&c_key, _src, _err, lstrip);
        is0_exe(c_n, _cur_freekeyS(c_key); goto rls_ret);
        _n_setKeyS(c_n, c_key);

        // -- add child to obj
        _obj_add(r, c_key, c_n);
    }

    //! parse next children
    while(**_src == ',')
    {
        *_src = lstrip(*_src + 1);

        // -- parse key
        is1_exe(**_src != '\"', break);
        c_key = 0;
        __parse_STR(&c_key, _src, _err, lstrip);
        is0_exe(c_key     , goto rls_ret);

        // -- parse child
        is1_exe(**_src != ':', goto err_set);
        *_src = lstrip(*_src + 1);
        c_n   = __parse_obj(&c_key, _src, _err, lstrip);
        is0_exe(c_n, goto rls_ret);
        _n_setKeyS(c_n, c_key);

        // -- link to dict
        is0_exe(_obj_add(r, c_key, c_n), __ejson_free((_ejsr)c_n); goto rls_ret);
    }

    //! check complited
    is1_exeret(**_src == '}', *_src = lstrip(*_src + 1), (_ejsn)r);

err_set:
    *_err = *_src;

rls_ret:
    __ejson_free(r);

    return 0;
}

static _ejsn __parse_ARR(cstr key, constr* _src, constr* _err, __lstrip_cb lstrip)
{
    _ejsr r; _ejsn c_n;

    _n_newA(r); _n_setKeyS(r, key);

    *_src = lstrip(*_src + 1);
    is1_exeret(**_src == ']', *_src = lstrip(*_src + 1), (_ejsn)r);

    _arr_init(r);

    do{
        c_n   = __parse_obj(0, _src, _err, lstrip);
        is0_exe(c_n, goto err_ret;);
        _arr_appd(r, c_n);

    }while(**_src == ',' && (*_src = lstrip(*_src + 1)) && **_src != ']');

    //! check complited
    //is1_exeret(**_src == ']', *_src = lstrip(*_src + 1), (_ejsn)r);
    *_src = lstrip(*_src + 1);
    return (_ejsn)r;

err_ret:
    *_err = *_src;
    __ejson_free(r);

    return 0;
}

/** -----------------------------------------------------
 *
 *  ejson clear and free
 *
 *  -----------------------------------------------------
 */

static int  __ejson_free    (_ejsr r);
static int  __ejson_free_ex (_ejsr r, eobj_rls_ex_cb rls, eval prvt);
static int  __ejson_clear   (_ejsr r);
static int  __ejson_clear_ex(_ejsr r, eobj_rls_ex_cb rls, eval prvt);

int ejson_clear  (ejson o)
{
    return o ? __ejson_clear(_eo_rn(o)) : 0;
}

int ejson_clearEx(ejson o, eobj_rls_ex_cb rls, eval prvt)
{
    if(!rls) return ejson_clear(o);
    return o ? __ejson_clear_ex(_eo_rn(o), rls, prvt) : 0;
}

int ejson_free(ejson o)
{
    is1_ret(!o || _eo_linked(o), 0);
    return __ejson_free(_eo_rn(o));
}

int ejson_freeEx(ejson o, eobj_rls_ex_cb rls, eval prvt)
{
    if(!rls) return ejson_free(o);

    is1_ret(!o || _eo_linked(o), 0);
    return __ejson_free_ex(_eo_rn(o), rls, prvt);
}

static int __ejson_clear(_ejsr r)
{
    int cnt = 0;

    switch (_r_typeo(r)) {
        case EOBJ : if(_r_head(r)){ cnt += __ejson_free((_ejsr)_r_head(r));} _obj_clear(r); break;
        case EARR : if(_r_head(r)){ cnt += __ejson_free((_ejsr)_r_head(r));} _arr_clear(r); break;
        default   : return 0;
    }

    _r_len (r) = 0;
    _r_head(r) = _r_tail(r) = 0;

    return cnt;
}

static int __ejson_clear_ex(_ejsr r, eobj_rls_ex_cb rls, eval prvt)
{
    int cnt = 0;

    switch (_r_typeo(r)) {
        case EPTR :
        case ERAW : rls(_n_o((_ejsn)r), prvt); break;
        case EOBJ : if(_r_head(r)){ cnt += __ejson_free_ex(r, rls, prvt);} _obj_clear(r); break;
        case EARR : if(_r_head(r)){ cnt += __ejson_free_ex(r, rls, prvt);} _arr_clear(r); break;
        default   : return 0;
    }

    _r_len (r) = 0;
    _r_head(r) = _r_tail(r) = 0;

    return cnt;
}

static int __ejson_free(_ejsr r)
{
    _ejsn itr; int cnt = 0;

    do{
        switch (_r_typeo(r))
        {
            case EOBJ : if(_r_tail(r)) {_n_lnext(_r_tail(r)) = _n_lnext(r);_n_lnext(r)= _r_head(r);} _obj_free(r);break;
            case EARR : if(_r_tail(r)) {_n_lnext(_r_tail(r)) = _n_lnext(r);_n_lnext(r)= _r_head(r);} _arr_free(r);break;
            default   : break;
        }
        if(_r_keyS(r)) _r_freekeyS(r);
        itr = _n_lnext(r);
        _r_free(r);
        r = (_ejsr)itr;

        cnt ++;
    }while(r);

    return cnt;
}

static int __ejson_free_ex(_ejsr r, eobj_rls_ex_cb rls, eval prvt)
{
    _ejsn itr; int cnt = 0;

    do{
        switch (_r_typeo(r))
        {
            case EPTR :
            case ERAW : rls((eobj)_r_o(r), prvt); break;
            case EOBJ : if(_r_tail(r)) {_n_lnext(_r_tail(r)) = _n_lnext(r);_n_lnext(r)= _r_head(r);} _obj_free(r);break;
            case EARR : if(_r_tail(r)) {_n_lnext(_r_tail(r)) = _n_lnext(r);_n_lnext(r)= _r_head(r);} _arr_free(r);break;
            default   : break;
        }
        if(_r_keyS(r)) _r_freekeyS(r);
        itr = _n_lnext(r);
        _r_free(r);
        r = (_ejsr)itr;

        cnt ++;
    }while(r);

    return cnt;
}

void ejson_show(ejson r)
{
    estr s; bool c_line = false;

    if(!r)
    {
        puts("ejson(0x0): (nullptr)");
        return;
    }

    s = ejson_toS(r, 0, PRETTY);

    switch (_eo_typeo(r)) {
        case EOBJ:
        case EARR: c_line = _eo_len(r) > 0;
    }

    printf("ejson(%p):%s%s\n", (cptr)r, c_line ? "\n" : " ",  s);

    _s_free(s);

    fflush(stdout);
}

constr ejson_errp() { return err_p    ; }
constr ejson_err () { return eerrget(); }

/** -----------------------------------------------------
 *
 *  ejson checking
 *
 *  -----------------------------------------------------
 */
static uint __check_KEY(constr* _src, constr* _err, __lstrip_cb lstrip, ejson set);
static uint __check_NUM(constr* _src, constr* _err, __lstrip_cb lstrip);
static uint __check_STR(constr* _src, constr* _err, __lstrip_cb lstrip);
static uint __check_ARR(constr* _src, constr* _err, __lstrip_cb lstrip);
static uint __check_OBJ(constr* _src, constr* _err, __lstrip_cb lstrip);

static uint __check_obj(constr* _src, constr* _err, __lstrip_cb lstrip);

uint   ejson_checkS  (constr json){ return ejson_checkSEx(json, &eerrget(), ENDCHECK); }
uint   ejson_checkSEx(constr json, constr* _err, eopts opts)
{
    constr err; __lstrip_cb lstrip; uint cnt = 0;

    is0_exe(json, err = "NULL"; goto failed);

    lstrip = opts & COMMENT ? __lstrip2 : __lstrip1;

    is0_exe(cnt = __check_obj(&json, &err, lstrip), goto failed);

    if(opts & ENDCHECK)
    {
        json = lstrip(json);

        is1_exe(*json, goto failed;);
    }

    return cnt;

failed:
    if(_err) *_err = json;

    return 0;
}

uint ejson_checkF  (constr path) { return ejson_checkFEx(path, 0, COMMENT); }
uint ejson_checkFEx(constr path, constr* _err, eopts opts)
{
    estr s; constr err;

    is0_exe(path, err = "NULL"; goto failed);

    s = estr_fromFile(path, 10 * 1024 * 1024 );  // 10M
    if(s)
    {
        uint ret = ejson_checkSEx(s, &err, opts);

        estr_free(s);

        return ret;
    }
    else
        err = "file not exists";

failed:
    if(_err) *_err = err;

    return 0;
}


static uint __check_obj(constr* _src, constr* _err, __lstrip_cb lstrip)
{
    switch (**_src) {
        case 'n' :  is0_exeret(strncmp(*_src, "null" , 4), *_src = lstrip(*_src + 4), 1); break;
        case 'f' :  is0_exeret(strncmp(*_src, "false", 5), *_src = lstrip(*_src + 5), 1); break;
        case 't' :  is0_exeret(strncmp(*_src, "true" , 4), *_src = lstrip(*_src + 4), 1); break;
        case '\"':  return __check_STR(_src, _err, lstrip);
        case '[' :  return __check_ARR(_src, _err, lstrip);
        case '{' :  return __check_OBJ(_src, _err, lstrip);
        default  :  if(**_src == '-' || (**_src >= '0' && **_src <= '9'))
                        return __check_NUM(_src, _err, lstrip);
    }

    return 0;	// failure: err src
}

static uint __check_NUM(constr* _src, constr* _err, __lstrip_cb lstrip)
{
    E_UNUSED(_err);

    constr s  = *_src;

    if (*s == '-')                                 s++;               // Has sign?
    if (*s == '0')                                 s++;               // is zero
    if (*s >= '1' && *s   <= '9')                       do s++; while (*s >= '0' && *s <= '9');	  // Number?
    if (*s == '.' && s[1] >= '0' && s[1] <= '9') { s++; do s++; while (*s >= '0' && *s <= '9'); } // Fractional part?
    if (*s == 'e' || *s   == 'E')         // Exponent?
    {
        s++;

        if      (*s == '+') s++;
        else if (*s == '-') s++;	// With sign?

        while (*s >= '0' && *s <= '9') s++;  // Number?
    }

    *_src = lstrip(s);

    return 1;
}

static uint __check_STR(constr* _src, constr* _err, __lstrip_cb lstrip)
{
    E_UNUSED(_err);

    int len = __scan_str_len(*_src);

    if(len < 0)
    {
        *_src = *_src - len;    // set err pos

        return 0;
    }

    *_src = lstrip(*_src + len + 2);

    return 1;
}

static uint __check_ARR(constr* _src, constr* _err, __lstrip_cb lstrip)
{
    uint cnt, ret;

    *_src = lstrip(*_src + 1);

    is1_exeret(**_src == ']', *_src = lstrip(*_src + 1), 1);

    cnt = 1;
    do{
        is0_ret(ret = __check_obj(_src, _err, lstrip), 0);
        cnt += ret;
    }while(**_src == ',' && (*_src = lstrip(*_src + 1)));

    is1_exeret(**_src == ']', *_src = lstrip(*_src + 1), cnt);

    return 0;
}

static uint __check_KEY(constr* _src, constr* _err, __lstrip_cb lstrip , ejson set)
{
    E_UNUSED(_err);

    int len = __scan_str_len(*_src);

    if(len < 0)
    {
        *_src = *_src - len;

        return 0;
    }
    else
    {
        bool ok;

        if(len <= 128)
        {
            char key[129];

            memcpy(key, *_src + 1, len);
            key[len] = '\0';

            ok = ejson_addT(set, key, ETRUE);
        }
        else
        {

            cstr key = emalloc(len + 1);

            memcpy(key, *_src + 1, len);
            key[len] = '\0';

            ok = ejson_addT(set, key, ETRUE);

            efree(key);
        }

        if(ok)
        {
            *_src = lstrip(*_src + len + 2);

            return 1;
        }
        else
        {
            //! todo: set err
        }
    }

    return 0;
}

static uint __check_OBJ(constr* _src, constr* _err, __lstrip_cb lstrip)
{
    ejson keyset; uint cnt, ret;

    keyset = ejson_new(EOBJ, EVAL_ZORE);
    *_src  = lstrip(*_src + 1);

    cnt = 1;
    while(**_src == '\"')
    {
        // -- check key
        is0_exe(__check_KEY(_src, _err, lstrip, keyset), goto err_set);

        // -- check obj
        is1_exe(**_src != ':', goto err_set);
        *_src = lstrip(*_src + 1);
        is0_exe(ret = __check_obj(_src, _err, lstrip), goto rls_ret);

        cnt += ret;

        // -- strip to next
        *_src   = lstrip(*_src);
        if(**_src == ',')
            *_src  = lstrip(*_src + 1);
    }
    is1_exeret(**_src == '}', *_src  = lstrip(*_src + 1); __ejson_free(_eo_rn(keyset)), cnt);

err_set:

rls_ret:
    __ejson_free(_eo_rn(keyset));

    return 0;
}

/** -----------------------------------------------------
 *
 *  ejson add
 *
 *  -----------------------------------------------------
 */

#define __ __always_inline
static     ejson __ejson_makeRoom(_ejsr r, ejson   in, bool overwrite, bool find);
static  __ ejson __ejson_addJson (_ejsr r, constr key, constr src);
static  __ ejson __ejson_addO    (_ejsr r, constr key, ejson   in );
#undef  __

#define _eo_setT(o, t)  if(t == EOBJ){ _obj_bzero(_eo_rn(o)); _obj_init(_eo_rn(o));} else if(t == EARR) { _arr_bzero(_eo_rn(o)); }
#define _t_olen(t)      (t < EOBJ ? 0 : _R_OLEN )
#define _s_olen(s)      (s ? strlen(s) + 1 : 1)

ejson   ejson_addJ(ejson r, constr key, constr json) { is0_ret(r, 0); return __ejson_addJson(_eo_rn(r), key, json); }
ejson   ejson_addT(ejson r, constr key, etypeo type) { _ejsn_t b = {{0}, {.s = (cstr)key}, {._len = _t_olen(type), ._typ = {.__1 = {EJSON, type, 0, 0}}}, {0}}; ejson o = __ejson_makeRoom(_eo_rn(r), _n_o(&b), 0, 0);  if(o) {               _eo_setT (o, type);             _eo_typeco (o) = _n_typeco(&b)   ; } return o; }
ejson   ejson_addI(ejson r, constr key, i64    val ) { _ejsn_t b = {{0}, {.s = (cstr)key}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_I }}, {0}}; ejson o = __ejson_makeRoom(_eo_rn(r), _n_o(&b), 0, 0);  if(o) {               _eo_setI (o, val );             _eo_typecoe(o) = _EJSON_COE_NUM_I; } return o; }
ejson   ejson_addF(ejson r, constr key, f64    val ) { _ejsn_t b = {{0}, {.s = (cstr)key}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_F }}, {0}}; ejson o = __ejson_makeRoom(_eo_rn(r), _n_o(&b), 0, 0);  if(o) {               _eo_setF (o, val );             _eo_typecoe(o) = _EJSON_COE_NUM_F; } return o; }
ejson   ejson_addS(ejson r, constr key, constr str ) { _ejsn_t b = {{0}, {.s = (cstr)key}, {._len =  _s_olen(str), ._typ = {.t_coe = _EJSON_COE_STR   }}, {0}}; ejson o = __ejson_makeRoom(_eo_rn(r), _n_o(&b), 0, 0);  if(o) { _n_len(&b)--; _eo_setS (o, str, _n_len(&b));  _eo_typeco (o) = _EJSON_CO_STR   ; } return o; }
ejson   ejson_addP(ejson r, constr key, conptr ptr ) { _ejsn_t b = {{0}, {.s = (cstr)key}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_PTR   }}, {0}}; ejson o = __ejson_makeRoom(_eo_rn(r), _n_o(&b), 0, 0);  if(o) {               _eo_setP (o, ptr );             _eo_typeco (o) = _EJSON_CO_PTR   ; } return o; }
ejson   ejson_addR(ejson r, constr key, uint   len ) { _ejsn_t b = {{0}, {.s = (cstr)key}, {._len =       len + 1, ._typ = {.t_coe = _EJSON_COE_RAW   }}, {0}}; ejson o = __ejson_makeRoom(_eo_rn(r), _n_o(&b), 0, 0);  if(o) {               _eo_wipeR(o, len );             _eo_typeco (o) = _EJSON_CO_RAW   ; } return o; }
ejson   ejson_addO(ejson r, constr key, ejson  o   ) { is0_ret(r, 0); return __ejson_addO(_eo_rn(r), key, o); }

ejson   ejson_rAddJ(ejson r, constr rawk, constr key, constr json) { return ejson_addJ(_getObjByRawk(_eo_rn(r), rawk), key, json); }
ejson   ejson_rAddT(ejson r, constr rawk, constr key, etypeo type) { return ejson_addT(_getObjByRawk(_eo_rn(r), rawk), key, type); }
ejson   ejson_rAddI(ejson r, constr rawk, constr key, i64    val ) { return ejson_addI(_getObjByRawk(_eo_rn(r), rawk), key, val ); }
ejson   ejson_rAddF(ejson r, constr rawk, constr key, f64    val ) { return ejson_addF(_getObjByRawk(_eo_rn(r), rawk), key, val ); }
ejson   ejson_rAddS(ejson r, constr rawk, constr key, constr str ) { return ejson_addS(_getObjByRawk(_eo_rn(r), rawk), key, str ); }
ejson   ejson_rAddP(ejson r, constr rawk, constr key, conptr ptr ) { return ejson_addP(_getObjByRawk(_eo_rn(r), rawk), key, ptr ); }
ejson   ejson_rAddR(ejson r, constr rawk, constr key, uint   len ) { return ejson_addR(_getObjByRawk(_eo_rn(r), rawk), key, len ); }
ejson   ejson_rAddO(ejson r, constr rawk, constr key, ejson  o   ) { return ejson_addO(_getObjByRawk(_eo_rn(r), rawk), key, o   ); }

ejson   ejson_iAddJ(ejson r, u32    idx , constr key, constr json) { return ejson_addJ(_getObjByIdx (_eo_rn(r), idx ), key, json); }
ejson   ejson_iAddT(ejson r, u32    idx , constr key, etypeo type) { return ejson_addT(_getObjByIdx (_eo_rn(r), idx ), key, type); }
ejson   ejson_iAddI(ejson r, u32    idx , constr key, i64    val ) { return ejson_addI(_getObjByIdx (_eo_rn(r), idx ), key, val ); }
ejson   ejson_iAddF(ejson r, u32    idx , constr key, f64    val ) { return ejson_addF(_getObjByIdx (_eo_rn(r), idx ), key, val ); }
ejson   ejson_iAddS(ejson r, u32    idx , constr key, constr str ) { return ejson_addP(_getObjByIdx (_eo_rn(r), idx ), key, str ); }
ejson   ejson_iAddP(ejson r, u32    idx , constr key, conptr ptr ) { return ejson_addS(_getObjByIdx (_eo_rn(r), idx ), key, ptr ); }
ejson   ejson_iAddR(ejson r, u32    idx , constr key, uint   len ) { return ejson_addR(_getObjByIdx (_eo_rn(r), idx ), key, len ); }
ejson   ejson_iAddO(ejson r, u32    idx , constr key, ejson  o   ) { return ejson_addO(_getObjByIdx (_eo_rn(r), idx ), key, o   ); }

ejson   ejson_pAddJ(ejson r, constr path, constr key, constr json) { return ejson_addJ(_getObjByPath(_eo_rn(r), path), key, json); }
ejson   ejson_pAddT(ejson r, constr path, constr key, etypeo type) { return ejson_addT(_getObjByPath(_eo_rn(r), path), key, type); }
ejson   ejson_pAddI(ejson r, constr path, constr key, i64    val ) { return ejson_addI(_getObjByPath(_eo_rn(r), path), key, val ); }
ejson   ejson_pAddF(ejson r, constr path, constr key, f64    val ) { return ejson_addF(_getObjByPath(_eo_rn(r), path), key, val ); }
ejson   ejson_pAddS(ejson r, constr path, constr key, constr str ) { return ejson_addS(_getObjByPath(_eo_rn(r), path), key, str ); }
ejson   ejson_pAddP(ejson r, constr path, constr key, conptr ptr ) { return ejson_addP(_getObjByPath(_eo_rn(r), path), key, ptr ); }
ejson   ejson_pAddR(ejson r, constr path, constr key, uint   len ) { return ejson_addR(_getObjByPath(_eo_rn(r), path), key, len ); }
ejson   ejson_pAddO(ejson r, constr path, constr key, ejson  o   ) { return ejson_addO(_getObjByPath(_eo_rn(r), path), key, o   ); }

static ejson __ejson_makeRoom(_ejsr r, eobj in, bool overwrite, bool find)
{
    dictLink_t l; _ejsn n; //uint len;

    E_UNUSED(overwrite); E_UNUSED(find);

    is1_ret(!_r_o(r), 0);

    switch (_r_typeo(r))
    {
        case EOBJ:  is0_ret(_key_is_valid(_eo_keyS(in)), 0);

                    //len = strlen(_eo_keyS(in));
                    if(!_obj_getSL(r, _eo_keyS(in), &l)) return 0;

                    n = _n_newm(_eo_len(in)); _n_init(n);

                    _n_setKeyS(n, _cur_dupkeyS(_eo_keyS(in)));

                    _obj_link(r, n, &l);

                    return _n_o(n);

        case EARR:  n = _n_newm(_eo_len(in)); _n_init(n);

                    if(!_arr_hd(r)) _arr_init(r);

                    _arr_appd(r, n);

                    return _n_o(n);

        default  :  eerrset(_ERRSTR_TYPEDF); return 0;
    }

    return 0;
}

static inline ejson __ejson_addJson(_ejsr r, constr key, constr src)
{

#undef  _opt
#define _opt __lstrip1

    cstr hk; _ejsn n; dictLink_t l; constr err;

    hk = NULL;

    _lstrip1(src);
    if(*src == '\"')        // maybe have a key in src
    {
        __parse_STR(&hk, &src, &err, _opt);

        if(hk)
        {
            if(*src == ':')
            {
                src++; _lstrip1(src);
            }
            else if(*src != '\0')  // wrong json format
            {
                _cur_freekeyS(hk);

                return 0;
            }
        }
    }

    switch (_r_typeo(r))
    {
        case EOBJ:  if(_key_is_valid(key))         // input key is valid, using it
                    {
                        is0_exe(_obj_getSL(r, key, &l), goto err_ret;);

                        if(*src)  n = __parse_obj(0, &src, &err, _opt);
                        else      _n_newS(n, hk);       // here use hk to create ESTR obj

                        is0_exe(n, goto err_ret;);

                        _n_setKeyS(n, _cur_dupkeyS(key));

                        is1_exe(hk, _cur_freekeyS(hk));
                    }
                    else
                    {
                        is1_exe(!*src || _key_is_invalid(hk) || !_obj_getSL(r, hk, &l) || !(n = __parse_obj(0, &src, &err, _opt)), goto err_ret;);

                        _n_setKeyS(n, hk);
                    }

                    _obj_link(r, n, &l);

                    return _n_o(n);

        case EARR:  if(*src)    // == ':'
                    {
                        is0_exe(n = __parse_obj(0, &src, &err, _opt), goto err_ret;);
                    }
                    else        // == '\0'
                    {
                        _n_newS(n, hk);
                        is0_exe(n, goto err_ret;);
                    }

                    _n_setKeyS(n, hk);

                    _arr_appd(r, n);

                    return _n_o(n);

        default   : eerrset(_ERRSTR_TYPEDF); goto err_ret;
    }

err_ret:
    if(hk)
        _cur_freekeyS(hk);

    return 0;
}

static ejson __ejson_addO(_ejsr r, constr key, eobj   o   )
{
    cstr o_k; dictLink_t l;

    is1_ret(_eo_linked(o) || un_eq(_eo_typec(o), EJSON), 0);

    switch (_r_typeo(r))
    {
        case EOBJ:  o_k = _eo_keyS(o);
                    if(_key_is_valid(key))
                    {
                        is0_ret(_obj_getSL(r, key, &l), 0);

                        if(o_k) _cur_freekeyS(o_k);

                        _eo_keyS(o) = _cur_dupkeyS(key);
                    }
                    else if(_key_is_valid(o_k))
                    {
                        is0_ret(_obj_getSL(r, o_k, &l), 0);
                    }
                    else
                        return 0;

                    _obj_link(r, _eo_dn(o), &l);

                    return o;

        case EARR:  _arr_appd(r, _eo_dn(o));

                    return o;

        default  :  eerrset(_ERRSTR_TYPEDF); return 0;
    }

    return 0;
}

/** -----------------------------------------------------
 *
 *  ejson val
 *
 *  -----------------------------------------------------
 */

bool   ejson_isEmpty(eobj o) { return _ec_isEmpty(o); }

//! rawkey
ejson  ejson_r      (ejson r, constr rawk) { return _getObjByRawk(_eo_rn(r), rawk);}
i64    ejson_rValI  (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); _eo_retI(r); }
f64    ejson_rValF  (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); _eo_retF(r); }
constr ejson_rValS  (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); _eo_retS(r); }
cptr   ejson_rValP  (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); _eo_retP(r); }
cptr   ejson_rValR  (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); _eo_retR(r); }
etypeo ejson_rType  (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); _eo_retT(r); }
constr ejson_rTypeS (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); return eobj_typeS((eobj)r); }
uint   ejson_rLen   (ejson r, constr rawk) { r = _getObjByRawk(_eo_rn(r), rawk); _eo_retL(r); }
bool   ejson_rIsTrue(ejson r, constr rawk) { return __eobj_isTrue(_getObjByRawk(_eo_rn(r), rawk));}

//! idx
ejson  ejson_i      (ejson r, uint idx) { if(r && _eo_typeco(r) ==_EJSON_CO_ARR ) { _ejsn n = _arr_find(_eo_rn(r), idx); return n ? _n_o(n) : 0; }  return 0;}
i64    ejson_iValI  (ejson r, uint idx) { r = ejson_i(r, idx); _eo_retI(r); }
f64    ejson_iValF  (ejson r, uint idx) { r = ejson_i(r, idx); _eo_retF(r); }
constr ejson_iValS  (ejson r, uint idx) { r = ejson_i(r, idx); _eo_retS(r); }
cptr   ejson_iValP  (ejson r, uint idx) { r = ejson_i(r, idx); _eo_retP(r); }
cptr   ejson_iValR  (ejson r, uint idx) { r = ejson_i(r, idx); _eo_retR(r); }
etypeo ejson_iType  (ejson r, uint idx) { r = ejson_i(r, idx); _eo_retT(r); }
constr ejson_iTypeS (ejson r, uint idx) { r = ejson_i(r, idx); return eobj_typeS(r); }
uint   ejson_iLen   (ejson r, uint idx) { r = ejson_i(r, idx); _eo_retL(r); }
bool   ejson_iIsTrue(ejson r, uint idx) { return __eobj_isTrue(ejson_i(r, idx)); }

//! path
ejson  ejson_p      (ejson r, constr path) { return _getObjByPath(_eo_rn(r), path);}
i64    ejson_pValI  (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); _eo_retI(r); }
f64    ejson_pValF  (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); _eo_retF(r); }
constr ejson_pValS  (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); _eo_retS(r); }
cptr   ejson_pValP  (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); _eo_retP(r); }
cptr   ejson_pValR  (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); _eo_retR(r); }
etypeo ejson_pType  (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); _eo_retT(r); }
constr ejson_pTypeS (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); return eobj_typeS(r); }
uint   ejson_pLen   (ejson r, constr path) { r = _getObjByPath(_eo_rn(r), path); _eo_retL(r); }
bool   ejson_pIsTrue(ejson r, constr path) { return __eobj_isTrue(_getObjByPath(_eo_rn(r), path));}



/** -----------------------------------------------------
 *
 *  ejson format
 *
 *  -----------------------------------------------------
 */

static void __wrap_str (cstr src, int _len, estr* s);

#define     __wrap_KEY(      n,       s)   __wrap_str(_n_keyS(n), _cur_lenkeyS(_n_keyS(n)), s)
#define     __wrap_STR(      n,       s)   __wrap_str(_n_valS(n), _n_len      (        n ), s)
static void __wrap_NUM(_ejsn n, estr* s);
static void __wrap_OBJ(_ejsn n, estr* s, int depth);
static void __wrap_ARR(_ejsn n, estr* s, int depth);

static estr __wrap_ejsn(_ejsn n, estr* s, int depth);

estr ejson_rToS(ejson o, constr rawk, estr* out, eopts opts) { return ejson_toS(_getObjByRawk(_eo_rn(o), rawk), out, opts); }
estr ejson_iToS(ejson o, u32    idx , estr* out, eopts opts) { return ejson_toS(_getObjByIdx (_eo_rn(o), idx ), out, opts); }
estr ejson_pToS(ejson o, constr path, estr* out, eopts opts) { return ejson_toS(_getObjByPath(_eo_rn(o), path), out, opts); }
estr ejson_toS (ejson o,              estr* out, eopts opts)
{
    estr buf;

    is0_ret(o, 0);

    if(out)
    {
        estr_clear(*out);
    }
    else
    {
        buf = 0;
        out = &buf;
    }

    return  __wrap_ejsn(_eo_dn(o), out, opts & PRETTY ? 0 : -1);
}



static estr __wrap_ejsn(_ejsn n, estr* s, int depth)
{
    switch (_n_typeo(n))
    {
        case ENULL  :  estr_catB(*s, "null" , 4); break;
        case EFALSE :  estr_catB(*s, "false", 5); break;
        case ETRUE  :  estr_catB(*s, "true" , 4); break;
        case ENUM   :  __wrap_NUM(n, s);     break;
        case ESTR   :  __wrap_STR(n, s);     break;
        case EPTR   :  estr_catP(*s, "\"(PTR@%p)\"", _n_valP(n)); break;
        case ERAW   :  estr_catP(*s, "\"(RAW %d)\"", _n_len (n)); break;
        case EARR   :  __wrap_ARR(n, s, depth);  break;
        case EOBJ   :  __wrap_OBJ(n, s, depth);  break;

        default     : eerrlog("invalid type in __wrap_exec()"); abort();
    }

    return *s;
}

static void __wrap_NUM(_ejsn n, estr* s)
{
    if(!_n_typee(n))
    {
        if ( is_eq(_n_valI(n), 0) )
            estr_catB(*s, "0", 1);              // special case for 0.
        else
            estr_catF(*s, "%I", _n_valI(n));    // i64
    }
    else                                        // f64
    {
        f64 d = _n_valF(n);

        if      (d * 0 != 0)									    estr_catB(*s, "\"[nan]\"", 7);  /* This checks for NaN and Infinity */
        else if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60)	estr_catP(*s, "%.0f",d);
        else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)					estr_catP(*s, "%e",d);
        else                                                     	estr_catP(*s, "%f",d);
    }
}

static void __wrap_str(cstr src, int _len, estr* s)
{
    constr ptr; bool flag = 0;  unsigned char u; char c;

    estr_catB(*s, "\"", 1);

    for (ptr=src; (c = *ptr);ptr++) if((c>0 && c<32)||(c=='\"')||(c=='\\')) {flag=1;break;}
    if (!flag)
    {
        estr_catB(*s, src, _len);
        estr_catB(*s, "\"", 1);

        return ;
    }

    ptr = src;
    while (*ptr)
    {
        if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') estr_catB(*s, ptr++, 1);

        else
        {
            estr_catB(*s, "\\", 1);
            switch(u = *ptr++)
            {
                case '\\':	estr_catB(*s, "\\"   , 1);	break;
                case '\"':	estr_catB(*s, "\""   , 1);	break;
                case '\b':	estr_catB(*s, "b"    , 1);	break;
                case '\f':	estr_catB(*s, "f"    , 1);	break;
                case '\n':	estr_catB(*s, "n"    , 1);	break;
                case '\r':	estr_catB(*s, "r"    , 1);	break;
                case '\t':	estr_catB(*s, "t"    , 1);	break;
                default  :  estr_catP(*s, "u%04x", u);  break;	/* escape and print */
            }
        }
    }
    estr_catB(*s, "\"", 1);
}

static void __wrap_OBJ(_ejsn n, estr* s ,int depth)
{
    // -- Explicitly handle empty object case
    if (!_n_len(n))
    {
        estr_catB(*s, "{}", 2);
        return ;
    }

    // -- Compose the output
    n   = _r_head((_ejsr)n);
    if(depth >= 0)
    {
        depth++;

        estr_catB(*s, "{\n", 2);

        while(n)
        {
            estr_catC(*s, '\t', depth);

            __wrap_KEY(n, s);
            estr_catB(*s, ": ", 2);

            __wrap_ejsn(n, s, depth);
            estr_catB(*s, ",\n", 2);

            n = _n_lnext(n);
        }
        estr_decrLen(*s,    2);
        estr_catB(*s, "\n", 1);
        estr_catC(*s, '\t', depth - 1);
        estr_catB(*s, "}" , 1);
    }
    else
    {
        estr_catB(*s, "{\n", 1);

        while(n)
        {
            __wrap_KEY(n, s);
            estr_catB(*s, ": ", 1);

            __wrap_ejsn(n, s, depth);
            estr_catB(*s, ",\n", 1);

            n = _n_lnext(n);
        }
        estr_decrLen(*s,   1);
        estr_catB(*s, "}", 1);
    }
}

static void __wrap_ARR(_ejsn n, estr* s, int depth)
{
    // -- Explicitly handle numentries == 0
    if (!_n_len(n))
    {
        estr_catB(*s, "[]", 2);
        return ;
    }

    // -- Compose the output array.
    {
        u8 gap;

        gap = depth >= 0 ? 2 : 1;
        n   = _r_head((_ejsr)n);

        estr_catB(*s, "[", 1);
        while(n)
        {
            __wrap_ejsn(n, s, depth);
            estr_catB(*s, ", ", gap);

            n = _n_lnext(n);
        }
        estr_decrLen(*s, gap);
        estr_catB(*s, "]", 1);
    }
}

/** -----------------------------------------------------
 *
 *  ejson comparing
 *
 */

int ejson_rCmpI(ejson r, constr rawk, i64    val) { return ejson_cmpI(_getObjByRawk(_eo_rn(r), rawk), val);}
int ejson_rCmpF(ejson r, constr rawk, f64    val) { return ejson_cmpF(_getObjByRawk(_eo_rn(r), rawk), val);}
int ejson_rCmpS(ejson r, constr rawk, constr str) { return ejson_cmpS(_getObjByRawk(_eo_rn(r), rawk), str);}

int ejson_iCmpI(ejson r, u32    idx , i64    val) { return ejson_cmpI(_getObjByIdx (_eo_rn(r), idx), val);}
int ejson_iCmpF(ejson r, u32    idx , f64    val) { return ejson_cmpF(_getObjByIdx (_eo_rn(r), idx), val);}
int ejson_iCmpS(ejson r, u32    idx , constr str) { return ejson_cmpS(_getObjByIdx (_eo_rn(r), idx), str);}

int ejson_pCmpI(ejson r, constr path, i64    val) { return ejson_cmpI(_getObjByPath(_eo_rn(r), path), val);}
int ejson_pCmpF(ejson r, constr path, f64    val) { return ejson_cmpF(_getObjByPath(_eo_rn(r), path), val);}
int ejson_pCmpS(ejson r, constr path, constr str) { return ejson_cmpS(_getObjByPath(_eo_rn(r), path), str);}

/** -----------------------------------------------------
 *
 *  ejson iterating
 *
 */

#define _o_is_parent(r) (r && (_eo_typeco(r) == _EJSON_CO_OBJ || _eo_typeco(r) == _EJSON_CO_ARR))

ejson  ejson_first(ejson r) { return (_o_is_parent(r) && _r_head(_eo_rn(r))) ? _n_o(_r_head(_eo_rn(r))) : 0; }
ejson  ejson_last (ejson r) { return (_o_is_parent(r) && _r_tail(_eo_rn(r))) ? _n_o(_r_tail(_eo_rn(r))) : 0; }
ejson  ejson_next (ejson o) { return (o && _n_lnext(_eo_dn(o))) ? _n_o(_n_lnext(_eo_dn(o))) : 0; }
ejson  ejson_prev (ejson o) { return (o && _n_lprev(_eo_dn(o))) ? _n_o(_n_lprev(_eo_dn(o))) : 0; }

ejson  ejson_rFirst(ejson r, constr rawk) { return ejson_first(_getObjByRawk(_eo_rn(r), rawk)); }
ejson  ejson_rLast (ejson r, constr rawk) { return ejson_last (_getObjByRawk(_eo_rn(r), rawk)); }

ejson  ejson_pFirst(ejson r, constr path) { return ejson_first(_getObjByPath(_eo_rn(r), path)); }
ejson  ejson_pLast (ejson r, constr path) { return ejson_last (_getObjByPath(_eo_rn(r), path)); }

/** -----------------------------------------------------
 *
 *  ejson take and free
 *
 *  -----------------------------------------------------
 */
ejson ejson_takeH(ejson r)              { if(r){ switch(_eo_typeco(r)) { case _EJSON_CO_OBJ: return _obj_popH (_eo_rn(r)           ); case _EJSON_CO_ARR: return _arr_popH(_eo_rn(r)            ); }} return 0;}
ejson ejson_takeT(ejson r)              { if(r){ switch(_eo_typeco(r)) { case _EJSON_CO_OBJ: return _obj_popT (_eo_rn(r)           ); case _EJSON_CO_ARR: return _arr_popT(_eo_rn(r)            ); }} return 0;}
ejson ejson_takeO(ejson r, ejson     o) { if(r){ switch(_eo_typeco(r)) { case _EJSON_CO_OBJ: return _obj_takeN(_eo_rn(r), _eo_dn(o)); case _EJSON_CO_ARR: return _arr_takeN(_eo_rn(r), _eo_dn(o)); }} return 0;}
ejson ejson_takeP(ejson r, constr path) { return _rmObjByPath(_eo_rn(r), path); }
ejson ejson_takeR(ejson r, constr rawk) { return _rmObjByRawk(_eo_rn(r), rawk); }
ejson ejson_takeI(ejson r, int     idx) { return (r &&  _eo_typeco(r) == _EJSON_CO_ARR) ? _arr_takeI(_eo_rn(r), idx) : 0; }

int  ejson_freeH(ejson r)              { return ejson_free(ejson_takeH(r     )); }
int  ejson_freeT(ejson r)              { return ejson_free(ejson_takeH(r     )); }
int  ejson_freeO(ejson r, ejson     o) { return ejson_free(ejson_takeO(r,   o)); }
int  ejson_freeP(ejson r, constr path) { return ejson_free(_rmObjByPath(_eo_rn(r), path)); }
int  ejson_freeR(ejson r, constr rawk) { return ejson_free(_rmObjByRawk(_eo_rn(r), rawk)); }
int  ejson_freeI(ejson r, int     idx) { return ejson_free(ejson_takeI(r, idx)); }

int  ejson_freeHEx(ejson r,              eobj_rls_ex_cb rls, eval prvt) { return ejson_freeEx(ejson_takeH(r     ), rls, prvt); }
int  ejson_freeTEx(ejson r,              eobj_rls_ex_cb rls, eval prvt) { return ejson_freeEx(ejson_takeT(r     ), rls, prvt); }
int  ejson_freeOEx(ejson r, ejson     o, eobj_rls_ex_cb rls, eval prvt) { return ejson_freeEx(ejson_takeO(r,   o), rls, prvt); }
int  ejson_freePEx(ejson r, constr path, eobj_rls_ex_cb rls, eval prvt) { return ejson_freeEx(_rmObjByPath(_eo_rn(r), path), rls, prvt);}
int  ejson_freeREx(ejson r, constr rawk, eobj_rls_ex_cb rls, eval prvt) { return ejson_freeEx(_rmObjByRawk(_eo_rn(r), rawk), rls, prvt);}
int  ejson_freeIEx(ejson r, int     idx, eobj_rls_ex_cb rls, eval prvt) { return ejson_freeEx(ejson_takeI(r, idx), rls, prvt); }

/** -----------------------------------------------------
 *
 *  ejson set
 *
 *  -----------------------------------------------------
 */

static __always_inline void __ejson_free_obj(_ejsr r);
static __always_inline void __ejson_free_arr(_ejsr r);

static ejson __ejson_makeRoom_set_k(_ejsr r, eobj in);
static ejson __ejson_makeRoom_set_i(_ejsr r, eobj in);
static ejson __ejson_makeRoom_set_p(_ejsr r, eobj in);

#define _INITED     1
#undef  _eo_setT
#define _eo_setT(o, t, inited)                                          \
if(!inited){                                                            \
    if     (t == EOBJ){ _obj_bzero(_eo_rn(o)); _obj_init(_eo_rn(o));}   \
    else if(t == EARR){ _arr_bzero(_eo_rn(o));                      }   \
}

ejson ejson_rSetT(ejson r, constr rawk, etypeo type) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len = _t_olen(type), ._typ = {.__1 = {EJSON, type, 0, 0}}}, {0}}; o = __ejson_makeRoom_set_k(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setT (o, type, b.obj.r[0]);    _eo_typeco (o) = _n_typeco(&b)   ; } return o; }
ejson ejson_rSetI(ejson r, constr rawk, i64    val ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_I }}, {0}}; o = __ejson_makeRoom_set_k(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setI (o, val   );              _eo_typecoe(o) = _EJSON_COE_NUM_I; } return o; }
ejson ejson_rSetF(ejson r, constr rawk, f64    val ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_F }}, {0}}; o = __ejson_makeRoom_set_k(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setF (o, val   );              _eo_typecoe(o) = _EJSON_COE_NUM_F; } return o; }
ejson ejson_rSetS(ejson r, constr rawk, constr str ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =  _s_olen(str), ._typ = {.t_coe = _EJSON_COE_STR   }}, {0}}; o = __ejson_makeRoom_set_k(_eo_rn(r), _n_o(&b)); if(o) { _n_len(&b)--; _eo_setS (o, str, _n_len(&b));     _eo_typeco (o) = _EJSON_CO_STR   ; } return o; }
ejson ejson_rSetP(ejson r, constr rawk, constr ptr ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_PTR   }}, {0}}; o = __ejson_makeRoom_set_k(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setP (o, ptr   );              _eo_typeco (o) = _EJSON_CO_PTR   ; } return o; }
ejson ejson_rSetR(ejson r, constr rawk, uint   len ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =       len + 1, ._typ = {.t_coe = _EJSON_COE_RAW   }}, {0}}; o = __ejson_makeRoom_set_k(_eo_rn(r), _n_o(&b)); if(o) {               _eo_wipeR(o, len   );              _eo_typeco (o) = _EJSON_CO_RAW   ; } return o; }

ejson ejson_iSetT(ejson r, u32    idx , etypeo type) { ejson o; _ejsn_t b = {{0}, {.i =        idx}, {._len = _t_olen(type), ._typ = {.__1 = {EJSON, type, 0, 0}}}, {0}}; o = __ejson_makeRoom_set_i(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setT (o, type, b.obj.r[0]);    _eo_typeco (o) = _n_typeco(&b)   ; } return o; }
ejson ejson_iSetI(ejson r, u32    idx , i64    val ) { ejson o; _ejsn_t b = {{0}, {.i =        idx}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_I }}, {0}}; o = __ejson_makeRoom_set_i(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setI (o, val   );              _eo_typecoe(o) = _EJSON_COE_NUM_I; } return o; }
ejson ejson_iSetF(ejson r, u32    idx , f64    val ) { ejson o; _ejsn_t b = {{0}, {.i =        idx}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_F }}, {0}}; o = __ejson_makeRoom_set_i(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setF (o, val   );              _eo_typecoe(o) = _EJSON_COE_NUM_F; } return o; }
ejson ejson_iSetS(ejson r, u32    idx , constr str ) { ejson o; _ejsn_t b = {{0}, {.i =        idx}, {._len =  _s_olen(str), ._typ = {.t_coe = _EJSON_COE_STR   }}, {0}}; o = __ejson_makeRoom_set_i(_eo_rn(r), _n_o(&b)); if(o) { _n_len(&b)--; _eo_setS (o, str, _n_len(&b));     _eo_typecoe(o) = _EJSON_COE_STR  ; } return o; }
ejson ejson_iSetP(ejson r, u32    idx , constr ptr ) { ejson o; _ejsn_t b = {{0}, {.i =        idx}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_PTR   }}, {0}}; o = __ejson_makeRoom_set_i(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setP (o, ptr   );              _eo_typecoe(o) = _EJSON_COE_PTR  ; } return o; }
ejson ejson_iSetR(ejson r, u32    idx , uint   len ) { ejson o; _ejsn_t b = {{0}, {.i =        idx}, {._len =       len + 1, ._typ = {.t_coe = _EJSON_COE_RAW   }}, {0}}; o = __ejson_makeRoom_set_i(_eo_rn(r), _n_o(&b)); if(o) {               _eo_wipeR(o, len   );              _eo_typecoe(o) = _EJSON_COE_RAW  ; } return o; }

ejson ejson_pSetT(ejson r, constr rawk, etypeo type) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len = _t_olen(type), ._typ = {.__1 = {EJSON, type, 0, 0}}}, {0}}; o = __ejson_makeRoom_set_p(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setT (o, type, b.obj.r[0]);    _eo_typeco (o) = _n_typeco(&b)   ; } return o; }
ejson ejson_pSetI(ejson r, constr rawk, i64    val ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_I }}, {0}}; o = __ejson_makeRoom_set_p(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setI (o, val   );              _eo_typecoe(o) = _EJSON_COE_NUM_I; } return o; }
ejson ejson_pSetF(ejson r, constr rawk, f64    val ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_NUM_F }}, {0}}; o = __ejson_makeRoom_set_p(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setF (o, val   );              _eo_typecoe(o) = _EJSON_COE_NUM_F; } return o; }
ejson ejson_pSetS(ejson r, constr rawk, constr str ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =  _s_olen(str), ._typ = {.t_coe = _EJSON_COE_STR   }}, {0}}; o = __ejson_makeRoom_set_p(_eo_rn(r), _n_o(&b)); if(o) { _n_len(&b)--; _eo_setS (o, str, _n_len(&b));     _eo_typeco (o) = _EJSON_CO_STR   ; } return o; }
ejson ejson_pSetP(ejson r, constr rawk, constr ptr ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =             8, ._typ = {.t_coe = _EJSON_COE_PTR   }}, {0}}; o = __ejson_makeRoom_set_p(_eo_rn(r), _n_o(&b)); if(o) {               _eo_setP (o, ptr   );              _eo_typeco (o) = _EJSON_CO_PTR   ; } return o; }
ejson ejson_pSetR(ejson r, constr rawk, uint   len ) { ejson o; _ejsn_t b = {{0}, {.s = (cstr)rawk}, {._len =       len + 1, ._typ = {.t_coe = _EJSON_COE_RAW   }}, {0}}; o = __ejson_makeRoom_set_p(_eo_rn(r), _n_o(&b)); if(o) {               _eo_wipeR(o, len   );              _eo_typeco (o) = _EJSON_CO_RAW   ; } return o; }

#define _get_curlen(n, cur_len)                                         \
do{                                                                     \
    switch(_n_typeo(n))                                                 \
    {                                                                   \
        case EFALSE :                                                   \
        case ETRUE  :                                                   \
        case ENULL  : cur_len = _n_len(n); break;                       \
        case ENUM   : cur_len = 8; break;                               \
        case EPTR   : cur_len = 8; break;                               \
        case ESTR   : cur_len = _n_len(n) + 1; break;                   \
        case ERAW   : cur_len = _n_len(n) + 1; break;                   \
        case EOBJ   : __ejson_free_obj((_ejsr)n);                       \
                      cur_len = _R_OLEN; break;                         \
        case EARR   : __ejson_free_arr((_ejsr)n);                       \
                      cur_len = _R_OLEN; break;                         \
                                                                        \
        default     : return 0;                                         \
    }                                                                   \
}while(0)

#define _obj_update_node(r, o, n, l)                                    \
if(o != n)                                                              \
{                                                                       \
    if(_n_lprev(n)){ _n_lnext(_n_lprev(n)) = n; } else _r_head(r) = n;  \
    if(_n_lnext(n)){ _n_lprev(_n_lnext(n)) = n; } else _r_tail(r) = n;  \
                                                                        \
    o = n;                                                              \
                                                                        \
    *l._prev = n;                                                       \
}

#define _arr_update_node(r, _o, n)                                      \
if(_o != n)                                                             \
{                                                                       \
    if(_n_lprev(n)){ _n_lnext(_n_lprev(n)) = n; } else _r_head(r) = n;  \
    if(_n_lnext(n)){ _n_lprev(_n_lnext(n)) = n; } else _r_tail(r) = n;  \
                                                                        \
    if(_arr_hd(r)->o == _o) _arr_hd(r)->o = n;                          \
                                                                        \
    _o = n;                                                             \
}

static ejson __ejson_makeRoom_set_k(_ejsr r, eobj in)
{
    dictLink_t l; _ejsn n; int ret;

    is1_ret(!_r_o(r) || !_key_is_valid(_eo_keyS(in)), 0);

    switch (_r_typeo(r))
    {
        case EOBJ:  if( !(ret = _obj_getSL_ex(r, _eo_keyS(in), &l)) )     //! already exist
                    {
                        uint cur_len;

                        n = *l._prev;

                        if(_eo_typeo(in) == _n_typeo(n))
                        {
                            in->r[0] = _INITED;                 // used by ejson_setT, setted so it will not be reinited later
                            return _n_o(n);
                        }

                        _get_curlen(n, cur_len);

                        if(cur_len < _eo_len(in))
                        {
                            _ejsn newn = _n_newr(n, _eo_len(in));

                            _obj_update_node(r, n, newn, l);
                        }
                    }
                    else if(ret == 1)
                    {
                        n = _n_newm(_eo_len(in)); _n_init(n);

                        _n_setKeyS(n, _cur_dupkeyS(_eo_keyS(in)));

                        _obj_link(r, n, &l);
                    }
                    else
                        return 0;

                    return _n_o(n);

        case EARR:  {
                        cstr endp; int id;

                        is0_ret(*_eo_keyS(in), 0);

                        id = strtol(_eo_keyS(in), &endp, 10);

                        is1_ret(*endp, 0);

                        //! already exist
                        if((n = _arr_find(r, id)))
                        {
                            uint cur_len;

                            if(_eo_typeo(in) == _n_typeo(n))
                            {
                                in->r[0] = _INITED;                 // used by ejson_setT, setted so it will not be reinited later
                                return _n_o(n);
                            }

                            _get_curlen(n, cur_len);

                            if(cur_len < _eo_len(in))
                            {
                                _ejsn newn = _n_newr(n, _eo_len(in));

                                _arr_update_node(r, n, newn);
                            }

                            return _n_o(n);
                        }
                        else
                        {
                            //! for arr obj, we do not create new obj

                            return 0;
                        }
                    }

                    break;
    }

    return 0;
}

static ejson __ejson_makeRoom_set_i(_ejsr r, eobj in)
{
    _ejsn n;

    is1_ret(!_r_o(r) || _r_typeco(r) != _EJSON_CO_ARR, 0);

    if((n = _arr_find(r, (uint)_eo_keyI(in))))
    {
        uint cur_len;

        if(_eo_typeo(in) == _n_typeo(n))
        {
            in->r[0] = _INITED;                 // used by ejson_setT, setted so it will not be reinited later
            return _n_o(n);
        }

        _get_curlen(n, cur_len);

        if(cur_len < _eo_len(in))
        {
            _ejsn newn = _n_newr(n, _eo_len(in));

            _arr_update_node(r, n, newn);
        }

        return _n_o(n);
    }

    return 0;
}

static ejson __ejson_makeRoom_set_p(_ejsr r, eobj in)
{
    constr key; constr p; int klen; int id; dictLink_t l;

    _ejsn n;

    is1_ret(!_r_o(r) || _r_typec(r) != EJSON || !_key_is_valid(_eo_keyS(in)), 0);

    p = _eo_keyS(in);

    n = (_ejsn)r;

    do{
        klen = __getAKey(p, &key, &p);

        is1_ret(klen < 0, 0);

        r = (_ejsr)n;

        if(!*p)
        {
            switch (_r_typeo(r))
            {
                case EOBJ:  if(!_obj_getBL_ex(r, key, klen, &l))     //! already exist
                            {
                                uint cur_len;

                                n = *l._prev;

                                if(_eo_typeo(in) == _n_typeo(n))
                                {
                                    in->r[0] = _INITED;                 // used by ejson_setT, setted so it will not be reinited later
                                    return _n_o(n);
                                }

                                _get_curlen(n, cur_len);

                                if(cur_len < _eo_len(in))
                                {
                                    _ejsn newn = _n_newr(n, _eo_len(in));

                                    _obj_update_node(r, n, newn, l);
                                }
                            }
                            else
                            {
                                n = _n_newm(_eo_len(in)); _n_init(n);

                                _n_setKeyS(n, _cur_newkeyS(klen)); memcpy(_n_keyS(n), key, klen); _n_keyS(n)[klen] = '\0';

                                _obj_link(r, n, &l);
                            }

                            return _n_o(n);

                case EARR:  {
                                cstr endp; int id;

                                is0_ret(*key, 0);

                                id = strtol(key, &endp, 10);

                                if(*endp && *endp != ']')
                                    return 0;

                                //! already exist
                                if((n = _arr_find(r, id)))
                                {
                                    uint cur_len;

                                    if(_eo_typeo(in) == _n_typeo(n))
                                    {
                                        in->r[0] = _INITED;                 // used by ejson_setT, setted so it will not be reinited later
                                        return _n_o(n);
                                    }

                                    _get_curlen(n, cur_len);

                                    if(cur_len < _eo_len(in))
                                    {
                                        _ejsn newn = _n_newr(n, _eo_len(in));

                                        _arr_update_node(r, n, newn);
                                    }

                                    return _n_o(n);
                                }
                                else
                                {
                                    //! for arr obj, we do not create new obj

                                    return 0;
                                }
                            }

                            break;
            }

            return 0;
        }

        switch (_r_typeo(r))
        {
            case EOBJ:  if(!_obj_getBL_ex(r, key, klen, &l)) //! already exist
                        {
                            if(_n_typeo(*l._prev) != EOBJ)
                            {
                                return 0;
                            }

                            n = *l._prev;
                        }
                        else
                        {
                            _n_newO(n); _obj_init(n);

                            _n_setKeyS(n, _cur_newkeyS(klen)); memcpy(_n_keyS(n), key, klen); _n_keyS(n)[klen] = '\0';

                            _obj_link(r, n, &l);
                        }

                        break;

            case EARR:  {
                            cstr endp;

                            is1_ret(klen == 0, 0);

                            id = strtol(key, &endp, 10);

                            //! must parse over then is a valid number
                            if(*endp && *endp != ']' && *endp != '.')
                                return 0;

                            n = _arr_find(r, id);
                        }

                        if(!n || _n_typeo(n) != EOBJ)
                            return 0;

                        break;

            default  :  return 0;
        }

    }while(*p);

    return 0;
}

static __always_inline void __ejson_free_obj(_ejsr r)
{
    if(_r_head(r)){ __ejson_free((_ejsr)_r_head(r));}
    _obj_free(r);

    _r_len (r) = 0;
    _r_head(r) = _r_tail(r) = 0;
}

static __always_inline void __ejson_free_arr(_ejsr r)
{
    if(_arr_hd(r))
    {
        if(_r_head(r)){ __ejson_free((_ejsr)_r_head(r));}
        _arr_free(r);

        _r_len (r) = 0;
        _r_head(r) = _r_tail(r) = 0;
    }
}

/** -----------------------------------------------------
 *
 *  ejson sub str
 *
 *  -----------------------------------------------------
 */

static eobj  __ejson_new_obj_s(eobj o, int newolen)
{
    _ejsn n;

    _n_newSm(n, newolen);

    _n_l(n)    = _eo_l(o);
    _n_h(n)    = _eo_h(o);

    _n_setKeyS(n, _eo_keyS(o));

    return _n_o(n);
}

static ejson __ejson_subS_r(_ejsr r, constr rawk, constr from, constr to)
{
    _ejsn n; dictLink_t l; eobj new_o;

    switch (_r_typeo(r))
    {
        case EOBJ:  if(!_obj_getSL_ex(r, rawk, &l))     //! found
                    {
                        n = *l._prev;

                        is1_ret(_n_typeo(n) != ESTR, 0);

                        new_o = __eobj_subS(_n_o(n), from, to, __ejson_new_obj_s);

                        if(new_o)
                        {
                            if(new_o != _n_o(n))
                            {
                                _ejsn new_n = _eo_dn(new_o);

                                _n_free(n);
                                _obj_update_node(r, n, new_n, l);
                            }

                            return _n_o(n);
                        }
                    }

                    return 0;

        case EARR:  {
                        cstr endp; int id;

                        id = strtol(rawk, &endp, 10);

                        is1_ret(*endp, NULL);

                        if((n = _arr_find(r, id)))
                        {
                            is1_ret(_n_typeo(n) != ESTR, 0);

                            new_o = __eobj_subS(_n_o(n), from, to, __ejson_new_obj_s);

                            if(new_o)
                            {
                                if(new_o != _n_o(n))
                                {
                                    _ejsn new_n = _eo_dn(new_o);

                                    _n_free(n);
                                    _arr_update_node(r, n, new_n);
                                }

                                return _n_o(n);
                            }
                        }
                    }

                    return 0;
    }

    return 0;
}

ejson ejson_rReplaceS(ejson r, constr rawk, constr from, constr to)
{
    is1_ret(!r || _eo_typec(r) != EJSON || !_key_is_valid(rawk), 0);

    return __ejson_subS_r(_eo_rn(r), rawk, from, to);
}

ejson ejson_pReplaceS(ejson _r, constr path, constr from, constr to)
{
    constr key; constr p; int klen; int id; dictLink_t l; eobj new_o;

    _ejsn n; _ejsr r;

    is1_ret(!_r || _eo_typec(_r) != EJSON || !_key_is_valid(path), 0);

    p = path;

    r = _eo_rn(_r);
    n = (_ejsn)r;

    do{
        klen = __getAKey(p, &key, &p);

        is1_ret(klen < 0, 0);

        r = (_ejsr)n;

        switch (_r_typeo(r))
        {
            case EOBJ:  if(!_obj_getBL_ex(r, key, klen, &l))    //! found
                        {
                            n = *l._prev;

                            if(!*p)
                            {
                                is1_ret(_n_typeo(n) != ESTR, 0);

                                new_o = __eobj_subS(_n_o(n), from, to, __ejson_new_obj_s);

                                if(new_o)
                                {
                                    if(new_o != _n_o(n))
                                    {
                                        _ejsn new_n = _eo_dn(new_o);

                                        _n_free(n);
                                        _obj_update_node(r, n, new_n, l);
                                    }

                                    return _n_o(n);
                                }

                                return 0;
                            }

                            continue;
                        }
                        return 0;

            case EARR:  {
                            cstr endp;

                            is1_ret(klen == 0, 0);

                            id = strtol(key, &endp, 10);

                            //! must parse over then is a valid number
                            if(*endp && *endp != ']' && *endp != '.')
                                return 0;

                            n = _arr_find(r, id);

                            is1_ret(!n, 0);

                            if(!*p)
                            {
                                is1_ret(_n_typeo(n) != ESTR, 0);

                                new_o = __eobj_subS(_n_o(n), from, to, __ejson_new_obj_s);

                                if(new_o)
                                {
                                    if(new_o != _n_o(n))
                                    {
                                        _ejsn new_n = _eo_dn(new_o);

                                        _n_free(n);
                                        _arr_update_node(r, n, new_n);
                                    }

                                    return _n_o(n);
                                }

                                return 0;
                            }
                        }

                        break;

            default  :  return 0;
        }

    }while(*p);

    return 0;
}

/** -----------------------------------------------------
 *
 *  ejson counter
 *
 *  -----------------------------------------------------
 */

#define _ERR_COUNTOR    LLONG_MIN

static i64 __ejson_makeRoom_counter_r(_ejsr r, constr rawk, i64 val);
static i64 __ejson_makeRoom_counter_i(_ejsr r, u32    idx , i64 val);
static i64 __ejson_makeRoom_counter_p(_ejsr r, constr path, i64 val);

i64  ejson_pp  (ejson o)        { if(o) { switch (_eo_typecoe(o)) { case _EJSON_COE_NUM_I: return _eo_valI(o) += 1;  case _EJSON_COE_NUM_F: return (i64)(_eo_valF(o) += 1); }} return _ERR_COUNTOR; }
i64  ejson_mm  (ejson o)        { if(o) { switch (_eo_typecoe(o)) { case _EJSON_COE_NUM_I: return _eo_valI(o) -= 1;  case _EJSON_COE_NUM_F: return (i64)(_eo_valF(o) -= 1); }} return _ERR_COUNTOR; }
i64  ejson_incr(ejson o, i64 v) { if(o) { switch (_eo_typecoe(o)) { case _EJSON_COE_NUM_I: return _eo_valI(o) += v;  case _EJSON_COE_NUM_F: return (i64)(_eo_valF(o) += v); }} return _ERR_COUNTOR; }
i64  ejson_decr(ejson o, i64 v) { if(o) { switch (_eo_typecoe(o)) { case _EJSON_COE_NUM_I: return _eo_valI(o) -= v;  case _EJSON_COE_NUM_F: return (i64)(_eo_valF(o) -= v); }} return _ERR_COUNTOR; }

i64  ejson_kpp  (ejson r, constr rawk)       { return __ejson_makeRoom_counter_r(_eo_rn(r), rawk,  1); }
i64  ejson_kmm  (ejson r, constr rawk)       { return __ejson_makeRoom_counter_r(_eo_rn(r), rawk, -1); }
i64  ejson_kincr(ejson r, constr rawk, i64 v){ return __ejson_makeRoom_counter_r(_eo_rn(r), rawk,  v); }
i64  ejson_kdecr(ejson r, constr rawk, i64 v){ return __ejson_makeRoom_counter_r(_eo_rn(r), rawk, -v); }

i64  ejson_ipp  (ejson r, u32    idx )       { return __ejson_makeRoom_counter_i(_eo_rn(r), idx,  1);  }
i64  ejson_imm  (ejson r, u32    idx )       { return __ejson_makeRoom_counter_i(_eo_rn(r), idx, -1);  }
i64  ejson_iincr(ejson r, u32    idx , i64 v){ return __ejson_makeRoom_counter_i(_eo_rn(r), idx,  v);  }
i64  ejson_idecr(ejson r, u32    idx , i64 v){ return __ejson_makeRoom_counter_i(_eo_rn(r), idx, -v);  }

i64  ejson_ppp  (ejson r, constr path)       { return __ejson_makeRoom_counter_p(_eo_rn(r), path,  1); }
i64  ejson_pmm  (ejson r, constr path)       { return __ejson_makeRoom_counter_p(_eo_rn(r), path, -1); }
i64  ejson_pincr(ejson r, constr path, i64 v){ return __ejson_makeRoom_counter_p(_eo_rn(r), path,  v); }
i64  ejson_pdecr(ejson r, constr path, i64 v){ return __ejson_makeRoom_counter_p(_eo_rn(r), path, -v); }


static i64 __ejson_makeRoom_counter_r(_ejsr r, constr rawk, i64 val)
{
    dictLink_t l; _ejsn n; int ret;

    is1_ret(!_r_o(r) || !_key_is_valid(rawk), _ERR_COUNTOR);

    switch (_r_typeo(r))
    {
        case EOBJ:  if(! (ret = _obj_getSL_ex(r, rawk, &l)))     //! found
                    {
                        n = *l._prev;

                        switch (_n_typeoe(n)) {
                            case _ENUM_I: return      (_n_valI(n) += val);
                            case _ENUM_F: return (i64)(_n_valF(n) += val);
                        }

                        return _ERR_COUNTOR;
                    }
                    else if(ret == 1)
                    {
                        _n_newI(n, val);
                        _n_setKeyS(n, _cur_dupkeyS(rawk));
                        _obj_link(r, n, &l);

                        return val;
                    }

                    break;

        case EARR:  {
                        cstr endp; int id;

                        is0_ret(*rawk, _ERR_COUNTOR);

                        id = strtol(rawk, &endp, 10);

                        is1_ret(*endp, _ERR_COUNTOR);

                        //! already exist
                        if((n = _arr_find(r, id)))
                        {
                            switch (_n_typeoe(n)) {
                                case _ENUM_I: return      (_n_valI(n) += val);
                                case _ENUM_F: return (i64)(_n_valF(n) += val);
                            }
                        }
                        else
                        {
                            //! for arr obj, we do not create new obj

                            return _ERR_COUNTOR;
                        }
                    }

                    break;
    }

    return _ERR_COUNTOR;
}

static i64 __ejson_makeRoom_counter_i(_ejsr r, u32 idx, i64 val)
{
    _ejsn n;

    is1_ret(!_r_o(r) || _r_typeco(r) != _EJSON_CO_ARR, _ERR_COUNTOR);

    if((n = _arr_find(r, idx)))
    {
        switch (_n_typeoe(n)) {
            case _ENUM_I: return      (_n_valI(n) += val);
            case _ENUM_F: return (i64)(_n_valF(n) += val);
        }
    }

    return _ERR_COUNTOR;
}

i64 __ejson_makeRoom_counter_p(_ejsr r, constr path, i64 val)
{
    constr key; constr p; int klen; int id; dictLink_t l;

    _ejsn n;

    is1_ret(!_r_o(r) || _r_typec(r) != EJSON || !_key_is_valid(path), 0);

    p = path;
    n = (_ejsn)r;

    do{
        klen = __getAKey(p, &key, &p);

        is1_ret(klen < 0, 0);

        r = (_ejsr)n;

        if(!*p)
        {
            switch (_r_typeo(r))
            {
                case EOBJ:  if(!_obj_getBL_ex(r, key, klen, &l))     //! founds
                            {
                                n = *l._prev;

                                switch (_n_typeoe(n)) {
                                    case _ENUM_I: return      (_n_valI(n) += val);
                                    case _ENUM_F: return (i64)(_n_valF(n) += val);
                                }

                                return _ERR_COUNTOR;
                            }
                            else
                            {
                                _n_newI(n, val);
                                _n_setKeyS(n, _cur_newkeyS(klen)); memcpy(_n_keyS(n), key, klen); _n_keyS(n)[klen] = '\0';
                                _obj_link(r, n, &l);

                                return val;
                            }

                case EARR:  {
                                cstr endp; int id;

                                is0_ret(*key, _ERR_COUNTOR);

                                id = strtol(key, &endp, 10);

                                if(*endp && *endp != ']')
                                    return _ERR_COUNTOR;

                                //! already exist
                                if((n = _arr_find(r, id)))
                                {
                                    switch (_n_typeoe(n)) {
                                        case _ENUM_I: return      (_n_valI(n) += val);
                                        case _ENUM_F: return (i64)(_n_valF(n) += val);
                                    }
                                }
                                else
                                {
                                    //! for arr obj, we do not create new obj
                                    return _ERR_COUNTOR;
                                }
                            }

                            break;
            }

            return _ERR_COUNTOR;
        }

        switch (_r_typeo(r))
        {
            case EOBJ:  if(!_obj_getBL_ex(r, key, klen, &l)) //! already exist
                        {
                            if(_n_typeo(*l._prev) != EOBJ)
                            {
                                return _ERR_COUNTOR;
                            }

                            n = *l._prev;
                        }
                        else
                        {
                            _n_newO(n); _obj_init(n);

                            _n_setKeyS(n, _cur_newkeyS(klen)); memcpy(_n_keyS(n), key, klen); _n_keyS(n)[klen] = '\0';

                            _obj_link(r, n, &l);
                        }

                        break;

            case EARR:  {
                            cstr endp;

                            is1_ret(klen == 0, _ERR_COUNTOR);

                            id = strtol(key, &endp, 10);

                            //! must parse over then is a valid number
                            if(*endp && *endp != ']' && *endp != '.')
                                return 0;

                            n = _arr_find(r, id);
                        }

                        if(!n || _n_typeo(n) != EOBJ)
                            return _ERR_COUNTOR;

                        break;

            default  :  return _ERR_COUNTOR;
        }

    }while(*p);

    return _ERR_COUNTOR;
}

/** -----------------------------------------------------
 *
 *  ejson sort
 *
 *  -----------------------------------------------------
 */

#pragma pack(1)
typedef struct __sort_args_s{
    eobj_cmp_ex_cb  cmp;
    eval            prvt;
}__sort_args_t, * __sort_args;
#pragma pack()

static _ejsn __merg_sort(_ejsn a, _ejsn b, uint len, __sort_args args);
static void  __ejson_sort(_ejsr r, __sort_args args);

ejson  ejson_sort (ejson r,              eobj_cmp_cb cmp) { if(r){ __sort_args_t args = {(eobj_cmp_ex_cb)cmp, EVAL_0}; __ejson_sort(_eo_rn(r), &args); }return r; }
ejson  ejson_rSort(ejson r, constr rawk, eobj_cmp_cb cmp) { return ejson_sort(_getObjByRawk(_eo_rn(r), rawk), cmp); }
ejson  ejson_pSort(ejson r, constr path, eobj_cmp_cb cmp) { return ejson_sort(_getObjByPath(_eo_rn(r), path), cmp); }

ejson  ejson_sort_r (ejson r,              eobj_cmp_ex_cb cmp, eval prvt) { if(r) { __sort_args_t args = {cmp, prvt }; __ejson_sort(_eo_rn(r), &args);} return r; }
ejson  ejson_rSort_r(ejson r, constr rawk, eobj_cmp_ex_cb cmp, eval prvt) { return ejson_sort_r(_getObjByRawk(_eo_rn(r), rawk), cmp, prvt);}
ejson  ejson_pSort_r(ejson r, constr path, eobj_cmp_ex_cb cmp, eval prvt) { return ejson_sort_r(_getObjByPath(_eo_rn(r), path), cmp, prvt);}

static void __ejson_sort(_ejsr r, __sort_args args)
{
    is1_ret((_r_typeco(r) != _EJSON_CO_OBJ && _r_typeco(r) != _EJSON_CO_ARR) || _r_len(r) <= 1, );

    _r_head(r) = __merg_sort(_r_head(r), _r_tail(r), _r_len(r), args);
    while(_n_lnext(_r_tail(r))) _r_tail(r) = _n_lnext(_r_tail(r));
}

static _ejsn __merg_sort(_ejsn a, _ejsn b, uint len, __sort_args args)
{
    if(len == 2)
    {
        _n_lprev(a) = 0;
        _n_lnext(b) = 0;

        if(args->cmp(_n_o(a), _n_o(b), args->prvt) > 0)
        {
            _n_lnext(a) = _n_lnext(b);
            _n_lprev(b) = _n_lprev(a);
            _n_lnext(b) = a;
            _n_lprev(a) = b;

            return b;
        }
        else
        {
            return a;
        }
    }
    else if(len > 2)
    {
        _ejsn mid, midn; uint idx, midi;

        _n_lprev(a) = 0;
        _n_lnext(b) = 0;

        midi = (len - 1) / 2;   // split to [0, midi] (midi, len - 1]
        mid  = _n_lnext(a);
        idx  = 1;

        while(idx != midi)
        {
            mid = _n_lnext(mid);
            idx++;
        }
        idx++;  // to count

        //! do sort
        midn = _n_lnext(mid);
        a = __merg_sort(a   , mid,       idx, args);
        b = __merg_sort(midn,   b, len - idx, args);

        //! do merge
        {
            _ejsn_t h; mid = &h;

            _n_lnext(mid) = a;

            do
            {
                while(_n_lnext(mid))
                {
                    if(args->cmp(_n_o(_n_lnext(mid)), _n_o(b), args->prvt) > 0)
                    {
                        _ejsn next = _n_lnext(b);

                        _n_lnext(b)   = _n_lnext(mid);
                        _n_lprev(b)   = _n_lprev(_n_lnext(mid));

                        _n_lprev(_n_lnext(mid)) = b;
                        _n_lnext(mid)           = b;

                        b = next;

                        if(!b)
                            goto over;
                    }

                    mid = _n_lnext(mid);
                }

                if(args->cmp(_n_o(b), _n_o(mid), args->prvt) > 0)
                {
                    _n_lnext(mid) = b;
                    _n_lprev(b)   = mid;

                    break;
                }
                else
                {
                    _ejsn next = _n_lnext(b);

                    _n_lprev(b)   = _n_lprev(mid);
                    _n_lnext(b)   = mid;

                    _n_lnext(_n_lprev(b)) = b;
                    _n_lprev(mid)         = b;

                    b = next;

                    if(!b)
                        goto over;
                }
            }while(1);
over:
            return _n_lnext(&h);
        }
    }

    _n_lprev(a) = 0;
    _n_lnext(a) = 0;

    return a;
}

int __KEYS_ACS(eobj a, eobj b)
{
    cstr k_a, k_b;

    k_a = _eo_keyS(a);
    k_b = _eo_keyS(b);

    if(k_a)
    {
        if(k_b) return estr_cmp(k_a, k_b);
        else    return 1;
    }

    return k_b ? -1 : 0;
}

int __KEYS_DES(eobj a, eobj b)
{
    cstr k_a, k_b;

    k_a = _eo_keyS(a);
    k_b = _eo_keyS(b);

    if(k_a)
    {
        if(k_b) return -estr_cmp(k_a, k_b);
        else    return -1;
    }

    return k_b ? 1 : 0;
}

int __VALI_ACS(eobj a, eobj b)
{
    if(_eo_typeo(a) == ENUM)
    {
        if(_eo_typeo(b) == ENUM)  return _eo_valI(a) > _eo_valI(b) ? 1 : 0;   // swap when return val > 0
        else                      return 0;
    }

    return _eo_typeo(b) == ENUM ? -1 : 0;
}

int __VALI_DES(eobj a, eobj b)
{
    if(_eo_typeo(a) == ENUM)
    {
        if(_eo_typeo(b) == ENUM) return _eo_valI(b) > _eo_valI(a) ? 1 : 0;
        else                     return 0;
    }

    return _eo_typeo(b) == ENUM ? 1 : 0;
}

// --------------------------- dict definition -----------------------
#define DICT_HASH_FUNCTION_SEED 5381;
uint __MurmurHash2(const void *key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    static uint32_t seed = DICT_HASH_FUNCTION_SEED;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    default:;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}

static inline uint __djbHashB(constr buf, int len) {
    register uint hash = (uint)5381;

    while (len--)
        hash += (hash << 5) + *buf++;   // hash * 33 + c

    return hash & ~(1u << 31);
}

static inline uint __djbHashS(constr buf) {
    register uint hash = (uint)5381;

    while (*buf)
        hash += (hash << 5) + *buf++;   // hash * 33 + c

    return hash & ~(1u << 31);
}

static inline int _dict_init(dict d)
{
    _dict_resetHt(d);
    d->rehashidx = -1;

    return DICT_OK;
}

static inline dict _dict_new()
{
    dict d = malloc(sizeof(*d));

    _dict_init(d);
    return d;
}

static inline void _dict_clear(dict d)
{
    if(d->ht[1].size > 0)
    {
        free(d->ht[0].table);
        d->ht[0] = d->ht[1];
        _dict_htreset(&d->ht[1]);
    }
    else
    {
        memset(d->ht[0].table, 0, sizeof(ejson*) * d->ht[0].size);
    }

    d->ht[0].used = 0;
    d->rehashidx = -1;
}

static inline void _dict_free(dict d)
{
    free(d->ht[0].table);
    free(d->ht[1].table);
    free(d);
}

static inline int _dictRehash(dict d, int n)
{
    int empty_visits = n * 10;
    is0_ret(_dictIsRehashing(d), 0);

    while(n-- && d->ht[0].used != 0)
    {
        _ejsn de, nextde;

        while(d->ht[0].table[d->rehashidx] == NULL) {
            d->rehashidx++;
            if (--empty_visits == 0) return 1;
        }

        de = d->ht[0].table[d->rehashidx];

         while(de) {
             unsigned int h;
             nextde = _n_dnext(de);
             h = _dictHashKeyS(_n_keyS(de), _cur_lenkeyS(_n_keyS(de))) & d->ht[1].sizemask;
             _n_dnext(de) = d->ht[1].table[h];
             d->ht[1].table[h] = de;
             d->ht[0].used--;
             d->ht[1].used++;
             de = nextde;
         }
         d->ht[0].table[d->rehashidx] = NULL;
         d->rehashidx++;
    }

    if (d->ht[0].used == 0) {
        free(d->ht[0].table);
        d->ht[0] = d->ht[1];
        _dict_htreset(&d->ht[1]);
        d->rehashidx = -1;
        return DICT_OK;
    }

    return DICT_ERR;
}

static inline void _dictRehashPtrStep(dict d)
{
    _dictRehash(d, 1);
}

static inline uld _dictNextPower(uld size)
{
    uld i = DICT_HT_INITIAL_SIZE;

    if (size >= LONG_MAX) return LONG_MAX;
    while(1) {
        if (i >= size)
            return i;
        i *= 2;
    }
}

static inline int _dictExpand(dict d, uld size)
{
    dictht n;
    uld realsize = _dictNextPower(size);

    is1_ret(_dictIsRehashing(d) || d->ht[0].used > size, DICT_ERR);
    is1_ret(realsize == d->ht[0].size,                   DICT_ERR);

    n.size     = realsize;
    n.sizemask = realsize - 1;
    n.table    = calloc(realsize * sizeof(void*), 1);
    n.used     = 0;

    is1_exeret(d->ht[0].table == NULL, d->ht[0] = n, DICT_OK);

    d->ht[1] = n;
    d->rehashidx = 0;
    return DICT_OK;
}

static inline int _dictExpandIfNeeded(dict d)
{
    /* Incremental rehashing already in progress. Return. */
    if (_dictIsRehashing(d)) return DICT_OK;

    /* If the hash table is empty expand it to the initial size. */
    if (d->ht[0].size == 0) return _dictExpand(d, DICT_HT_INITIAL_SIZE);

    /* If we reached the 1:1 ratio, and we are allowed to resize the hash
     * table (global setting) or we should avoid it but the ratio between
     * elements/buckets is over the "safe" threshold, we resize doubling
     * the number of buckets. */
    if (d->ht[0].used >= d->ht[0].size &&
        (_dict_can_resize ||
         d->ht[0].used/d->ht[0].size > _dict_force_resize_ratio))
    {
        return _dictExpand(d, d->ht[0].used*2);
    }
    return DICT_OK;
}

static int _dictKeyIndexB(dict d, const void* key, int key_len)
{
    uint h, idx, table;
    _ejsn he;

    /* Expand the hash table if needed */
    if (_dictExpandIfNeeded(d) == DICT_ERR)
        return -1;
    /* Compute the key hash value */
    h = _dictHashKeyB(key, key_len);
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        /* Search if this slot does not already contain the given key */
        he = d->ht[table].table[idx];
        while(he) {
            if ( !_cur_ncmpkeyS(key, _n_keyS(he), key_len) )
                return -1;
            he = _n_dnext(he);
        }
        if (!_dictIsRehashing(d)) break;
    }
    return idx;
}

static int _dictKeyIndexS(dict d, const void* key)
{
    uint h, idx, table;
    _ejsn he;

    /* Expand the hash table if needed */
    if (_dictExpandIfNeeded(d) == DICT_ERR)
        return -1;
    /* Compute the key hash value */
    h = _dictHashKeyS(key, strlen(key) );
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        /* Search if this slot does not already contain the given key */
        he = d->ht[table].table[idx];
        while(he) {
            if ( 0 == _cur_cmpkeyS(key, _n_keyS(he)) )
                return -1;
            he = _n_dnext(he);
        }
        if (!_dictIsRehashing(d)) break;
    }
    return idx;
}

static inline _ejsn _dict_addB(dict d, constr k, int k_len, _ejsn n)
{
    int     idx;
    dictht* ht;

    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);

    is1_ret((idx = _dictKeyIndexB(d, k, k_len)) == -1, NULL); // already exist

    ht = _dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    _n_dnext(n)    = ht->table[idx];
    ht->table[idx] = n;
    ht->used++;

    return n;
}

static inline _ejsn _dict_addS(dict d, constr k, _ejsn n)
{
    int     idx;
    dictht* ht;

    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);

    is1_ret((idx = _dictKeyIndexS(d, k)) == -1, NULL); // already exist

    ht = _dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    _n_dnext(n)    = ht->table[idx];
    ht->table[idx] = n;
    ht->used++;

    return n;
}

static _ejsn _dict_findB(dict d, constr k, int k_len)
{
    _ejsn he;
    unsigned int h, idx, table;

    if(d->ht[0].size == 0) return NULL; /* We don't have a table at all */
    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);
    h = _dictHashKeyB(k, k_len);
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        while(he) {
            if ( !_cur_ncmpkeyS(k, _n_keyS(he), k_len) )
                return he;
            he = _n_dnext(he);
        }
        if (!_dictIsRehashing(d)) return NULL;
    }
    return NULL;
}

static _ejsn _dict_findS(dict d, constr k)
{
    _ejsn he;
    unsigned int h, idx, table;

    if(d->ht[0].size == 0) return NULL; /* We don't have a table at all */
    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);
    h = _dictHashKeyS(k, _cur_lenkeyS(k) );
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        while(he) {
            if ( !_cur_cmpkeyS(k, _n_keyS(he)) )
                return he;
            he = _n_dnext(he);
        }
        if (!_dictIsRehashing(d)) return NULL;
    }
    return NULL;
}

static _ejsn _dict_findB_ex(dict d, constr k, int k_len, bool rm)
{
    unsigned int h, idx;
    _ejsn he, prevHe;
    int table;

    if (d->ht[0].size == 0) return NULL; /* d->ht[0].table is NULL */
    if (_dictIsRehashing(d)) _dictRehashPtrStep(d);
    h = _dictHashKeyB(k, k_len);

    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        prevHe = NULL;
        while(he) {
            if (!_cur_ncmpkeyS(k, _n_keyS(he), k_len)) {
                if(rm)
                {
                    /* Unlink the element from the list */
                    if (prevHe) _n_dnext(prevHe)        = _n_dnext(he);
                    else        d->ht[table].table[idx] = _n_dnext(he);
                    d->ht[table].used--;
                }

                return he;
            }
            prevHe  = he;
            he      = _n_dnext(he);
        }
        if (!_dictIsRehashing(d)) break;
    }
    return NULL; /* not found */
}

static _ejsn _dict_findS_ex(dict d, constr k, bool rm)
{
    unsigned int h, idx;
    _ejsn he, prevHe;
    int table;

    if (d->ht[0].size == 0) return NULL; /* d->ht[0].table is NULL */
    if (_dictIsRehashing(d)) _dictRehashPtrStep(d);
    h = _dictHashKeyS(k, _cur_lenkeyS(k) );

    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        prevHe = NULL;
        while(he) {
            if (!_cur_cmpkeyS(k, _n_keyS(he))) {
                if(rm)
                {
                    /* Unlink the element from the list */
                    if (prevHe) _n_dnext(prevHe)        = _n_dnext(he);
                    else        d->ht[table].table[idx] = _n_dnext(he);
                    d->ht[table].used--;
                }

                return he;
            }
            prevHe  = he;
            he      = _n_dnext(he);
        }
        if (!_dictIsRehashing(d)) break;
    }
    return NULL; /* not found */
}

static int _dict_getBL(dict d, constr k, int k_len, L l)
{
    int     idx;
    dictht* ht;

    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);

    is1_ret((idx = _dictKeyIndexB(d, k, k_len)) == -1, 0); // already exist

    ht = _dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    l->_pos  = &ht->table[idx];
    l->_used = &ht->used;

    return 1;
}

static int _dict_getBL_ex(dict d, constr k, int k_len, L l)
{
    int     idx;
    dictht* ht;

    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);

    {
        uint h, table;
        _ejsn he;

        /* Expand the hash table if needed */
        if (_dictExpandIfNeeded(d) == DICT_ERR)
            return -1;
        /* Compute the key hash value */
        h = _dictHashKeyB(k, k_len );
        for (table = 0; table <= 1; table++) {
            idx = h & d->ht[table].sizemask;
            /* Search if this slot does not already contain the given key */
            he = d->ht[table].table[idx];
            l->_prev = &d->ht[table].table[idx];
            while(he) {
                if ( 0 == _cur_ncmpkeyS(k, _n_keyS(he), k_len) )
                {
                    ht = &d->ht[table];
                    l->_pos  = &ht->table[idx];
                    l->_used = &ht->used;

                    return 0;
                }

                l->_prev = &_n_dnext(he);
                he       =  _n_dnext(he);
            }
            if (!_dictIsRehashing(d)) break;
        }
    }

    ht = _dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    l->_pos  = &ht->table[idx];
    l->_used = &ht->used;

    return 1;
}

static int _dict_getSL(dict d, constr k, L l)
{
    int     idx;
    dictht* ht;

    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);

    is1_ret((idx = _dictKeyIndexS(d, k)) == -1, 0); // already exist

    ht = _dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    l->_pos  = &ht->table[idx];
    l->_used = &ht->used;

    return 1;
}

static int _dict_getSL_ex(dict d, constr k, L l)
{
    int     idx;
    dictht* ht;

    if(_dictIsRehashing(d)) _dictRehashPtrStep(d);

    {
        uint h, table;
        _ejsn he;

        /* Expand the hash table if needed */
        if (_dictExpandIfNeeded(d) == DICT_ERR)
            return -1;
        /* Compute the key hash value */
        h = _dictHashKeyS(k, strlen(k) );
        for (table = 0; table <= 1; table++) {
            idx = h & d->ht[table].sizemask;
            /* Search if this slot does not already contain the given key */
            he = d->ht[table].table[idx];
            l->_prev = &d->ht[table].table[idx];
            while(he) {
                if ( 0 == _cur_cmpkeyS(k, _n_keyS(he)) )
                {
                    ht = &d->ht[table];
                    l->_pos  = &ht->table[idx];
                    l->_used = &ht->used;

                    return 0;
                }

                l->_prev = &_n_dnext(he);
                he       =  _n_dnext(he);
            }
            if (!_dictIsRehashing(d)) break;
        }
    }

    ht = _dictIsRehashing(d) ? &d->ht[1] : &d->ht[0];
    l->_pos  = &ht->table[idx];
    l->_used = &ht->used;

    return 1;
}

static _ejsn _dict_del(dict d, _ejsn del)
{
    unsigned int h, idx;
    _ejsn he, prevHe;
    int table;

    if (d->ht[0].size == 0) return NULL; /* d->ht[0].table is NULL */
    if (_dictIsRehashing(d)) _dictRehashPtrStep(d);
    h = _dictHashKeyS(_n_keyS(del), _cur_lenkeyS(_n_keyS(del)));

    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        prevHe = NULL;
        while(he) {
            if (!_cur_cmpkeyS(_n_keyS(del), _n_keyS(he))) {
                if(he != del)   return NULL;    // not match

                /* Unlink the element from the list */
                if (prevHe) _n_dnext(prevHe)        = _n_dnext(he);
                else        d->ht[table].table[idx] = _n_dnext(he);
                d->ht[table].used--;
                return he;
            }
            prevHe  = he;
            he      = _n_dnext(he);
        }
        if (!_dictIsRehashing(d)) break;
    }
    return NULL; /* not found */
}

/// --------------------------- inner OBJ operation

static inline _ejsn _obj_add(_ejsr r, cstr key, _ejsn n)
{
    is0_ret(_dict_addS(_obj_hd(r), key, n), 0);
    _n_linked(n) = 1;

    if(!_r_head(r)){_r_head(r) =           _r_tail(r)  = n;}

    else           {_n_lprev(n) =          _r_tail(r)     ;
                    _r_tail (r) = _n_lnext(_r_tail(r)) = n;}

    _r_len(r)++;

    return n;
}

static inline void _obj_link(_ejsr r, _ejsn n, L l)
{
    _n_linked(n) = 1;

    _dict_link(*l, n);

    if(!_r_head(r)){_r_head(r) =          _r_tail(r)  = n;}

    else           {_n_lprev(n ) =          _r_tail(r)     ;
                    _r_tail(r)   = _n_lnext(_r_tail(r)) = n;}

    _r_len(r)++;
}


static inline eobj _obj_popH(_ejsr r)
{
    _ejsn n;

    is0_ret(_r_len(r), 0);

    _dict_del(_obj_hd(r), (n = _r_head(r)));

    if(1 == _r_len(r)){_r_head(r) = _r_tail(r)            = NULL;}

    else              {_r_head(r)                         = _n_lnext(n);
                                    _n_lprev(_n_lnext(n)) = NULL;}

    _n_lprev(n) = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    _r_len(r)--;

    return _n_o(n);
}

static inline eobj _obj_popT(_ejsr r)
{
    _ejsn n;

    is0_ret(_r_len(r), 0);

    _dict_del(_obj_hd(r), (n = _r_tail(r)));

    if(1 == _r_len(r)){_r_head(r) = _r_tail(r)            = NULL         ;}

    else              {_r_tail(r)                         = _n_lprev(n);
                                    _n_lnext(_n_lprev(n)) = NULL         ;}

    _n_lprev(n) = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    _r_len(r)--;

    return _n_o(n);
}

static inline eobj  _obj_takeN(_ejsr r, _ejsn n)
{
    if(!_dict_del(_obj_hd(r), n))  return NULL;

    if   (_n_lprev(n))  _n_lnext(_n_lprev(n)) = _n_lnext(n);
    else                _r_head(r)            = _n_lnext(n);

    if   (_n_lnext(n))  _n_lprev(_n_lnext(n)) = _n_lprev(n);
    else                _r_tail(r)            = _n_lprev(n);

    _n_lprev(n) = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    _r_len(r)--;

    return _n_o(n);
}

static inline eobj  _obj_findS_ex(_ejsr r, constr k, bool rm)
{
    _ejsn n;

    if(rm)
    {
        n = _dict_findS_ex(_r_dict(r), k, rm);

        if(n)
        {
            if   (_n_lprev(n))  _n_lnext(_n_lprev(n)) = _n_lnext(n);
            else                _r_head(r)            = _n_lnext(n);

            if   (_n_lnext(n))  _n_lprev(_n_lnext(n)) = _n_lprev(n);
            else                _r_tail(r)            = _n_lprev(n);

            _n_lprev(n) = _n_lnext(n) = NULL;
            _n_linked(n) = 0;

            _r_len(r)--;
        }
    }
    else
    {
        n = _obj_findS(r , k);
    }

    return n ? _n_o(n) : 0;
}

/// -------------------------------- inner ARR operation

// -- add in tail
static void _arr_appd(_ejsr r, _ejsn n)
{
    _n_linked(n) = 1;

    if(!_r_head(r)){_r_head(r) =          _r_tail(r)  = n;}

    else           {_n_lprev(n)=          _r_tail(r)     ;
                    _r_tail(r) = _n_lnext(_r_tail(r)) = n;}

    _r_len(r)++;
}

// -- add in head
static _ejsn _arr_push(_ejsr r, _ejsn n)
{
    _n_linked(n) = 1;

    if(!_r_head(r)){_r_head(r) =          _r_tail(r)  = n;}

    else           {_n_lnext(n)  =          _r_head(r)       ;
                    _r_head(r) = _n_lprev(_r_head(r)) = n;
                    //if(((H)_r_list(r))->i) ((H)_elist(r))->i++;
    }

    _r_len(r)++;

    return n;
}

// -- remove in head
static eobj _arr_popH(_ejsr r)
{
    _ejsn n;

    is0_ret(_r_len(r), 0);

    n = _r_head(r);

    if(1 == _r_len(r)){_r_head(r) = _r_tail(r)          = NULL;}

    else              { _r_head(r)                           = _n_lnext(n);
                        _n_lprev(_n_lnext(n)) = NULL;
                           //if(((H)_elist(r))->i) ((H)_elist(r))->i--;
    }

    _r_len(r)--;

    _n_lprev(n)  = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    return _n_o(n);
}

// -- remove in tail
static eobj _arr_popT(_ejsr r)
{
    _ejsn n; list_t* h;

    is0_ret(_r_len(r), 0);

    n = _r_tail(r);
    h = _arr_hd(r);

    if(1 == _r_len(r)){_r_head(r) = _r_tail(r)          = NULL;}

    else              {_r_tail(r)                       = _n_lprev(n);
                                  _n_lnext(_n_lprev(n)) = NULL         ;
                       if(h->i && h->o == n) {h->o = _n_lprev(n); h->i--;} ;}

    _r_len(r)--;

    _n_lprev(n) = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    return _n_o(n);
}

static _ejsn _arr_find(_ejsr r, uint idx)
{
    _ejsn n; uint i; list_t* h;

    is1_ret(idx >= _r_len(r), 0);

    h = _arr_hd(r);

    if(idx >= (i = h->i)) { n = i ? h->o : _r_head(r); for(; i != idx; i++) n = _n_lnext(n); }
    else                  { n =     h->o             ; for(; i != idx; i--) n = _n_lprev(n); }

    h->i = idx;
    h->o = n;

    return n;
}

static _ejsn _arr_find_ex(_ejsr r, uint idx, bool rm)
{
    _ejsn n; uint i; list_t* h;

    is1_ret(idx >= _r_len(r), 0);

    h = _arr_hd(r);

    if(idx >= (i = h->i)) { n = i ? h->o : _r_head(r); for(; i != idx; i++) n = _n_lnext(n); }
    else                  { n =     h->o             ; for(; i != idx; i--) n = _n_lprev(n); }

    if(rm)
    {

    if   (_n_lprev(n)) _n_lnext(_n_lprev(n)) = _n_lnext(n);
    else               _r_tail(r)            = _n_lnext(n);

    if   (_n_lnext(n)){_n_lprev(h->o = _n_lnext(n)) = _n_lprev(n);            h->i = idx    ;}
    else              {         h->o = _r_tail(r)   = _n_lprev(n);    if(idx) h->i = idx - 1;}

    _r_len(r)--;

    _n_lprev(n)  = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    }

    return n;
}


static eobj _arr_takeI(_ejsr r, uint idx)
{
    _ejsn n; uint i; list_t* h;

    is1_ret(idx >= _r_len(r), 0);

    h = _arr_hd(r);

    if(idx >= (i = h->i)) { n = i ? h->o : _r_head(r); for(; i != idx; i++) n = _n_lnext(n); }
    else                  { n =     h->o             ; for(; i != idx; i--) n = _n_lprev(n); }

    if   (_n_lprev(n)) _n_lnext(_n_lprev(n)) = _n_lnext(n);
    else               _r_head(r)            = _n_lnext(n);

    if   (_n_lnext(n)){_n_lprev(h->o = _n_lnext(n)) = _n_lprev(n);            h->i = idx    ;}
    else              {         h->o = _r_tail(r)   = _n_lprev(n);    if(idx) h->i = idx - 1;}

    _r_len(r)--;

    _n_lprev(n) = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    return _n_o(n);
}

static eobj _arr_takeN(_ejsr r, _ejsn del)
{
    _ejsn n; uint i; list_t* h;

    h = _arr_hd(r);

    if((i = h->i))  {        for(              n =          h->o ; n && n != del; n = _n_lnext(n), i++);
                      if(!n) for(i = h->i - 1, n = _n_lprev(h->o); n && n != del; n = _n_lprev(n), i--); }
    else            {        for(i = 0       , n = _r_head (r   ); n && n != del; n = _n_lnext(n), i++); }

    is0_ret(n, 0);

    if   (_n_lprev(n)) _n_lnext(_n_lprev(n)) = _n_lnext(n);
    else               _r_head(r)            = _n_lnext(n);

    if   (_n_lnext(n)){_n_lprev(h->o = _n_lnext(n)) = _n_lprev(n);          h->i = i    ;}
    else              {         h->o = _r_tail (r)  = _n_lprev(n);    if(i) h->i = i - 1;}

    _r_len(r)--;

    _n_lprev(n) = _n_lnext(n) = NULL;
    _n_linked(n) = 0;

    return _n_o(n);
}

inline constr ejson_version() {   return EJSON_VERSION;    }


