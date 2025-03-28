#pragma once

#if defined(_MSC_VER)

#include <intrin.h>
#include <assert.h>

#define _COMPILER_MSVC 1

// C11 thread local
#define _Thread_local __declspec(thread)

// C11 static assert
#define _Static_assert static_assert

// Nullability qualifiers
// SAL is much more powerful than these
#define _Nonnull
#define _Nullable
#define _Null_unspecified

// Pure functions
#define _Pure

#define _Analysis_noreturn _Analysis_noreturn_

// We use inline functions for metaprogramming and really, REALLY want them
// to be inlined
#define _meta_inline __forceinline
#define _no_inline __declspec(noinline)
#pragma warning (disable:6255)
#define stackAlloc(sz) _alloca(sz)

// Sometimes things need to be aligned precisely
#define alignMem(bytes) __declspec(align(bytes))

// Hint that some functions never return
#define _no_return __declspec(noreturn)

#if defined(_WIN32)
#define _PLATFORM_WIN 1
#define _PLATFORM_STR "win"
#else
// this should be impossible
#error Unsupported operating system ???
#endif

#if defined(_M_IX86)
#define _ARCH_X86 1
#define _ARCH_STR "x86"
#define _32BIT 1
#elif defined (_M_X64)
#define _ARCH_X64 1
#define _ARCH_STR "x64"
#define _64BIT 1
#else
#error Unsupported architecture
#endif

#elif defined (__clang__) || (defined(__GNUC__) && __GNUC__ > 4)

#if defined(__clang__)
#define _COMPILER_CLANG 1

// Nullability qualifiers
#pragma clang diagnostic ignored "-Wnullability-completeness"

#elif defined(__GNUC__)
#define _COMPILER_GCC 1

// Nullability qualifiers
#define _Nonnull
#define _Nullable
#define _Null_unspecified

#endif

// Pure functions
#define _Pure __attribute__((pure))

#if defined(__clang_analyzer__)
#define _Analysis_noreturn __attribute__((noreturn))
#else
#define _Analysis_noreturn
#endif

// C11 thread local is already supported

// We use inline functions for metaprogramming and really, REALLY want them
// to be inlined
#define _meta_inline __attribute__((always_inline)) inline
#define _no_inline __attribute__((noinline))
#define stackAlloc(sz) alloca(sz)

// Sometimes things need to be aligned precisely
#define alignMem(bytes) __attribute__((aligned(bytes)))

// Hint that some functions never return
#define _no_return __attribute__((noreturn))

#if defined(__linux__)
#include <alloca.h>
#define _PLATFORM_UNIX 1
#define _PLATFORM_LINUX 1
#define _PLATFORM_STR "linux"
#elif defined(__FreeBSD__)
#define _PLATFORM_UNIX 1
#define _PLATFORM_FBSD 1
#define _PLATFORM_STR "fbsd"
#elif defined(__EMSCRIPTEN__)
#define _PLATFORM_WASM 1
#define _PLATFORM_STR "wasm"
#else
#error Unsupported operating system
#endif

#if defined(__x86_32__)
#define _ARCH_X86 1
#define _ARCH_STR "x86"
#define _32BIT 1
#elif defined(__x86_64__)
#define _ARCH_X64 1
#define _ARCH_STR "x64"
#define _64BIT 1
#elif defined (__aarch64__)
#define _ARCH_ARM64 1
#define _ARCH_STR "arm64"
#define _64BIT 1
#elif defined(__EMSCRIPTEN__)
#define _ARCH_WASM 1
#define _ARCH_STR "wasm32"
#define _32BIT 1
#else
#error Unsupported architecture
#endif

#else
#error Unsupported compiler
#endif

#if defined(_32BIT)
#define _word_align alignMem(4)
#elif defined(_64BIT)
#define _word_align alignMem(8)
#else
#error Unsupported architecture
#endif

#define _PLATFORM_ARCH_STR (_PLATFORM_STR "-" _ARCH_STR)
