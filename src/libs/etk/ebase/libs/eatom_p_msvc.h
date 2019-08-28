#ifndef EATOM_P_MSVC_H_
#define EATOM_P_MSVC_H_

/** ------------------------------------------------
 *
 *  Thanks to JEMALLOC, this file is rebuilt from
 *
 *      jemalloc/include/jemalloc/internal/atomic_msvc.h
 *
 *  ------------------------------------------------
*/

//#include <Windows.h>  // (include Windows.h)

typedef enum {
    __eatomic_memory_order_relaxed,
    __eatomic_memory_order_acquire,
    __eatomic_memory_order_release,
    __eatomic_memory_order_acq_rel,
    __eatomic_memory_order_seq_cst
} __eatomic_memory_order_t;

typedef char  __eatomic_repr_0_t;
typedef short __eatomic_repr_1_t;
typedef long  __eatomic_repr_2_t;
typedef i64   __eatomic_repr_3_t;

#define __EATOMIC_INLINE static __always_inline

__EATOMIC_INLINE void
__eatomic_fence(__eatomic_memory_order_t mo) {
    _ReadWriteBarrier();
#  if defined(_M_ARM) || defined(_M_ARM64)
    /* ARM needs a barrier for everything but relaxed. */
    if (mo != __eatomic_memory_order_relaxed) {
        MemoryBarrier();
    }
#  elif defined(_M_IX86) || defined (_M_X64)
    /* x86 needs a barrier only for seq_cst. */
    if (mo == __eatomic_memory_order_seq_cst) {
        MemoryBarrier();
    }
#  else
#  error "Don't know how to create atomics for this platform for MSVC."
#  endif
    _ReadWriteBarrier();
}

#define ATOMIC_INTERLOCKED_REPR(lg_size) __eatomic_repr_ ## lg_size ## _t

#define ATOMIC_CONCAT(a, b) ATOMIC_RAW_CONCAT(a, b)
#define ATOMIC_RAW_CONCAT(a, b) a ## b

#define ATOMIC_INTERLOCKED_NAME(base_name, lg_size) ATOMIC_CONCAT(      \
    base_name, ATOMIC_INTERLOCKED_SUFFIX(lg_size))

#define ATOMIC_INTERLOCKED_SUFFIX(lg_size)                              \
    ATOMIC_CONCAT(ATOMIC_INTERLOCKED_SUFFIX_, lg_size)

#define ATOMIC_INTERLOCKED_SUFFIX_0 8
#define ATOMIC_INTERLOCKED_SUFFIX_1 16
#define ATOMIC_INTERLOCKED_SUFFIX_2
#define ATOMIC_INTERLOCKED_SUFFIX_3 64

#define __EATOMIC_GENERATE_ATOMICS(type, short_type, lg_size)           \
typedef struct {                                                        \
    ATOMIC_INTERLOCKED_REPR(lg_size) repr;                              \
} __eatomic_##short_type##_t;                                           \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_load_##short_type(const __eatomic_##short_type##_t *a,        \
    __eatomic_memory_order_t mo) {                                      \
    ATOMIC_INTERLOCKED_REPR(lg_size) ret = a->repr;                     \
    if (mo != __eatomic_memory_order_relaxed) {                         \
        __eatomic_fence(__eatomic_memory_order_acquire);                \
    }                                                                   \
    return (type) ret;                                                  \
}                                                                       \
                                                                        \
