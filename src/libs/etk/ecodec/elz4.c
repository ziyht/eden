/// =====================================================================================
///
///       Filename:  elz4.c
///
///    Description:  a lz4 encode and decode wraper
///
///        Version:  1.0.0
///        Created:  2017-05-03 10:00:18 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================
///
///

#include "libs/lz4/lib/lz4.h"
#include "libs/lz4/lib/lz4frame.h"
#include "libs/lz4/lib/lz4hc.h"

#include "eutils.h"

#include "elz4.h"

#define ESTR_VERSION "elz4 1.0.2"       // fix bugs of elz4_decb()

constr elz4_err(int code)
{
    return LZ4F_getErrorName((LZ4F_errorCode_t)code);
}

i64 elz4_encb(constr in, uint inlen, estr* out)
{
    estr s; i64 ret;

    if(!in || !out || inlen < 1)
    {
        return 0;
    }

    s = *out;
    estr_clear(s);
    estr_ensure(s, LZ4_COMPRESSBOUND(inlen));

    ret = LZ4_compress_fast(in, s, inlen, (uint)estr_cap(s), 1);

    if(ret > 0)
    {
        estr_incrLen(s, ret);
        s[ret] = '\0';
    }

    *out = s;

    return ret;
}

int elz4_encb2b(constr in, uint inlen, cstr  dst, uint dlen)
{
    if(!in || !dst || inlen < 1)
        return 0;

    return LZ4_compress_fast(in, dst, inlen, dlen, 1);
}

i64 elz4_decb(constr in, uint inlen, estr* out)
{
    estr s; int ret; uint cap;

    if(!in || !out || inlen < 1)
    {
        return 0;
    }

    s = *out;
    estr_clear(s);
    estr_ensure(s, inlen * 5);

    do{

        cap = (uint)estr_cap(s);

        ret = LZ4_decompress_safe(in, s, inlen, cap);

        if(ret > 0)
        {
            estr_incrLen(s, ret);
            s[ret] = '\0';

            break;
        }

        if(ret == -7)
        {
            if(cap >= LZ4_MAX_INPUT_SIZE && cap >= inlen * 100)
                break;

            estr_ensure(s, cap * 2);
        }
        else
            break;
    }while(1);

    *out = s;

    return ret;
}

int elz4_decb2b(constr in, uint inlen, cstr  dst, uint dlen)
{
    int ret;

    if(!in || !dst || inlen < 1 || dlen < 1)
    {
        return 0;
    }

    ret = LZ4_decompress_safe(in, dst, inlen, dlen);

    return ret;
}

u32 elz4_bound (u32 srcSize)
{
    return LZ4_COMPRESSBOUND(srcSize);
}

typedef struct __elz4s_enc_s{

    LZ4_stream_t        lz4s;
    estr                buf;
    uint                cap;
    int                 level;

}__elz4s_enc_t, * __elz4s_enc;

static void __elz4s_enc_release(__elz4s_enc h)
{
    is0_ret(h, );

    estr_free(h->buf);
}

typedef struct __elz4s_dec_s{

    LZ4_streamDecode_t  lz4s;

    constr              in;
    uint                inlen;
    estr                buf;
    uint                cap;
    uint                block;

}__elz4s_dec_t, * __elz4s_dec;

static void __elz4s_dec_release(__elz4s_dec h)
{
    is0_ret(h, );

    estr_free(h->buf);
}

typedef struct elz4s_s {
    __elz4s_enc ench;
    __elz4s_dec dech;
}elz4s_t;

#define __DF_BUF_SIZE   (4 * 1024 * 1024)

elz4s elz4s_new()
{
    return (elz4s)ecalloc(1, sizeof(elz4s_t));
}

void  elz4s_free(elz4s h)
{
    is0_ret(h, );

    __elz4s_enc_release(h->ench);
    __elz4s_dec_release(h->dech);

    efree(h->ench);
    efree(h->dech);

    efree(h);
}

elz4s elz4s_encNew  (uint cap, int level)
{
    elz4s h = elz4s_new();

    if(elz4s_encInit(h, cap, level))
    {
        return h;
    }

    elz4s_free(h);

    return 0;
}

