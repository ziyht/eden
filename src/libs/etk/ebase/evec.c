/// =====================================================================================
///
///       Filename:  evec.h
///
///    Description:  easy vector for c
///
///        Version:  1.0.2
///        Created:  04/01/2019 08:51:34 AM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#include <string.h>
#include <assert.h>

#include "ealloc.h"
#include "etype.h"
#include "eutils.h"
#include "estr.h"
#include "eobj_p.h"

#include "libs/estr_p.h"

#include "evec.h"

#define EVEC_VERSION "evec 1.0.2"       // fix bug and do some optimization

#define _EVEC_CHECK 1   // debug using

#pragma pack(1)

typedef struct _evec_split_s * _evec_split, * _split;
typedef struct _evec_split_s{

    //!
    //!              |-------------- fpos: 3
    //!              |           |-- rpos: 7
    //!              |           |
    //!     |  |  |  |**|**|**|**|  |  |
    //!     0  1  2  3  4  5  6  7  8  9    cap : 10
    //!
    u32         cap;        // current capacity of this split
    u32         fpos;       // pos of first elem
    u32         rpos;       // pos after the last elem

    u16         esize;      // elem size

    eval        base;
}_evev_split_t;

typedef struct _pos_s
{
    //! input
    evec        v;      // evec handle
    constr      in;
    uint        inlen;
    uint        cnt;

    //! target
    _split      s;      // split
    uint        pos;    // postion (direct)
}_pos_t, * _pos;

#pragma pack()

#define _split_esize(sp)        (sp)->esize
#define _split_fpos(sp)         (sp)->fpos
#define _split_rpos(sp)         (sp)->rpos
#define _split_cap(sp)          (sp)->cap
#define _split_base(sp)         (sp)->base.s

#define _split_len(sp)          (_split_rpos(sp) - _split_fpos(sp))
#define _split_avail(sp)        (_split_cap(sp) - (_split_len(sp)))

#define _split_msize(sp, cnt)   ((cnt) * _split_esize(sp))

#define _split_ppos(sp, p)      ((p) % _split_cap(sp))                                       // pos -> real pos
#define _split_pptr(sp, p)      (_split_base(sp) + _split_ppos(sp, p) * _split_esize(sp))    // pos -> real ptr

#define _split_ipos(sp, i)      _split_ppos(sp, _split_fpos(sp) + (i))                       // idx -> real pos
#define _split_iptr(sp, i)      _split_pptr(sp, _split_fpos(sp) + (i))                       // idx -> real ptr
#define _split_ival(sp, i)      (*(eval*)_split_iptr(sp, i))                                 // idx -> eval

#define _split_fptr(sp)         (_split_base(sp) + (_split_fpos(sp) * _split_esize(sp)))                        // ptr of front (first element)
#define _split_rptr(sp)         (_split_base(sp) + (_split_rpos(sp) * _split_esize(sp)))                        // ptr of rear
#define _split_lptr(sp)         (_split_base(sp) + (_split_ppos(sp, (_split_rpos(sp) - 1)) * _split_esize(sp))) // ptr of last element
#define _split_eptr(sp)         (_split_base(sp) + (_split_cap(sp) * _split_esize(sp)))                         // ptr of base end

#define __split_fpos_decr(s, cnt)           \
do{                                         \
    _split_fpos(s) -= cnt;                  \
    if(_split_fpos(s) == UINT_MAX)          \
    {                                       \
        _split_fpos(s) += _split_cap(s);    \
        _split_rpos(s) += _split_cap(s);    \
    }                                       \
}while(0)

#define __split_fpos_incr(s, cnt)           \
do{                                         \
    _split_fpos(s) += cnt;                  \
    if(_split_fpos(s) >= _split_cap(s))     \
    {                                       \
        _split_fpos(s) -= _split_cap(s);    \
        _split_rpos(s) -= _split_cap(s);    \
    }                                       \
}while(0)

