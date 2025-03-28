#pragma once

#include <cx/platform/cpp.h>
#include <cx/utils/macros/salieri.h>
#include <cx/utils/macros/optarg.h>
#include <cx/utils/macros/unused.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef XALLOC_REMAP_MALLOC
 // these need to be pulled in first so our #defines don't cause chaos when they
 // are included
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
// never use this; we have heap profilers these days
#undef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

// If this macro is defined, causes the xalloc system to use the
// system-provided allocator rather than mimalloc. There may be some
// loss of functionality and performance when this option is in use,
// but it may be neeed on systems with very tightly constrainted limits
// on virtual address space allocation.
// #define XALLOC_USE_SYSTEM_MALLOC 1

CX_C_BEGIN

#define XA_LG_ALIGN_MASK ((unsigned int)0x3f)

// Align to a 2^exp byte boundary.
// exp may be 1-63
#define XA_Align(exp)   ((unsigned int)(exp & XA_LG_ALIGN_MASK))
// Zero-fills returned memory
#define XA_Zero         ((unsigned int)0x0040)

enum XA_OPTIONAL_FLAG_ENUM {
    XA_Optional_High =  0x0100,
    XA_Optional_Low  =  0x0200,
    XA_Optional_None =  0x0400
};
#define XA_Optional(typ) ((unsigned int)XA_Optional_##typ)
#define XA_Optional_Mask (XA_Optional_High|XA_Optional_Low|XA_Optional_None)

// Allows the function to fail and return NULL rather than triggering an out-of-memory assertion.
// Recommended but not required if using XA_Align.
// May be invoked as XA_Optional(High|Low|None) to inform the allocator how hard it should try to find
// memory if there is none available. The default if not specified is High.
// High = Try somewhat hard, it would be nice to have this memory
// Low = This memory isn't that important, don't try particularly hard
// None = Don't try at all, simply fail if no memory is available
#define XA_Opt XA_Optional(High)

enum XA_OOM_PHASE_ENUM {
    // In the low-effort phase, the OOM handler should try to free memory that is low-hanging fruit.
    // It MUST NOT acquire any locks, even optimistically.
    // This phase is best suited for freeing memory that is known garbage and is for certain not in
    // use, such as a deferred-free scenario.
    XA_LowEffort = 1,

    // In the high-effort phase, the OOM handler may optimistically acquire locks in a non-blocking
    // manner, but MUST NOT wait on a lock. This phase is suited for cleaning up cache memory or other
    // nonessential resources.
    XA_HighEffort,

    // In the urgent phase, the OOM handler should go to any lengths necessary to free memory, including
    // waiting on locks, but be careful to avoid possible deadlocks. The urgent phase is only invoked as
    // a last-ditch effort before asserting and crashing the program.
    XA_Urgent,

    // The fatal phase occurs only if a non-optional allocation has exhausted all options and is about
    // to trigger an assertion. It can be used for a program to intervene and exit in some other manner,
    // but be aware that it is quite likely there is little or no memory available and any allocations
    // will fail.
    XA_Fatal = -1
};

#ifndef _MSC_VER
// &* is a safe way to ensure the operand is a pointer, even a pointer to void
#define _xa_ptr_ptr_verify(ptr) unused_noeval(&*(*(ptr)))
#else
// &* doesn't work for void* on MSVC, check for void* conversion instead, but this results
// in a false negative for intptr_t* or pointer-sized ints
#define _xa_ptr_ptr_verify(ptr) unused_noeval((void*)(*(ptr)))
#endif

// In an out-of-memory situation, all registered OOM callbacks will be called.
// Depending on the type of allocation, they may be called repeatedly with different phases -- see the
// phase enum above for details on how to handle each phase.
// It is guaranteed that only one thread will be inside an OOM callback at a time -- all other threads
// must wait on the first thread to enter the callback to return.
typedef void(*xaOOMCallback)(int phase, size_t allocsz);

// Install function to call in out-of-memory conditions
void xaAddOOMCallback(xaOOMCallback cb);
void xaRemoveOOMCallback(xaOOMCallback cb);

// void *xaAlloc(size_t size, [flags])
// Allocate memory of at least sz
// Flags include XA_Align(pow), XA_Zero, XA_Optional(tier)
// Returns: pointer to memory
#define xaAlloc(size, ...) _xaAlloc(size, opt_flags(__VA_ARGS__))

// void *xaAllocStruct(typename, [flags])
// Convenience function to allocate a struct-sized block of memory.
// Flags include XA_Align(pow), XA_Zero, XA_Optional(tier)
// Returns: pointer to memory
#define xaAllocStruct(typn, ...) (typn*)_xaAlloc(sizeof(typn), opt_flags(__VA_ARGS__))

_Check_return_
_When_(flags & XA_Optional_Mask, _Must_inspect_result_ _Ret_maybenull_)
_When_(!(flags & XA_Optional_Mask), _Ret_notnull_)
_When_(!(flags & XA_Optional_Mask) && (flags & XA_Zero), _Ret_valid_)
_Post_writable_byte_size_(size)
void *_xaAlloc(size_t size, unsigned int flags);

