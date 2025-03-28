#pragma once

#include <cx/cx.h>
#include <cx/thread/atomic.h>

enum FUTEX_Status {
    FUTEX_Error   = 0,          // unexpected error
    FUTEX_Waited  = 0x1,        // successfully waited for wakeup
    FUTEX_Retry   = 0x2,        // old value was not what was expected, try again
    FUTEX_Timeout = 0x4,        // timeout reached
};
#define FUTEX_LOOP 0x3          // convenient mask for certain futex loops (Waited or Retry)

typedef struct Futex {
    atomic(int32) val;
    atomic(uint16) _ps;                 // platform-specific value
    atomic(uint8) _ps_lock;             // _ps spinlock
} Futex;

_Static_assert(sizeof(Futex) <= sizeof(int64), "Invalid Futex structure packing");

void futexInit(_Out_ Futex *ftx, int32 val);

_meta_inline int32 futexVal(_Inout_ Futex *ftx) {
    return atomicLoad(int32, &ftx->val, Relaxed);
}

// sets the value but does NOT wake up any waiting threads; use with caution
// and pair with either futexWake or futexWakeAll
_meta_inline void futexSet(_Inout_ Futex *ftx, int32 val) {
    atomicStore(int32, &ftx->val, val, Relaxed);
}

// If futex value is equal to oldval, puts the thread to sleep until futexWake
// is called on the same futex, or until the timeout expires.
// Returns true only if the thread actually slept.
int futexWait(_Inout_ Futex *ftx, int32 oldval, int64 timeout);

void futexWake(_Inout_ Futex *ftx);
void futexWakeMany(_Inout_ Futex *ftx, int count);
void futexWakeAll(_Inout_ Futex *ftx);
