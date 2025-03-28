#include <cx/time.h>
#include <cx/thread/futex.h>
#include <cx/platform/cpu.h>
#include <cx/platform/win.h>
#include <cx/platform/os.h>
#include <cx/utils.h>

typedef BOOL (WINAPI *WaitOnAddress_t)(volatile VOID *Address, PVOID CompareAddress, SIZE_T AddressSize, DWORD dwMilliseconds);
typedef void (WINAPI *WakeByAddressSingle_t)(PVOID Address);
typedef void (WINAPI *WakeByAddressAll_t)(PVOID Address);

typedef long (NTAPI *NtCreateKeyedEvent_t)(PHANDLE KeyedEventHandle, ACCESS_MASK DesiredAccess, void *ObjectAttributes, ULONG Reserved);
typedef long (NTAPI *NtWaitForKeyedEvent_t)(HANDLE KeyedEventHandle, PVOID Key, BOOLEAN Alertable, PLARGE_INTEGER Timeout);
typedef long (NTAPI *NtReleaseKeyedEvent_t)(HANDLE KeyedEventHandle, PVOID Key, BOOLEAN Alertable, PLARGE_INTEGER Timeout);

// NTSTATUS return codes
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS 0
#endif
#ifndef STATUS_TIMEOUT
#define STATUS_TIMEOUT 0x00000102
#endif

static WaitOnAddress_t pWaitOnAddress;
static WakeByAddressSingle_t pWakeByAddressSingle;
static WakeByAddressAll_t pWakeByAddressAll;

static NtCreateKeyedEvent_t pNtCreateKeyedEvent;
static NtWaitForKeyedEvent_t pNtWaitForKeyedEvent;
static NtReleaseKeyedEvent_t pNtReleaseKeyedEvent;
static HANDLE ftxke;

static LazyInitState _futexInitState;
static void _futexInit(void *unused)
{
    // try to load native futex-like API
    HANDLE hDll = LoadLibrary(TEXT("API-MS-Win-Core-Synch-l1-2-0.dll"));
    // don't call FreeLibrary since it _MIGHT_ not be referenced by anything else yet

    if (hDll) {
        pWaitOnAddress = (WaitOnAddress_t)GetProcAddress(hDll, "WaitOnAddress");
        pWakeByAddressSingle = (WakeByAddressSingle_t)GetProcAddress(hDll, "WakeByAddressSingle");
        pWakeByAddressAll = (WakeByAddressAll_t)GetProcAddress(hDll, "WakeByAddressAll");

        // shouldn't happen, but ensure we have all the functions
        if (!pWakeByAddressSingle || !pWakeByAddressAll)
            pWaitOnAddress = NULL;
    }

    hDll = LoadLibrary(TEXT("ntdll.dll"));
    if (hDll) {
        pNtCreateKeyedEvent = (NtCreateKeyedEvent_t)GetProcAddress(hDll, "NtCreateKeyedEvent");
        pNtWaitForKeyedEvent = (NtWaitForKeyedEvent_t)GetProcAddress(hDll, "NtWaitForKeyedEvent");
        pNtReleaseKeyedEvent = (NtReleaseKeyedEvent_t)GetProcAddress(hDll, "NtReleaseKeyedEvent");

        if (!pNtWaitForKeyedEvent || !pNtReleaseKeyedEvent)
            pNtCreateKeyedEvent = NULL;
    }

    // initialized keyed event
    if (pNtCreateKeyedEvent) {
        if (pNtCreateKeyedEvent(&ftxke, GENERIC_READ | GENERIC_WRITE, NULL, 0) != 0)
            ftxke = NULL;
    }

    // make sure we have at least one usable synchronization primitive
    if (!(pWaitOnAddress || ftxke)) {
        MessageBox(NULL, TEXT("Incompatible operating system"), TEXT("Error"), MB_ICONERROR | MB_OK);
        exit(1);
    }
}