#define __split_rpos_decr(s, cnt)           \
do{                                         \
    _split_rpos(s) -= cnt;                  \
}while(0)

#define __split_rpos_incr(s, cnt)           \
do{                                         \
    _split_rpos(s) += cnt;                  \
}while(0)

#pragma pack(1)

typedef struct evec_s
{
    _evev_split_t   s;

    //! data
    u32             reserves;           // how many item rooms wanted to reserve
    u8              type;               // etypev
}evec_t;

#pragma pack()

#define _v_cnt(v)           _split_len((&v->s))
#define _v_cap(v)           _split_cap((&v->s))
#define _v_esize(v)         _split_esize((&v->s))
#define _v_reserve(v)       (v)->reserves
#define _v_type(v)          (v)->type
#define _v_avail(v)         _split_avail((&v->s))

#define EVEC_MAX_PREALLOC   (1024 * 1024)       // 1M

static void __split_init(_split s, int need_rooms, int esize)
{
    if(need_rooms * esize < 8)
    {
        need_rooms = pow2gt(8 / esize);
    }

    _split_cap(  s) = need_rooms;
    _split_esize(s) = esize;
    _split_base( s) = ecalloc(need_rooms, esize);
}

static __always_inline void __split_adjust_data(_split s, int appd_rooms, _pos p)
{
    int movef = _split_cap(s)  - _split_fpos(s);
    int mover = _split_rpos(s) - _split_cap(s);

    if(movef <= mover)
    {
        if(movef <= appd_rooms)
        {
            memcpy (_split_base(s) + (_split_cap(s) + appd_rooms - movef) * _split_esize(s), _split_fptr(s) ,_split_msize(s, movef));
        }
        else
        {
            memmove(_split_base(s) + (_split_cap(s) + appd_rooms - movef) * _split_esize(s) ,_split_fptr(s) ,_split_msize(s, movef));
        }

        _split_fpos(s) += appd_rooms;
        _split_rpos(s) += appd_rooms;
        p->pos         += appd_rooms;
    }
    else
    {
        if(mover <= appd_rooms)
        {
            memcpy(_split_eptr(s), _split_base(s), _split_msize(s, mover));
        }
        else
        {
            memcpy(_split_eptr(s), _split_base(s), _split_msize(s, appd_rooms));

            mover -= appd_rooms;

            memmove(_split_base(s), _split_pptr(s, appd_rooms), _split_msize(s, appd_rooms));

            _split_rpos(s) -= appd_rooms;   // wrong
        }
    }
}

static uint __split_expand(_split s, int need_rooms, _pos p)
{
    uint new_space; cstr new_base;

    new_space =  _split_esize(s) * (need_rooms + _split_cap(s));

    if(new_space < EVEC_MAX_PREALLOC) new_space  = pow2gt(new_space);
    else                              new_space += EVEC_MAX_PREALLOC;

    new_base = erealloc(_split_base(s), new_space);

    if(!new_base)
    {
        return 0;
    }
    _split_base(s) = new_base;

    need_rooms = new_space / _split_esize(s) - _split_cap (s);    // record appd rooms
    if( _split_rpos(s) >  _split_cap (s))
        __split_adjust_data(s, need_rooms, p);

    _split_cap (s) = new_space / _split_esize(s);

    return need_rooms;
}

/**
 * @brief __split_squeeze_in_self
 *
 *      -- squeeze space to write, be sure there have enough sapce to squeeze before call it
 *
 * @param s   - split to squeeze
 * @param pos - direct position where to write data
 * @param cnt - the elements count wanted to write
 * @return    - the pos for write
 */
