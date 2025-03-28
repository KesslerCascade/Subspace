#include <cx/time.h>
#include <cx/thread/futex.h>
#include <cx/platform/cpu.h>
#include <cx/platform/os.h>
#include <cx/utils.h>

#include <emscripten/threading.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

void futexInit(Futex *ftx, int32 val) {
    atomicStore(int32, &ftx->val, val, Relaxed);
    atomicStore(uint16, &ftx->_ps, 0, Relaxed);
    atomicStore(uint8, &ftx->_ps_lock, 0, Relaxed);
}

int futexWait(Futex *ftx, int32 oldval, int64 timeout) {
    // early out if the value already doesn't match
    if (atomicLoad(int32, &ftx->val, Relaxed) != oldval)
        return FUTEX_Retry;

    int sret = emscripten_futex_wait(&ftx->val, oldval,
            (timeout == timeForever) ? INFINITY : (double)timeout / 1000.);

    if (sret == 0)
        return FUTEX_Waited;
    if (sret == -EWOULDBLOCK)
        return FUTEX_Retry;
    if (sret == -ETIMEDOUT)
        return FUTEX_Timeout;
    return FUTEX_Error;
}

void futexWake(Futex *ftx)
{
    emscripten_futex_wake(&ftx->val, 1);
}

void futexWakeMany(Futex *ftx, int count)
{
    emscripten_futex_wake(&ftx->val, count);
}

void futexWakeAll(Futex *ftx)
{
    emscripten_futex_wake(&ftx->val, INT_MAX);
}
