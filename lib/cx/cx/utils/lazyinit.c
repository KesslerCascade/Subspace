#include "lazyinit.h"
#include "cx/thread/atomic.h"
#include "cx/platform/cpu.h"

// extern inline void lazyInit(LazyInitState *state, LazyInitCallback initfunc, void *userData);

_Use_decl_annotations_
void _lazyInitInternal(bool *init, bool *initProgress, LazyInitCallback initfunc, void *userData)
{
    bool concurrent = atomicExchange(bool, (atomic(bool)*)initProgress, true, AcqRel);
    if (!concurrent) {
        // we are the first thread to try to initialize
        initfunc(userData);
        atomicStore(bool, (atomic(bool)*)init, true, Relaxed);
    } else {
        // another thread is performing the initialization
        // spin until it finishes
        while (!atomicLoad(bool, (atomic(bool)*)init, Relaxed))
            _CPU_PAUSE;
    }
}
