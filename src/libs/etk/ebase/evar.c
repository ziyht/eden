/// =====================================================================================
///
///       Filename:  evar.h
///
///    Description:  a struct to hold variant data
///
///        Version:  1.0
///        Created:  04/01/2019 08:51:34 AM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================
///

#include "eutils.h"

#include "evar.h"
#include "estr.h"
#include "libs/estr_p.h"

#define EVAR_VERISON "evar 1.0.0"

static const u8 _len_map[] = __EVAR_ITEM_LEN_MAP;

#define __ETYPEV_ARR_MASK  1
#define __ETYPEV_PTR_MASK  2

#define _v_isnav(v)         (E_NAV == _v_type(v))

#define _v_type( v)         (v).type
#define _v_isptr(v)         ((v).__ & __ETYPEV_PTR_MASK)
#define _v_isarr(v)         ((v).__ & __ETYPEV_ARR_MASK)

#define _v_iptr(v, i)       ((char*)(v).v.r + (v).esize * i)
#define _v_ival(v, i)       ((eval*)_v_iptr(v, i))

#define _vp_iptr(vp, i)     ((char*)(vp)->v.r + (vp)->esize * i)
#define _vp_ival(vp, i)     ((eval*)_vp_iptr( vp, i))

#define _vp_type( vp)       ((vp)->type)

#define _vp_isarr(vp)       ((vp)->__ & __ETYPEV_ARR_MASK)
#define _vp_isptr(vp)       ((vp)->__ & __ETYPEV_PTR_MASK)

#define _v_vp(     _v)      ((evarp)(_v).v.p)
#define _v_vp_type( v)      _vp_type (_v_vp(v))
#define _v_vp_isarr(v)      _vp_isarr(_v_vp(v))

#define _v_valI(v) ((_v_type(v) == E_I64) ? v.v.i64 : (_v_type(v) == E_F64) ? (i64)v.v.f64 : 0)
#define _v_valF(v) ((_v_type(v) == E_F64) ? v.v.f64 : (_v_type(v) == E_I64) ? (f64)v.v.i64 : 0)
#define _v_valS(v) ((_v_type(v) == E_STR) ? v.v.s   : 0)
#define _v_valP(v) ((_v_type(v) == E_PTR) ? v.v.p   : 0)
#define _v_valR(v) ((_v_type(v) == E_RAW) ? v.v.p   : 0)

static evarp __evar_newa(etypev t, int cnt, int esize)
{
    evarp v;

    //! if esize == 1, it maybe used as str, so append 1 byte
    v = ecalloc(1, cnt * esize + sizeof(*v) - sizeof(eval) + (esize == 1));

    v->__    = __ETYPEV_ARR_MASK;
    v->type  = t;
    v->esize = esize;
    v->cnt   = cnt;

    return v;
}

static evarp __evar_dupa(evarp in)
{
    evarp vp = __evar_newa(in->type, in->cnt, in->esize);

    switch (in->type)
    {
        case E_STR:
        case E_RAW: {
                        register uint i = 0;

                        while(i != in->cnt)
                            vp->v.sa[i] = estr_dup(in->v.sa[i]);
                    }
                    break;

        default   : memcpy(vp->v.r, in->v.r, in->cnt * in->esize);
                    break;
    }

    return vp;
}

evar  evar_gen (etypev t, int cnt, int esize)
{
    is1_ret(t == E_NAV, EVAR_NAV);

    if(t<= E_RAW) esize = _len_map[t];     // get internal size

    if(esize < 1 || cnt < 1)
        return EVAR_NAV;

    if(t <= E_PTR && esize * cnt <= 8)
    {
        return (evar){  0,
                        t, esize, cnt, EVAL_0
                    };
    }

    return (evar){  __ETYPEV_PTR_MASK,
                    t, esize, cnt, EVAL_P(__evar_newa(t, cnt, esize))
                  };
}

uint evarp_free(evarp vp)
{
    uint cnt = 0;

    is1_ret(vp->type == E_NAV, 0);

    if(_vp_isptr(vp))
    {
        evarp vp2   = vp->v.p;
        cnt         = vp->cnt;

        switch (vp2->type)
        {
            case E_STR:
            case E_RAW:{
                            estr* s, *e;

                            s = vp2->v.sa;
                            e = s + cnt;

                            do{
                                _s_free(*s);
                            }
                            while(++s < e);
                        }
        }

        efree(vp2);
    }
    else
        cnt = vp->cnt;

    *vp = EVAR_NAV;

    return cnt;
}

