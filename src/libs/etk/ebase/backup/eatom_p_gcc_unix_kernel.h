#ifndef EATOM_P_GCC_UNIX_KERNEL_H_
#define EATOM_P_GCC_UNIX_KERNEL_H_

#error "this is a abandoned version in previous time, only for backup, never using and checked"

/**
  * from linux kernel, do not using this now
  */

#define CONFIG_SMP

#ifdef  CONFIG_SMP
#define LOCK_PREFIX_HERE \
        ".pushsection .smp_locks,\"a\"\n"	\
        ".balign 4\n"				\
        ".long 671f - .\n" /* offset */		\
        ".popsection\n"				\
        "671:"

#define LOCK_PREFIX LOCK_PREFIX_HERE "\n\tlock; "

#else /* ! CONFIG_SMP */
#define LOCK_PREFIX_HERE ""
#define LOCK_PREFIX ""
#endif

//! -----------------------------------------------------------
//! An exchange-type operation
//!
//! Return the initial value in MEM.
//!
//!

#define __xchg8_op__( p, i, op, lock) ({ u8  __ret = (i); asm volatile (lock #op "b %b0, %1\n" : "+q" (__ret), "+m" (*(p)) : : "memory", "cc"); __ret; })
#define __xchg16_op__(p, i, op, lock) ({ u16 __ret = (i); asm volatile (lock #op "w %w0, %1\n" : "+r" (__ret), "+m" (*(p)) : : "memory", "cc"); __ret; })
#define __xchg32_op__(p, i, op, lock) ({ u32 __ret = (i); asm volatile (lock #op "l %0, %1\n"  : "+r" (__ret), "+m" (*(p)) : : "memory", "cc"); __ret; })
#define __xchg64_op__(p, i, op, lock) ({ u64 __ret = (i); asm volatile (lock #op "q %q0, %1\n" : "+r" (__ret), "+m" (*(p)) : : "memory", "cc"); __ret; })

//! -----------------------------------------------------------
//! Atomic exchange
//!
//! Note: no "lock" prefix even on SMP: xchg always implies lock anyway.
//! Since this is generally used to protect other memory information, we
//! use "asm volatile" and "memory" clobbers to prevent gcc from moving
//! information around.
//!

#define __xchg8( p, i) __xchg8_op__( (p), (i), xchg, "")
#define __xchg16(p, i) __xchg16_op__((p), (i), xchg, "")
#define __xchg32(p, i) __xchg32_op__((p), (i), xchg, "")
#define __xchg64(p, i) __xchg64_op__((p), (i), xchg, "")

//! -----------------------------------------------------------
//! Atomic compare and exchange.
//!
//! Compare OLD with MEM, if identical, store NEW in MEM.
//! Return the initial value in MEM.
//! Success is indicated by comparing RETURN with OLD.
//!
//! __cmpxchg()       is locked when multiple CPUs are online
//! __cmpxchg_sync()  is always locked
//! __cmpxchg_local() is never locked
//!

#define __cmpxchg8( ep, o, n)   __raw8_cmpxchg( ep, o, n, LOCK_PREFIX)
#define __cmpxchg16(ep, o, n)   __raw16_cmpxchg(ep, o, n, LOCK_PREFIX)
#define __cmpxchg32(ep, o, n)   __raw32_cmpxchg(ep, o, n, LOCK_PREFIX)
#define __cmpxchg64(ep, o, n)   __raw64_cmpxchg(ep, o, n, LOCK_PREFIX)

#define __cmpxchg8_sync(  ep, o, n)   __raw8_cmpxchg( ep, o, n, "lock; ")
#define __cmpxchg16_sync( ep, o, n)   __raw16_cmpxchg(ep, o, n, "lock; ")
#define __cmpxchg32_sync( ep, o, n)   __raw32_cmpxchg(ep, o, n, "lock; ")
#define __cmpxchg64_sync( ep, o, n)   __raw64_cmpxchg(ep, o, n, "lock; ")

#define __cmpxchg8_local( ep, o, n)   __raw8_cmpxchg( ep, o, n, "")
#define __cmpxchg16_local(ep, o, n)   __raw16_cmpxchg(ep, o, n, "")
#define __cmpxchg32_local(ep, o, n)   __raw32_cmpxchg(ep, o, n, "")
#define __cmpxchg64_local(ep, o, n)   __raw64_cmpxchg(ep, o, n, "")

#define __raw8_cmpxchg( ep, o, n, lock) ({ u8  __ret; u8  __old = (o);  u8 __new = (n); volatile u8  *__ptr = (volatile u8  *)(ep); asm volatile(lock "cmpxchgb %2,%1" : "=a" (__ret), "+m" (*__ptr) : "q" (__new), "0" (__old) : "memory"); __ret; })
#define __raw16_cmpxchg(ep, o, n, lock) ({ u16 __ret; u16 __old = (o); u16 __new = (n); volatile u16 *__ptr = (volatile u16 *)(ep);	asm volatile(lock "cmpxchgw %2,%1" : "=a" (__ret), "+m" (*__ptr) : "r" (__new), "0" (__old) : "memory"); __ret; })
#define __raw32_cmpxchg(ep, o, n, lock) ({ u32 __ret; u32 __old = (o); u32 __new = (n); volatile u32 *__ptr = (volatile u32 *)(ep);	asm volatile(lock "cmpxchgl %2,%1" : "=a" (__ret), "+m" (*__ptr) : "r" (__new), "0" (__old) : "memory"); __ret; })
#define __raw64_cmpxchg(ep, o, n, lock) ({ u64 __ret; u64 __old = (o); u64 __new = (n); volatile u64 *__ptr = (volatile u64 *)(ep);	asm volatile(lock "cmpxchgq %2,%1" : "=a" (__ret), "+m" (*__ptr) : "r" (__new), "0" (__old) : "memory"); __ret; })