static int __split_squeeze_in_self(_split s, uint pos, uint cnt)
{
    //! 1. no need to move
    if(pos == _split_rpos(s))
    {
        __split_rpos_incr(s, cnt);

        return pos;
    }

    //! 2. no need to move
    if(pos == _split_fpos(s))
    {
        __split_fpos_decr(s, cnt);

        return _split_fpos(s);
    }

    //! 3. move front is more wisely
    if(pos - _split_fpos(s) <= _split_rpos(s) - pos)
    {
        int front = _split_fpos(s) - cnt;

        if(front >= 0)
        {
            if(pos <= _split_cap(s))
            {
                //!    |*    ******|
                //!           ^^^

                //! -> |* *___*****|
                memmove(_split_pptr(s, front), _split_fptr(s), _split_msize(s, pos - _split_fpos(s)));
                pos -= cnt;
            }
            else if(pos > _split_cap(s))
            {
                //!     |*********    *|       |*********    *|
                //!          ^^                      ^

                //! ->  |*********  *__|       |*********   *_|
                //!          ^^                      ^
                memmove(_split_pptr(s, front)              , _split_fptr(s), _split_msize(s, _split_cap(s) - _split_fpos(s)));

                //! ->  |__*******  ***|       |_********   **|
                //!          ^^                      ^
                memcpy (_split_pptr(s, _split_cap(s) - cnt), _split_base(s), _split_msize(s, cnt));

                //! ->  |__*******  ***|       |_********   **|
                //!        ^^                       ^
                pos = pos - cnt;

                //! ->  |**__*****  ***|       |****_****   **|
                //!        ^^                       ^
                memmove(_split_base(s), _split_pptr(s, pos), _split_msize(s, cnt));
            }

             _split_fpos(s) = front;
        }
        else
        {
            //!    | ********  |
            //!       ^^^

            //! first step
            memcpy (_split_pptr(s, _split_cap(s) + front), _split_fptr(s), _split_msize(s, -front));

            //! second step
            memmove(_split_base(s), _split_pptr(s, _split_fpos(s) - front), _split_msize(s, pos + front));

            pos -= _split_fpos(s);  // convert to idx

            _split_fpos(s) = _split_cap(s) + front;
            _split_rpos(s) = _split_cap(s) + _split_rpos(s);

            return pos += _split_fpos(s);   // convert to pos
        }
    }

    //! 4. move rear is more wisely
    else
    {
        uint rear = _split_rpos(s) + cnt;

        if(rear > _split_cap(s))
        {
            if(pos >= _split_cap(s))
            {
                //!    |*******      **|
                //!        ^^^

                memmove(_split_pptr(s, pos + cnt), _split_pptr(s, pos), _split_msize(s, _split_rpos(s) - pos));
            }
            else if(_split_rpos(s) > _split_cap(s))
            {
                //!    |**     *****|
                //!             ^^^

                //! -> |___**  *****|
                //!             ^^^
                memmove(_split_pptr(s, cnt), _split_base(s), _split_msize(s, _split_rpos(s) % _split_cap(s)));

                //! -> |*****  **___|
                //!             ^^^
                memcpy(_split_base(s),       _split_pptr(s, _split_cap(s) - cnt), _split_msize(s, cnt));

                //! -> |*****  *___*|
                //!             ^^^
                int move3 =  _split_cap(s) - pos - cnt;
                memmove(_split_pptr(s, _split_cap(s) - move3), _split_pptr(s, pos), _split_msize(s, move3));
            }
            else
            {
                //!    |   *******  |
                //!          ^^^

                //! -> |*  ******   |
                //!          ^^^
                int move1 = rear % _split_cap(s);
                memcpy(_split_base(s),   _split_pptr(s, _split_rpos(s) - move1), _split_msize(s, move1));

                //! -> |*  **___****|
                //!          ^^^
                memmove(_split_pptr(s, pos + cnt), _split_pptr(s, pos), _split_msize(s, _split_rpos(s) - pos - move1));
            }
        }
        else
        {
            //!    |  *******   |
            //!          ^^^

            //! -> |  ******___*|
            memmove(_split_pptr(s, pos + cnt), _split_pptr(s, pos), _split_msize(s, _split_rpos(s) - pos));
        }

        _split_rpos(s) = rear;
    }

    return pos;
}

