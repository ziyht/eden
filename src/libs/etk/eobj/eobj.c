#include "eobj_p.h"
#include "eutils.h"

#define EOBJ_VERSION     "eobj 1.0.5"   // adjust implementation of some APIs

static ekey __empty_key;

ekey   eobj_key  (eobj o) { if(o && _eo_hkey(o)             ) { return _eo_key (o); } return __empty_key; }
i64    eobj_keyI (eobj o) { if(o && _eo_typek(o) == _EO_KEYI) { return _eo_keyI(o); } return           0; }
constr eobj_keyS (eobj o) { if(o && _eo_typek(o) == _EO_KEYS) { return _eo_keyS(o); } return           0; }
etypeo eobj_typeo(eobj o) { return o ? _eo_typeo(o) : EOBJ_UNKNOWN; }
etypec eobj_typec(eobj o) { return o ? _eo_typec(o) : ECON_UNKNOWN; }
uint   eobj_len  (eobj o) { return o ? _eo_len(o)   :            0; }
i64    eobj_valI (eobj o) { _eo_retI(o); }
f64    eobj_valF (eobj o) { _eo_retF(o); }
constr eobj_valS (eobj o) { _eo_retS(o); }
cptr   eobj_valP (eobj o) { _eo_retP(o); }
cptr   eobj_valR (eobj o) { _eo_retR(o); }

int  eobj_cmpI(eobj o, i64    val) { return o ? _eo_typeo(o) == ENUM ? __eobj_valI(o) < val ? -1 : __eobj_valI(o) > val ? 1 : 0 : -3 : -2; }
int  eobj_cmpF(eobj o, f64    val) { return o ? _eo_typeo(o) == ENUM ? __eobj_valF(o) < val ? -1 : __eobj_valF(o) > val ? 1 : 0 : -3 : -2; }
int  eobj_cmpS(eobj o, constr str) { return o ? _eo_typeo(o) == ESTR ? str ? strcmp(_eo_valS(o), str) : -4 : -3 : -2;}


eobj eobj_setKeyI(eobj o, i64 key)
{
    if(o)
    {
        is1_ret(_eo_linked(o), 0);

        switch (_eo_typec(o)) {
            case ERB    :
            case EDICT  :
            case ESL    : if(_eo_keys(o)) _eobj_free(_eo_keyS(o));

                          _eo_typek(o) = _EO_KEYI;
                          _eo_keyI(o)  = key;

            default     : return 0;
        }
    }

    return o;
}

eobj eobj_setKeyS(eobj o, constr key)
{
    if(o)
    {
        is1_ret(_eo_linked(o), 0);

        switch (_eo_typec(o)) {
            case ERB    :
            case EDICT  :
            case ESL    : if(_eo_keys(o)) _eobj_free(_eo_keyS(o));

                          _eo_typek(o) = _EO_KEYS;
                          _eo_keyS(o)  = strdup(key);

            default     : return 0;
        }
    }

    return o;
}