_Use_decl_annotations_
void futexInit(Futex *ftx, int32 val) {
    lazyInit(&_futexInitState, _futexInit, NULL);
    atomicStore(int32, &ftx->val, val, Relaxed);
    atomicStore(uint16, &ftx->_ps, 0, Relaxed);
    atomicStore(uint8, &ftx->_ps_lock, 0, Relaxed);
}

_Use_decl_annotations_
int futexWait(Futex *ftx, int32 oldval, int64 timeout) {
    // early out if the value already doesn't match
    if (atomicLoad(int32, &ftx->val, Relaxed) != oldval)
        return FUTEX_Retry;

    if (pWaitOnAddress) {
        // Use WaitOnAddress if it's available
        if (pWaitOnAddress(&ftx->val, &oldval, sizeof(int32), (timeout == timeForever) ? INFINITE : (DWORD)timeToMsec(timeout)))
            return FUTEX_Waited;
        else if (GetLastError() == ERROR_TIMEOUT)
            return FUTEX_Timeout;
        else
            return FUTEX_Retry;
    } else {
        // Fall back to keyed events
        LARGE_INTEGER ketimeout;

        // keyed events use 100s of nanoseconds as timeout.
        ketimeout.QuadPart = -10 * timeout;

        // NT Keyed Events don't work _quite_ like we want them to, which makes implementing
        // futexes in userspace difficult.

        // There's a nasty race condition that is hard to avoid without kernel support:
        // THREAD A                     THREAD B
        // Check ftx->val == oldval
        //                              Modify ftx->val
        //                              Try to wake up a thread (fails because nobody is waiting)
        // Wait for keyed event

        // To avert this, we use a spinlock to ensure that NtReleaseKeyedEvent is not called during
        // that small window, while updating a second atomic to tell the wakeup that it needs to
        // keep trying to wake something up. It's not optimal, but since the keyed event path is
        // only used on old OSes, it's an acceptable compromise.

        // double check the the value didn't change on us, with the lock held
        uint8 oldwait = 0;
        int32 count = 0;
        while (!atomicCompareExchange(uint8, weak, &ftx->_ps_lock, &oldwait, 1, Acquire, Relaxed)) {
            _CPU_PAUSE;
            if (++count > 2)
                osYield();      // relieve some pressure if we can't get it
            oldwait = 0;
        }
        if (atomicLoad(int32, &ftx->val, Relaxed) != oldval) {
            atomicFetchSub(uint8, &ftx->_ps_lock, 1, Relaxed);
            return FUTEX_Retry;
        }
        // we're going to sleep, let the waiter thread know
        atomicFetchAdd(uint16, &ftx->_ps, 1, Relaxed);
        // unlock spinlock
        atomicFetchSub(uint8, &ftx->_ps_lock, 1, Release);

        long status = pNtWaitForKeyedEvent(ftxke, &ftx->val, 0, (timeout == timeForever) ? NULL : &ketimeout);
        atomicFetchSub(uint16, &ftx->_ps, 1, Release);

        if (status == STATUS_TIMEOUT)
            return FUTEX_Timeout;
        if (status == STATUS_SUCCESS)
            return FUTEX_Waited;
        return FUTEX_Error;
    }
}