/**
 * @brief __split_write_hard
 *
 *      -- write data to split, be sure there have enough sapce to write
 *
 * @param s     - split to write
 * @param in    - ptr of input data
 * @param inlen - len of input data
 * @param pos   - the direct pos to write
 * @param cnt   - element cnt to write
 */

static void __split_write_hard(_split s, int pos, constr in, uint inlen, uint cnt)
{
    pos = __split_squeeze_in_self(s, pos, cnt);

    if(!in)
        return;

    pos = _split_ppos(s, pos);

    if(pos + cnt <= s->cap)
    {
        memcpy(_split_pptr(s, pos), in, inlen > (_split_esize(s) * cnt) ? (_split_esize(s) * cnt) : inlen);
    }
    else
    {
        //! first step
        uint can_copy = _split_esize(s) * (s->cap - pos);

        //! second step
        if(can_copy >= inlen)
        {
            memcpy(_split_pptr(s, pos), in , inlen);
        }
        else
        {
            memcpy(_split_pptr(s, pos), in           , can_copy);

            memcpy(_split_base(s)     , in + can_copy, inlen - can_copy);
        }
    }
}

static cptr __split_insert(_split s, _pos_t* pos)
{
    cptr out;

    int overload = pos->cnt - _split_avail(&pos->v->s);

    if(overload > 0)
    {
        if(!__split_expand(s, overload, pos))
            return false;
    }

    out = _split_pptr(pos->s, pos->pos);

    __split_write_hard(pos->s, pos->pos, pos->in, pos->inlen, pos->cnt);

    return out;
}

static bool __split_erase_room(_split s, _pos_t* pos)
{
    uint can_erase;

    can_erase = _split_rpos(s) - pos->pos;

    if(pos->cnt > can_erase)
        pos->cnt = can_erase;

    {
        uint move_f = pos->pos  - _split_fpos(s);
        uint move_r = can_erase - pos->cnt;

        if(move_f < move_r)
        {
            uint ffto, frto;

            //! adjust pos so next operation will be more easier
            if(pos->pos > _split_cap(s))
            {
                /**
                 *    __
                 *  |*00****.....**|
                 *
                 *  ->
                 *   __
                 *  |00*****.....**|
                 *
                 */
                memmove(_split_base(s), _split_base(s) + _split_esize(s) * pos->cnt, _split_esize(s) * pos->cnt);

                pos->pos = _split_cap(s);
            }

            ffto = _split_fpos(s) + pos->cnt;
            frto = pos->pos + pos->cnt;

            if(ffto < _split_cap(s) && frto > _split_cap(s))
            {
                //! step 1:
                memcpy(_split_base(s), _split_fptr(s) + (pos->pos - (frto - _split_cap(s))) * _split_esize(s), (frto - _split_cap(s)) * _split_esize(s));

                //! step 2:
                memmove(_split_pptr(s, ffto), _split_fptr(s),  (pos->cnt - frto - _split_cap(s)) * _split_esize(s));
            }
            else
            {
                memmove(_split_fptr(s) + _split_esize(s) * pos->cnt, _split_fptr(s), _split_esize(s) * pos->cnt);
            }

            __split_fpos_incr(s, pos->cnt);
        }
        else
        {
            uint rfto, rrto;

            if(pos->pos + pos->cnt < _split_cap(s) && _split_rpos(s) >= _split_cap(s) )
            {
                /**
                 *            ___
                 *  |**...****000*|
                 *
                 *  ->
                 *             ___
                 *  |**...*****000|
                 *
                 */

                int moves = _split_cap(s) - pos->pos - pos->cnt;

                memmove(_split_pptr(s, pos->pos), _split_pptr(s, _split_cap(s) - moves), _split_msize(s, moves));

                pos->pos -= moves;
            }

            rfto = pos->pos;
            rrto = _split_rpos(s) - pos->cnt;

            if(rfto < _split_cap(s) && rrto > _split_cap(s))
            {
                int rfpos = pos->pos + pos->cnt;

                //! step 1:
                memcpy(_split_pptr(s, rfto), _split_pptr(s, rfpos), _split_msize(s, _split_cap(s) - rfto));

                //! step 2:
                memmove(_split_base(s), _split_pptr(s, rfpos + _split_cap(s) - rfto), _split_msize(s, _split_rpos(s) - rfpos - (_split_cap(s) - rfto)) );
            }
            else
            {
                memmove(_split_pptr(s, rfto), _split_pptr(s, pos->pos + pos->cnt), _split_msize(s, pos->cnt));
            }

            __split_rpos_decr(s, pos->cnt);
        }

        return true;
    }

    return false;
}