bool  elz4s_encInit (elz4s h, uint cap, int level)
{
    is0_ret(h, false);

    if(!h->ench)
    {
        h->ench = ecalloc(1, sizeof(*h->ench));

        LZ4_initStream(&h->ench->lz4s, sizeof(h->ench->lz4s));
    }
    else
        LZ4_resetStream_fast(&h->ench->lz4s);

    h->ench->cap      = cap == 0 ? __DF_BUF_SIZE : cap;
    h->ench->level    = level;

    estr_clear(h->ench->buf);
    estr_ensure(h->ench->buf, 64);

    estr_incrLen(h->ench->buf, sizeof(uint));   // room to store max block size

    return true;
}

i64   elz4s_encNext (elz4s h, constr in, uint inlen)
{
    i64 ret; uint* sizep;

    is0_ret(h, -100);

    if(!h->ench)
    {
        elz4s_encInit(h, 0, 0);
    }

    {
        __elz4s_enc eh = h->ench;

        if(estr_cap(eh->buf) < eh->cap)
        {
            estr_ensure(eh->buf, LZ4_compressBound(inlen) + sizeof(*sizep));
        }

        sizep = (uint*)estr_cur(eh->buf);

        ret = LZ4_compress_fast_continue(&eh->lz4s, in, (cstr)sizep + sizeof(*sizep), inlen, (uint)estr_avail(eh->buf), eh->level);

        if(ret > 0)
        {
            *sizep = (uint)ret;
            estr_incrLen(eh->buf, ret + sizeof(*sizep));

            if(*((uint*)eh->buf) < inlen)
                *((uint*)eh->buf) = inlen;
        }
        else
            LZ4_resetStream_fast(&eh->lz4s);

        return ret;
    }

    return 0;
}

estr  elz4s_encData (elz4s h) { if(h && h->ench) return                h->ench->buf ; return 0;}
uint  elz4s_encDLen (elz4s h) { if(h && h->ench) return (uint)estr_len(h->ench->buf); return 0;}
void  elz4s_encClear(elz4s h) { if(h && h->ench) estr_clear(h->ench->buf); }
uint  elz4s_encFlush(elz4s h, cstr dst, uint dlen)
{
    if(!h || !h->ench)
        return 0;

    u32 len = (uint)estr_len(h->ench->buf);

    if(len <= dlen)
    {
        memcpy(dst, h->ench->buf, len);
        estr_clear(h->ench->buf);

        return len;
    }

    return 0;
}

bool  elz4s_decInit (elz4s h, uint cap)
{
    is0_ret(h, false);

    if(!h->dech)
    {
        h->dech = ecalloc(1, sizeof(*h->dech));
    }

    LZ4_setStreamDecode(&h->dech->lz4s, NULL, 0);

    h->dech->cap   = cap == 0 ? __DF_BUF_SIZE : cap;
    h->dech->block = 32;

    return true;
}

bool  elz4s_decBegin(elz4s h, constr in, uint inlen)
{
    is0_ret(h, false);

    if(!in || inlen < 9)
        return false;

    if(!h->dech)
    {
        elz4s_decInit(h, 0);
    }

    h->dech->in    = in    + sizeof(uint);
    h->dech->inlen = inlen - sizeof(uint);
    h->dech->block = *((uint*)in);

    return true;
}

int   elz4s_decNext (elz4s h)
{
    int ret; uint curlen; constr curin;

    is1_ret(!h || !h->dech, false);

    {
        __elz4s_dec dh = h->dech;

        if(dh->inlen == 0)
            return 0;

        if(estr_cap(dh->buf) < dh->cap)
            estr_ensure(dh->buf, dh->block);

        curlen = *((uint*)h->dech->in) + sizeof(uint);
        curin  = h->dech->in + sizeof(uint);

        //! overflow
        if(curlen > dh->inlen)
        {
            return -101;
        }

        ret = LZ4_decompress_safe_continue(&dh->lz4s, curin, estr_cur(dh->buf), curlen - sizeof(uint), (uint)estr_avail(dh->buf));

        if(ret < 0)
        {
            return ret;
        }

        dh->in    += curlen;
        dh->inlen -= curlen;

        estr_incrLen(dh->buf, ret);
        dh->buf[estr_len(dh->buf)] = '\0';
    }

    return ret;
}

