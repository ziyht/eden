#ifndef EATOM_P_H_
#define EATOM_P_H_

#if defined(__linux__) || defined(__APPLE__)
#include "eatom_p_gcc_sync.h"
//#include "eatom_p_gcc_atomic.h"
//#include "eatom_p_c11.h"
#elif defined(WIN32)
#include "eatom_p_msvc.h"
#endif

__EATOMIC_GENERATE_INT_ATOMICS(i8 , 8 , 0)
__EATOMIC_GENERATE_INT_ATOMICS(i16, 16, 1)
__EATOMIC_GENERATE_INT_ATOMICS(i32, 32, 2)
__EATOMIC_GENERATE_INT_ATOMICS(i64, 64, 3)

#define __eatom_get8( p)                __eatom_get8_m (p, __eatomic_memory_order_acq_rel)
#define __eatom_get16(p)                __eatom_get16_m(p, __eatomic_memory_order_acq_rel)
#define __eatom_get32(p)                __eatom_get32_m(p, __eatomic_memory_order_acq_rel)
#define __eatom_get64(p)                __eatom_get64_m(p, __eatomic_memory_order_acq_rel)

#define __eatom_get8_m( p, mo)          __eatomic_load_8 ((__eatomic_8_t  *)p, (mo))
#define __eatom_get16_m(p, mo)          __eatomic_load_16((__eatomic_16_t *)p, (mo))
#define __eatom_get32_m(p, mo)          __eatomic_load_32((__eatomic_32_t *)p, (mo))
#define __eatom_get64_m(p, mo)          __eatomic_load_64((__eatomic_64_t *)p, (mo))

#define __eatom_set8( p, i)             __eatom_set8_m (p, i, __eatomic_memory_order_acq_rel)
#define __eatom_set16(p, i)             __eatom_set16_m(p, i, __eatomic_memory_order_acq_rel)
#define __eatom_set32(p, i)             __eatom_set32_m(p, i, __eatomic_memory_order_acq_rel)
#define __eatom_set64(p, i)             __eatom_set64_m(p, i, __eatomic_memory_order_acq_rel)

#define __eatom_set8_m( p, i, mo)       __eatomic_exchange_8 ((__eatomic_8_t  *)p, i, (mo))
#define __eatom_set16_m(p, i, mo)       __eatomic_exchange_16((__eatomic_16_t *)p, i, (mo))
#define __eatom_set32_m(p, i, mo)       __eatomic_exchange_32((__eatomic_32_t *)p, i, (mo))
#define __eatom_set64_m(p, i, mo)       __eatomic_exchange_64((__eatomic_64_t *)p, i, (mo))

#define __eatom_add8( p, v)             __eatom_add8_m (p, v, __eatomic_memory_order_acq_rel)
#define __eatom_add16(p, v)             __eatom_add16_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_add32(p, v)             __eatom_add32_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_add64(p, v)             __eatom_add64_m(p, v, __eatomic_memory_order_acq_rel)

#define __eatom_add8_m( p, v, mo)       (__eatom_fadd8_m( p, v, (mo)) + v)
#define __eatom_add16_m(p, v, mo)       (__eatom_fadd16_m(p, v, (mo)) + v)
#define __eatom_add32_m(p, v, mo)       (__eatom_fadd32_m(p, v, (mo)) + v)
#define __eatom_add64_m(p, v, mo)       (__eatom_fadd64_m(p, v, (mo)) + v)

#define __eatom_fadd8( p, v)            __eatom_fadd8_m (p, v, __eatomic_memory_order_acq_rel)
#define __eatom_fadd16(p, v)            __eatom_fadd16_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_fadd32(p, v)            __eatom_fadd32_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_fadd64(p, v)            __eatom_fadd64_m(p, v, __eatomic_memory_order_acq_rel)

#define __eatom_fadd8_m( p, v, mo)      __eatomic_fetch_add_8 ((__eatomic_8_t *)p, v, (mo))
#define __eatom_fadd16_m(p, v, mo)      __eatomic_fetch_add_16((__eatomic_16_t*)p, v, (mo))
#define __eatom_fadd32_m(p, v, mo)      __eatomic_fetch_add_32((__eatomic_32_t*)p, v, (mo))
#define __eatom_fadd64_m(p, v, mo)      __eatomic_fetch_add_64((__eatomic_64_t*)p, v, (mo))