/// -----------------------------------------------------------------------
//! evec basic
///

static evec __evec_new(etypev type, int size);

evec evec_new(etypev type, u16 esize)
{
    static const u8 _size_map[] = __EVAR_ITEM_LEN_MAP;

    if(type == E_NAV)
        return 0;

    if(type < E_USER)
    {
        return __evec_new(type, _size_map[type]);
    }

    if(esize == 0)
        return 0;

    return esize > 0 ? __evec_new(E_USER, esize) : 0;
}

static evec __evec_new(etypev type, int esize)
{
    evec out = ecalloc(1, sizeof(*out));

    __split_init(&out->s, 1, esize);

    _v_type  (out) = type;

    return out;
}

uint   evec_len  (evec v)   { return v ? _v_cnt  (v) :     0; }
uint   evec_cap  (evec v)   { return v ? _v_cap  (v) :     0; }
uint   evec_esize(evec v)   { return v ? _v_esize(v) :     0; }
uint   evec_avail(evec v)   { return v ? _v_avail(v) :     0; }
etypev evec_type (evec v)   { return v ? _v_type (v) : E_NAV; }
constr evec_typeS(evec v)   { return evar_typeS((evar){0, evec_type(v), 0, 0, EVAL_0}); }

/// ------------ evec clear and free ---------------------
///
///
///

static void __split_clear   (evec v, _split s);
static void __split_clear_ex(evec v, _split s, eobj_rls_ex_cb rls, eval prvt);
static void __split_free    (evec v, _split s);
static void __split_free_ex (evec v, _split s, eobj_rls_ex_cb rls, eval prvt);

int evec_clear  (evec v){ return evec_clearEx(v, 0, EVAL_ZORE);}
int evec_clearEx(evec v, eobj_rls_ex_cb rls, eval prvt)
{
    int cnt;

    is0_ret(v, 0);

    cnt = _v_cnt(v);

    if(rls) __split_clear_ex(v, &v->s, rls, prvt);
    else    __split_clear   (v, &v->s);

    return cnt;
}

int  evec_free  (evec v){ return evec_freeEx(v, 0, EVAL_ZORE); }
int  evec_freeEx(evec v, eobj_rls_ex_cb rls, eval prvt)
{
    int cnt;

    is0_ret(v, 0);

    cnt = _v_cnt(v);

    if(rls) __split_free_ex(v, &v->s, rls, prvt);
    else    __split_free   (v, &v->s);

    efree(v);

    return cnt ? cnt : -1;
}

static void __split_clear(evec v, _split s)
{
    switch (_v_type(v))
    {
        case E_STR:
        case E_RAW: {
                        uint pos;

                        for(pos = _split_fpos(s) ; pos < _split_rpos(s); pos++)
                        {
                            _s_free( *(estr*)_split_pptr(s, pos) );
                        }
                    }
                    break;
    }

    if(_split_rpos(s) >= _split_cap(s))
    {
        memset(_split_fptr(s), 0, _split_eptr(s) - _split_fptr(s));
        memset(_split_base(s), 0, _split_rptr(s) - _split_base(s));
    }
    else
    {
        memset(_split_fptr(s), 0, _split_avail(s) * _split_esize(s));
    }

    _split_fpos(s) = 0;
    _split_rpos(s) = 0;
}