_Use_decl_annotations_
void futexWake(Futex *ftx)
{
    if (pWaitOnAddress) {
        // fast path, this happens most of the time
        pWakeByAddressSingle(&ftx->val);
    } else {
        LARGE_INTEGER ketimeout = { 0 };
        bool ret = false;
        uint16 nwaiters;

        // get spinlock
        uint8 oldwait = 0;
        while (!atomicCompareExchange(uint8, weak, &ftx->_ps_lock, &oldwait, 1, Acquire, Relaxed)) {
            // yield instead of pause here to give waiting threads priority since they're going to go to
            // sleep anyway; sacrifices a bit of performance on wakeup when there's contention but
            // significantly boosts maximum concurrency
            osYield();
            oldwait = 0;
        }

        // First, try to immediately wake something up with the spinlock held
        if (pNtReleaseKeyedEvent(ftxke, &ftx->val, 0, &ketimeout) == STATUS_SUCCESS)
            ret = true;
        else
            nwaiters = atomicLoad(uint16, &ftx->_ps, Relaxed);  // want to read this with lock held the first time

        // unlock spinlock
        atomicFetchSub(uint8, &ftx->_ps_lock, 1, Relaxed);

        if (ret)
            return;

        ketimeout.QuadPart = -10000;                // 1ms delay
        // Didn't wake anything up, but if we know we have waiters -- try a little harder
        while (nwaiters > 0) {
            if (pNtReleaseKeyedEvent(ftxke, &ftx->val, 0, &ketimeout) == STATUS_SUCCESS)
                return;

            // refresh in case all the waiters timed out
            nwaiters = atomicLoad(uint16, &ftx->_ps, Relaxed);
        }
    }
}

_Use_decl_annotations_
void futexWakeMany(Futex *ftx, int count)
{
    if (pWaitOnAddress) {
        // fast path, this happens most of the time
        while (count > 0) {
            pWakeByAddressSingle(&ftx->val);
            --count;
        }
    } else {
        LARGE_INTEGER ketimeout = { 0 };
        uint16 nwaiters;

        // get spinlock
        uint8 oldwait = 0;
        while (!atomicCompareExchange(uint8, weak, &ftx->_ps_lock, &oldwait, 1, Acquire, Relaxed)) {
            _CPU_PAUSE;
            oldwait = 0;
        }
        nwaiters = atomicLoad(uint16, &ftx->_ps, Relaxed);
        // unlock spinlock
        atomicFetchSub(uint8, &ftx->_ps_lock, 1, Relaxed);

        ketimeout.QuadPart = -10000;                // max 1ms delays

        // try to wake up all the threads requested, if there are enough waiters
        count = min(nwaiters, count);
        for (int i = 0; i < count; i++) {
            pNtReleaseKeyedEvent(ftxke, &ftx->val, 0, &ketimeout);

            // but early out rather than waiting on timeouts if we hit 0 (concurrent wake)
            if ((atomicLoad(uint16, &ftx->_ps, Relaxed)) == 0)
                return;
        }
    }
}

_Use_decl_annotations_
void futexWakeAll(Futex *ftx)
{
    if (pWaitOnAddress) {
        // fast path, this happens most of the time
        pWakeByAddressAll(&ftx->val);
    }
    else {
        LARGE_INTEGER ketimeout = { 0 };
        uint16 nwaiters;

        // get spinlock
        uint8 oldwait = 0;
        while (!atomicCompareExchange(uint8, weak, &ftx->_ps_lock, &oldwait, 1, Acquire, Relaxed)) {
            _CPU_PAUSE;
            oldwait = 0;
        }
        nwaiters = atomicLoad(uint16, &ftx->_ps, Relaxed);
        // unlock spinlock
        atomicFetchSub(uint8, &ftx->_ps_lock, 1, Relaxed);

        ketimeout.QuadPart = -10000;                // max 1ms delays
        // try to wake up as many threads as we know about.
        // this is for broadcast events, so we don't really have to worry about
        // anybody new starting to wait once this loop begins

        // Possible alternative approach is to just keep calling NtReleaseKeyedEvent until it
        // returns STATUS_TIMEOUT, i.e. no more threads to wake up. Better/worse? Does it matter?

        for (int i = 0; i < nwaiters; i++) {
            pNtReleaseKeyedEvent(ftxke, &ftx->val, 0, &ketimeout);

            // but early out rather than waiting on timeouts if we do hit 0
            if ((atomicLoad(uint16, &ftx->_ps, Relaxed)) == 0)
                return;
        }
    }
}