#define __eatom_sub8( p, v)             __eatom_sub8_m (p, v, __eatomic_memory_order_acq_rel)
#define __eatom_sub16(p, v)             __eatom_sub16_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_sub32(p, v)             __eatom_sub32_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_sub64(p, v)             __eatom_sub64_m(p, v, __eatomic_memory_order_acq_rel)

#define __eatom_sub8_m( p, v, mo)       (__eatom_fsub8_m( p, v, (mo)) - v)
#define __eatom_sub16_m(p, v, mo)       (__eatom_fsub16_m(p, v, (mo)) - v)
#define __eatom_sub32_m(p, v, mo)       (__eatom_fsub32_m(p, v, (mo)) - v)
#define __eatom_sub64_m(p, v, mo)       (__eatom_fsub64_m(p, v, (mo)) - v)

#define __eatom_fsub8( p, v)            __eatom_fsub8_m (p, v, __eatomic_memory_order_acq_rel)
#define __eatom_fsub16(p, v)            __eatom_fsub16_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_fsub32(p, v)            __eatom_fsub32_m(p, v, __eatomic_memory_order_acq_rel)
#define __eatom_fsub64(p, v)            __eatom_fsub64_m(p, v, __eatomic_memory_order_acq_rel)

#define __eatom_fsub8_m( p, v, mo)      __eatomic_fetch_sub_8 ((__eatomic_8_t *)p, v, (mo))
#define __eatom_fsub16_m(p, v, mo)      __eatomic_fetch_sub_16((__eatomic_16_t*)p, v, (mo))
#define __eatom_fsub32_m(p, v, mo)      __eatomic_fetch_sub_32((__eatomic_32_t*)p, v, (mo))
#define __eatom_fsub64_m(p, v, mo)      __eatomic_fetch_sub_64((__eatomic_64_t*)p, v, (mo))

#define __eatom_cas8( p, e, d)          __eatom_cas8_m (p, (e), (d), __eatomic_memory_order_acq_rel)
#define __eatom_cas16(p, e, d)          __eatom_cas16_m(p, (e), (d), __eatomic_memory_order_acq_rel)
#define __eatom_cas32(p, e, d)          __eatom_cas32_m(p, (e), (d), __eatomic_memory_order_acq_rel)
#define __eatom_cas64(p, e, d)          __eatom_cas64_m(p, (e), (d), __eatomic_memory_order_acq_rel)

#define __eatom_cas8_m( p, e, d, mo)    __eatomic_compare_and_swap_8 ((__eatomic_8_t* )p, (e), (d), (mo))
#define __eatom_cas16_m(p, e, d, mo)    __eatomic_compare_and_swap_16((__eatomic_16_t*)p, (e), (d), (mo))
#define __eatom_cas32_m(p, e, d, mo)    __eatomic_compare_and_swap_32((__eatomic_32_t*)p, (e), (d), (mo))
#define __eatom_cas64_m(p, e, d, mo)    __eatomic_compare_and_swap_64((__eatomic_64_t*)p, (e), (d), (mo))

#define __eatom_cae8( p, ep, d)         __eatom_cae8_m (p, (i8* )(ep), (d), __eatomic_memory_order_acq_rel, __eatomic_memory_order_acquire)
#define __eatom_cae16(p, ep, d)         __eatom_cae16_m(p, (i16*)(ep), (d), __eatomic_memory_order_acq_rel, __eatomic_memory_order_acquire)
#define __eatom_cae32(p, ep, d)         __eatom_cae32_m(p, (i32*)(ep), (d), __eatomic_memory_order_acq_rel, __eatomic_memory_order_acquire)
#define __eatom_cae64(p, ep, d)         __eatom_cae64_m(p, (i64*)(ep), (d), __eatomic_memory_order_acq_rel, __eatomic_memory_order_acquire)

#define __eatom_cae8_m( p, ep, d, smo, fmo) __eatomic_compare_exchange_weak_8 ((__eatomic_8_t * )p, (i8* )(ep), (d), (smo), (fmo))
#define __eatom_cae16_m(p, ep, d, smo, fmo) __eatomic_compare_exchange_weak_16((__eatomic_16_t* )p, (i16*)(ep), (d), (smo), (fmo))
#define __eatom_cae32_m(p, ep, d, smo, fmo) __eatomic_compare_exchange_weak_32((__eatomic_32_t* )p, (i32*)(ep), (d), (smo), (fmo))
#define __eatom_cae64_m(p, ep, d, smo, fmo) __eatomic_compare_exchange_weak_64((__eatomic_64_t* )p, (i64*)(ep), (d), (smo), (fmo))

#endif
