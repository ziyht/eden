/// =====================================================================================
///
///       Filename:  eatom.h
///
///    Description:  atom operation
///                  rebuild from gcc arch or linux kernel src
///
///                  note:
///                      the atom operation sometimes can
///
///
///        Version:  1.0
///        Created:  09/14/2017 14:47:25 PM
///       Revision:  none
///       Compiler:  gcc
///
///         Author:  Haitao Yang, joyhaitao@foxmail.com
///        Company:
///
/// =====================================================================================

#ifndef __E_ATOM_H__
#define __E_ATOM_H__

#define EATOM_VERSION "eatom 1.0.0"

#include "etype.h"
#include "eobj.h"
#include "libs/eatom_p.h"

/** -----------------------------------------------------------
 *  mem fence
 *
*/

#define eatom_mo_relaxed            __eatomic_memory_order_relaxed
#define eatom_mo_acquire            __eatomic_memory_order_acquire
#define eatom_mo_release            __eatomic_memory_order_release
#define eatom_mo_acq_rel            __eatomic_memory_order_acq_rel
#define eatom_mo_seq_cst            __eatomic_memory_order_seq_cst

#define eatom_fence(mo)             __eatomic_fence((mo))

/** -----------------------------------------------------------
 *  Atomic value get/set
 *
 *  get/set a value from/to ptr
 *
*/

#define eatom_get8( a)              __eatom_get8 (&(a))
#define eatom_get16(a)              __eatom_get16(&(a))
#define eatom_get32(a)              __eatom_get32(&(a))
#define eatom_get64(a)              __eatom_get64(&(a))

#define eatom_set8( a, i)           __eatom_set8 (&(a), (i))        // return origin
#define eatom_set16(a, i)           __eatom_set16(&(a), (i))        // return origin
#define eatom_set32(a, i)           __eatom_set32(&(a), (i))        // return origin
#define eatom_set64(a, i)           __eatom_set64(&(a), (i))        // return origin

#define eatom_get8_m( a, mo)        __eatom_get8_m (&(a))
#define eatom_get16_m(a, mo)        __eatom_get16_m(&(a))
#define eatom_get32_m(a, mo)        __eatom_get32_m(&(a))
#define eatom_get64_m(a, mo)        __eatom_get64_m(&(a))

#define eatom_set8_m( a, i, mo)     __eatom_set8_m (&(a), (i), (mo))  // return origin
#define eatom_set16_m(a, i, mo)     __eatom_set16_m(&(a), (i), (mo))  // return origin
#define eatom_set32_m(a, i, mo)     __eatom_set32_m(&(a), (i), (mo))  // return origin
#define eatom_set64_m(a, i, mo)     __eatom_set64_m(&(a), (i), (mo))  // return origin

/** -----------------------------------------------------------
 *  Atomic value add/sub
 *
 *  add/set a value to ptr
 *
*/

#define eatom_add8(  a, i)      __eatom_add8( &(a), (i))        // return a + i
#define eatom_add16( a, i)      __eatom_add16(&(a), (i))        // return a + i
#define eatom_add32( a, i)      __eatom_add32(&(a), (i))        // return a + i
#define eatom_add64( a, i)      __eatom_add64(&(a), (i))        // return a + i

#define eatom_fadd8( a, i)      __eatom_fadd8( &(a), (i))       // return a
#define eatom_fadd16(a, i)      __eatom_fadd16(&(a), (i))       // return a
#define eatom_fadd32(a, i)      __eatom_fadd32(&(a), (i))       // return a
#define eatom_fadd64(a, i)      __eatom_fadd64(&(a), (i))       // return a

#define eatom_sub8(  a, i)      __eatom_sub8 (&(a), (i))        // return a - i
#define eatom_sub16( a, i)      __eatom_sub16(&(a), (i))        // return a - i
#define eatom_sub32( a, i)      __eatom_sub32(&(a), (i))        // return a - i
#define eatom_sub64( a, i)      __eatom_sub64(&(a), (i))        // return a - i

#define eatom_fsub8(  a, i)     __eatom_fsub8 (&(a), (i))       // return a
#define eatom_fsub16( a, i)     __eatom_fsub16(&(a), (i))       // return a
#define eatom_fsub32( a, i)     __eatom_fsub32(&(a), (i))       // return a
#define eatom_fsub64( a, i)     __eatom_fsub64(&(a), (i))       // return a

/** -----------------------------------------------------------
 *  Atomic compare and swap.
 *
 *  Compare OLD with MEM, if identical, store NEW in MEM.
 *  Return the initial value in MEM.
 *  Success is indicated by comparing RETURN with OLD.
 *
*/
#define eatom_cas8( a, e, d)    __eatom_cas8 (&(a), (e), (d))   // return a
#define eatom_cas16(a, e, d)    __eatom_cas16(&(a), (e), (d))   // return a
#define eatom_cas32(a, e, d)    __eatom_cas32(&(a), (e), (d))   // return a
#define eatom_cas64(a, e, d)    __eatom_cas64(&(a), (e), (d))   // return a

/** -----------------------------------------------------------
 *  Atomic compare and exchange.
 *
 *  Compare MEM in a and ep
 *  If equal, store d in a and RETURN true
 *  Else set val in a to ep and RETURN false
 *
*/
#define eatom_cae8( a, e, d)     __eatom_cae8 (&(a), &(e), (d))
#define eatom_cae16(a, e, d)     __eatom_cae16(&(a), &(e), (d))
#define eatom_cae32(a, e, d)     __eatom_cae32(&(a), &(e), (d))
#define eatom_cae64(a, e, d)     __eatom_cae64(&(a), &(e), (d))

#endif  // __E_ATOM_H__
