/// =====================================================================================
///
///       Filename:  evec.h
///
///    Description:  easy vector for c
///
///        Version:  0.1
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
#include "estr_p.h"
#include "eobj_p.h"

#include "evec.h"

#define EVEC_VERSION "evec 0.1.1"       // acomplishment of split add, but it's not live up to my expectation

#define _EVEC_CHECK 1   // debug using

#pragma pack(1)

typedef struct _evec_split_s * _evec_split, * _split;
typedef struct _evec_split_s{

    _split      prev;
    _split      next;

    //!
    //!              |-------------- front: 3
    //!              |           |-- rear : 7
    //!              |           |
    //!     |  |  |  |**|**|**|**|  |  |
    //!     0  1  2  3  4  5  6  7  8  9    cap : 10
    //!

    u16         esize;      // elem size
    u32         cap;        // current capacity of this split
    u32         front;      // pos of first elem
    u32         rear;       // pos after the last elem

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

#define _split_prev(s)          s->prev
#define _split_next(s)          s->next
#define _split_esize(s)         s->esize
#define _split_fpos(s)          s->front
#define _split_rpos(s)          s->rear
#define _split_cap(s)           s->cap
#define _split_base(_s)        _s->base.s
#define _split_mask(s)          //s->mask

#define _split_len(s)           (s->rear - s->front)
#define _split_avail(s)         (_split_cap(s) - (_split_len(s)))

#define _split_ppos(s, p)       ((p) % _split_cap(s))

#define _split_fptr(s)          (_split_base(s) + (_split_fpos(s) * _split_esize(s)))                        // ptr of front (first element)
#define _split_rptr(s)          (_split_base(s) + (_split_rpos(s) * _split_esize(s)))                        // ptr of rear
#define _split_lptr(s)          (_split_base(s) + (_split_ppos(s, (_split_rpos(s) - 1) * _split_esize(s)))   // ptr of last element
#define _split_eptr(s)          (_split_base(s) + (_split_cap(s) * _split_esize(s)))                         // ptr of base end

#define _split_msize(s, cnt)    ((cnt) * _split_esize(s))
#define _split_pptr(s, p)       (_split_base(s) + _split_ppos(s, p) * _split_esize(s))

#pragma pack(1)

typedef struct evec_s
{
    //! data
    u32         cnt;                // all element count
    u32         cap;                // all buffer  cap

    _split      head;               // point to the first split
    _split      tail;               // point to the last  split
    u32         splits;             // splits count
    u32         factor;             // factor for create, expand, shrink, merging, delete splits operations
    u16         esize;              // element size

    //! setting
    u16         max_cap;            // the max cap of a split can hold
    u32         reserves;           // how many items cap wanted to reserve

    u8          type;               // etypev
    uint        need_merging : 1;
}evec_t;

#pragma pack()

#define _v_head(v)          (v)->head
#define _v_tail(v)          (v)->tail
#define _v_cnt(v)           (v)->cnt
#define _v_cap(v)           (v)->cap
#define _v_esize(v)         (v)->esize
#define _v_splits(v)        (v)->splits
#define _v_factor(v)        (v)->factor
#define _v_reserve(v)       (v)->reserves
#define _v_type(v)          (v)->type

#define EVEC_MAX_PREALLOC   (1024 * 1024)       // 1M
#define EVEC_MAX_FACTOR     31


//!  factor setting
#define _merge_factor(f)    (f * 2)
#define _split_factor(f)    0
#define _incre_factor(f)    (f > 8 ? 8 * 8 * 8 : f * f * f)

#define _expand_ceiling(f)  (1 << f)

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


static _split __split_new(int need_rooms, int esize)
{
    _split s = ecalloc(1, sizeof(*s));

    while(need_rooms * esize < 8)
    {
        need_rooms = pow2gt(need_rooms);
    }

    _split_cap(  s) = need_rooms;
    _split_esize(s) = esize;
    _split_base( s) = ecalloc(need_rooms, esize);
    //_split_mask( s) = need_rooms - 1;

    return s;
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

    if(new_space > _expand_ceiling(p->v->factor))
        return 0;

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
    //_split_mask(s) = _split_cap (s) - 1;

    _v_cap(p->v) += need_rooms;

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

static void __split_appd_hard(_split s, constr in, int cnt)
{
    __split_write_hard(s, _split_rpos(s), in, INT_MAX, cnt);
}

static int __split_appd_safe(_split s, constr in, uint cnt)
{
    if(cnt > _split_avail(s))
        cnt = _split_avail(s);

    __split_appd_hard(s, in, cnt);

    return cnt;
}

static void __split_push_hard(_split s, constr in, uint cnt)
{
    __split_write_hard(s, _split_fpos(s), in, INT_MAX, cnt);
}

static int __split_push_safe(_split s, constr in, uint cnt)
{
    if(cnt > _split_avail(s))
        cnt = _split_avail(s);

    __split_push_hard(s, in, cnt);

    return cnt;
}

static int __split_squeeze_to_prev(_split s, uint cnt)
{
    int moved;

    if(_split_fpos(s) + cnt <= _split_cap(s))
    {
        moved = __split_appd_safe(_split_prev(s), _split_fptr(s), cnt);
        __split_fpos_incr(s, moved);

        return moved;
    }
    else
    {
        //! step 1:
        moved  = __split_appd_safe(_split_prev(s), _split_fptr(s), _split_cap(s) - _split_fpos(s));
        __split_fpos_incr(s, moved);

        //! step 2:
        moved += __split_squeeze_to_prev(s, cnt - moved);
    }

    return moved;
}

static int __split_squeeze_to_next(_split s, int cnt)
{
    uint moved = _split_rpos(s) - cnt;      // as new rpos

    if(_split_rpos(s) <= _split_cap(s) || moved >= _split_cap(s))
    {
        moved = __split_push_safe(_split_next(s), _split_pptr(s, moved), cnt);
        __split_rpos_decr(s, moved);

        return moved;
    }
    else
    {
        //! step 1:
        moved  = __split_push_safe(_split_next(s), _split_base(s), _split_rpos(s) - _split_cap(s));
        __split_rpos_decr(s, moved);

        //! step 2:
        moved += __split_squeeze_to_next(s, cnt - moved);
    }

    return moved;
}

/**
 * @brief __split_squeeze_room
 *
 *
 *
 */
static bool __split_squeeze_room(_split s, uint pos, uint cnt, _pos info)
{
    if(_split_avail(s) >= cnt)
    {
        info->s   = s;
        info->pos = pos;

        return true;
    }
    else
    {
        int prev_avail, next_avail, prev_moves, next_moves, need_rooms;

        prev_moves = pos - _split_fpos(s);
        next_moves = _split_rpos(s) - pos;
        prev_avail = _split_prev(s) ? _split_avail(_split_prev(s)) : 0;
        next_avail = _split_next(s) ? _split_avail(_split_next(s)) : 0;

        //! not have enough space, quited
        if(cnt > _split_avail(s) + prev_avail + next_avail)
        {
            return false;
        }

        need_rooms = cnt - _split_avail(s);     // need_rooms >= 1

        if(0 == prev_moves)
        {
            if(prev_avail >= need_rooms)
            {
                info->s   = _split_prev(s);
                info->pos = _split_rpos(_split_prev(s));
                return true;
            }
            else
            {
                __split_squeeze_to_next(s, need_rooms - prev_avail);
                return __split_squeeze_room(s, pos, cnt, info);
            }
        }
        else if(0 == next_moves)
        {
            if(next_avail >= need_rooms)
            {
                info->s   = s;
                info->pos = pos;

                return true;
            }
            else
            {
                __split_squeeze_to_prev(s, need_rooms - next_avail);
                return __split_squeeze_room(s, _split_ppos(s, pos), cnt, info);
            }
        }

        if(prev_moves <= next_moves)
        {
            need_rooms -= __split_squeeze_to_prev(s, need_rooms < prev_moves ? need_rooms : prev_moves);

            if(need_rooms)
            {
#if _EVEC_CHECK
                need_rooms -= __split_squeeze_to_next(s, need_rooms);
                assert(need_rooms == 0);
#else
                __split_squeeze_to_next(s, need_rooms);
#endif
            }

            return __split_squeeze_room(s, _split_ppos(s, pos), cnt, info);
        }
        else
        {
            need_rooms -= __split_squeeze_to_next(s, need_rooms < next_moves ? need_rooms : next_moves);

            if(need_rooms)
            {
#if _EVEC_CHECK
                need_rooms -= __split_squeeze_to_prev(s, need_rooms);
                assert(need_rooms == 0);
#else
                __split_squeeze_to_prev(s, need_rooms)
#endif
            }

            return __split_squeeze_room(s, _split_ppos(s, pos), cnt, info);
        }
    }

    return 0;
}

#define _elist_join_prev(n, a)          \
do{                                     \
    _n_prev(a) = _n_prev(n);            \
    _n_next(a) = n;                     \
    _n_prev(n) = a;                     \
                                        \
    if(_n_prev(a))                      \
        _n_next(_n_prev(a)) = a;        \
                                        \
}while(0)

#define _elist_join_next(n, a)          \
do{                                     \
    _n_prev(a) = n;                     \
    _n_next(a) = _n_next(n);            \
    _n_next(n) = a;                     \
                                        \
    if(_n_next(a))                      \
        _n_prev(_n_next(a)) = a;        \
                                        \
}while(0)

//! compat to elist helper macros
#define _n_next(n) _split_next(n)
#define _n_prev(n) _split_prev(n)

#define _split_join_prev(s, a) _elist_join_prev(s, a)
#define _split_join_next(s, a) _elist_join_next(s, a)

#define _v_join_prev(v, s, a)  do{ _split_join_prev(s, a); if(!_split_prev(a)) _v_head(v) = a; }while(0)
#define _v_join_next(v, s, a)  do{ _split_join_next(s, a); if(!_split_next(a)) _v_tail(v) = a; }while(0)

static int __cal_factor(u64 size)
{
    int factor = 0;

    while(size)
    {
        size >>= 1;
        factor++;
    }

    return factor;
}

/**
 * @brief __split_make_room
 *
 *      make new rooms for writen, we call this to make rooms when
 * there have no enough space to write data in cur split and closing
 * neighbour splits.
 *
 * @return (_split)-1  create a new split to write, set to s->prev
 *         (_split) 1  create a new split to write, set to s->next
 *         (_split)..  using return split to write
 */
static bool __split_make_room(_split s, int pos, int cnt, _pos p)
{
    int prev_avail, next_avail, prev_moves, next_moves, need_rooms; ;

    //! first we move out the elements from cur split to neighbours
    //!
    //! after this operation, will only have three situations:
    //!
    //!     1. pos now is at front pos or rear pos in cur split
    //!
    //!        in this case,
    //!
    //!     2. pos now is not at front pos or rear pos in cur split
    //!
    //!        in this case, both neighbours is full or not available
    //!     to wirte
    //!
    //!     3.
    //!
    //!
    //!
    {
        prev_moves = pos - _split_fpos(s);
        next_moves = _split_rpos(s) - pos;
        prev_avail = _split_prev(s) ? _split_avail(_split_prev(s)) : 0;
        next_avail = _split_next(s) ? _split_avail(_split_next(s)) : 0;

        if(prev_avail)
        {
            __split_squeeze_to_prev(s, prev_moves);
            prev_avail = _split_avail(_split_prev(s));
            prev_moves = pos - _split_fpos(s);
        }

        if(next_avail)
        {
            __split_squeeze_to_next(s, next_moves);
            next_avail = _split_avail(_split_next(s));
            next_moves = _split_rpos(s) - pos;
        }

        need_rooms = cnt - _split_avail(s) - prev_avail - next_avail;
    }

    //!
    //! if this split is a head or tail
    //!
    //!    step 1: try to expand it
    //!    step 2: create a new split
    //!
    {
        if(__split_expand(s, need_rooms, p))
        {
            return true;
        }
        else if(!_split_next(s) || !_split_prev(s))
        {
            _split new_s = __split_new(need_rooms, _split_esize(s));

            if(!_split_next(s))  _v_join_next(p->v, s, new_s);
            else                 _v_join_prev(p->v, s, new_s);

            p->v->splits++;
            p->v->cap   += new_s->cap;
            p->v->factor = __cal_factor(p->v->esize * p->v->cnt);

            return __split_squeeze_room(s, pos, cnt, p);
        }
    }

    //!
    //! now we sure this split is between head and tail
    //!
    //!     when add element to internal splits, we will expand it only if
    //! it is empty to write, else we do not try to expand it, only create
    //! new split to write
    //!
    //!     the new created split will be merged automatically in later operations
    //! when checked needed
    //!
    {
        _split new_s = __split_new(need_rooms, _split_esize(s));

        if(0 == prev_avail && 0 == next_avail)
        {
            if(prev_moves < next_moves)
            {
                //! p             s        n
                //! |****|        |****r0| |****|
                //!                 ^--6
                //!
                //! p             s        n
                //! |****| |r000| |****r0| |****|       <- inserted before
                //!                 ^--6
                //!
                //! p             s        n
                //! |****| |**r0| |00**r0| |****|       <- squeezed(moved 2)
                //!           ^--6
                //!
                _v_join_prev(p->v, s, new_s);
            }
            else
            {
                //! p      s               n
                //! |****| |****r0|        |****|
                //!            ^--6
                //!
                //! p       s              n
                //! |****| |****r0| |r000| |****|       <- inserted after
                //!            ^--6
                //!
                //! p      s               n
                //! |****| |***r00| |*r00| |****|       <- squeezed(moved 1)
                //!            ^--
                //!

                _v_join_next(p->v, s, new_s);
            }

            return __split_squeeze_room(s, pos, cnt, p);
        }
        else if(0 == next_avail)
        {
            //! p           s         n
            //! |**r0|      |00***r0| |****|
            //!                ^--8
            //!
            //!             s
            //! |**r0| |r0| |00***r0| |****|            <- inserted before and squeezed(moved 1)
            //!    ^--8
            //!
            _v_join_prev(p->v, s, new_s);

            return __split_squeeze_room(s, pos, cnt, p);
        }
        else if(0 == prev_avail)
        {
            //! p      s            n
            //! |****| |***r0|      |****|
            //!            ^--4
            //! p      s            n
            //! |****| |***r0| |r0| |****|              <- inserted after and squeezed(moved 1)
            //!            ^--4
            //!
            _v_join_next(p->v, s, new_s);

            return __split_squeeze_room(s, pos, cnt, p);
        }

        //! p      s        n
        //! |***r| |r000|   |***r|
        //!         ^--8
        //!
        //! p      s            n
        //! |***r| |r00000| |***r|
        //!     ^--8
        //!

        int fa = p->v->factor;
        p->v->factor = EVEC_MAX_FACTOR;
        __split_expand(s, need_rooms, p);
        p->v->factor = fa;

        return __split_squeeze_room(s, pos, cnt, p);
    }

    return false;
}

static cptr __pos_write(_pos pos)
{
    cptr p = _split_pptr(pos->s, pos->pos);

    if(pos->cnt <= _split_avail(pos->s))
    {
        __split_write_hard(pos->s, pos->pos, pos->in, pos->inlen, pos->cnt);
    }
    else
    {
        int write = _split_avail(pos->s);

        __split_write_hard(pos->s, pos->pos, pos->in, pos->inlen, write);

        pos->cnt   -= write;
        pos->in    += write * _split_esize(pos->s);
        pos->inlen -= write * _split_esize(pos->s);

        //! write to next
        __split_write_hard(_split_next(pos->s), _split_fpos(pos->s), pos->in, pos->inlen, pos->cnt);
    }

    _v_cnt(pos->v) += pos->cnt;

    return p;
}

static cptr __split_insert(_split s, _pos_t* pos)
{
    if(!__split_squeeze_room(s, pos->pos, pos->cnt, pos))
    {
        if(!__split_make_room(s, pos->pos, pos->cnt, pos))
            return false;
    }

    return __pos_write(pos);
}

static bool __split_erase_room(_split s, _pos_t* pos)
{
    uint can_erase;

again:

    can_erase = _split_rpos(s) - pos->pos;

    if(can_erase >= pos->cnt)
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
    else
    {
        __split_rpos_decr(s, can_erase);

        s = _split_next(s);

        if(s)
        {
            pos->cnt -= can_erase;
            pos->pos  = _split_fpos(s);

            goto again;
        }
    }

    return false;
}

static void __split_erase(_split s, _pos_t* pos)
{

}

/// -----------------------------------------------------------------------
//! evec basic
///

static evec __evec_new(etypev type, int size);

evec evec_new(etypev type, u16 esize)
{
    static const u8 _size_map[] = __EVAR_ITEM_LEN_MAP;

    type &= __ETYPEV_VAR_MASK;

    if(type == E_NAV)
        return 0;

    if(type != E_USER)
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

    _v_esize (out) = esize;
    _v_factor(out) = 2;
    _v_head  (out) = __split_new(1, _v_esize(out));
    _v_tail  (out) = _v_head(out);
    _v_cap   (out) = _split_cap(_v_head(out));
    _v_splits(out) = 1;
    _v_type  (out) = type;

    return out;
}

uint   evec_len (evec v)   { return v ? _v_cnt (v)  : 0; }
uint   evec_cap (evec v)   { return v ? _v_cap (v)  : 0; }
uint   evec_esize(evec v)  { return v ? _v_esize(v) : 0; }
etypev evec_type(evec v)   { return v ? _v_type(v)  : E_NAV;}

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
    int cnt; _split s, next; int rsv;

    is0_ret(v, 0);

    cnt = _v_cnt(v);
    s   = _v_head(v);
    rsv = _v_reserve(v) ? _v_reserve(v) : 1;

    _v_splits(v) = 0;
    _v_tail  (v) = 0;

    if(rls)
    {
        while(s)
        {
            next = _split_next(s);

            if(rsv > 0)
            {
                __split_clear_ex(v, s, rls, prvt);

                rsv -= _split_cap(s);

                _v_splits(v)++;
                _v_tail  (v) = s;
            }
            else
            {
                if(_v_tail  (v) == _split_prev(s))
                {
                    _split_next(_split_prev(s)) = 0;
                }

                __split_free_ex(v, s, rls, prvt);
            }

            s = next;
        }
    }
    else
    {
        while(s)
        {
            next = _split_next(s);

            if(rsv > 0)
            {
                __split_clear(v, s);

                rsv -= _split_cap(s);

                _v_splits(v)++;
                _v_tail  (v) = s;
            }
            else
            {
                if(_v_tail  (v) == _split_prev(s))
                {
                    _split_next(_split_prev(s)) = 0;
                }

                __split_free(v, s);
            }

            s = next;
        }
    }

    return cnt;
}

int  evec_free  (evec v){ return evec_freeEx(v, 0, EVAL_ZORE); }
int  evec_freeEx(evec v, eobj_rls_ex_cb rls, eval prvt)
{
    int cnt; _split s, next;

    is0_ret(v, 0);

    cnt = _v_cnt(v);

    s = _v_head(v);

    if(rls)
    {
        while(s)
        {
            next = _split_next(s);

            __split_free_ex(v, s, rls, prvt);

            s = next;
        }
    }
    else
    {
        while(s)
        {
            next = _split_next(s);

            __split_free(v, s);

            s = next;
        }
    }

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
                            b.hdr._len = _s_len(b.obj.s);

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
                            b.hdr._len = _s_len(b.obj.s);

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

static void __split_free(evec v, _split s)
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

    efree(_split_base(s));
    efree(s);
}

static void __split_free_ex(evec v, _split s, eobj_rls_ex_cb rls, eval prvt)
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
                            b.hdr._len = _s_len(b.obj.s);

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
                            b.hdr._len = _s_len(b.obj.s);

                            rls(&b.obj, prvt);
                        }
                    }
                    break;
    }

    efree(_split_base(s));
    efree(s);
}