__EATOMIC_INLINE void                                                   \
__eatomic_store_##short_type(__eatomic_##short_type##_t *a,             \
    type val, __eatomic_memory_order_t mo) {                            \
    if (mo != __eatomic_memory_order_relaxed) {                         \
        __eatomic_fence(__eatomic_memory_order_release);                \
    }                                                                   \
    a->repr = (ATOMIC_INTERLOCKED_REPR(lg_size)) val;                   \
    if (mo == __eatomic_memory_order_seq_cst) {                         \
        __eatomic_fence(__eatomic_memory_order_seq_cst);                \
    }                                                                   \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_exchange_##short_type(__eatomic_##short_type##_t *a, type val,\
    __eatomic_memory_order_t mo) {                                      \
    return (type)ATOMIC_INTERLOCKED_NAME(_InterlockedExchange,          \
        lg_size)(&a->repr, (ATOMIC_INTERLOCKED_REPR(lg_size))val);      \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_compare_and_swap_##short_type(__eatomic_##short_type##_t *a,	\
    type expected, type desired, __eatomic_memory_order_t mo) {         \
    return (type)ATOMIC_INTERLOCKED_NAME(_InterlockedCompareExchange,   \
        lg_size)(&a->repr, desired, expected);                          \
}                                                                       \
                                                                        \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_weak_##short_type(__eatomic_##short_type##_t *a,	\
    type *expected, type desired, __eatomic_memory_order_t success_mo,  \
    __eatomic_memory_order_t failure_mo) {                              \
    ATOMIC_INTERLOCKED_REPR(lg_size) e =                                \
        (ATOMIC_INTERLOCKED_REPR(lg_size))*expected;                    \
    ATOMIC_INTERLOCKED_REPR(lg_size) d =                                \
        (ATOMIC_INTERLOCKED_REPR(lg_size))desired;                      \
    ATOMIC_INTERLOCKED_REPR(lg_size) old =                              \
        ATOMIC_INTERLOCKED_NAME(_InterlockedCompareExchange,            \
        lg_size)(&a->repr, d, e);                                       \
    if (old == e) {                                                     \
        return true;                                                    \
    } else {                                                            \
        *expected = (type)old;                                          \
        return false;                                                   \
    }                                                                   \
}                                                                       \
                                                                        \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_strong_##short_type(__eatomic_##short_type##_t *a,	\
    type *expected, type desired, __eatomic_memory_order_t success_mo,  \
    __eatomic_memory_order_t failure_mo) {                              \
    /* We implement the weak version with strong semantics. */          \
    return __eatomic_compare_exchange_weak_##short_type(a, expected,    \
        desired, success_mo, failure_mo);                               \
}


#define __EATOMIC_GENERATE_INT_ATOMICS(type, short_type, lg_size)       \
__EATOMIC_GENERATE_ATOMICS(type, short_type, lg_size)                   \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_add_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return (type)ATOMIC_INTERLOCKED_NAME(_InterlockedExchangeAdd,       \
        lg_size)(&a->repr, (ATOMIC_INTERLOCKED_REPR(lg_size))val);      \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_add_fetch_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return (type)ATOMIC_INTERLOCKED_NAME(_InterlockedExchangeAdd,       \
        lg_size)(&a->repr, (ATOMIC_INTERLOCKED_REPR(lg_size))val) + val;\
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_sub_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    /*                                                                  \
     * MSVC warns on negation of unsigned operands, but for us it       \
     * gives exactly the right semantics (MAX_TYPE + 1 - operand).      \
     */                                                                 \
    __pragma(warning(push))                                             \
    __pragma(warning(disable: 4146))                                    \
    return __eatomic_fetch_add_##short_type(a, -val, mo);               \
    __pragma(warning(pop))                                              \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_sub_fetch_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    /*                                                                  \
     * MSVC warns on negation of unsigned operands, but for us it       \
     * gives exactly the right semantics (MAX_TYPE + 1 - operand).      \
     */                                                                 \
    __pragma(warning(push))                                             \
    __pragma(warning(disable: 4146))                                    \
    return __eatomic_fetch_add_##short_type(a, -val, mo) - val;         \
    __pragma(warning(pop))                                              \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_and_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return (type)ATOMIC_INTERLOCKED_NAME(_InterlockedAnd, lg_size)(     \
        &a->repr, (ATOMIC_INTERLOCKED_REPR(lg_size))val);               \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_or_##short_type(__eatomic_##short_type##_t *a,          \
    type val, __eatomic_memory_order_t mo) {                            \
    return (type)ATOMIC_INTERLOCKED_NAME(_InterlockedOr, lg_size)(      \
        &a->repr, (ATOMIC_INTERLOCKED_REPR(lg_size))val);               \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_xor_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return (type)ATOMIC_INTERLOCKED_NAME(_InterlockedXor, lg_size)(     \
        &a->repr, (ATOMIC_INTERLOCKED_REPR(lg_size))val);               \
}

#endif
