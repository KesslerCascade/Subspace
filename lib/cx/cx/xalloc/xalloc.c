#include <cx/cx.h>
#include "xalloc_private.h"

#include <cx/debug/assert.h>
#include <cx/utils/macros.h>
#include <string.h>
#include <errno.h>

LazyInitState _xaInitState;

#ifndef XALLOC_USE_SYSTEM_MALLOC
static void xaMimallocError(int err, void *arg)
{
    if (err == EFAULT) {
        relFatalError("Heap corruption detected");
    } else if (err == EAGAIN) {
        devFatalError("Double free detected");
    } else if (err == EINVAL) {
        devFatalError("Tried to free an invalid pointer");
    }
}
#endif

static void _xaInit(void *data)
{
#ifndef XALLOC_USE_SYSTEM_MALLOC
    mi_register_error(xaMimallocError, NULL);

#ifndef CX_BUILDING_CXOBJGEN
    _xaInitOutput();
#endif
#endif // XALLOC_USE_SYSTEM_MALLOC
}

#ifndef XALLOC_USE_SYSTEM_MALLOC

// default case -- use embedded mimalloc
#define _xa_sys_zalloc_aligned mi_zalloc_aligned
#define _xa_sys_malloc_aligned mi_malloc_aligned
#define _xa_sys_zalloc mi_zalloc
#define _xa_sys_malloc mi_malloc

#define _xa_sys_rezalloc_aligned mi_rezalloc_aligned
#define _xa_sys_realloc_aligned mi_realloc_aligned
#define _xa_sys_rezalloc mi_rezalloc
#define _xa_sys_realloc mi_realloc

#define _xa_sys_free mi_free

#define _xa_sys_usable_size mi_usable_size
#define _xa_sys_good_size mi_good_size
#define _xa_sys_collect mi_collect

#else

// use system malloc instead
#include <stdlib.h>
#if defined(_PLATFORM_LINUX)
#include <malloc.h>
#elif defined(_PLATFORM_FBSD)
#include <malloc_np.h>
#endif

inline static void *_xa_sys_malloc_aligned(size_t size, size_t alignment)
{
#ifdef _COMPILER_MSVC
    // MSVCRT doesn't support aligned allocation without using a speical free(),
    // which we can't do!
    // So return unaligned memory instead. TODO: Maybe better to fail here?
    return malloc(size);
#else
    return aligned_alloc(size, alignment);
#endif
}

inline static void *_xa_sys_zalloc_aligned(size_t size, size_t alignment)
{
    void *blk = _xa_sys_malloc_aligned(size, alignment);
    if(!blk)
        return NULL;

    memset(blk, 0, size);
    return blk;
}

inline static void *_xa_sys_zalloc(size_t size)
{
    return calloc(1, size);
}

#define _xa_sys_malloc malloc

#define _xa_sys_realloc realloc

#define _xa_sys_free free

#ifdef _COMPILER_MSVC
#define _xa_sys_usable_size _msize
#else
#define _xa_sys_usable_size malloc_usable_size
#endif

// standard malloc doesn't support any of these, have to always copy the data

inline static void *_xa_sys_rezalloc_aligned(void *origblk, size_t size, size_t alignment)
{
    void *nblk = _xa_sys_zalloc_aligned(size, alignment);
    if(!nblk)
        return NULL;

    memcpy(nblk, origblk, _xa_sys_usable_size(origblk));
    free(origblk);
    return nblk;
}

inline static void *_xa_sys_realloc_aligned(void *origblk, size_t size, size_t alignment)
{
    void *nblk = _xa_sys_malloc_aligned(size, alignment);
    if(!nblk)
        return NULL;

    memcpy(nblk, origblk, _xa_sys_usable_size(origblk));
    free(origblk);
    return nblk;
}

inline static void *_xa_sys_rezalloc(void *origblk, size_t size)
{
    void *nblk = calloc(1, size);
    if(!nblk)
        return NULL;

    memcpy(nblk, origblk, _xa_sys_usable_size(origblk));
    free(origblk);
    return nblk;
}

