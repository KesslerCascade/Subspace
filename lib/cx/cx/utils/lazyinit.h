#pragma once

#include <cx/cx.h>

CX_C_BEGIN

typedef struct LazyInitState {
    bool init;
    bool initProgress;
} LazyInitState;

typedef void(*LazyInitCallback)(void *userData);

void _lazyInitInternal(_Inout_ bool *init, _Inout_ bool *initProgress, _In_ LazyInitCallback initfunc, _In_opt_ void *userData);

// Calls the function initfunc() exactly once in a thread-safe manner.
// All simultaneous threads are guarnteed to not pass this point until
// after initfunc() has been completely executed by one of them.
// This operation becomes very cheap once initialization is complete
// and all caches have synchronized.
_meta_inline void lazyInit(_Inout_ LazyInitState *state, _In_ LazyInitCallback initfunc, _In_opt_ void *userData)
{
    if (!state->init)
        _lazyInitInternal(&state->init, &state->initProgress, initfunc, userData);
}

CX_C_END