static constr __evar_typeS(etypev type)
{
    switch (type)
    {
        case E_NAV : return "E_NAV";

        case E_I8  : return "E_I8";
        case E_I16 : return "E_I16";
        case E_I32 : return "E_I32";
        case E_I64 : return "E_I64";

        case E_U8  : return "E_U8";
        case E_U16 : return "E_U16";
        case E_U32 : return "E_U32";
        case E_U64 : return "E_U64";

        case E_F32 : return "E_F32";
        case E_F64 : return "E_F64";

        case E_PTR : return "E_PTR";
        case E_STR : return "E_STR";
        case E_RAW : return "E_RAW";

        case E_USER: return "E_USER";

        default    : return "E_CSTM";   // custem
    }

    return "E_UNKNOWN";
}

uint   evar_cnt  (evar v)  { is1_ret(_v_isnav(v), 0); return _v_isptr(v) ? _v_vp(v)->cnt   : v.cnt  ; }
uint   evar_esize(evar v)  { is1_ret(_v_isnav(v), 0); return _v_isptr(v) ? _v_vp(v)->esize : v.esize; }
u64    evar_space(evar v)  { is1_ret(_v_isnav(v), 0); return _v_isptr(v) ? _v_vp(v)->cnt * _v_vp(v)->esize : v.cnt * v.esize   ; }
etypev evar_type (evar v)  { return _v_isptr(v) ?              _v_vp_type(v)  :              _v_type(v) ; }
constr evar_typeS(evar v)  { return _v_isptr(v) ? __evar_typeS(_v_vp_type(v)) : __evar_typeS(_v_type(v)); }

evar   evar_dup(evar v, bool deep)
{
    if(_v_isptr(v))
    {
        return (evar){0,v.type,
                        v.esize,
                        v.cnt,
                        EVAL_P(__evar_dupa(&v))
                      };
    }

    return v;
}

bool evar_iSetV(evar v, uint idx, evar in)
{
    if(_v_isptr(v))
    {
        evarp vp = v.v.p;

        assert((vp->type) == (in.type));

        is1_ret(vp->cnt <= idx, false);

        switch (vp->type)
        {

            case E_STR : __estr_wrtB((estr*)_vp_iptr(vp, idx), in.v.s, strlen(in.v.s));
                         break;

            case E_RAW : __estr_wrtB((estr*)_vp_iptr(vp, idx), in.v.p, in.esize);
                         break;

            case E_USER: assert(vp->esize == in.esize);
                         memcpy(_vp_iptr(vp, idx), in.v.p, vp->esize);
                         break;

            default    : switch (vp->esize) {
                             case  1: _vp_ival(vp, idx)->i8  = in.v.i8;    break;
                             case  2: _vp_ival(vp, idx)->i16 = in.v.i16;   break;
                             case  4: _vp_ival(vp, idx)->i32 = in.v.i32;   break;
                             case  8: _vp_ival(vp, idx)->i64 = in.v.i64;   break;

                             default: return false;
                         }

                         break;
        }

        return true;
    }

    return false;
}

bool evar_iSet(evar v, uint idx, conptr  in, int inlen)
{
    if(_v_isptr(v))
    {
        evarp vp = v.v.p;

        is1_ret(vp->cnt <= idx, false);

        switch (vp->type)
        {
            case E_STR :
            case E_RAW : __estr_wrtB((estr*)_vp_iptr(vp, idx), in, inlen);
                         break;

            case E_USER: is0_ret(vp->esize == inlen, false);

                         memcpy(_vp_iptr(vp, idx), in, inlen);
                         break;

            default    : if(inlen > vp->esize) inlen = vp->esize;

                         switch (inlen) {
                             case  1: _vp_ival(vp, idx)->i8  = ((evalp)in)->i8;    break;
                             case  2: _vp_ival(vp, idx)->i16 = ((evalp)in)->i16;   break;
                             case  4: _vp_ival(vp, idx)->i32 = ((evalp)in)->i32;   break;
                             case  8: _vp_ival(vp, idx)->i64 = ((evalp)in)->i64;   break;

                             default: return false;
                         }

                         break;
        }

        return true;
    }

    return false;
}


bool evar_iSetI(evar v, uint idx, i64    val) { return evar_iSetV(v, idx, EVAR_I64(val)); }
bool evar_iSetF(evar v, uint idx, f64    val) { return evar_iSetV(v, idx, EVAR_F64(val)); }
bool evar_iSetS(evar v, uint idx, constr str) { return evar_iSetV(v, idx, EVAR_CS (str)); }
bool evar_iSetP(evar v, uint idx, conptr ptr) { return evar_iSetV(v, idx, EVAR_CP (ptr)); }
bool evar_iSetR(evar v, uint idx, evar   val) { return evar_iSetV(v, idx,          val ); }