/// -----------------------------------------------------
//! evec add
///
///
static void __evec_get_pos(evec v, uint idx, _pos p  );
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
    _pos_t p = {v, in, len, cnt, 0, 0};

    if(idx == 0)
    {
        p.s   = _v_head(v);
        p.pos = _split_fpos(p.s);

    }
    else if(idx >= _v_cnt(v))
    {
        p.s = _v_tail(v);
        p.pos = _split_rpos(p.s);
    }
    else
        __evec_get_pos(v, idx, &p);

    return __split_insert(p.s, &p);
}

static bool __evec_addV(evec v, uint idx, evar var)
{
    int v_type;

    is1_ret(!v, false);

    v_type = var.type & __ETYPEV_VAR_MASK;

    switch (_v_type(v))
    {
        case E_USER :   if(v_type != E_RAW)
                            return false;

                        return var.type & __ETYPEV_PTR_MASK ? __evec_addB(v, idx, var.v.p, var.esize, 1)
                                                            : __evec_addB(v, idx, var.v.r, var.esize, 1);

        default     :   if(v_type != _v_type(v))
                            return false;

                        switch(v_type)
                        {
                            case E_STR :    var.v.s = estr_dupS(var.v.s  );
                                            break;

                            case E_RAW :    var.v.p   = estr_newLen(var.v.p, var.esize);
                                            var.esize = 8;
                                            break;
                        }

                        break;

    }

    return __evec_addB(v, idx, var.v.r, var.esize, var.cnt ? var.cnt : 1);
}

