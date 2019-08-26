#ifndef EATOM_P_GCC_SYNC_H_
#define EATOM_P_GCC_SYNC_H_

/** ------------------------------------------------
 *
 *  Thanks to JEMALLOC, this file is rebuilt from
 *
 *      jemalloc/include/jemalloc/internal/atomic_gcc_sync.h
 *
 *  ------------------------------------------------
*/

typedef enum {
    __eatomic_memory_order_relaxed,
    __eatomic_memory_order_acquire,
    __eatomic_memory_order_release,
    __eatomic_memory_order_acq_rel,
    __eatomic_memory_order_seq_cst
} __eatomic_memory_order_t;

#define __EATOMIC_INLINE static __always_inline

__EATOMIC_INLINE void
__eatomic_fence(__eatomic_memory_order_t mo) {
    /* Easy cases first: no barrier, and full barrier. */
    if (mo == __eatomic_memory_order_relaxed) {
        asm volatile("" ::: "memory");
        return;
    }
    if (mo == __eatomic_memory_order_seq_cst) {
        asm volatile("" ::: "memory");
        __sync_synchronize();
        asm volatile("" ::: "memory");
        return;
    }
    asm volatile("" ::: "memory");
#  if defined(__i386__) || defined(__x86_64__)
    /* This is implicit on x86. */
#  elif defined(__ppc64__)
    asm volatile("lwsync");
#  elif defined(__ppc__)
    asm volatile("sync");
#  elif defined(__sparc__) && defined(__arch64__)
    if (mo == __eatomic_memory_order_acquire) {
        asm volatile("membar #LoadLoad | #LoadStore");
    } else if (mo == __eatomic_memory_order_release) {
        asm volatile("membar #LoadStore | #StoreStore");
    } else {
        asm volatile("membar #LoadLoad | #LoadStore | #StoreStore");
    }
#  else
    __sync_synchronize();
#  endif
    asm volatile("" ::: "memory");
}

/*
 * A correct implementation of seq_cst loads and stores on weakly ordered
 * architectures could do either of the following:
 *   1. store() is weak-fence -> store -> strong fence, load() is load ->
 *      strong-fence.
 *   2. store() is strong-fence -> store, load() is strong-fence -> load ->
 *      weak-fence.
 * The tricky thing is, load() and store() above can be the load or store
 * portions of a gcc __sync builtin, so we have to follow GCC's lead, which
 * means going with strategy 2.
 * On strongly ordered architectures, the natural strategy is to stick a strong
 * fence after seq_cst stores, and have naked loads.  So we want the strong
 * fences in different places on different architectures.
 * atomic_pre_sc_load_fence and atomic_post_sc_store_fence allow us to
 * accomplish this.
 */

__EATOMIC_INLINE void
atomic_pre_sc_load_fence() {
#  if defined(__i386__) || defined(__x86_64__) ||                       \
    (defined(__sparc__) && defined(__arch64__))
    __eatomic_fence(__eatomic_memory_order_relaxed);
#  else
    __eatomic_fence(__eatomic_memory_order_seq_cst);
#  endif
}

__EATOMIC_INLINE void
atomic_post_sc_store_fence() {
#  if defined(__i386__) || defined(__x86_64__) ||                       \
    (defined(__sparc__) && defined(__arch64__))
    __eatomic_fence(__eatomic_memory_order_seq_cst);
#  else
    __eatomic_fence(__eatomic_memory_order_relaxed);
#  endif

}

#define __EATOMIC_GENERATE_ATOMICS(type, short_type,                     \
    /* unused */ lg_size)                                               \
typedef struct {                                                        \
    type volatile repr;                                                 \
} __eatomic_##short_type##_t;                                           \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_load_##short_type(const __eatomic_##short_type##_t *a,        \
    __eatomic_memory_order_t mo) {                                      \
    if (mo == __eatomic_memory_order_seq_cst) {                         \
        atomic_pre_sc_load_fence();                                     \
    }                                                                   \
    type result = a->repr;                                              \
    if (mo != __eatomic_memory_order_relaxed) {                         \
        __eatomic_fence(__eatomic_memory_order_acquire);                \
    }                                                                   \
    return result;                                                      \
}                                                                       \
                                                                        \
__EATOMIC_INLINE void                                                   \
__eatomic_store_##short_type(__eatomic_##short_type##_t *a,             \
    type val, __eatomic_memory_order_t mo) {                            \
    if (mo != __eatomic_memory_order_relaxed) {                         \
        __eatomic_fence(__eatomic_memory_order_release);                \
    }                                                                   \
    a->repr = val;                                                      \
    if (mo == __eatomic_memory_order_seq_cst) {                         \
        atomic_post_sc_store_fence();                                   \
    }                                                                   \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_exchange_##short_type(__eatomic_##short_type##_t *a, type val,\
    __eatomic_memory_order_t mo) {                                      \
    /*                                                                  \
     * Because of FreeBSD, we care about gcc 4.2, which doesn't have    \
     * an atomic exchange builtin.  We fake it with a CAS loop.         \
     */                                                                 \
    while (true) {                                                      \
        type old = a->repr;                                             \
        if (__sync_bool_compare_and_swap(&a->repr, old, val)) {         \
            return old;                                                 \
        }                                                               \
    }                                                                   \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_compare_and_swap_##short_type(__eatomic_##short_type##_t *a,	\
    type expected, type desired, __eatomic_memory_order_t mo) {         \
    return (type)__sync_val_compare_and_swap(&a->repr, expected, desired);  \
}                                                                       \
                                                                        \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_weak_##short_type(__eatomic_##short_type##_t *a,	\
    type *expected, type desired,                                       \
    __eatomic_memory_order_t success_mo,                                \
    __eatomic_memory_order_t failure_mo) {                              \
    type prev = __sync_val_compare_and_swap(&a->repr, *expected,        \
        desired);                                                       \
    if (prev == *expected) {                                            \
        return true;                                                    \
    } else {                                                            \
        *expected = prev;                                               \
        return false;                                                   \
    }                                                                   \
}                                                                       \
__EATOMIC_INLINE bool                                                   \
__eatomic_compare_exchange_strong_##short_type(__eatomic_##short_type##_t *a,	\
    type *expected, type desired,                                       \
    __eatomic_memory_order_t success_mo,                                \
    __eatomic_memory_order_t failure_mo) {                              \
    type prev = __sync_val_compare_and_swap(&a->repr, *expected,        \
        desired);                                                       \
    if (prev == *expected) {                                            \
        return true;                                                    \
    } else {                                                            \
        *expected = prev;                                               \
        return false;                                                   \
    }                                                                   \
}

#define __EATOMIC_GENERATE_INT_ATOMICS(type, short_type,                 \
    /* unused */ lg_size)                                               \
__EATOMIC_GENERATE_ATOMICS(type, short_type, /* unused */ lg_size)       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_add_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __sync_fetch_and_add(&a->repr, val);                         \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_sub_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __sync_fetch_and_sub(&a->repr, val);                         \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_and_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __sync_fetch_and_and(&a->repr, val);                         \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_or_##short_type(__eatomic_##short_type##_t *a, type val,\
    __eatomic_memory_order_t mo) {                                      \
    return __sync_fetch_and_or(&a->repr, val);                          \
}                                                                       \
                                                                        \
__EATOMIC_INLINE type                                                   \
__eatomic_fetch_xor_##short_type(__eatomic_##short_type##_t *a, type val,	\
    __eatomic_memory_order_t mo) {                                      \
    return __sync_fetch_and_xor(&a->repr, val);                         \
}

#endif
