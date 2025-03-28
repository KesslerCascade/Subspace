#pragma once

#include <cx/cx.h>
#include <cx/utils/compare.h>

// Scratch buffers are a set of rotating buffers that are intended for short-term
// temporary storage, to be allocated and thrown away. Each thread has its own
// set of scratch buffers.

// Rules for using scratch buffers:

// 1. Do NOT save a pointer to a scratch buffer in any kind of persistent structure.
// 2. Do NOT share pointers to scratch buffers across thread boundaries.
// 3. Do NOT use scratch buffers in a function that can be called recursively.
// 4. Be aware of what functions do -- any function you call that uses scratch buffers
//    could invalidate buffers the caller was using. To be safe, don't assume they
//    survive such a call.
// 5. Keep the size of scratch buffers reasonable.
// 6. Don't expect scratch buffer allocation to be super fast. It's fine for non-critical
//    paths but don't use it excessively.

// Number of buffers per thread:
// Must be a power of 2. Try to balance per-thread overhead with stack depth.
#define SCRATCH_NBUFFERS 32
#define SCRATCH_MIN_BUFFER_SIZE 64
#define SCRATCH_MAX_REASONABLE_BUFFER_SIZE 1024

CX_C_BEGIN

typedef struct ScratchPerThreadInfo {
    int32 next;
    void *buf[SCRATCH_NBUFFERS];
} ScratchPerThreadInfo;

extern _Thread_local ScratchPerThreadInfo *_scratch_pti;

// scratchGet does not clear memory, do not assume it's zero-filled!
_Ret_notnull_
inline void *scratchGet(size_t sz)
{
    if (!_scratch_pti) {
        _scratch_pti = (ScratchPerThreadInfo*)xaAlloc(sizeof(ScratchPerThreadInfo), XA_Zero);
    }

    int32 cur = _scratch_pti->next;
    _scratch_pti->next = (_scratch_pti->next + 1) & (SCRATCH_NBUFFERS - 1);

    sz = clamplow(sz, SCRATCH_MIN_BUFFER_SIZE);
    if (!_scratch_pti->buf[cur]) {
        _scratch_pti->buf[cur] = xaAlloc(sz);
    } else {
        size_t cursz = xaSize(_scratch_pti->buf[cur]);

        if (cursz < sz ||
            (cursz > SCRATCH_MAX_REASONABLE_BUFFER_SIZE
             && sz < SCRATCH_MAX_REASONABLE_BUFFER_SIZE)) {
            // xaResize would copy data we don't care about
            xaFree(_scratch_pti->buf[cur]);
            _scratch_pti->buf[cur] = xaAlloc(sz);
        }
    }

    return _scratch_pti->buf[cur];
}

CX_C_END