/**
 * @brief __evec_get_pos
 *
 *   be sure of 0 <= idx < _v_cnt(v) before call it
 *
 */
static void __evec_get_pos(evec v, uint idx, _pos p)
{
    uint cur_len;

    //! find it
    {
        if(idx < _v_cnt(v) - idx )
        {
            p->s = _v_head(v);

            do{
                cur_len = _split_len(p->s);

                if(cur_len > idx)
                {
                    p->pos = _split_fpos(p->s) + idx;

                    break;
                }

                idx  -= cur_len;
                p->s  = _split_next(p->s);
            }while(1);
        }
        else
        {
            idx = _v_cnt(v) - idx;

            p->s = _v_tail(v);

            do{
                cur_len = _split_len(p->s);

                if(cur_len >= idx)
                {
                    p->pos = _split_rpos(p->s) - idx;

                    break;
                }

                idx  -= cur_len;
                p->s  = _split_prev(p->s);
            }while(1);
        }
    }
}

/// -----------------------------------------------------
//! evec val
///
///

cptr evec_at   (evec v, uint idx) { _pos_t p; is1_ret(!v || idx >= _v_cnt(v), 0); __evec_get_pos(v,           idx, &p); return _split_pptr(p.s, p.pos); }
cptr evec_first(evec v)           { _pos_t p; is1_ret(!v ||       !_v_cnt(v), 0); __evec_get_pos(v,             0, &p); return _split_pptr(p.s, p.pos); }
cptr evec_last (evec v)           { _pos_t p; is1_ret(!v ||       !_v_cnt(v), 0); __evec_get_pos(v, _v_cnt(v) - 1, &p); return _split_pptr(p.s, p.pos); }