static __always_inline i64  __evar_valI(evar v) { return _v_valI(v); }
static __always_inline f64  __evar_valF(evar v) { return _v_valF(v); }
static __always_inline cstr __evar_valS(evar v) { return _v_valS(v); }
static __always_inline cptr __evar_valP(evar v) { return _v_valP(v); }
static __always_inline cptr __evar_valR(evar v) { return _v_valR(v); }
static __always_inline uint __evar_lenS(evar v) { return _v_type(v) == E_STR ? _v_isarr(v) ? (uint)_s_len(v.v.s) : strlen(v.v.s) : 0; }
static __always_inline uint __evar_lenR(evar v) { return _v_type(v) == E_RAW ? _v_isarr(v) ?       v.esize       : strlen(v.v.s) : 0; }

i64  evar_valI(evar v) { return _v_valI(v); }
f64  evar_valF(evar v) { return _v_valF(v); }
cstr evar_valS(evar v) { return _v_valS(v); }
cptr evar_valP(evar v) { return _v_valP(v); }
cptr evar_valR(evar v) { return _v_valR(v); }

uint evar_lenS(evar v) { return __evar_lenS(v); }
uint evar_lenR(evar v) { return __evar_lenR(v); }

evar evar_i    (evar v, uint idx)
{
    evarp vp = _v_isptr(v) ? _v_vp(v) : &v;

    is1_ret(idx >= vp->cnt, EVAR_NAV);

    switch (_vp_type(vp))
    {
        case E_STR :
        case E_RAW : return (evar){0, vp->type, vp->esize, 1, EVAL_P(_vp_ival(vp, idx)->s) };
        case E_USER: return _v_isptr(v) ? (evar){0, vp->type, vp->esize, 1, EVAL_P(_vp_ival(vp, idx)->r) }
                                        : (evar){0, vp->type, vp->esize, 1, EVAL_P(_vp_ival(vp, idx)->s) };
        default    : switch (vp->esize) {
                        case  1: return (evar){0,vp->type, vp->esize, 1, EVAL_I8 (_vp_ival(vp, idx)->i8 ) };
                        case  2: return (evar){0,vp->type, vp->esize, 1, EVAL_I16(_vp_ival(vp, idx)->i16) };
                        case  4: return (evar){0,vp->type, vp->esize, 1, EVAL_I32(_vp_ival(vp, idx)->i32) };
                        case  8: return (evar){0,vp->type, vp->esize, 1, EVAL_I64(_vp_ival(vp, idx)->i64) };

                        default: return EVAR_NAV;     // this should not happen
                     }
    }

    return EVAR_NAV;
}

cptr evar_iPtr (evar v, uint idx)
{
    if(_v_isptr(v))
    {
        evarp vp = _v_vp(v);

        is1_ret(idx >= vp->cnt, 0);

        switch (_vp_type(vp))
        {
            case E_STR :
            case E_RAW : return _vp_ival(vp, idx)->s;
            default    : return _vp_ival(vp, idx)->r;
        }
    }

    is1_ret(idx >= v.cnt, 0);

    switch (_v_type(v))
    {
        case E_STR :
        case E_RAW :
        case E_USER: return _v_ival(v, idx)->s;

        default    : break;

    }

    return 0;
}

cptr evarp_iPtr (evarp vp, uint idx)
{
    if(_vp_isptr(vp))
    {
        vp = _v_vp(*vp);
    }

    is1_ret(idx >= vp->cnt, 0);

    switch (_vp_type(vp))
    {
        case E_STR :
        case E_RAW : return _vp_ival(vp, idx)->s;

        default    : if(_vp_type(vp) < E_USER)
                         return _vp_ival(vp, idx)->r;

                     return _vp_isarr(vp) ? _vp_ival(vp, idx)->r
                                          : _vp_ival(vp, idx)->s;

    }

    return 0;
}

eval evar_iVal (evar v, uint idx) { return evar_i(v, idx).v; }
i64  evar_iValI(evar v, uint idx) { return __evar_valI(evar_i(v, idx)); }
f64  evar_iValF(evar v, uint idx) { return __evar_valF(evar_i(v, idx)); }
cstr evar_iValS(evar v, uint idx) { return __evar_valS(evar_i(v, idx)); }
cptr evar_iValP(evar v, uint idx) { return __evar_valP(evar_i(v, idx)); }
cptr evar_iValR(evar v, uint idx) { return __evar_valR(evar_i(v, idx)); }

uint evar_iLenS(evar v, uint idx) { return __evar_lenS(evar_i(v, idx)); }
uint evar_iLenR(evar v, uint idx) { return __evar_lenR(evar_i(v, idx)); }

static uint memcmpl(constr pa, constr pb, uint len)
{
    uint i = 0;

    u64* pa_ = (u64*)pa;
    u64* pb_ = (u64*)pb;

    while(len - i > 8)
    {
        if(*pa_ != *pb_)
            break;

        pa_++;
        pb_++;
        i += 8;
    }

    for(; i < len; i++)
        if(pa[i] != pb[i])
            return i;

    return 0;
}