estr  elz4s_decData (elz4s h) { if(h && h->dech) return                h->dech->buf ; return 0;}
uint  elz4s_decDLen (elz4s h) { if(h && h->dech) return (uint)estr_len(h->dech->buf); return 0;}
void  elz4s_decClear(elz4s h) { if(h && h->dech) estr_clear(h->dech->buf); }
uint  elz4s_decFlush(elz4s h, cstr dst, uint dlen)
{
    if(!h || !h->dech)
        return 0;

    u32 len = (uint)estr_len(h->dech->buf);

    if(len <= dlen)
    {
        memcpy(dst, h->dech->buf, len);
        estr_clear(h->dech->buf);

        return len;
    }

    return 0;
}

i64 elz4f_encb(constr in, u64 inlen, estr* out)
{
    estr s; i64 ret;

    if(!in || !out || inlen < 1)
    {
        return 0;
    }

    s = *out;
    estr_clear(s);
    estr_ensure(s, LZ4F_compressFrameBound(inlen, 0));

    ret = LZ4F_compressFrame(s, estr_avail(s), in, inlen, 0);

    if(ret > 0)
    {
        estr_incrLen(s, ret);
    }

    *out = s;

    return ret;
}

i64 elz4f_decb(constr in, u64 inlen, estr* out)
{
    LZ4F_dctx* ctx; size_t next, srcSize, curDec; constr end;
    estr s;

    //! a frame len need at least 7 bytes
    if(!in || !out || inlen < 7)
    {
        return 0;
    }

    LZ4F_createDecompressionContext(&ctx, LZ4F_VERSION);

    s        = *out;
    srcSize  = inlen;
    end      = in + inlen;
    next     = 0;
    estr_clear(s);

    do{

        estr_ensure(s, next);
        curDec   = estr_avail(s);

        next = LZ4F_decompress(ctx, estr_cur(s), &curDec, in, &srcSize, 0);

        if((i64)next < 0)
        {
            estr_clear(s);
            break;
        }

        estr_incrLen(s, curDec);

        if(next == 0)
            break;

        in      += srcSize;
        srcSize  = end - in;

    }while(1);

    LZ4F_freeDecompressionContext(ctx);

    estr_cur(s)[0] = '\0';
    *out = s;

    return (i64)next < 0 ? next : estr_len(s);
}

i64 elz4f_encb2b(constr in, u64 inlen, cstr  dst, u64 dlen)
{
    if(!in || !dst || inlen < 1)
    {
        return 0;
    }

    return LZ4F_compressFrame(dst, dlen, in, inlen, 0);
}

i64 elz4f_decb2b(constr in, u64 inlen, cstr  dst, u64 dlen)
{
    LZ4F_dctx* ctx; size_t next, srcSize, curDec; constr end;
    size_t len;

    //! a frame len need at least 7 bytes
    if(!in || !dst || inlen < 7 || dlen < 1)
    {
        return 0;
    }

    LZ4F_createDecompressionContext(&ctx, LZ4F_VERSION);

    srcSize  = inlen;
    end      = in + inlen;
    next     = 0;
    len      = 0;

    do{
        curDec = dlen - len;

        if(curDec < next)
        {
            dst[0] = 0;
            return -103;
        }

        next = LZ4F_decompress(ctx, dst + len, &curDec, in, &srcSize, 0);

        if((i64)next < 0)
        {
            dst[0] = 0;
            break;
        }

        len += curDec;

        if(next == 0)
        {
            dst[len] = '\0';
            break;
        }

        in      += srcSize;
        srcSize  = end - in;
    }while(1);

    LZ4F_freeDecompressionContext(ctx);

    return (i64)next < 0 ? next : len;
}

u64 elz4f_bound(u64 srcSize)
{
    return LZ4F_compressBound(srcSize, 0);
}

typedef struct __elz4f_enc_s{

    LZ4F_cctx*              ctx;
    LZ4F_preferences_t      pre;
    estr                    buf;
    u64                     cur;
    u64                     cap;
    i8                      begin;

}__elz4f_enc_t, * __elz4f_enc;

static void __elz4f_enc_release(__elz4f_enc eh)
{
    is0_ret(eh, );

    estr_free(eh->buf);
    LZ4F_freeCompressionContext(eh->ctx);
}

