#pragma once

#if defined(_M_X64)
#define CX_MemoryBarrier __faststorefence
#define CX_ReadWriteBarrier _ReadWriteBarrier
#elif defined(_M_IX86)
__forceinline void CX_MemoryBarrier(void)
{
    long Barrier;

    _InterlockedOr(&Barrier, 0);
    return;
}
#define CX_ReadWriteBarrier _ReadWriteBarrier
#else
#error "Don't know how to create atomics for this platform for MSVC."
#endif

#include "cx/utils/macros.h"

#define atomicInit(...) {__VA_ARGS__}

typedef enum {
    ATOMIC_MO_Relaxed,
    ATOMIC_MO_Acquire,
    ATOMIC_MO_Release,
    ATOMIC_MO_AcqRel,
    ATOMIC_MO_SeqCst
} AtmoicMemoryOrder;

typedef char _cx_atomic_repr_0_t;
typedef short _cx_atomic_repr_1_t;
typedef long _cx_atomic_repr_2_t;
typedef __int64 _cx_atomic_repr_3_t;

_meta_inline void
_atomicFence(AtmoicMemoryOrder mo)
{
    CX_ReadWriteBarrier();
#  if defined(_M_ARM) || defined(_M_ARM64)
        /* ARM needs a barrier for everything but relaxed. */
    if (mo != ATOMIC_MO_Relaxed) {
        CX_MemoryBarrier();
    }
#  elif defined(_M_IX86) || defined (_M_X64)
        /* x86 needs a barrier only for seq_cst. */
    if (mo == ATOMIC_MO_SeqCst) {
        CX_MemoryBarrier();
    }
#  else
#  error "Don't know how to create atomics for this platform for MSVC."
#  endif
    CX_ReadWriteBarrier();
}
#define atomicFence(order) _atomicFence(ATOMIC_MO_##order)

#define CX_ATOMIC_IL_REPR(lg_size) _cx_atomic_repr_ ## lg_size ## _t

#define CX_ATOMIC_IL_NAME(base_name, lg_size) tokconcat(                \
    base_name, CX_ATOMIC_IL_SUFFIX(lg_size))

#define CX_ATOMIC_IL_SUFFIX(lg_size)                                    \
    tokconcat(CX_ATOMIC_IL_SUFFIX_, lg_size)

#define CX_ATOMIC_IL_SUFFIX_0 8
#define CX_ATOMIC_IL_SUFFIX_1 16
#define CX_ATOMIC_IL_SUFFIX_2
#define CX_ATOMIC_IL_SUFFIX_3 64

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

#define CX_GENERATE_ATOMICS(type, short_type, lg_size)                  \
typedef struct {                                                        \
        CX_ATOMIC_IL_REPR(lg_size) repr;                                \
} cx_atomic_##short_type;                                               \
                                                                        \
_meta_inline type                                                       \
_atomicLoad_##short_type(const cx_atomic_##short_type *a,               \
    AtmoicMemoryOrder mo) {                                             \
        CX_ATOMIC_IL_REPR(lg_size) ret = a->repr;                       \
        if (mo != ATOMIC_MO_Relaxed) {                                  \
                _atomicFence(ATOMIC_MO_Acquire);                        \
        }                                                               \
        return (type) ret;                                              \
}                                                                       \
                                                                        \
_meta_inline void                                                       \
_atomicStore_##short_type(cx_atomic_##short_type *a,                    \
    type val, AtmoicMemoryOrder mo) {                                   \
        if (mo != ATOMIC_MO_Relaxed) {                                  \
                _atomicFence(ATOMIC_MO_Release);                        \
        }                                                               \
        a->repr = (CX_ATOMIC_IL_REPR(lg_size)) val;                     \
        if (mo == ATOMIC_MO_SeqCst) {                                   \
                _atomicFence(ATOMIC_MO_SeqCst);                         \
        }                                                               \
}                                                                       \
                                                                        \
_meta_inline type                                                       \
_atomicExchange_##short_type(cx_atomic_##short_type *a, type val,       \
    AtmoicMemoryOrder mo) {                                             \
        return (type)CX_ATOMIC_IL_NAME(_InterlockedExchange,            \
            lg_size)(&a->repr, (CX_ATOMIC_IL_REPR(lg_size))val);        \
}                                                                       \
                                                                        \
_meta_inline bool                                                       \
_atomicCompareExchange_weak_##short_type(cx_atomic_##short_type *a,     \
    type *expected, type desired, AtmoicMemoryOrder success_mo,         \
    AtmoicMemoryOrder failure_mo) {                                     \
        CX_ATOMIC_IL_REPR(lg_size) e =                                  \
            (CX_ATOMIC_IL_REPR(lg_size))*expected;                      \
        CX_ATOMIC_IL_REPR(lg_size) d =                                  \
            (CX_ATOMIC_IL_REPR(lg_size))desired;                        \
        CX_ATOMIC_IL_REPR(lg_size) old =                                \
            CX_ATOMIC_IL_NAME(_InterlockedCompareExchange,              \
                lg_size)(&a->repr, d, e);                               \
        if (old == e) {                                                 \
                return true;                                            \
        } else {                                                        \
                *expected = (type)old;                                  \
                return false;                                           \
        }                                                               \
}                                                                       \
                                                                        \