// bool xaResize(void **ptr, size_t size, [flags])
// Reallocate ptr to be at least sz bytes large, copying it if necessary.
// If ptr points to NULL, allocates new memory.
// Returns: True if successfully resized and ptr updated.
// If XA_Opt is set, returns false on failure and ptr is unchanged.
#define xaResize(ptr, size, ...) (_xa_ptr_ptr_verify(ptr), _xaResize((void**)(ptr), size, opt_flags(__VA_ARGS__)))
_At_(*ptr, _Pre_maybenull_)
_When_(!(flags & XA_Optional_Mask), _At_(*ptr, _Post_writable_byte_size_(size)))
bool _xaResize(_Inout_ void **ptr, size_t size, unsigned int flags);

// Frees the memory at ptr
// Does nothing if ptr is NULL
void xaFree(_Pre_maybenull_ _Post_invalid_ void *ptr);

// bool xaRelease(void **ptr)
// Frees the pointer with release semantics.
// This frees *ptr (if it is non-NULL) and sets it to NULL.
#define xaRelease(ptr) (_xa_ptr_ptr_verify(ptr), _xaRelease((void**)(ptr)))
_At_(*ptr, _Pre_maybenull_ _Post_null_)
bool _xaRelease(_Inout_ void **ptr);

// Returns: size of memory at ptr
// In normal (max performance mode), this will have a lower bound at the number of bytes that
// were required by xaAlloc, but may be more. If more, it is safe to use the extra space
// without reallocating.
// In secure mode, this will always return the exact number of bytes that were requested by
// the original allocation. The reason is that in secure mode, any slack space in the allocation
// is filled with padding and used to check for buffer overruns.
// See also xaOptSize(), which is useful for determining the optimal size to allocate in
// secure mode to allow room for buffer growth.
size_t xaSize(_In_ void *ptr);

// Returns the optimal size to allocate to fit a given number of bytes. This returns the
// underlying size class that fits the given number of bytes.
// This is most useful when the allocator is in secure mode to determine how much bigger
// a buffer can be allocated without moving to the next size class, since in that mode
// the difference between the requested size and actual allocated size is NOT usable by
// the caller due to secure padding.
size_t xaOptSize(size_t sz);

// Flushes any deferred free() operations and returns as much memory to the OS as possible
void xaFlush();

CX_C_END

// String utilities because cstrDup is tied in with xalloc
// and there's no better place for them
#include <cx/xalloc/cstrutil.h>

#ifdef XALLOC_REMAP_MALLOC
#define malloc(sz) xa_malloc(sz)
#define calloc(num, sz) xa_calloc(num, sz)
#define free(ptr) xa_free(ptr)
#define realloc(ptr, sz) xa_realloc(ptr, sz)
#define strdup(s) cstrDup(s)
#define _strdup(s) cstrDup(s)
#define wcsdup(s) cstrDupw(s)
#define _wcsdup(s) cstrDupw(s)

#ifdef _WIN32
#undef _malloc_dbg
#define _malloc_dbg(sz, t, f, ln) xa_malloc(sz)
#undef _calloc_dbg
#define _calloc_dbg(num, sz, t, f, ln) xa_calloc(num, sz)
#undef _free_dbg
#define _free_dbg(ptr, t) xa_free(ptr)
#undef _realloc_dbg
#define _realloc_dbg(ptr, sz, t, f, ln) xa_realloc(ptr, sz)
#undef _strdup_dbg
#define _strdup_dbg(s, t, f, ln) cstrDup(s)
#undef _wcsdup_dbg
#define _wcsdup_dbg(s, t, f, ln) cstrDupw(s)
#endif
#endif

// Normally the compatibility interface uses optional allocations to emulate
// the behavior of malloc() returning NULL when out of memory. Some applications
// may not wish to do that and would rather assert and/or install an XA_Fatal
// handler.

// These applications can define XALLOC_COMPAT_REQUIRE before including xalloc.h
// to force the interface to use required allocations instead.

#ifdef XALLOC_COMPAT_REQUIRE
#define XA_COMPAT_OPT_FLAG 0
#else
#define XA_COMPAT_OPT_FLAG XA_Opt
#endif

// for compatibility with 3rd party libraries only
inline void *xa_malloc(size_t size)
{
    return _xaAlloc(size, XA_COMPAT_OPT_FLAG);
}

inline void *xa_calloc(size_t number, size_t size)
{
    return _xaAlloc(number * size, XA_Zero | XA_COMPAT_OPT_FLAG);
}

inline void *xa_realloc(void *ptr, size_t size)
{
    return _xaResize(&ptr, size, XA_COMPAT_OPT_FLAG) ? ptr : (void *)0;
}

inline void xa_free(void *ptr)
{
    xaFree(ptr);
}

inline char *xa_strdup(const char *src)
{
    return cstrDup(src);
}