constr __eobj_typeS(eobj o, bool beauty)
{
    #define SDS_STACK_MASK 8    // form estr.c

    typedef struct _inner_{
        uint cap;
        uint len;
        u8   type;
        char s[16];
    }_inner_;

    static _inner_ b_nil    = {5, 5, SDS_STACK_MASK, "ENIL    "};
    static _inner_ b_false  = {5, 5, SDS_STACK_MASK, "EFALSE  "};
    static _inner_ b_true   = {4, 4, SDS_STACK_MASK, "ETRUE   "};
    static _inner_ b_null   = {4, 4, SDS_STACK_MASK, "ENULL   "};
    static _inner_ b_num    = {6, 6, SDS_STACK_MASK, "ENUM    "};
    static _inner_ b_str    = {6, 6, SDS_STACK_MASK, "ESTR    "};
    static _inner_ b_ptr    = {5, 5, SDS_STACK_MASK, "EPTR    "};
    static _inner_ b_raw    = {5, 5, SDS_STACK_MASK, "ERAW    "};
    static _inner_ b_obj    = {3, 3, SDS_STACK_MASK, "EOBJ    "};
    static _inner_ b_arr    = {5, 5, SDS_STACK_MASK, "EARR    "};
    static _inner_ b_unknown= {6, 6, SDS_STACK_MASK, "EUNKNOWN"};

    static _inner_ t_nil    = {5, 5, SDS_STACK_MASK, "ENIL"    };
    static _inner_ t_false  = {5, 5, SDS_STACK_MASK, "EFALSE"  };
    static _inner_ t_true   = {4, 4, SDS_STACK_MASK, "ETRUE"   };
    static _inner_ t_null   = {4, 4, SDS_STACK_MASK, "ENULL"   };
    static _inner_ t_num    = {6, 6, SDS_STACK_MASK, "ENUM"    };
    static _inner_ t_str    = {6, 6, SDS_STACK_MASK, "ESTR"    };
    static _inner_ t_ptr    = {5, 5, SDS_STACK_MASK, "EPTR"    };
    static _inner_ t_raw    = {5, 5, SDS_STACK_MASK, "ERAW"    };
    static _inner_ t_obj    = {3, 3, SDS_STACK_MASK, "EOBJ"    };
    static _inner_ t_arr    = {5, 5, SDS_STACK_MASK, "EARR"    };
    static _inner_ t_unknown= {6, 6, SDS_STACK_MASK, "EUNKNOWN"};

    if(beauty)
    {
        if(!o) return b_nil.s;

        switch (_eo_typeo(o))
        {
            case _EFALSE : return b_false  .s ;
            case _ETRUE  : return b_true   .s ;
            case _ENULL  : return b_null   .s ;
            case _ENUM   : return b_num    .s ;
            case _ESTR   : return b_str    .s ;
            case _EPTR   : return b_ptr    .s ;
            case _ERAW   : return b_raw    .s ;
            case _EOBJ   : return b_obj    .s ;
            case _EARR   : return b_arr    .s ;
            default      : return b_unknown.s ;
        }
    }
    else
    {
        if(!o) return t_nil.s;

        switch (_eo_typeo(o))
        {
            case _EFALSE : return t_false  .s ;
            case _ETRUE  : return t_true   .s ;
            case _ENULL  : return t_null   .s ;
            case _ENUM   : return t_num    .s ;
            case _ESTR   : return t_str    .s ;
            case _EPTR   : return t_ptr    .s ;
            case _ERAW   : return t_raw    .s ;
            case _EOBJ   : return t_obj    .s ;
            case _EARR   : return t_arr    .s ;
            default      : return t_unknown.s ;
        }
    }

    return "";
}

constr eobj_typeS(eobj o)
{
    return __eobj_typeS(o, 1);
}

/// \brief __cstr_replace_str - replace str @param from in s to str @param to
///
/// \param s       : a _s type str, we assume it is valid
/// \param end     : a cstr pointer, point to a pointer who point to the end '\0' of @param s now
/// \param last    : a cstr pointer, point to a pointer who point to the end '\0' of @param s when finish replacing, this ptr is predicted before call this func
/// \param from    : the cstr you want to be replaced, we assume it is valid
/// \param fromlen : the length of @param from, it must < tolen
/// \param to      : the cstr you want to replace to, we assume it is valid
/// \param tolen   : the length of @param to, it must > fromlen
///
/// @note:
///     1. fromlen < tolen needed, or you will not get the correct result, and you do not need this func if fromlen <= tolen
///     2. s must have enough place to hold the whole str when after
///
/// from: aaa        fromlen: 3
/// to  : abcdefg    tolen  : 7
/// s   : 11111aaa111aaa11111111_____________________
///                             |       |-- last = end + (7 - 3)*2
///                             |---------- end
///
///
static void __cstr_replace_str(cstr s, cstr* end, cstr* last, constr from, size fromlen, constr to, size tolen)
{
    cstr fd;

    if((fd = strstr(s, from)))
    {
        cstr mv_from, mv_to; size mv_len;

        __cstr_replace_str(fd + 1, end, last, from, fromlen, to, tolen);

        mv_from = fd + fromlen;
        mv_len  = *end  - fd - fromlen;
        mv_to   = *last - mv_len;

        memmove(mv_to, mv_from, mv_len);

        *last = mv_to - tolen;
        memcpy(*last, to, tolen);

        *end = fd;
    }

    return ;
}

