#ifndef EATOM_P_C11_H_
#define EATOM_P_C11_H_

/** ------------------------------------------------
 *
 *  Thanks to JEMALLOC, this file is rebuilt from
 *
 *      jemalloc/include/jemalloc/internal/atomic_c11.h
 *
 *  ------------------------------------------------
*/

#include <stdatomic.h>

#define __EATOMIC_INLINE static __always_inline

#define __eatomic_memory_order_t       memory_order
#define __eatomic_memory_order_relaxed memory_order_relaxed
#define __eatomic_memory_order_acquire memory_order_acquire
#define __eatomic_memory_order_release memory_order_release
#define __eatomic_memory_order_acq_rel memory_order_acq_rel
#define __eatomic_memory_order_seq_cst memory_order_seq_cst

#define __eatomic_fence atomic_thread_fence

#define __EATOMIC_GENERATE_ATOMICS(type, short_type,                    \
    /* unused */ lg_size)                                               \
typedef _Atomic(type) __eatomic_##short_type##_t;                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_load_##short_type(const __eatomic_##short_type##_t *a,        \
    __eatomic_memory_order_t mo) {                                      \
    /*                                                                  \
     * A strict interpretation of the C standard prevents               \
     * atomic_load from taking a const argument, but it's               \
     * convenient for our purposes. This cast is a workaround.          \
     */                                                                 \
    __eatomic_##short_type##_t* a_nonconst =                            \
        (__eatomic_##short_type##_t*)a;                                 \
    return atomic_load_explicit(a_nonconst, mo);                        \
}                                                                       \
                                                                        \
__EATOMIC_INLINE void                                                   \
__eatomic_store_##short_type(__eatomic_##short_type##_t *a,             \
    type val, __eatomic_memory_order_t mo) {                            \
    atomic_store_explicit(a, val, mo);                                  \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_exchange_##short_type(__eatomic_##short_type##_t *a, type val,\
    __eatomic_memory_order_t mo) {                                      \
    return atomic_exchange_explicit(a, val, mo);                        \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_compare_and_swap_##short_type(__eatomic_##short_type##_t *a,	\
type expected, type desired, __eatomic_memory_order_t mo) {             \
    atomic_compare_exchange_weak_explicit(a, &expected, desired,        \
         (mo), (mo));                                                   \
    return expected;                                                    \
}                                                                       \
                                                                        \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_weak_##short_type(__eatomic_##short_type##_t *a,	\
    type *expected, type desired, __eatomic_memory_order_t success_mo,  \
    __eatomic_memory_order_t failure_mo) {                              \
    return atomic_compare_exchange_weak_explicit(a, expected,           \
        desired, success_mo, failure_mo);                               \
}                                                                       \
                                                                        \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_strong_##short_type(__eatomic_##short_type##_t *a,	\
    type *expected, type desired, __eatomic_memory_order_t success_mo,  \
    __eatomic_memory_order_t failure_mo) {                              \
    return atomic_compare_exchange_strong_explicit(a, expected,         \
        desired, success_mo, failure_mo);                               \
}

/*
 * Integral types have some special operations available that non-integral ones
 * lack.
 */
#define __EATOMIC_GENERATE_INT_ATOMICS(type, short_type,                \
    /* unused */ lg_size)                                               \
__EATOMIC_GENERATE_ATOMICS(type, short_type, /* unused */ lg_size)      \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_add_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return atomic_fetch_add_explicit(a, val, mo);                       \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_add_fetch_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return atomic_add_fetch_explicit(a, val, mo);                       \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_sub_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return atomic_fetch_sub_explicit(a, val, mo);                       \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_sub_fetch_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return atomic_sub_fetch_explicit(a, val, mo);                       \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_and_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return atomic_fetch_and_explicit(a, val, mo);                       \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_or_##short_type(__eatomic_##short_type##_t *a,          \
    type val, __eatomic_memory_order_t mo) {                            \
    return atomic_fetch_or_explicit(a, val, mo);                        \
}                                                                       \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_xor_##short_type(__eatomic_##short_type##_t *a,         \
    type val, __eatomic_memory_order_t mo) {                            \
    return atomic_fetch_xor_explicit(a, val, mo);                       \
}

#endif