int  evar_cmp(evar a, evar b)
{
    int ta = _v_type(a);

    if(ta != _v_type(b))
        return (ta - _v_type(a)) > 0 ? 1 : -1;

    evarp vpa = _v_isptr(a) ? a.v.p : a.v.r;
    evarp vpb = _v_isptr(b) ? b.v.p : b.v.r;
    uint  ca  = vpa->cnt;
    uint  cb  = vpb->cnt;
    uint  cnt = ca > cb ? cb : ca;
    uint  pos;

    if(cnt == 1)
    {
        switch (_vp_type(vpa))
        {
            case E_I8  : if(vpa->v.i8  != vpb->v.i8 ) return vpa->v.i8  > vpb->v.i8  ? 1 : -1;
            case E_I16 : if(vpa->v.i16 != vpb->v.i16) return vpa->v.i16 > vpb->v.i16 ? 1 : -1;
            case E_I32 : if(vpa->v.i32 != vpb->v.i32) return vpa->v.i32 > vpb->v.i32 ? 1 : -1;
            case E_I64 : if(vpa->v.i64 != vpb->v.i64) return vpa->v.i64 > vpb->v.i64 ? 1 : -1;
            case E_U8  : if(vpa->v.u8  != vpb->v.u8 ) return vpa->v.u8  > vpb->v.u8  ? 1 : -1;
            case E_U16 : if(vpa->v.u16 != vpb->v.u16) return vpa->v.u16 > vpb->v.u16 ? 1 : -1;
            case E_U32 : if(vpa->v.u32 != vpb->v.u32) return vpa->v.u32 > vpb->v.u32 ? 1 : -1;
            case E_U64 : if(vpa->v.u64 != vpb->v.u64) return vpa->v.u64 > vpb->v.u64 ? 1 : -1;
            case E_F32 : if(vpa->v.f32 != vpb->v.f32) return vpa->v.f32 > vpb->v.f32 ? 1 : -1;
            case E_F64 : if(vpa->v.f64 != vpb->v.f64) return vpa->v.f64 > vpb->v.f64 ? 1 : -1;

            case E_PTR : if(vpa->v.p   != vpb->v.p  ) return vpa->v.p   > vpb->v.p   ? 1 : -1;

            case E_STR : { int r = estr_cmp(vpa->v.s, vpb->v.s); if(r) return r; }
            case E_RAW : { int r = estr_cmp(vpa->v.s, vpb->v.s); if(r) return r; }

            case E_USER: { int r = memcmp(vpa->v.r, vpb->v.r, vpa->esize); if(r) return r; }
        }
    }
    else
    {
        uint len = cnt * vpa->esize;

        pos = memcmpl(vpa->v.r, vpb->v.r, len);

        if(pos != len)
        {
            pos = pos / vpa->esize;

            switch (_vp_type(vpa))
            {
                case E_I8  : return vpa->v.i8a [pos] - vpb->v.i8a [pos] > 0 ? 1 : -1;
                case E_I16 : return vpa->v.i16a[pos] - vpb->v.i16a[pos] > 0 ? 1 : -1;
                case E_I32 : return vpa->v.i32a[pos] - vpb->v.i32a[pos] > 0 ? 1 : -1;
                case E_I64 : return vpa->v.i64a[pos] - vpb->v.i64a[pos] > 0 ? 1 : -1;
                case E_U8  : return vpa->v.u8a [pos] - vpb->v.u8a [pos] > 0 ? 1 : -1;
                case E_U16 : return vpa->v.u16a[pos] - vpb->v.u16a[pos] > 0 ? 1 : -1;
                case E_U32 : return vpa->v.u32a[pos] - vpb->v.u32a[pos] > 0 ? 1 : -1;
                case E_U64 : return vpa->v.u64a[pos] - vpb->v.u64a[pos] > 0 ? 1 : -1;
                case E_F32 : return vpa->v.f32a[pos] - vpb->v.f32a[pos] > 0 ? 1 : -1;
                case E_F64 : return vpa->v.f64a[pos] - vpb->v.f64a[pos] > 0 ? 1 : -1;

                case E_PTR : return vpa->v.sa  [pos] - vpb->v.sa  [pos] > 0 ? 1 : -1;

                case E_STR : { int r = estr_cmp(vpa->v.sa[pos], vpb->v.sa[pos]); if(r) return r; }
                case E_RAW : { int r = estr_cmp(vpa->v.sa[pos], vpb->v.sa[pos]); if(r) return r; }

                case E_USER: { int r = memcmp(_vp_ival(vpa, pos)->r, _vp_ival(vpb, pos)->r, vpa->esize); if(r) return r; }
            }
        }
    }

    if(ca == cb)
        return 0;

    return (ca - cb) > 0 ? 1 : -1;
}
