#pragma once

#include "xalloc.h"
#include <cx/platform/base.h>
#include <cx/utils/lazyinit.h>
#include <mimalloc.h>

extern LazyInitState _xaInitState;

static _meta_inline int _xaMaxOOMPhase(unsigned int flags)
{
    if (!(flags & XA_Optional_Mask))
        return XA_Urgent;

    if (flags & XA_Optional_High)
        return XA_HighEffort;
    if (flags & XA_Optional_Low)
        return XA_LowEffort;

    // XA_Optional_None doesn't do any OOM freeing at all
    return 0;
}

// go through OOM callbacks to try to free up some memory
void _xaFreeUpMemory(int phase, size_t allocsz);

// If flags do not include one of the optional flags, this function
// asserts and does not return.
_When_(!(flags & XA_Optional_Mask), _Analysis_noreturn_)
void _xaAllocFailure(size_t allocsz, unsigned int flags);

#ifndef XALLOC_USE_SYSTEM_MALLOC
void _xaInitOutput(void);
#endif
