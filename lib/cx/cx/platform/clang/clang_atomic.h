#pragma once

#include <stdatomic.h>

#define atomicInit(...) (__VA_ARGS__)

#define AtomicMemoryOrder memory_order
#define ATOMIC_MO_Relaxed memory_order_relaxed
#define ATOMIC_MO_Acquire memory_order_acquire
#define ATOMIC_MO_Release memory_order_release
#define ATOMIC_MO_AcqRel memory_order_acq_rel
#define ATOMIC_MO_SeqCst memory_order_seq_cst

#define atomicFence(order) atomic_thread_fence(ATOMIC_MO_##order)

#define atomic(type) cx_atomic_##type
#define atomicLoad(type, atomic_ptr, order)                             \
    _atomicLoad_##type(atomic_ptr, ATOMIC_MO_##order)
#define atomicStore(type, atomic_ptr, val, order)                       \
    _atomicStore_##type(atomic_ptr, val, ATOMIC_MO_##order)
#define atomicExchange(type, atomic_ptr, val, order)                    \
    _atomicExchange_##type(atomic_ptr, val, ATOMIC_MO_##order)
#define atomicCompareExchange(type, semantics, atomic_ptr, expected_ptr,\
                              desired, successorder, failorder)         \
    _atomicCompareExchange_##semantics##_##type(atomic_ptr,             \
        expected_ptr, desired,                                          \
        ATOMIC_MO_##successorder, ATOMIC_MO_##failorder)

#define atomicFetchAdd(type, atomic_ptr, val, order)                    \
    _atomicFetchAdd_##type(atomic_ptr, val, ATOMIC_MO_##order)
#define atomicFetchSub(type, atomic_ptr, val, order)                    \
    _atomicFetchSub_##type(atomic_ptr, val, ATOMIC_MO_##order)
#define atomicFetchAnd(type, atomic_ptr, val, order)                    \
    _atomicFetchAnd_##type(atomic_ptr, val, ATOMIC_MO_##order)
#define atomicFetchOr(type, atomic_ptr, val, order)                     \
    _atomicFetchOr_##type(atomic_ptr, val, ATOMIC_MO_##order)
#define atomicFetchXor(type, atomic_ptr, val, order)                    \
    _atomicFetchXor_##type(atomic_ptr, val, ATOMIC_MO_##order)

#define CX_GENERATE_ATOMICS(type, short_type,                           \
    /* unused */ lg_size)                                               \
typedef _Atomic(type) cx_atomic_##short_type;                           \
                                                                        \
_meta_inline type                                                       \
_atomicLoad_##short_type(const cx_atomic_##short_type *a,               \
    AtomicMemoryOrder mo) {                                             \
        /*                                                              \
         * A strict interpretation of the C standard prevents           \
         * atomic_load from taking a const argument, but it's           \
         * convenient for our purposes. This cast is a workaround.      \
         */                                                             \
        cx_atomic_##short_type* a_nonconst =                            \
            (cx_atomic_##short_type*)a;                                 \
        return atomic_load_explicit(a_nonconst, mo);                    \
}                                                                       \
                                                                        \
_meta_inline void                                                       \
_atomicStore_##short_type(cx_atomic_##short_type *a,                    \
    type val, AtomicMemoryOrder mo) {                                   \
        atomic_store_explicit(a, val, mo);                              \
}                                                                       \
                                                                        \
_meta_inline type                                                       \
_atomicExchange_##short_type(cx_atomic_##short_type *a, type val,       \
    AtomicMemoryOrder mo) {                                             \
        return atomic_exchange_explicit(a, val, mo);                    \
}                                                                       \
                                                                        \
_meta_inline bool                                                       \
_atomicCompareExchange_weak_##short_type(cx_atomic_##short_type *a,     \
    type *expected, type desired, AtomicMemoryOrder success_mo,         \
    AtomicMemoryOrder failure_mo) {                                     \
        return atomic_compare_exchange_weak_explicit(a, expected,       \
            desired, success_mo, failure_mo);                           \
}                                                                       \
                                                                        \
_meta_inline bool                                                       \
_atomicCompareExchange_strong_##short_type(cx_atomic_##short_type *a,   \
    type *expected, type desired, AtomicMemoryOrder success_mo,         \
    AtomicMemoryOrder failure_mo) {                                     \
        return atomic_compare_exchange_strong_explicit(a, expected,     \
            desired, success_mo, failure_mo);                           \
}

#define CX_EXTERN_ATOMICS(type, short_type)                             \
extern inline type                                                      \
_atomicLoad_##short_type(const cx_atomic_##short_type *a,               \
    AtomicMemoryOrder mo);                                              \
                                                                        \
extern inline void                                                      \
_atomicStore_##short_type(cx_atomic_##short_type *a,                    \
    type val, AtomicMemoryOrder mo);                                    \
                                                                        \
extern inline type                                                      \
_atomicExchange_##short_type(cx_atomic_##short_type *a, type val,       \
    AtomicMemoryOrder mo);                                              \
                                                                        \
extern inline bool                                                      \
_atomicCompareExchange_weak_##short_type(cx_atomic_##short_type *a,     \
    type *expected, type desired, AtomicMemoryOrder success_mo,         \
    AtomicMemoryOrder failure_mo);                                      \
                                                                        \
extern inline bool                                                      \
_atomicCompareExchange_strong_##short_type(cx_atomic_##short_type *a,   \
    type *expected, type desired, AtomicMemoryOrder success_mo,         \
    AtomicMemoryOrder failure_mo);

/*
 * Integral types have some special operations available that non-integral ones
 * lack.
 */
#define CX_GENERATE_INT_ATOMICS(type, short_type,                       \
    /* unused */ lg_size)                                               \
CX_GENERATE_ATOMICS(type, short_type, /* unused */ lg_size)             \
                                                                        \
_meta_inline type                                                       \
_atomicFetchAdd_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo) {                                   \
        return atomic_fetch_add_explicit(a, val, mo);                   \
}                                                                       \
_meta_inline type                                                       \
_atomicFetchSub_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo) {                                   \
        return atomic_fetch_sub_explicit(a, val, mo);                   \
}                                                                       \
_meta_inline type                                                       \
_atomicFetchAnd_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo) {                                   \
        return atomic_fetch_and_explicit(a, val, mo);                   \
}                                                                       \
_meta_inline type                                                       \
_atomicFetchOr_##short_type(cx_atomic_##short_type *a,                  \
    type val, AtomicMemoryOrder mo) {                                   \
        return atomic_fetch_or_explicit(a, val, mo);                    \
}                                                                       \
_meta_inline type                                                       \
_atomicFetchXor_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo) {                                   \
        return atomic_fetch_xor_explicit(a, val, mo);                   \
}

#define CX_EXTERN_INT_ATOMICS(type, short_type)                         \
CX_EXTERN_ATOMICS(type, short_type)                                     \
                                                                        \
extern inline type                                                      \
_atomicFetchAdd_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo);                                    \
extern inline type                                                      \
_atomicFetchSub_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo);                                    \
extern inline type                                                      \
_atomicFetchAnd_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo);                                    \
extern inline type                                                      \
_atomicFetchOr_##short_type(cx_atomic_##short_type *a,                  \
    type val, AtomicMemoryOrder mo);                                    \
extern inline type                                                      \
_atomicFetchXor_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtomicMemoryOrder mo);