inline static size_t _xa_sys_good_size(size_t size) { return size; }

// no standard way of doing this
inline static void _xa_sys_collect(bool unused) {}

#endif

_Use_decl_annotations_
void *_xaAlloc(size_t size, unsigned int flags)
{
    void *ret = NULL;

    lazyInit(&_xaInitState, _xaInit, NULL);

    for (int oomphase = 0, oommaxphase = _xaMaxOOMPhase(flags); oomphase <= oommaxphase; oomphase++) {
        if (oomphase > 0)
            _xaFreeUpMemory(oomphase, size);    // previous loop's allocation failed; try to free up some memory and try again

        if (flags & XA_LG_ALIGN_MASK) {
            if (flags & XA_Zero) {
                ret = _xa_sys_zalloc_aligned(size, (size_t)1 << (flags & XA_LG_ALIGN_MASK));
            } else {
                ret = _xa_sys_malloc_aligned(size, (size_t)1 << (flags & XA_LG_ALIGN_MASK));
            }
        } else {
            if (flags & XA_Zero)
                ret = _xa_sys_zalloc(size);
            else
                ret = _xa_sys_malloc(size);
        }

        if (ret)
            return ret;
    }

    // if this isn't an optional allocation, assert rather than return NULL
    _xaAllocFailure(size, flags);

    return NULL;
}

// Reallocate ptr to be at least sz byte large, copying it if necessary.
// NOTE: Unlike realloc, ptr cannot be NULL!
// Returns: pointer to memory
_Use_decl_annotations_
bool _xaResize(void **ptr, size_t size, unsigned int flags)
{
    void *ret = NULL;

    if (!ptr)
        return false;

    lazyInit(&_xaInitState, _xaInit, NULL);

    for (int oomphase = 0, oommaxphase = _xaMaxOOMPhase(flags); oomphase <= oommaxphase; oomphase++) {
        if (oomphase > 0)
            _xaFreeUpMemory(oomphase, size);    // previous loop's allocation failed; try to free up some memory and try again

        if (flags & XA_LG_ALIGN_MASK) {
            if (flags & XA_Zero) {
                ret = _xa_sys_rezalloc_aligned(*ptr, size, (size_t)1 << (flags & XA_LG_ALIGN_MASK));
            } else {
                ret = _xa_sys_realloc_aligned(*ptr, size, (size_t)1 << (flags & XA_LG_ALIGN_MASK));
            }
        } else {
            if (flags & XA_Zero) {
                ret = _xa_sys_rezalloc(*ptr, size);
            } else {
                ret = _xa_sys_realloc(*ptr, size);
            }
        }

        if (ret) {
            *ptr = ret;
            return true;
        }
    }

    // if this isn't an optional allocation, assert rather than return false
    _xaAllocFailure(size, flags);

    return false;
}

// Frees the memory at ptr
_Use_decl_annotations_
void xaFree(void *ptr)
{
    lazyInit(&_xaInitState, _xaInit, NULL);
    _xa_sys_free(ptr);
}

_Use_decl_annotations_
bool _xaRelease(void **ptr)
{
    if (!ptr) return false;

    void *origptr = *ptr;
    *ptr = NULL;

    xaFree(origptr);            // origptr may be NULL but will be ignored by xaFree
    return origptr != NULL;
}

_Use_decl_annotations_
size_t xaSize(void *ptr)
{
    lazyInit(&_xaInitState, _xaInit, NULL);
    return _xa_sys_usable_size(ptr);
}

size_t xaOptSize(size_t sz)
{
    return _xa_sys_good_size(sz);
}

void xaFlush()
{
    lazyInit(&_xaInitState, _xaInit, NULL);
    _xa_sys_collect(true);
}

// instantiate these so they can be used as function pointers
extern inline void *xa_malloc(size_t size);
extern inline void *xa_calloc(size_t number, size_t size);
extern inline void *xa_realloc(void *ptr, size_t size);
extern inline void xa_free(void *ptr);
extern inline char *xa_strdup(const char *src);
