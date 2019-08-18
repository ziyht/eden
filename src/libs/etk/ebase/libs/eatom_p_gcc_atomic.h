#ifndef EATOM_P_GCC_ATOMIC_H_
#define EATOM_P_GCC_ATOMIC_H_

/** ------------------------------------------------
 *
 *  Thanks to JEMALLOC, this file is rebuilt from
 *
 *      jemalloc/include/jemalloc/internal/atomic_gcc_atomic.h
 *
 *  ------------------------------------------------
*/

#define __EATOMIC_INLINE static __always_inline

#define UNUSED

typedef enum {
    __eatomic_memory_order_relaxed,
    __eatomic_memory_order_acquire,
    __eatomic_memory_order_release,
    __eatomic_memory_order_acq_rel,
    __eatomic_memory_order_seq_cst
} __eatomic_memory_order_t;

__EATOMIC_INLINE int
__eatomic_enum_to_builtin(__eatomic_memory_order_t mo) {
    switch (mo) {
    case __eatomic_memory_order_relaxed:
        return __ATOMIC_RELAXED;
    case __eatomic_memory_order_acquire:
        return __ATOMIC_ACQUIRE;
    case __eatomic_memory_order_release:
        return __ATOMIC_RELEASE;
    case __eatomic_memory_order_acq_rel:
        return __ATOMIC_ACQ_REL;
    case __eatomic_memory_order_seq_cst:
        return __ATOMIC_SEQ_CST;
    }
    /* Can't happen; the switch is exhaustive. */
    //not_reached();
    perror("__eatomic_enum_to_builtin() checked unsupported __eatomic_memory_order");
    return -1;
}

__EATOMIC_INLINE void
__eatomic_fence(__eatomic_memory_order_t mo) {
    __atomic_thread_fence(__eatomic_enum_to_builtin(mo));
}

#define JEMALLOC_GENERATE_ATOMICS(type, short_type,                     \
    /* unused */ lg_size)                                               \
typedef struct {                                                        \
    type repr;                                                          \
} __eatomic_##short_type##_t;                                           \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_load_##short_type(const __eatomic_##short_type##_t *a,        \
    __eatomic_memory_order_t mo) {                                      \
    type result;                                                        \
    __atomic_load(&a->repr, &result, __eatomic_enum_to_builtin(mo));    \
    return result;                                                      \
}                                                                       \
                                                                        \
__EATOMIC_INLINE void                                                   \
__eatomic_store_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    __atomic_store(&a->repr, &val, __eatomic_enum_to_builtin(mo));      \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_exchange_##short_type(__eatomic_##short_type##_t *a, type val,\
    __eatomic_memory_order_t mo) {                                      \
    type result;                                                        \
    __atomic_exchange(&a->repr, &val, &result,                          \
        __eatomic_enum_to_builtin(mo));                                 \
    return result;                                                      \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_compare_and_swap_##short_type(__eatomic_##short_type##_t *a,	\
type expected, type desired, __eatomic_memory_order_t mo) {             \
    __atomic_compare_exchange(&a->repr, &expected, &desired,            \
        true, __eatomic_enum_to_builtin(mo),                            \
        __eatomic_enum_to_builtin(mo));                                 \
    return expected;                                                    \
}                                                                       \
                                                                        \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_weak_##short_type(__eatomic_##short_type##_t *a,	\
    UNUSED type *expected, type desired,                                \
    __eatomic_memory_order_t success_mo,                                \
    __eatomic_memory_order_t failure_mo) {                              \
    return __atomic_compare_exchange(&a->repr, expected, &desired,      \
        true, __eatomic_enum_to_builtin(success_mo),                    \
        __eatomic_enum_to_builtin(failure_mo));                         \
}                                                                       \
                                                                        \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_strong_##short_type(__eatomic_##short_type##_t *a,	\
    UNUSED type *expected, type desired,                                \
    __eatomic_memory_order_t success_mo,                                \
    __eatomic_memory_order_t failure_mo) {                              \
    return __atomic_compare_exchange(&a->repr, expected, &desired,      \
        false,                                                          \
        __eatomic_enum_to_builtin(success_mo),                          \
        __eatomic_enum_to_builtin(failure_mo));                         \
}


#define JEMALLOC_GENERATE_INT_ATOMICS(type, short_type,                 \
    /* unused */ lg_size)                                               \
JEMALLOC_GENERATE_ATOMICS(type, short_type, /* unused */ lg_size)       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_add_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __atomic_fetch_add(&a->repr, val,                            \
        __eatomic_enum_to_builtin(mo));                                 \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_sub_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __atomic_fetch_sub(&a->repr, val,                            \
        __eatomic_enum_to_builtin(mo));                                 \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_and_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __atomic_fetch_and(&a->repr, val,                            \
        __eatomic_enum_to_builtin(mo));                                 \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_or_##short_type(__eatomic_##short_type##_t *a, type val,\
    __eatomic_memory_order_t mo) {                                      \
    return __atomic_fetch_or(&a->repr, val,                             \
        __eatomic_enum_to_builtin(mo));                                 \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_xor_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __atomic_fetch_xor(&a->repr, val,                            \
        __eatomic_enum_to_builtin(mo));                                 \
}

#endif