static void __split_clear_ex (evec v, _split s, eobj_rls_ex_cb rls, eval prvt)
{
    switch (_v_type(v))
    {
        case E_STR: {
                        uint pos;

                        for(pos = _split_fpos(s) ; pos < _split_rpos(s); pos++)
                        {
                            _s_free( *(estr*)_split_pptr(s, pos) );
                        }
                    }
                    break;

        case E_RAW: {
                        uint pos; _enode_t b;

                        _ehdr_type_oe(b.hdr) = _ERAW_P;

                        for(pos = _split_fpos(s) ; pos < _split_rpos(s); pos++)
                        {
                            b.obj.s    = *(estr*)_split_pptr(s, pos);
                            b.hdr._len = (uint)_s_len(b.obj.s);

                            rls(&b.obj, prvt);

                            _s_free( b.obj.s );
                        }
                    }
                    break;

        case E_USER:{
                        uint pos; _enode_t b;

                        _ehdr_type_oe(b.hdr) = _ERAW_P;

                        for(pos = _split_fpos(s) ; pos < _split_rpos(s); pos++)
                        {
                            b.obj.s    = _split_pptr(s, pos);
                            b.hdr._len = (uint)_s_len(b.obj.s);

                            rls(&b.obj, prvt);
                        }
                    }
                    break;
    }

    if(_split_rpos(s) >= _split_cap(s))
    {
        memset(_split_fptr(s), 0, _split_eptr(s) - _split_fptr(s));
        memset(_split_base(s), 0, _split_rptr(s) - _split_base(s));
    }
    else
    {
        memset(_split_fptr(s), 0, _split_avail(s) * _split_esize(s));
    }

    _split_fpos(s) = 0;
    _split_rpos(s) = 0;
}

static void __split_foreach_free_raw(_split s)
{
    if(_split_len(s) > 0)
    {
        estr* s_p, * end;

        if(_split_rpos(s) > _split_len(s))
        {
            s_p = (estr*)_split_base(s);
            end = (estr*)_split_lptr(s);

            while(s_p <= end)
            {
                _s_free(*s_p);
                s_p++;
            }

            s_p = (estr*)_split_fptr(s);
            end = (estr*)_split_eptr(s);

            while(s_p < end)
            {
                _s_free(*s_p);
                s_p++;
            }
        }
        else
        {
            s_p = (estr*)_split_fptr(s);
            end = (estr*)_split_lptr(s);

            while(s_p <= end)
            {
                _s_free(*s_p);
                s_p++;
            }
        }
    }
}

static void __split_free(evec v, _split s)
{
    if(_split_len(s) > 0)
    {
        switch (_v_type(v))
        {
            case E_STR:
            case E_RAW: __split_foreach_free_raw(s);

                        break;
        }
    }

    efree(_split_base(s));
}

static void __split_free_ex(evec v, _split s, eobj_rls_ex_cb rls, eval prvt)
{
    switch (_v_type(v))
    {
        case E_STR: __split_foreach_free_raw(s);

                    break;

        case E_RAW: {
                        uint pos; _enode_t b;

                        _ehdr_type_oe(b.hdr) = _ERAW_P;

                        for(pos = _split_fpos(s) ; pos < _split_rpos(s); pos++)
                        {
                            b.obj.s    = _split_pptr(s, pos);
                            b.hdr._len = (uint )_s_len(b.obj.s);

                            rls(&b.obj, prvt);

                            _s_free( b.obj.s );
                        }
                    }
                    break;

        case E_USER:{
                        uint pos; _enode_t b;

                        _ehdr_type_oe(b.hdr) = _ERAW_P;

                        for(pos = _split_fpos(s) ; pos < _split_rpos(s); pos++)
                        {
                            b.obj.s    = _split_pptr(s, pos);
                            b.hdr._len = (uint)_s_len(b.obj.s);

                            rls(&b.obj, prvt);
                        }
                    }
                    break;
    }

    efree(_split_base(s));
}

/// -----------------------------------------------------
//! evec add
///
///

static bool __evec_addV   (evec v, uint idx, evar var);