cptr evec_val (evec v, uint idx){ _pos_t p; is1_ret(!v || idx >= _v_cnt(v)                                               , 0); __evec_get_pos(v, idx, &p); return _split_pptr(p.s, p.pos);}
i64  evec_valI(evec v, uint idx){ _pos_t p; is1_ret(!v || idx >= _v_cnt(v) ||(_v_type(v) != E_I64 && _v_type(v) != E_F64), 0); __evec_get_pos(v, idx, &p); return _v_type(v) == E_I64 ? *(i64*)_split_pptr(p.s, p.pos) : *(f64*)_split_pptr(p.s, p.pos);}
f64  evec_valF(evec v, uint idx){ _pos_t p; is1_ret(!v || idx >= _v_cnt(v) ||(_v_type(v) != E_I64 && _v_type(v) != E_F64), 0); __evec_get_pos(v, idx, &p); return _v_type(v) == E_I64 ? *(i64*)_split_pptr(p.s, p.pos) : *(f64*)_split_pptr(p.s, p.pos);}
cstr evec_valS(evec v, uint idx){ _pos_t p; is1_ret(!v || idx >= _v_cnt(v) || _v_type(v) != E_STR                        , 0); __evec_get_pos(v, idx, &p); return *(cstr*)_split_pptr(p.s, p.pos);}
cptr evec_valP(evec v, uint idx){ _pos_t p; is1_ret(!v || idx >= _v_cnt(v) || _v_type(v) != E_PTR                        , 0); __evec_get_pos(v, idx, &p); return *(cptr*)_split_pptr(p.s, p.pos);}
cptr evec_valR(evec v, uint idx){ _pos_t p; is1_ret(!v || idx >= _v_cnt(v) || _v_type(v) != E_RAW                        , 0); __evec_get_pos(v, idx, &p); return *(cptr*)_split_pptr(p.s, p.pos);}

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
    __evec_get_pos(v, idx, &p);

    //! take it from split
    {
        p.v   = v;
        p.cnt = 1;
        __split_erase_room(p.s, &p);
    }

    //! set val to var to return
    {
        var = evar_gen(_v_type(v), cnt, _v_esize(v));
        evar_set(&var, 0, _split_pptr(p.s, p.pos), _v_esize(v));
    }

    return var;
}

evar evec_takeH(evec v)             { is1_ret(!v ||       !_v_cnt(v), EVAR_NAV); return __evec_take_var(v,             0, 1); }
evar evec_takeT(evec v)             { is1_ret(!v ||       !_v_cnt(v), EVAR_NAV); return __evec_take_var(v, _v_cnt(v) - 1, 1); }
evar evec_takeI(evec v, uint idx)   { is1_ret(!v || idx >= _v_cnt(v), EVAR_NAV); return __evec_take_var(v,           idx, 1); }


