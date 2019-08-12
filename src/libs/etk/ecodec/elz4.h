/// =====================================================================================
///
///       Filename:  elz4.h
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

#ifndef E_LZ4_H
#define E_LZ4_H

#include "etype.h"
#include "estr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** -----------------------------------------------------
 *
 *  elz4 basic
 *
 */

typedef struct elz4s_s*  elz4s;
typedef struct elz4f_s*  elz4f;

constr elz4_err(int code);

/** -----------------------------------------------------
 *
 *  elz4 encoding and decoding
 *
 * @return:
 *    0 if input is invalid
 *   <0 if err occured
 *   >0 if operate ok, the ret value is the bytes write to out in current operation
 *
 */
i64 elz4_encb  (constr in, uint inlen, estr* out);              // lz4   compress
i64 elz4_decb  (constr in, uint inlen, estr* out);              // lz4 decompress
int elz4_encb2b(constr in, uint inlen, cstr  dst, uint dlen);   // lz4   compress, the dlen must >= elz4_bound(inlen)
int elz4_decb2b(constr in, uint inlen, cstr  dst, uint dlen);   // lz4 decompress

u32 elz4_bound (u32 srcSize);

/** -----------------------------------------------------
 *
 *  elz4 stream encoding and decoding
 *
 * @note:
 *
 *  1. cap can be 0 and internal cap will set to 4M
 *
 *  2. elz4s_decoder only support the data compressed by elz4s_encoder
 *
 *  3. the data format of elz4s_encoder is:
 *
 *        |- max uncompressed block size
 *        |
 *    | 4byte | 4byte | block | 4byte | block | ... |
 *              next
 *              block
 *              size
 */

elz4s elz4s_new();
void  elz4s_free(elz4s h);

elz4s elz4s_encNew  (uint cap, int level);              // Returns a elz4s handle to do encode operation
bool  elz4s_encInit (elz4s h, uint cap, int level);     // Reinit elz4s handle so that it can do encode operation
i64   elz4s_encNext (elz4s h, constr in, uint inlen);   // Returns the size of encode data write to intenal buffer, return <0 note that err occured
estr  elz4s_encData (elz4s h);                          // Returns the internal buffer, you should not write or free it, we don't return a copy
uint  elz4s_encDLen (elz4s h);                          // Returns the len of internal buffer
void  elz4s_encClear(elz4s h);                          // Clear internal buffer
uint  elz4s_encFlush(elz4s h, cstr dst, uint dlen);     // flush internal buf to dst, the dlen must have enough space, cleared automatically

elz4s elz4s_decNew  (uint cap);                         // Returns a elz4s handle to do decode operation
bool  elz4s_decInit (elz4s h, uint cap);                // Reinit elz4s handle so that it can do decode operation
bool  elz4s_decBegin(elz4s h, constr in, uint inlen);   // Returns true if ok, else returns false
int   elz4s_decNext (elz4s h);                          // Returns 0: over, >0: current decode to internal buf, <0: err
estr  elz4s_decData (elz4s h);                          // Returns the internal buffer, you should not write or free it, we don't return a copy
uint  elz4s_decDLen (elz4s h);                          // Returns the len of internal buffer
void  elz4s_decClear(elz4s h);                          // Clear internal buffer
uint  elz4s_decFlush(elz4s h, cstr dst, uint dlen);     // Returns the len of data flushed to dst, the dlen must have enough space, cleared automatically if succeed


/** -----------------------------------------------------
 *
 *  elz4 frame encoding and decoding
 *
 * @note
 *  1. cap can be 0 and internal cap will set to 4M
 *  2. level -> 0 faster
 *  3. we'll set favorDecSpeed enabled when level >= LZ4HC_CLEVEL_OPT_MIN
 *
 */
i64   elz4f_encb  (constr in, u64 inlen, estr* out);             // lz4frame   compress
i64   elz4f_decb  (constr in, u64 inlen, estr* out);             // lz4frame decompress
i64   elz4f_encb2b(constr in, u64 inlen, cstr  dst, u64 dlen);    // lz4frame   compress, the dlen must >= elz4f_bound(inlen)
i64   elz4f_decb2b(constr in, u64 inlen, cstr  dst, u64 dlen);    // lz4frame decompress

u64   elz4f_bound (u64 srcSize);

elz4f elz4f_new();                                      // Returns a new handle to do encode or decode operatiom
void  elz4f_free(elz4f h);                              // Release and free the handle

elz4f elz4f_encNew  (u64 cap, int level);              // Returns a new handle to do encode operation
bool  elz4f_encInit (elz4f h, u64 cap, int level);     // Reinit elz4f handle so that it can do encode operation
bool  elz4f_encBegin(elz4f h, u64 cap, int level);     // Begin to encode data, and write a lz4f header to internal buffer, internal buffer will be cleared first
i64   elz4f_encNext (elz4f h, constr in, u64 inlen);   // Returns the size of encode data write to intenal buffer, it maybe 0, return <0 note that err occured
int   elz4f_encEnd  (elz4f h);                          // End the encode operation, write end code to internal buffer, you should get the internal data after do this
estr  elz4f_encData (elz4f h);                          // Returns the internal buffer, you should not write or free it, we don't return a copy
u64   elz4f_encDLen (elz4f h);                          // Returns the len of internal buffer
void  elz4f_encClear(elz4f h);                          // Clear internal buffer
i64   elz4f_encFlush(elz4f h, cstr dst, u64 dlen);     // flush internal buf to dst, the dlen must have enough space, cleared automatically

elz4f elz4f_decNew  (u64  cap);                         // Returns a new handle to do decode operation
bool  elz4f_decInit (elz4f h, u64 cap);                // Reinit elz4f handle so that it can do decode operation
bool  elz4f_decBegin(elz4f h, constr in, u64 inlen);   // Begin to decode a new input data, internal buffer will be cleared first
i64   elz4f_decNext (elz4f h);                          // Returns the len of current decode data to internal buffer
estr  elz4f_decData (elz4f h);                          // Returns the internal buffer, @note: only readble
u64   elz4f_decDLen (elz4f h);                          // Returns the len of internal buffer
void  elz4f_decClear(elz4f h);                          // Clear internal buffer
u64   elz4f_decFlush(elz4f h, cstr dst, u64 dlen);     // Returns the len of data flushed to dst, the dlen must have enough space, cleared automatically if succeed

#ifdef __cplusplus
}
#endif

#endif