typedef struct __elz4f_dec_s{

    LZ4F_dctx*              ctx;
    estr                    buf;
    u64                     cap;
    constr                  in;
    u64                     inlen;

}__elz4f_dec_t, * __elz4f_dec;

static void __elz4f_dec_release(__elz4f_dec dh)
{
    is0_ret(dh, );

    estr_free(dh->buf);
    LZ4F_freeDecompressionContext(dh->ctx);
}

typedef struct elz4f_s {

    __elz4f_enc  eh;
    __elz4f_dec  dh;

}elz4f_t;

static void __elz4f__release(elz4f h)
{
    __elz4f_enc_release(h->eh);
    __elz4f_dec_release(h->dh);

    efree(h->eh);
    efree(h->dh);
}

elz4f elz4f_new()
{
    elz4f h = ecalloc(1, sizeof (*h));

    return h;
}

void  elz4f_free    (elz4f h)
{
    is0_ret(h, );

    __elz4f__release(h);

    efree(h);
}

elz4f elz4f_encNew (u64 cap, int level)
{
    elz4f h = elz4f_new();

    if(elz4f_encInit(h, cap, level))
    {
        return h;
    }

    __elz4f__release(h);
    efree(h);

    return 0;
}

bool elz4f_encInit(elz4f h, u64 cap, int level)
{
    is0_ret(h, false);

    if(!h->eh)
    {
        h->eh = ecalloc(1, sizeof(*h->eh));

        LZ4F_createCompressionContext(&h->eh->ctx, LZ4F_VERSION);
    }

    h->eh->pre.compressionLevel = level;
    h->eh->cap                  = cap == 0 ? __DF_BUF_SIZE : cap;

    if(level >= LZ4HC_CLEVEL_OPT_MIN)
    {
        h->eh->pre.favorDecSpeed = 1;
    }

    estr_clear(h->eh->buf);
    h->eh->begin = 0;

    return true;
}

bool  elz4f_encBegin(elz4f h, u64  cap, int level)
{
    size_t ret;

    is0_ret(h, false);

    elz4f_encInit(h, cap, level);

    is1_ret(h->eh->begin, false);

    {
        __elz4f_enc eh = h->eh;

        estr_ensure(eh->buf, LZ4F_HEADER_SIZE_MAX);
        ret = LZ4F_compressBegin(eh->ctx, eh->buf, LZ4F_HEADER_SIZE_MAX, &eh->pre);
        if((i64)ret > 0)
        {
            estr_incrLen(eh->buf, ret);
            eh->begin = 1;

            return true;
        }
    }

    return false;
}

i64   elz4f_encNext(elz4f h, constr in, u64 inlen)
{
    size_t ret;

    is1_ret(!h || !h->eh, 0);

    if(!h->eh->begin)
    {
        elz4f_encBegin(h, h->eh->cap, h->eh->pre.compressionLevel);
    }

    {
        size_t need;
        __elz4f_enc eh = h->eh;

        need = LZ4F_compressBound(inlen, &eh->pre);

        if(estr_cap(eh->buf) < eh->cap)
        {
            estr_ensure(eh->buf, need);
        }

        ret = LZ4F_compressUpdate(eh->ctx, estr_cur(eh->buf), estr_avail(eh->buf),
                                  in, inlen, 0);

        if((i64)ret > 0)
        {
            estr_incrLen(eh->buf, ret);
        }
    }

    return ret;
}

int   elz4f_encEnd   (elz4f h)
{
    size_t ret;

    if(!h || !h->eh || !h->eh->begin) return 0;

    {
        __elz4f_enc eh = h->eh;

        estr_ensure(eh->buf, LZ4F_compressBound(0, &eh->pre));

        ret = LZ4F_compressEnd(eh->ctx, estr_cur(eh->buf), estr_avail(eh->buf), 0);

        estr_incrLen(eh->buf, ret);

        eh->begin = 0;
    }

    return (int)ret;
}