eobj   __eobj_subS(eobj o, constr from, constr to, __eobj_alloc alloc)
{
    i32 flen, tlen, offlen, offnow; cstr fd_s, cp_s, end_p;

    cstr s = EOBJ_VALS(o);

    is1_ret(!from || !to, 0);

    flen   = strlen(from);
    tlen   = strlen(to);
    offlen = tlen - flen;

    if(offlen < 0)
    {
        offlen = -offlen;
        offnow = 0;
        fd_s   = s;
        end_p  = s + _eo_len(o);

        if((fd_s = strstr(fd_s, from)))
        {
            memcpy(fd_s, to, tlen);     // replace it

            cp_s = (fd_s += flen);          // record the pos of str need copy
            offnow += offlen;               // record the off of str need copy

            while((fd_s = strstr(fd_s, from)))
            {
                memmove(cp_s - offnow, cp_s, fd_s - cp_s);   // move the str-need-copy ahead

                memcpy(fd_s - offnow, to, tlen);
                cp_s = (fd_s += flen);
                offnow += offlen;
            }

            memmove(cp_s - offnow, cp_s, end_p - cp_s);
            _eo_len(o) -= offnow;

            *(end_p - offnow) = '\0';
        }
    }
    else if(offlen == 0)
    {
        end_p  = s + _eo_len(o);

        fd_s = strstr(s, from);

        while(fd_s)
        {
            memcpy(fd_s, to, tlen);
            fd_s += flen;
            fd_s =  strstr(fd_s, from);
        }
    }
    else
    {
        offnow = _eo_len(o);
        end_p  = s + offnow;
        fd_s   = s;

        if((fd_s = strstr(fd_s, from)))
        {

            // -- get len need to expand
            offnow += offlen; fd_s += flen;

            while((fd_s = strstr(fd_s, from)))
            {
                offnow += offlen; fd_s += flen;
            }

            // -- have enough place, let's do it
            if((size)offnow <= _eo_len(o))
            {
                //! this operation is more efficient
                //! we set the up limit of stack call to 128
                if(((offnow - (end_p - s)) / offlen) <= 128)
                {
                    cstr last = s + offnow;
                    __cstr_replace_str(s, &end_p, &last, from, flen, to, tlen);
                    _eo_len(o) = offnow;
                }
                else
                {
                    cstr last, lpos; i64 mlen;

                    last = s + offnow;
                    lpos = end_p - flen;

                    while(lpos >= s)
                    {
                        if(lpos[0] == from[0] && 0 == memcmp(lpos, from, flen))
                        {
                            mlen  = (end_p - lpos - flen);

                            last -= mlen;
                            memmove(last, lpos + flen, mlen);

                            last -= tlen;
                            memcpy(last, to, tlen);

                            end_p = lpos;
                        }

                        lpos--;
                    }
                }

                s[offnow] = '\0';
            }
            else
            {
                eobj new_o; cstr new_p; i64 len;

                is0_ret(new_o = alloc(o, offnow), 0);  // new str

                // -- to new str
                cp_s  = fd_s = s;
                new_p = EOBJ_VALS(new_o);
                while((fd_s = strstr(fd_s, from)))
                {
                    memcpy(new_p, cp_s, (len = fd_s - cp_s)); new_p += len;
                    memcpy(new_p, to, tlen);                  new_p += tlen;

                    cp_s = (fd_s += flen);
                }

                memcpy(new_p, cp_s, end_p - cp_s);
                new_p[end_p - cp_s] = '\0';

                _eo_len(new_o) = offnow;

                return new_o;
            }
        }
    }

    return o;
}


