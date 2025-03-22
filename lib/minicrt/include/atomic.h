#pragma once

// Platform-specific atomics implementation (lifted from jemalloc 5.x)
// Modified copy of CX atomics for minicrt.
// This implements C11-style atomics.

#include <stdint.h>
#include <stdbool.h>

#if defined(_MSC_VER)
#include "msvc/msvc_atomic.h"
#else
#include "clang/clang_atomic.h"
#endif

/*
 * This header gives more or less a backport of C11 atomics. The user can write
 * MINICRT_GENERATE_ATOMICS(type, short_type, lg_sizeof_type); to generate
 * counterparts of the C11 atomic functions for type, as so:
 *   MINICRT_GENERATE_ATOMICS(int *, pi, 3);
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
MINICRT_GENERATE_ATOMICS(void *, ptr, 3)
MINICRT_GENERATE_ATOMICS(bool, bool, 0)
MINICRT_GENERATE_INT_ATOMICS(size_t, size_t, 3)
MINICRT_GENERATE_INT_ATOMICS(intptr_t, intptr_t, 3)
MINICRT_GENERATE_INT_ATOMICS(int8_t, int8_t, 0)
MINICRT_GENERATE_INT_ATOMICS(int16_t, int16_t, 1)
MINICRT_GENERATE_INT_ATOMICS(int32_t, int32_t, 2)
MINICRT_GENERATE_INT_ATOMICS(int64_t, int64_t, 3)
MINICRT_GENERATE_INT_ATOMICS(uintptr_t, uintptr_t, 3)
MINICRT_GENERATE_INT_ATOMICS(uint8_t, uint8_t, 0)
MINICRT_GENERATE_INT_ATOMICS(uint16_t, uint16_t, 1)
MINICRT_GENERATE_INT_ATOMICS(uint32_t, uint32_t, 2)
MINICRT_GENERATE_INT_ATOMICS(uint64_t, uint64_t, 3)
#else
MINICRT_GENERATE_ATOMICS(void *, ptr, 2)
MINICRT_GENERATE_ATOMICS(bool, bool, 0)
MINICRT_GENERATE_INT_ATOMICS(size_t, size_t, 2)
MINICRT_GENERATE_INT_ATOMICS(intptr_t, intptr_t, 2)
MINICRT_GENERATE_INT_ATOMICS(int8_t, int8_t, 0)
MINICRT_GENERATE_INT_ATOMICS(int16_t, int16_t, 1)
MINICRT_GENERATE_INT_ATOMICS(int32_t, int32_t, 2)
MINICRT_GENERATE_INT_ATOMICS(uintptr_t, uintptr_t, 2)
MINICRT_GENERATE_INT_ATOMICS(uint8_t, uint8_t, 0)
MINICRT_GENERATE_INT_ATOMICS(uint16_t, uint16_t, 1)
MINICRT_GENERATE_INT_ATOMICS(uint32_t, uint32_t, 2)
#endif