void  elz4f_encClear (elz4f h) { if(h && h->eh) estr_clear(h->eh->buf); }
estr  elz4f_encData  (elz4f h) { if(h && h->eh) return          h->eh->buf ; return 0; }
u64   elz4f_encDLen  (elz4f h) { if(h && h->eh) return estr_len(h->eh->buf); return 0; }
i64   elz4f_encFlush (elz4f h, cstr dst, u64 dlen)
{
    size_t ret;

    if(h && h->eh)
    {
        __elz4f_enc eh  = h->eh;
        u64         len = estr_len(eh->buf);

        if(len < eh->cap)
            estr_ensure(eh->buf, LZ4F_compressBound(0, &eh->pre));

        ret = LZ4F_flush(eh->ctx, eh->buf + len, estr_avail(eh->buf), 0);

        if((i64)ret > 0)
        {
            estr_incrLen(eh->buf, ret);
            len += ret;
        }

        if(len <= dlen)
        {
            memcpy(dst, eh->buf, len);
            estr_clear(eh->buf);

            return len;
        }
    }

    return 0;
}

elz4f elz4f_decNew   (u64 cap)
{
    elz4f h = ecalloc(1, sizeof (*h));

    elz4f_decInit(h, cap);

    return h;
}

bool  elz4f_decInit (elz4f h, u64 cap)
{
    is0_ret(h, false);

    if(!h->dh)
    {
        h->dh = ecalloc(1, sizeof(*h->dh));

        LZ4F_createDecompressionContext(&h->dh->ctx, LZ4F_VERSION);
    }
    else
    {
        LZ4F_resetDecompressionContext(h->dh->ctx);
    }

    h->dh->cap   = cap == 0 ? __DF_BUF_SIZE : cap;
    h->dh->inlen = 0;
    estr_clear(h->dh->buf);

    return true;
}

bool elz4f_decBegin (elz4f h, constr in, u64 inlen)
{
    size_t srcSize; size_t curDec; size_t next;

    if(!h || !in || inlen < 7)
        return false;

    elz4f_decInit(h, h->dh ? h->dh->cap : 0);

    {
        __elz4f_dec  dh = h->dh;

        estr_ensure(dh->buf, LZ4F_HEADER_SIZE_MAX);

        dh->in = in;
        dh->inlen = inlen;

        curDec  = estr_avail(dh->buf);
        srcSize = dh->inlen;

        next = LZ4F_decompress(dh->ctx, estr_cur(dh->buf), &curDec, dh->in, &srcSize, 0);

        if((i64)next > 0)
        {
            estr_incrLen(dh->buf, curDec);
            dh->in    += srcSize;
            dh->inlen -= srcSize;
        }
    }

    return (i64)next;
}

i64   elz4f_decNext(elz4f h)
{
    size_t srcSize;

    if(!h || !h->dh) return -102;

    {
        size_t curDec, next; i64 curLen; constr in, end; u64 inlen; estr s;

        __elz4f_dec dh = h->dh;

        if(!dh->inlen)
            return 0;

        s      = dh->buf;

        curLen = estr_len(s);
        in     = dh->in;
        inlen  = dh->inlen;
        end    = in + inlen;
        next   = 4;

        do{
            if(estr_cap(s) < dh->cap)
            {
                estr_ensure(s, next);
            }

            curDec  = estr_avail(s);
            srcSize = dh->inlen;

            next = LZ4F_decompress(dh->ctx, estr_cur(s), &curDec, in, &srcSize, 0);

            if((i64)next < 0)   // err
            {
                break;
            }

            estr_incrLen(s, curDec);

            in      += srcSize;
            srcSize  = end - in;

            if(next == 0)
            {
                estr_cur(s)[0] = '\0';
                break;
            }

        }while(1);

        curLen = estr_len(s) - curLen;

        if(curLen)
        {
            dh->buf   = s;
            dh->in    = in;
            dh->inlen = srcSize;

            return curLen;
        }

        return next;
    }

    return 0;
}

void  elz4f_decClear (elz4f h) { if(h && h->dh) estr_clear(h->dh->buf); }
estr  elz4f_decData  (elz4f h) { if(h && h->dh) return          h->dh->buf ; return 0; }
u64   elz4f_decDLen  (elz4f h) { if(h && h->dh) return estr_len(h->dh->buf); return 0; }
u64   elz4f_decFlush (elz4f h, cstr dst, u64 dlen)
{
    u64 len;

    is1_ret(!h || !h->dh, 0);

    len = estr_len(h->dh->buf);

    if(len <= dlen)
    {
        memcpy(dst, h->dh->buf, len);
        estr_clear(h->dh->buf);

        return len;
    }

    return 0;
}
