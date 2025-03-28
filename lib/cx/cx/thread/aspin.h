#pragma once

#include <cx/math/lcg.h>
#include <cx/thread/atomic.h>
#include <cx/time/clock.h>
#include <cx/time/time.h>
#include <cx/platform/cpu.h>
#include <cx/platform/os.h>
#include <cx/utils/compare.h>

// Common functions for adaptive spin threading primitives

#define ASPIN_MAX_USEC 10                    // hard cap in microseconds
#define ASPIN_INITIAL_TARGET 10              // initial number of cycles to spin
#define ASPIN_NOSPIN (-2147483647 - 1)

#if DEBUG_LEVEL >= 2
#define ASPIN_PERF_STATS
#endif

typedef struct AdaptiveSpin
{
    atomic(int32) spintarget;
#ifdef ASPIN_PERF_STATS
    atomic(intptr) stats_uncontended;       // uncontended acquisition of primitive
    atomic(intptr) stats_spin;              // times primitive was acquired after spinning
    atomic(intptr) stats_futex;             // times primitive was acquired on non-spin (futex) path
    atomic(intptr) stats_capped;            // times spin loop was capped by ASPIN_MAX_USEC
    atomic(intptr) stats_timeout;           // times the primitive failed due to timeout
    atomic(intptr) stats_yield;             // times the primitive yielded CPU to reduce contention
#endif
} AdaptiveSpin;

typedef struct AdaptiveSpinState
{
    int64 now;
    int64 start;
    int64 endtime;
    int64 spincap;
    int32 curtarget;
    int32 spincount;
    int32 contention;
    uint32 rstate;
} AdaptiveSpinState;

_meta_inline void aspinRecordUncontended(_Inout_ AdaptiveSpin *aspin)
{
#ifdef ASPIN_PERF_STATS
    atomicFetchAdd(intptr, &aspin->stats_uncontended, 1, Relaxed);
#else
    (void)aspin;
#endif
}

_meta_inline void aspinRecordSpin(_Inout_ AdaptiveSpin *aspin)
{
#ifdef ASPIN_PERF_STATS
    atomicFetchAdd(intptr, &aspin->stats_spin, 1, Relaxed);
#else
    (void)aspin;
#endif
}

_meta_inline void aspinRecordFutex(_Inout_ AdaptiveSpin *aspin)
{
#ifdef ASPIN_PERF_STATS
    atomicFetchAdd(intptr, &aspin->stats_futex, 1, Relaxed);
#else
    (void)aspin;
#endif
}

_meta_inline void aspinRecordCapped(_Inout_ AdaptiveSpin *aspin)
{
#ifdef ASPIN_PERF_STATS
    atomicFetchAdd(intptr, &aspin->stats_capped, 1, Relaxed);
#else
    (void)aspin;
#endif
}

_meta_inline void aspinRecordTimeout(_Inout_ AdaptiveSpin *aspin)
{
#ifdef ASPIN_PERF_STATS
    atomicFetchAdd(intptr, &aspin->stats_timeout, 1, Relaxed);
#else
    (void)aspin;
#endif
}

_meta_inline void aspinRecordYield(_Inout_ AdaptiveSpin *aspin)
{
#ifdef ASPIN_PERF_STATS
    atomicFetchAdd(intptr, &aspin->stats_yield, 1, Relaxed);
#else
    (void)aspin;
#endif
}

_meta_inline void aspinInit(_Out_ AdaptiveSpin *aspin, bool nospin)
{
    memset(aspin, 0, sizeof(AdaptiveSpin));

    // never spin if there's only a single core, just a waste of CPU
    if (osPhysicalCPUs() == 1)
        nospin = true;

    atomicStore(int32, &aspin->spintarget, nospin ? ASPIN_NOSPIN : ASPIN_INITIAL_TARGET, Relaxed);
}

_meta_inline void aspinBegin(_Inout_ AdaptiveSpin *aspin, _Out_ AdaptiveSpinState *ass, int64 timeout)
{
    ass->now = clockTimer();
    ass->start = ass->now;
    ass->spincap = ass->start + ASPIN_MAX_USEC;
    ass->endtime = (timeout == timeForever) ? timeForever : ass->start + timeout;
    ass->curtarget = atomicLoad(int32, &aspin->spintarget, Relaxed);
    if (ass->curtarget < 1 && ass->curtarget != ASPIN_NOSPIN)
        ass->curtarget = ASPIN_INITIAL_TARGET;
    ass->spincount = (ass->curtarget == ASPIN_NOSPIN) ? 0 : ass->curtarget * 2;       // -1 == nospin
    ass->contention = 0;
    ass->rstate = (ass->now & 0xffffffff);
}