bool evec_pushV(evec v, evar   var) { return __evec_addV(v, 0       ,          var ); }
bool evec_pushI(evec v, i64    val) { return __evec_addV(v, 0       , EVAR_I64(val)); }
bool evec_pushF(evec v, f64    val) { return __evec_addV(v, 0       , EVAR_F64(val)); }
bool evec_pushS(evec v, constr str) { return __evec_addV(v, 0       , EVAR_CS (str)); }
bool evec_pushP(evec v, conptr ptr) { return __evec_addV(v, 0       , EVAR_CP (ptr)); }
bool evec_pushR(evec v, uint   len) { return __evec_addV(v, 0       , EVAR_RAW(0, len)); }

bool evec_appdV(evec v, evar   var) { return __evec_addV(v, UINT_MAX,          var ); }
bool evec_appdI(evec v, i64    val) { return __evec_addV(v, UINT_MAX, EVAR_I64(val)); }
bool evec_appdF(evec v, f64    val) { return __evec_addV(v, UINT_MAX, EVAR_F64(val)); }
bool evec_appdS(evec v, constr str) { return __evec_addV(v, UINT_MAX, EVAR_CS (str)); }
bool evec_appdP(evec v, conptr ptr) { return __evec_addV(v, UINT_MAX, EVAR_CP (ptr)); }
bool evec_appdR(evec v, uint   len) { return __evec_addV(v, UINT_MAX, EVAR_RAW(0, len)); }

bool evec_addV(evec v, uint i, evar   var) { return __evec_addV(v, i,          var ); }
bool evec_addI(evec v, uint i, i64    val) { return __evec_addV(v, i, EVAR_I64(val)); }
bool evec_addF(evec v, uint i, f64    val) { return __evec_addV(v, i, EVAR_F64(val)); }
bool evec_addS(evec v, uint i, constr str) { return __evec_addV(v, i, EVAR_CS (str)); }
bool evec_addP(evec v, uint i, conptr ptr) { return __evec_addV(v, i, EVAR_CP (ptr)); }
bool evec_addR(evec v, uint i, uint   len) { return __evec_addV(v, i, EVAR_RAW(0, len)); }

static bool __evec_addB(evec v, uint idx, constr in, uint len, int cnt)
{
    _pos_t p = {v, in, len, cnt, &v->s, 0};

    if(idx == 0)
    {
        p.pos = _split_fpos(p.s);
    }
    else if(idx >= _v_cnt(v))
    {
        p.pos = _split_rpos(p.s);
    }
    else
        p.pos = _split_fpos(p.s) + idx;

    return __split_insert(p.s, &p);
}

static bool __evec_addV(evec v, uint idx, evar var)
{
    int v_type;

    is1_ret(!v, false);

    v_type = var.type;

    if(v_type != _v_type(v))
        return false;

    if(v_type >= E_USER)
    {
        return __evec_addB(v, idx, evar_iPtr(var, 0), var.esize, evar_cnt(var));
    }

    switch(v_type)
    {
        case E_STR :    var.v.s = estr_dupS(var.v.s  );
                        break;

        case E_RAW :    var.v.p   = estr_newLen(var.v.p, var.esize);
                        var.esize = 8;
                        break;
    }

    return __evec_addB(v, idx, var.v.r, var.esize, var.cnt ? var.cnt : 1);
}

/// -----------------------------------------------------
//! evec val
///
///

#define  __evec_i_ret_eval(v, idx)                                                  \
do                                                                                  \
{                                                                                   \
    switch (_v_type(v))                                                             \
    {                                                                               \
        case E_STR :                                                                \
        case E_RAW : return _split_ival(&v->s, idx);                                \
        case E_USER: return EVAL_P(_split_ival(&v->s, idx).r);                      \
        default    : switch (v->s.esize) {                                          \
                        case  1: return EVAL_I8 (_split_ival(&v->s, idx).i8 );      \
                        case  2: return EVAL_I16(_split_ival(&v->s, idx).i16);      \
                        case  4: return EVAL_I32(_split_ival(&v->s, idx).i32);      \
                        case  8: return EVAL_I64(_split_ival(&v->s, idx).i64);      \
                                                                                    \
                        default: return EVAL_0;     /*this should not happen*/      \
                     }                                                              \
    }                                                                               \
}while(0)

