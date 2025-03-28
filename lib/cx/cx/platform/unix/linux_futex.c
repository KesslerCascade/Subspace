#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <cx/time.h>
#include <cx/thread/futex.h>
#include <cx/platform/cpu.h>
#include <cx/platform/os.h>
#include <cx/utils.h>

#include <linux/futex.h>
#include <sys/syscall.h>
#include <features.h>
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

    struct timespec to;
    if (timeout != timeForever)
        timeToRelTimespec(&to, timeout);

    int sret = syscall(SYS_futex, (uint32_t*)&ftx->val, FUTEX_WAIT_PRIVATE, (uint32_t)oldval,
                (timeout == timeForever) ? NULL : &to, NULL, 0);

    if (sret == 0)
        return FUTEX_Waited;
    if (sret == -1 && errno == EAGAIN)
        return FUTEX_Retry;
    if (sret == -1 && errno == ETIMEDOUT)
        return FUTEX_Timeout;
    return FUTEX_Error;
}

void futexWake(Futex *ftx)
{
    syscall(SYS_futex, (uint32_t*)&ftx->val, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
}

void futexWakeMany(Futex *ftx, int count)
{
    syscall(SYS_futex, (uint32_t *)&ftx->val, FUTEX_WAKE_PRIVATE, count, NULL, NULL, 0);
}

void futexWakeAll(Futex *ftx)
{
    syscall(SYS_futex, (uint32_t*)&ftx->val, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, NULL, 0);
}