_meta_inline bool aspinSpin(_Inout_ AdaptiveSpin *aspin, _Inout_ AdaptiveSpinState *ass)
{
    // clockTimer may be an expensive system call on some platforms.
    // Only update clock once every 8 loops, for a tighter spin and less latency.
    if ((ass->spincount & 7) == 7)
        ass->now = clockTimer();

    // check if we hit the hard cap on spin time
    if (ass->spincount > 0 && ass->now > ass->spincap) {
        // this sets the target to half the current spincount, since the cap will be double that
        atomicStore(int32, &aspin->spintarget, (ass->curtarget * 2 - ass->spincount) / 2, Relaxed);
        ass->spincount = 0;
        aspinRecordCapped(aspin);
    }

    if (ass->spincount > 0) {
        --ass->spincount;
        _CPU_PAUSE;
        return true;
    }

    return false;
}

_meta_inline bool aspinTimeout(_Inout_ AdaptiveSpin *aspin, _Inout_ AdaptiveSpinState *ass)
{
    // early out if we don't have a timeout -- skip clock update
    if (ass->endtime == timeForever)
        return false;

    // if spinloop is done, update clock here instead since the futex wait
    // could be a very long time
    if (ass->spincount == 0)
        ass->now = clockTimer();

    if (ass->now > ass->endtime) {
        aspinRecordTimeout(aspin);
        return true;
    }
    return false;
}

_meta_inline void aspinAdapt(_Inout_ AdaptiveSpin *aspin, _Inout_ AdaptiveSpinState *ass)
{
    // don't adapt if we timed out entirely
    if (ass->now > ass->endtime)
        return;

    if (ass->curtarget != ASPIN_NOSPIN) {
        if (ass->spincount > 0) {
            // adjust adaptive target based on how much spinning we did
            int32 realtarget = atomicLoad(int32, &aspin->spintarget, Relaxed);
            atomicFetchAdd(int32, &aspin->spintarget, clamp(ass->curtarget - ass->spincount, -realtarget, realtarget) / 8 + 1, Relaxed);
            aspinRecordSpin(aspin);
        } else {
            if (ass->now <= ass->spincap) {
                // we had to go to futuxes, give it a boost
                atomicFetchAdd(int32, &aspin->spintarget, ass->curtarget / 8 + 1, Relaxed);
            }
            aspinRecordFutex(aspin);
        }
    } else {
        aspinRecordFutex(aspin);
    }
}

_meta_inline int64 aspinTimeoutRemaining(_In_ AdaptiveSpinState *ass)
{
    return (ass->endtime == timeForever) ? timeForever : ass->endtime - ass->now;
}

// call this function when there is contention on a CAS
_meta_inline void aspinHandleContention(_Inout_opt_ AdaptiveSpin *aspin, _Inout_ AdaptiveSpinState *ass)
{
    // This algorithm is cruicial to maintaining high performance even under extreme contention.
    // Earlier versions of these primitives started to suffer degradation when many concurrent
    // threads attempted to get the mutex/etc simultaneously. They would spend a lot of time
    // spinning on a CAS on the underlying atomic, attempting to get into a state where they
    // could wait with a futex/semaphore.

    // By calling this function after a failed CAS, a count of failures is maintained. As the
    // count increases, so does the probability that the thread will yield its quantum, reducing
    // contention enough for another thread to succeed on the CAS and break the cycle.

    // We use a poor quality but fast LCG pseudorandom number generator that is good enough
    // for this purpose.

    if (lcgRandom(&ass->rstate) % (++ass->contention + 7) > 8) {
        if (aspin)
            aspinRecordYield(aspin);
        osYield();
    } else {
        for(int i = ass->contention * ass->contention; i >= 0; --i) {
            _CPU_PAUSE;
        }
    }
}

_meta_inline void aspinEndContention(_Inout_ AdaptiveSpinState *ass)
{
    ass->contention = 0;
}