//! -----------------------------------------------------------
//! Atomic add operation.
//!
//! Return the initial value in MEM.
//!
//! __xadd()       is locked when multiple CPUs are online
//! __xadd_sync()  is always locked
//! __xadd_local() is never locked
//!

#define __xadd8_op__( ptr, inc, lock)	__xchg8_op__( (ptr), (inc), xadd, lock)
#define __xadd16_op__(ptr, inc, lock)	__xchg16_op__((ptr), (inc), xadd, lock)
#define __xadd32_op__(ptr, inc, lock)	__xchg32_op__((ptr), (inc), xadd, lock)
#define __xadd64_op__(ptr, inc, lock)	__xchg64_op__((ptr), (inc), xadd, lock)

#define __xadd8( p, i)       __xadd8_op__( (p), (i), LOCK_PREFIX)
#define __xadd16(p, i)       __xadd16_op__((p), (i), LOCK_PREFIX)
#define __xadd32(p, i)       __xadd32_op__((p), (i), LOCK_PREFIX)
#define __xadd64(p, i)       __xadd64_op__((p), (i), LOCK_PREFIX)

#define __xadd8_sync( p, i)  __xadd8_op__( (p), (i), "lock; ")
#define __xadd16_sync(p, i)  __xadd16_op__((p), (i), "lock; ")
#define __xadd32_sync(p, i)  __xadd32_op__((p), (i), "lock; ")
#define __xadd64_sync(p, i)  __xadd64_op__((p), (i), "lock; ")

#define __xadd8_local( p, i)  __xadd8_op__( (p), (i), "")
#define __xadd16_local(p, i)  __xadd16_op__((p), (i), "")
#define __xadd32_local(p, i)  __xadd32_op__((p), (i), "")
#define __xadd64_local(p, i)  __xadd64_op__((p), (i), "")

//! -----------------------------------------------------------
//! eatom definition
//!
//!     using linux kernel
//!

#define __eatom_get8( p)    (*(volatile i8  *)p)
#define __eatom_get16(p)    (*(volatile i16 *)p)
#define __eatom_get32(p)    (*(volatile i32 *)p)
#define __eatom_get64(p)    (*(volatile i64 *)p)

#define __eatom_set8( p, v) (__xchg8 (p, v), v)
#define __eatom_set16(p, v) (__xchg16(p, v), v)
#define __eatom_set32(p, v) (__xchg32(p, v), v)
#define __eatom_set64(p, v) (__xchg64(p, v), v)


static __always_inline i32 __eatom_add32(i32* p, i32 v)
{ asm volatile(LOCK_PREFIX "addl %1,%0" : "+m" (*p) : "ir" (v) );  return __eatom_get32(p);}

static __always_inline i64 __eatom_add64(i64* p, i64 v)
{ asm volatile(LOCK_PREFIX "addq %1,%0" : "=m" (*p) : "er" (v), "m" (*p) );  return __eatom_get64(p);}

static __always_inline i32 __eatom_sub32(i32* p, i32 v)
{ asm volatile(LOCK_PREFIX "subl %1,%0" : "+m" (*p) : "ir" (v) );  return __eatom_get32(p);}

static __always_inline i64 __eatom_sub64(i64* p, i64 v)
{ asm volatile(LOCK_PREFIX "subq %1,%0" : "=m" (*p) : "er" (v), "m" (*p) );  return __eatom_get64(p);}

#define __eatom_add32_isn(p, v) ({unsigned char c;asm volatile(LOCK_PREFIX "addl %2,%0; sets %1" : "+m" (*p), "=qm" (c) : "ir" (v) : "memory"); c;})

#define __eatom_sub32_is0(p,v) ({ unsigned char c; asm volatile(LOCK_PREFIX "subl %2,%0; sete %1" : "+m" (*p), "=qm" (c) : "ir" (v)           : "memory"); c;})
#define __eatom_sub64_is0(p,v) ({ unsigned char c; asm volatile(LOCK_PREFIX "subq %2,%0; sete %1" : "=m" (*p), "=qm" (c) : "er" (v), "m" (*p) : "memory"); c;})


#define __eatom_xadd32(p, v)  __xadd32(p, v)
#define __eatom_xadd64(p, v)  __xadd64(p, v)

#define __eatom_inc16(p)    ({ asm         (LOCK_PREFIX "addw $1, %0" : "+m" (*p)); *p ;})
#define __eatom_inc32(p)    ({ asm volatile(LOCK_PREFIX "incl %0"     : "+m" (*p)); *p ;})

#define __eatom_dec32(p)    ({ asm volatile(LOCK_PREFIX "decl %0"     : "+m" (*p)); *p ;})

#define __eatom_inc32_is0(p) ({	unsigned char c; asm volatile(LOCK_PREFIX "incl %0; sete %1" : "+m" (*p), "=qm" (c) : : "memory"); c != 0;})
#define __eatom_dec32_is0(p) ({	unsigned char c; asm volatile(LOCK_PREFIX "decl %0; sete %1" : "+m" (*p), "=qm" (c) : : "memory"); c != 0;})


#endif
