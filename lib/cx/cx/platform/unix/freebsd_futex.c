#include <cx/time.h>
#include <cx/thread/futex.h>
#include <cx/platform/cpu.h>
#include <cx/platform/os.h>
#include <cx/utils.h>

#include <sys/types.h>
#include <sys/umtx.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

void futexInit(Futex *ftx, int32 val) {
    atomicStore(int32, &ftx->val, val, Relaxed);
    atomicStore(uint16, &ftx->_ps, 0, Relaxed);
    atomicStore(uint8, &ftx->_ps_lock, 0, Relaxed);
}

int futexWait(Futex *ftx, int32 oldval, int64 timeout) {
    // early out if the value already doesn't match
    if (atomicLoad(int32, &ftx->val, Relaxed) != oldval)
        return FUTEX_Retry;

    struct _umtx_time to;
    void *uaddr = NULL, *uaddr2 = NULL;
    if (timeout != timeForever) {
        to._flags = 0;
        to._clockid = CLOCK_MONOTONIC;
        timeToRelTimespec(&to._timeout, timeout);
        uaddr = (void*)sizeof(to);
        uaddr2 = &to;
    }

    int sret = _umtx_op(&ftx->val, UMTX_OP_WAIT_UINT_PRIVATE, (uint32_t)oldval, uaddr, uaddr2);

    // FreeBSD doesn't actually tell us if the atomic doesn't match when the kernel
    // checks it, so we have no choice but to return FUTEX_Waited in that case.

    if (sret == 0)
        return FUTEX_Waited;
    if (sret == -1 && errno == ETIMEDOUT)
        return FUTEX_Timeout;
    return FUTEX_Error;
}

void futexWake(Futex *ftx)
{
    _umtx_op(&ftx->val, UMTX_OP_WAKE_PRIVATE, 1, NULL, NULL);
}

void futexWakeMany(Futex *ftx, int count)
{
    _umtx_op(&ftx->val, UMTX_OP_WAKE_PRIVATE, count, NULL, NULL);
}

void futexWakeAll(Futex *ftx)
{
    _umtx_op(&ftx->val, UMTX_OP_WAKE_PRIVATE, INT_MAX, NULL, NULL);
}