_meta_inline bool                                                       \
_atomicCompareExchange_strong_##short_type(cx_atomic_##short_type *a,   \
    type *expected, type desired, AtmoicMemoryOrder success_mo,         \
    AtmoicMemoryOrder failure_mo) {                                     \
        /* We implement the weak version with strong semantics. */      \
        return _atomicCompareExchange_weak_##short_type(a, expected,    \
            desired, success_mo, failure_mo);                           \
}

#define CX_EXTERN_ATOMICS(type, short_type)                             \
extern inline type                                                      \
_atomicLoad_##short_type(const cx_atomic_##short_type *a,               \
    AtmoicMemoryOrder mo);                                              \
                                                                        \
extern inline void                                                      \
_atomicStore_##short_type(cx_atomic_##short_type *a,                    \
    type val, AtmoicMemoryOrder mo);                                    \
                                                                        \
extern inline type                                                      \
_atomicExchange_##short_type(cx_atomic_##short_type *a, type val,       \
    AtmoicMemoryOrder mo);                                              \
                                                                        \
extern inline bool                                                      \
_atomicCompareExchange_weak_##short_type(cx_atomic_##short_type *a,     \
    type *expected, type desired, AtmoicMemoryOrder success_mo,         \
    AtmoicMemoryOrder failure_mo);                                      \
                                                                        \
extern inline bool                                                      \
_atomicCompareExchange_strong_##short_type(cx_atomic_##short_type *a,   \
    type *expected, type desired, AtmoicMemoryOrder success_mo,         \
    AtmoicMemoryOrder failure_mo);

#define CX_GENERATE_INT_ATOMICS(type, short_type, lg_size)              \
CX_GENERATE_ATOMICS(type, short_type, lg_size)                          \
                                                                        \
_meta_inline type                                                       \
_atomicFetchAdd_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtmoicMemoryOrder mo) {                                   \
        return (type)CX_ATOMIC_IL_NAME(_InterlockedExchangeAdd,         \
            lg_size)(&a->repr, (CX_ATOMIC_IL_REPR(lg_size))val);        \
}                                                                       \
                                                                        \
_meta_inline type                                                       \
_atomicFetchSub_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtmoicMemoryOrder mo) {                                   \
        /*                                                              \
         * MSVC warns on negation of unsigned operands, but for us it   \
         * gives exactly the right semantics (MAX_TYPE + 1 - operand).  \
         */                                                             \
        __pragma(warning(push))                                         \
        __pragma(warning(disable: 4146))                                \
        return _atomicFetchAdd_##short_type(a, -val, mo);               \
        __pragma(warning(pop))                                          \
}                                                                       \
_meta_inline type                                                       \
_atomicFetchAnd_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtmoicMemoryOrder mo) {                                   \
        return (type)CX_ATOMIC_IL_NAME(_InterlockedAnd, lg_size)(       \
            &a->repr, (CX_ATOMIC_IL_REPR(lg_size))val);                 \
}                                                                       \
_meta_inline type                                                       \
_atomicFetchOr_##short_type(cx_atomic_##short_type *a,                  \
    type val, AtmoicMemoryOrder mo) {                                   \
        return (type)CX_ATOMIC_IL_NAME(_InterlockedOr, lg_size)(        \
            &a->repr, (CX_ATOMIC_IL_REPR(lg_size))val);                 \
}                                                                       \
_meta_inline type                                                       \
_atomicFetchXor_##short_type(cx_atomic_##short_type *a,                 \
    type val, AtmoicMemoryOrder mo) {                                   \
        return (type)CX_ATOMIC_IL_NAME(_InterlockedXor, lg_size)(       \
            &a->repr, (CX_ATOMIC_IL_REPR(lg_size))val);                 \
}

#define CX_EXTERN_INT_ATOMICS(type, short_type)                         \
CX_EXTERN_ATOMICS(type, short_type)                                     \
                                                                        \
extern inline type                                                      \
atomicFetchAdd_##short_type(cx_atomic_##short_type *a,                  \
    type val, AtmoicMemoryOrder mo);                                    \
                                                                        \
extern inline type                                                      \
atomicFetchSub_##short_type(cx_atomic_##short_type *a,                  \
    type val, AtmoicMemoryOrder mo);                                    \
extern inline type                                                      \
atomicFetchAnd_##short_type(cx_atomic_##short_type *a,                  \
    type val, AtmoicMemoryOrder mo);                                    \
extern inline type                                                      \
atomicFetchOr_##short_type(cx_atomic_##short_type *a,                   \
    type val, AtmoicMemoryOrder mo);                                    \
extern inline type                                                      \
atomicFetchXor_##short_type(cx_atomic_##short_type *a,                  \
    type val, AtmoicMemoryOrder mo);
