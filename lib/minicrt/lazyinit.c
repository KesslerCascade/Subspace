#include "atomic.h"
#include "minicrt.h"

#ifdef _MSC_VER
#define _CPU_PAUSE _mm_pause()
#else
#define _CPU_PAUSE __asm__ volatile("pause")
#endif

void lazyinit_internal(unsigned char* init, unsigned char* initProgress,
                       lazy_init_callback_t initfunc, void* user)
{
    uint8_t concurrent = atomicExchange(uint8_t, (atomic(uint8_t)*)initProgress, 1, AcqRel);
    if (!concurrent) {
        // we are the first thread to try to initialize
        initfunc(user);
        atomicStore(uint8_t, (atomic(uint8_t)*)init, 1, Relaxed);
    } else {
        // another thread is performing the initialization
        // spin until it finishes
        while (!atomicLoad(uint8_t, (atomic(uint8_t)*)init, Relaxed)) {
            _CPU_PAUSE;
        }
    }
}
