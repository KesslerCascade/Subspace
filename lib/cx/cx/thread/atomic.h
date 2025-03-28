#pragma once

// Platform-specific atomics implementation (lifted from jemalloc 5.x)
// This implements C11-style atomics.
#include <cx/cx.h>
#include <cx/platform/base.h>

#if defined(_COMPILER_MSVC)
#include <cx/platform/msvc/msvc_atomic.h>
#elif defined(_COMPILER_CLANG) || defined(_COMPILER_GCC)
#include <cx/platform/clang/clang_atomic.h>
#endif

/*
 * This header gives more or less a backport of C11 atomics. The user can write
 * CX_GENERATE_ATOMICS(type, short_type, lg_sizeof_type); to generate
 * counterparts of the C11 atomic functions for type, as so:
 *   CX_GENERATE_ATOMICS(int *, pi, 3);
 * and then write things like:
 *   int *some_ptr;
 *   atomic(pi) atomic_ptr_to_int;
 *   atomicStore(pi, &atomic_ptr_to_int, some_ptr, Relaxed);
 *   int *prev_value = atomicExchange(pi, &ptr_to_int, NULL, AcqRel);
 *   assert(some_ptr == prev_value);
 * and expect things to work in the obvious way.
 *
 * Also included (with naming differences to avoid conflicts with the standard
 * library):
 *   atomicFence(MemoryOrder) (mimics C11's atomic_thread_fence).
 *   atomicInit (mimics C11's ATOMIC_VAR_INIT).
 */

#ifdef _64BIT
CX_GENERATE_ATOMICS(void *, ptr, 3)
CX_GENERATE_ATOMICS(bool, bool, 0)
CX_GENERATE_INT_ATOMICS(size_t, size, 3)
CX_GENERATE_INT_ATOMICS(intptr, intptr, 3)
CX_GENERATE_INT_ATOMICS(int8, int8, 0)
CX_GENERATE_INT_ATOMICS(int16, int16, 1)
CX_GENERATE_INT_ATOMICS(int32, int32, 2)
CX_GENERATE_INT_ATOMICS(int64, int64, 3)
CX_GENERATE_INT_ATOMICS(uintptr, uintptr, 3)
CX_GENERATE_INT_ATOMICS(uint8, uint8, 0)
CX_GENERATE_INT_ATOMICS(uint16, uint16, 1)
CX_GENERATE_INT_ATOMICS(uint32, uint32, 2)
CX_GENERATE_INT_ATOMICS(uint64, uint64, 3)
#else
CX_GENERATE_ATOMICS(void *, ptr, 2)
CX_GENERATE_ATOMICS(bool, bool, 0)
CX_GENERATE_INT_ATOMICS(size_t, size, 2)
CX_GENERATE_INT_ATOMICS(intptr, intptr, 2)
CX_GENERATE_INT_ATOMICS(int8, int8, 0)
CX_GENERATE_INT_ATOMICS(int16, int16, 1)
CX_GENERATE_INT_ATOMICS(int32, int32, 2)
CX_GENERATE_INT_ATOMICS(uintptr, uintptr, 2)
CX_GENERATE_INT_ATOMICS(uint8, uint8, 0)
CX_GENERATE_INT_ATOMICS(uint16, uint16, 1)
CX_GENERATE_INT_ATOMICS(uint32, uint32, 2)
#endif