eval evec_first(evec v) { is1_ret(!v, EVAL_0);                           __evec_i_ret_eval(v,   0); return EVAL_0; }
eval evec_last (evec v) { is1_ret(!v, EVAL_0); uint idx = _v_cnt(v) - 1; __evec_i_ret_eval(v, idx); return EVAL_0; }

eval evec_i    (evec v, uint idx)
{
    is1_ret(!v || idx >= _v_cnt(v), EVAL_0);

    cstr s = _split_ival(&v->s, idx).r;

    __evec_i_ret_eval(v, idx);

    return EVAL_0;
}

cptr evec_iPtr (evec v, uint idx)
{
    is1_ret(!v || idx >= _v_cnt(v), 0);

    switch (_v_type(v))
    {
        case E_STR :
        case E_RAW : return _split_ival(&v->s, idx).s;
        default    : return _split_ival(&v->s, idx).r;
    }

    return 0;
}


i64  evec_iValI(evec v, uint idx){ is1_ret(!v || idx >= _v_cnt(v), 0); return (_v_type(v) == E_I64) ? _split_ival(&v->s, idx).i64 : (_v_type(v) == E_F64) ? (i64)_split_ival(&v->s, idx).f64 : 0; }
f64  evec_iValF(evec v, uint idx){ is1_ret(!v || idx >= _v_cnt(v), 0); return (_v_type(v) == E_F64) ? _split_ival(&v->s, idx).f64 : (_v_type(v) == E_I64) ? (f64)_split_ival(&v->s, idx).i64 : 0; }
cstr evec_iValS(evec v, uint idx){ is1_ret(!v || idx >= _v_cnt(v), 0); return (_v_type(v) == E_STR) ? _split_ival(&v->s, idx).s : 0; }
cptr evec_iValP(evec v, uint idx){ is1_ret(!v || idx >= _v_cnt(v), 0); return (_v_type(v) == E_PTR) ? _split_ival(&v->s, idx).p : 0; }
cptr evec_iValR(evec v, uint idx){ is1_ret(!v || idx >= _v_cnt(v), 0); return (_v_type(v) == E_RAW) ? _split_ival(&v->s, idx).s : 0; }

/// -----------------------------------------------------
//! evec take
///
///

/**
 * @brief __evec_take_var
 *
 *  take a element from vec and returns the value
 *
 * @note:
 *  need be sure of 0 <= idx < _v_cnt(v) before call it
 *
 */

static evar __evec_take_var(evec v, uint idx, uint cnt)
{
    _pos_t p; evar var;

    if(_v_cnt(v) - idx < cnt )
        cnt = _v_cnt(v) - idx;

    //! find it
    p.pos = _split_fpos(&v->s) + idx;

    //! take it from split
    {
        p.s   = &v->s;
        p.v   = v;
        p.cnt = 1;
        __split_erase_room(p.s, &p);
    }

    //! set val to var to return
    {
        var = evar_gen(_v_type(v), cnt, _v_esize(v));
        evar_iSet(var, 0, _split_pptr(p.s, p.pos), _v_esize(v));
    }

    return var;
}

evar evec_takeH(evec v)             { is1_ret(!v ||       !_v_cnt(v), EVAR_NAV); return __evec_take_var(v,             0, 1); }
evar evec_takeT(evec v)             { is1_ret(!v ||       !_v_cnt(v), EVAR_NAV); return __evec_take_var(v, _v_cnt(v) - 1, 1); }
evar evec_takeI(evec v, uint idx)   { is1_ret(!v || idx >= _v_cnt(v), EVAR_NAV); return __evec_take_var(v,           idx, 1); }
