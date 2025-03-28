#include "prqueue.h"
#include <cx/utils/macros/unused.h>
#include <cx/time.h>

#define RESERVED_RETIRING 0x80000000
#define RESERVED_MASK 0x7fffffff

#ifdef PRQ_PERF_STATS
#define INCSTAT(prq, sname) atomicFetchAdd(uint64, &(prq)->stats.sname, 1, Relaxed)
#else
#define INCSTAT(prq, sname) nop_stmt
#endif

#define qdynamic(prq) (prq->growth != PRQ_Grow_None)

static void _prqInit(_Out_ PrQueue *prq, uint32 minsz, uint32 targetsz, uint32 maxsz,
                    PrqGrowth growth, PrqGrowth shrink)
{
    prq->minsz = minsz;
    prq->targetsz = targetsz;
    prq->maxsz = maxsz;
    prq->growth = growth;
    prq->shrink = growth == PRQ_Grow_None ? PRQ_Grow_None : shrink;
    prq->concurrence = clamplow(osLogicalCPUs() * 2, 4);
    prq->retired = NULL;
    prq->shrinkms = 500;
    prq->avgcount = 0;
    prq->avgcount_num = 0;
    atomicStore(int32, &prq->access, 0, Relaxed);

    PrqSegment *seg = xaAlloc(minsz * sizeof(void*) + offsetof(PrqSegment, buffer), XA_Zero);
    seg->size = minsz;
    atomicStore(ptr, &prq->current, seg, Release);

    if(qdynamic(prq)) {
        mutexInit(&prq->gcmtx);
        atomicStore(uint32, &prq->chgtime, (uint64)clockTimer() & 0xffffffff, Relaxed);
    }

#ifdef PRQ_PERF_STATS
    memset(&prq->stats, 0, sizeof(prq->stats));
#endif
}

_Use_decl_annotations_
void prqInitFixed(PrQueue *prq, uint32 sz)
{
    _prqInit(prq, sz, sz, sz, PRQ_Grow_None, PRQ_Grow_None);
}

_Use_decl_annotations_
void prqInitDynamic(PrQueue *prq, uint32 minsz, uint32 targetsz, uint32 maxsz,
                    PrqGrowth growth, PrqGrowth shrink)
{
    if(growth == 0)
        growth = PRQ_Grow_100;
    if(shrink == 0)
        shrink = PRQ_Grow_100;
    if(maxsz == 0)
        maxsz = MAX_UINT32;
    if(targetsz == 0)
        targetsz = minsz;

    _prqInit(prq, minsz, targetsz, maxsz, growth, shrink);
}

static uint32 prqGrowSize(PrqGrowth growth, uint32 size)
{
    switch(growth) {
    case PRQ_Grow_25:
        return clamplow(size + (size >> 2), size + 1);
    case PRQ_Grow_50:
        return clamplow(size + (size >> 1), size + 1);
    case PRQ_Grow_100:
    default:
        return clamplow(size << 1, size + 1);
    case PRQ_Grow_150:
        return clamplow((size << 1) + (size >> 1), size + 1);
    case PRQ_Grow_200:
        return clamplow(size * 3, size + 1);
    }
}

static PrqSegment *prqGrow(PrQueue *prq, PrqSegment *seg, uint32 newsz, bool *collision)
{
    PrqSegment *newseg = NULL;

    newsz = clamphigh(newsz, prq->maxsz);
    if(collision)
        *collision = false;

    if(newsz <= seg->size)
        return NULL;

    newseg = xaAlloc(newsz * sizeof(void *) + offsetof(PrqSegment, buffer), XA_Zero);
    void *expected = NULL;
    newseg->size = newsz;
    if(atomicCompareExchange(ptr, strong, &seg->nextseg, &expected, newseg, Release, Relaxed)) {
        INCSTAT(prq, grow);
        atomicStore(uint32, &prq->chgtime, (uint64)clockTimer() & 0xffffffff, Release);
        return newseg;
    } else {
        // failed to swap the pointer, this means another thread grew/shrank the buffer first
        INCSTAT(prq, grow_collision);
        xaFree(newseg);
        if(collision)
            *collision = true;
        return NULL;
    }
}

static uint32 prqShrinkSize(PrqGrowth shrink, uint32 size)
{
    switch(shrink) {
    case PRQ_Grow_25:
        return clamphigh(size - (size >> 4), size - 1);
        break;
    case PRQ_Grow_50:
        return clamphigh(size - (size >> 2), size - 1);
        break;
    case PRQ_Grow_100:
    default:
        return clamphigh(size >> 1, size - 1);
        break;
    case PRQ_Grow_150:
        return clamphigh((size >> 1) - (size >> 2), size - 1);
        break;
    case PRQ_Grow_200:
        return clamphigh(size / 3, size - 1);
        break;
    }
}

static PrqSegment *prqShrink(PrQueue *prq, PrqSegment *seg, uint32 newsz, bool *collision)
{
    PrqSegment *newseg = NULL;

    newsz = clamplow(newsz, prq->minsz);
    if (collision)
        *collision = false;

    if(newsz >= seg->size)
        return NULL;

    newseg = xaAlloc(newsz * sizeof(void *) + offsetof(PrqSegment, buffer), XA_Zero);
    void *expected = NULL;
    newseg->size = newsz;
    if(atomicCompareExchange(ptr, strong, &seg->nextseg, &expected, newseg, Release, Relaxed)) {
        INCSTAT(prq, shrink);
        atomicStore(uint32, &prq->chgtime, (uint64)clockTimer() & 0xffffffff, Release);
        return newseg;
    } else {
        // failed to swap the pointer, this means another thread grew/shrank the buffer first
        INCSTAT(prq, shrink_collision);
        xaFree(newseg);
        if(collision)
            *collision = true;
        return NULL;
    }
}

_Use_decl_annotations_
bool prqPush(PrQueue *prq, void *ptr)
{
    if(!ptr)
        return false;           // invalid to push a NULL pointer

    AdaptiveSpinState astate = { 0 };
    const bool dynamic = qdynamic(prq);

    // Safely get the current segment and indicate that we're using it
    if(dynamic)
        atomicFetchAdd(int32, &prq->access, 1, AcqRel);
    PrqSegment *seg = atomicLoad(ptr, &prq->current, Acquire);
    if(!seg) {
        if(dynamic)
            atomicFetchAdd(int32, &prq->access, -1, Relaxed);
        return false;
    }
    if(dynamic) {
        atomicFetchAdd(int32, &seg->inuse, 1, AcqRel);
        atomicFetchAdd(int32, &prq->access, -1, Relaxed);
    }

    PrqSegment *nextseg;
    bool full = false;
    bool didreserve = false;

fullretry:
    // Back out the previous write reservation
    if(didreserve) {
        uint32 reserved = atomicLoad(uint32, &seg->reserved, Relaxed);
        while((reserved & RESERVED_MASK) > 0) {
            // need to reduce masked part while preserving retiring bit
            if(atomicCompareExchange(uint32, weak, &seg->reserved, &reserved,
                                     (reserved & RESERVED_RETIRING) | ((reserved & RESERVED_MASK) - 1), Release, Acquire))
                break;
            INCSTAT(prq, reserved_contention);
            aspinHandleContention(NULL, &astate);
        }
        aspinEndContention(&astate);
        didreserve = false;
    }

    // Chase the segment chain if needed
    if(dynamic) {
        while((nextseg = atomicLoad(ptr, &seg->nextseg, Acquire))) {
            atomicFetchAdd(int32, &nextseg->inuse, 1, AcqRel);
            atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);
            seg = nextseg;
            full = false;
        }
    }

    bool appearedfull = false;
    uint32 count;

    // check for full segment
    count = atomicLoad(uint32, &seg->count, Acquire);

    // If we are still able to grow, prefer to do that if the segment appears full.
    // The reason is that growing preserves queue ordering better. We should only fall back
    // to scavenging for an empty slot if there's no other choice.
    if((dynamic && seg->size < prq->maxsz) && count >= seg->size)
        full = true;

    // grow if segment is actually full, usually can only happen on retry
    if(full) {
        if(dynamic && seg->size < prq->maxsz) {
            bool gcoll;
            if (prqGrow(prq, seg, prqGrowSize(prq->growth, seg->size), &gcoll)) {
                full = false;
                goto fullretry;         // will follow nextseg into the new segment we just allocated
            } else if (gcoll) {
                // failed to swap the pointer, this means another thread grew the buffer first
                goto fullretry;         // will follow nextseg into the new segment the other thread allocated
            }
        }

        // cannot grow any more, return failure
        if(dynamic)
            atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);
        return false;
    } else if(count >= seg->size) {
        // Segment APPEARS full, but we can't be certain until an insert is attempted.
        // This is a suboptimal path so we should try to avoid it when possible.
        // Do NOT increment the count in this case.
        appearedfull = true;
    }

    // get a write reservation if we need one
    if(dynamic) {
        uint32 reserved = atomicLoad(uint32, &seg->reserved, Relaxed);
        for(;;) {
            bool retiring = (reserved & RESERVED_RETIRING);
            reserved = (reserved & RESERVED_MASK);
            // if the segment is being retired, or if we somehow hit 2 billion writers, need to retry instead
            if(retiring || reserved == RESERVED_MASK) {
                INCSTAT(prq, push_noreserve_retiring);
                aspinEndContention(&astate);
                goto fullretry;
            }

            if(atomicCompareExchange(uint32, weak, &seg->reserved, &reserved, reserved + 1, Release, Acquire))
                break;
            INCSTAT(prq, reserved_contention);
            aspinHandleContention(NULL, &astate);
        }
        aspinEndContention(&astate);

        didreserve = true;
    }

    bool success;
    uint32 head = atomicLoad(uint32, &seg->head, Relaxed);
    uint32 slot;

retry:
    // actually do the update
    success = false;
    void *oldptr;
    // scan the segment, starting from the slot where we expect the tail to be
    for(slot = head + count; !success && slot < head + count + seg->size; slot++) {
        oldptr = atomicLoad(ptr, &seg->buffer[slot % seg->size], Relaxed);
        while(oldptr == NULL) {
            if(atomicCompareExchange(ptr, weak, &seg->buffer[slot % seg->size], &oldptr, ptr, Release, Acquire)) {
                if(slot == head + count)
                    INCSTAT(prq, push_optimal);
                else if(slot - (head + count) < (seg->size << 2))
                    INCSTAT(prq, push_fast);
                else
                    INCSTAT(prq, push_slow);

                // for perf reasons, we care that this happened on the suboptimal path of having to scan every slot
                if(appearedfull)
                    INCSTAT(prq, push_appeared_full);

                success = true;
                break;
            } else {
                aspinHandleContention(NULL, &astate);
                INCSTAT(prq, push_collision);
            }
        }
        aspinEndContention(&astate);
    }

    if(!success) {
        count = atomicLoad(uint32, &seg->count, Acquire);

        // if we're full, need to retry from the top
        if(count >= seg->size) {
            full = true;
            if (appearedfull)
                INCSTAT(prq, push_actually_full);
            INCSTAT(prq, push_full_retry);
            goto fullretry;
        }

        head = atomicLoad(uint32, &seg->head, Relaxed);
        INCSTAT(prq, push_retry);
        goto retry;
    }

    // Need to increment the count
    atomicFetchAdd(uint32, &seg->count, 1, AcqRel);

    // release write reservation if we needed one for this transaction
    if(didreserve) {
        uint32 reserved = atomicLoad(uint32, &seg->reserved, Relaxed);
        while((reserved & RESERVED_MASK) > 0) {
            // need to reduce masked part while preserving retiring bit
            if(atomicCompareExchange(uint32, weak, &seg->reserved, &reserved,
                                     (reserved & RESERVED_RETIRING) | ((reserved & RESERVED_MASK) - 1), Release, Acquire))
                break;
            INCSTAT(prq, reserved_contention);
            aspinHandleContention(NULL, &astate);
        }
        aspinEndContention(&astate);
    }

    // all done, release the buffer segment
    if(dynamic)
        atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);

    INCSTAT(prq, push);
    return true;
}

_Use_decl_annotations_
void *prqPop(PrQueue *prq)
{
    AdaptiveSpinState astate = { 0 };
    const bool dynamic = qdynamic(prq);

    // Safely get the current segment and indicate that we're using it
    if(dynamic)
        atomicFetchAdd(int32, &prq->access, 1, AcqRel);
    PrqSegment *seg = atomicLoad(ptr, &prq->current, Acquire);
    if(!seg) {
        if(dynamic)
            atomicFetchAdd(int32, &prq->access, -1, Relaxed);
        return NULL;
    }
    if(dynamic) {
        atomicFetchAdd(int32, &seg->inuse, 1, AcqRel);
        atomicFetchAdd(int32, &prq->access, -1, Relaxed);
    }

    // assist if we're over the concurrence factor, or using more than half the queue
    uint32 assist_thresh = min(prq->concurrence, seg->size / 2);
    bool empty = false;
    bool havenext = false;      // if this segment has been replaced by growing, we always want to try to assist

    uint32 count = atomicLoad(uint32, &seg->count, Acquire);
    uint32 head = atomicLoad(uint32, &seg->head, Relaxed);          // acquire/release on count maintains head ordering,
                                                                    // which doesn't have to be exact

    // check if the buffer APPEARS empty, it may still be EFFECTIVELY empty
    // if there are incomplete writes in progress
    empty = (count == 0);

retry:
    if(empty) {
        if(dynamic) {
            // if this is a dynamic queue, check the next segment
            PrqSegment *nextseg = atomicLoad(ptr, &seg->nextseg, Acquire);
            if(nextseg) {
                atomicFetchAdd(int32, &nextseg->inuse, 1, AcqRel);
                atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);
                seg = nextseg;
                count = atomicLoad(uint32, &seg->count, Acquire);
                head = atomicLoad(uint32, &seg->head, Relaxed);
                INCSTAT(prq, pop_segtraverse);
                empty = (count == 0);
                goto retry;
            }

            atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);
        }
            
        return NULL;                // queue is actually empty!
    } else if(dynamic) {
        havenext = (atomicLoad(ptr, &seg->nextseg, Relaxed) != NULL);
    }

    // now look for something to read
    uint32 slot;

    void *ptr = NULL;
    bool success = false;
    // scan slots in queue for something that's been written
    for(slot = head; slot < head + seg->size; slot++) {
        ptr = atomicLoad(ptr, &seg->buffer[slot % seg->size], Relaxed);
        if(ptr) {
            if(atomicCompareExchange(ptr, weak, &seg->buffer[slot % seg->size], &ptr, NULL, Release, Acquire)) {
                if(slot == head)
                    INCSTAT(prq, pop_optimal);
                else if (slot - head <= (seg->size >> 2))
                    INCSTAT(prq, pop_fast);
                else
                    INCSTAT(prq, pop_slow);
                INCSTAT(prq, pop);
                success = true;
                break;
            } else {
                INCSTAT(prq, pop_collision);
            }
        }
    }

    if(!success) {
        INCSTAT(prq, pop_nonobvious_empty);

        if(!dynamic)
            return NULL;

        // didn't find anything, try again with another segment if possible
        empty = true;
        goto retry;
    }

    // decrement the count ASAP
    atomicFetchAdd(uint32, &seg->count, -1, AcqRel);

    if(slot == head) {
        // if we read the first entry, need to update the head pointer
        uint32 oldhead = head;
        while(oldhead < slot + 1) {
            if(atomicCompareExchange(uint32, weak, &seg->head, &head, (slot + 1) % seg->size, Relaxed, Relaxed)) {
                break;
            } else {
                if(head < oldhead)
                    oldhead = head + seg->size;
                else
                    oldhead = head;
                INCSTAT(prq, head_contention);
                aspinHandleContention(NULL, &astate);
            }
        }
        aspinEndContention(&astate);
    } else if (slot - head > assist_thresh || havenext) {
        // we may need to assist another thread
        if(atomicCompareExchange(uint32, weak, &seg->head, &head, (slot + 1) % seg->size, Relaxed, Relaxed))
            INCSTAT(prq, pop_assist);
        else
            INCSTAT(prq, pop_assist_fail);
    }

    if (dynamic)
        atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);

    return ptr;
}

_Use_decl_annotations_
bool prqCollect(PrQueue *prq)
{
    uint32 totalcount = 0;
    if(!qdynamic(prq))
        return true;            // GC not needed for queues that can't grow

    if(!mutexTryAcquire(&prq->gcmtx))
        return false;

    INCSTAT(prq, gc_run);

    uint32 chgtime = atomicLoad(uint32, &prq->chgtime, Acquire);
    uint32 now = (uint64)clockTimer() & 0xffffffff;
    bool canshrink = (now - chgtime > timeMS(prq->shrinkms)) || (now < chgtime);      // handle rollover

    // try to retire the oldest segment if it's been replaced
    // ONLY safe to do this with the current segment as it's protected directly by prq->access
    PrqSegment *seg = atomicLoad(ptr, &prq->current, Acquire);
    PrqSegment *nextseg = atomicLoad(ptr, &seg->nextseg, Acquire);
    PrqSegment *retired;

    if(nextseg) {
        uint32 count = atomicLoad(uint32, &seg->count, Acquire);
        uint32 reserved = atomicLoad(uint32, &seg->reserved, Relaxed);

        if(!(reserved & RESERVED_RETIRING)) {
            // Phase 1: Add retired bit to block further writes
            atomicFetchOr(uint32, &seg->reserved, RESERVED_RETIRING, Relaxed);
            goto out;                       // no more work this cycle
        } else if(count == 0 && (reserved & RESERVED_MASK) == 0) {
            // Phase 2: Retire segment when it's empty and there are no pending writes
            // CAS is not needed on current because nothing else is allowed to change this pointer except GC, and we have the mutex
            atomicStore(ptr, &prq->current, nextseg, Release);

            // Add to the end of the retired segment chain
            if(prq->retired) {
                retired = prq->retired;
                while(retired->nextretired) {
                    retired = retired->nextretired;
                }
                retired->nextretired = seg;
            } else {
                prq->retired = seg;
            }

            INCSTAT(prq, seg_retired);
            goto out;                       // no more work this cycle
        }
    }

    // try to shrink the active segment at the end of the chain
    while(nextseg) {
        uint32 count = atomicLoad(uint32, &seg->count, Acquire);
        totalcount += count;
        seg = nextseg;
        nextseg = atomicLoad(ptr, &seg->nextseg, Acquire);
    }

    // keep a running average of how big the queue is -- but reset it if the number
    // of samples grows too large and we'd start losing too much precision
    if(prq->avgcount_num == 0 || prq->avgcount_num > seg->size / 4) {
        prq->avgcount = totalcount;
        prq->avgcount_num = 1;
    } else {
        prq->avgcount_num++;
        prq->avgcount += totalcount / prq->avgcount_num - prq->avgcount / prq->avgcount_num;
    }

    if(canshrink && prq->avgcount_num > 4 && seg->size > prq->targetsz) {
        uint32 shrinksz = clamplow(prqShrinkSize(prq->shrink, seg->size), prq->targetsz);

        // failsafe size check to make sure we're not shrinking too much
        if(prq->avgcount <= shrinksz) {
            if(prqShrink(prq, seg, shrinksz, NULL)) {
                prq->avgcount_num = 0;
                goto out;
            }
        }
    }

    // try to deallocate the oldest retired segment
    retired = prq->retired;
    if(retired) {
        uint32 access = atomicLoad(int32, &prq->access, Acquire);
        uint32 inuse = atomicLoad(int32, &retired->inuse, Acquire);
        // can only safely do this if both values are 0.
        // as they are interlocked, prq->access MUST be read first!
        if(access == 0 && inuse == 0) {
            prq->retired = retired->nextretired;
            xaFree(retired);
            INCSTAT(prq, seg_dealloc);
        } else {
            INCSTAT(prq, seg_dealloc_failinuse);
        }
    }

out:
    mutexRelease(&prq->gcmtx);
    return true;
}

_Use_decl_annotations_
uint32 prqCount(PrQueue *prq)
{
    const bool dynamic = qdynamic(prq);

    // Safely get the current segment and indicate that we're using it
    if(dynamic)
        atomicFetchAdd(int32, &prq->access, 1, AcqRel);
    PrqSegment *seg = atomicLoad(ptr, &prq->current, Acquire);
    if(!seg) {
        if(dynamic)
            atomicFetchAdd(int32, &prq->access, -1, Relaxed);
        return false;
    }
    if(dynamic) {
        atomicFetchAdd(int32, &seg->inuse, 1, AcqRel);
        atomicFetchAdd(int32, &prq->access, -1, Relaxed);
    }

    uint32 count = atomicLoad(uint32, &seg->count, Acquire);

    // dynamic queues also need to add the count from all chained segments
    if(dynamic) {
        PrqSegment *nextseg;
        while((nextseg = atomicLoad(ptr, &seg->nextseg, Acquire))) {
            atomicFetchAdd(int32, &nextseg->inuse, 1, AcqRel);
            atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);
            seg = nextseg;
            count += atomicLoad(uint32, &seg->count, Acquire);
        }
    }

    if(dynamic)
        atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);

    return count;
}

_Use_decl_annotations_
void *prqPeek(PrQueue *prq, uint32 n)
{
    const bool dynamic = qdynamic(prq);

    // Safely get the current segment and indicate that we're using it
    if(dynamic)
        atomicFetchAdd(int32, &prq->access, 1, AcqRel);
    PrqSegment *seg = atomicLoad(ptr, &prq->current, Acquire);
    if(!seg) {
        if(dynamic)
            atomicFetchAdd(int32, &prq->access, -1, Relaxed);
        return NULL;
    }
    if(dynamic) {
        atomicFetchAdd(int32, &seg->inuse, 1, AcqRel);
        atomicFetchAdd(int32, &prq->access, -1, Relaxed);
    }

    uint32 count = atomicLoad(uint32, &seg->count, Acquire);
    uint32 found = 0;
    void *ret = NULL;

    // skip over full segments if we're looking for something deeper in the queue
    if(dynamic) {
        PrqSegment *nextseg;
        while(n > count && (nextseg = atomicLoad(ptr, &seg->nextseg, Acquire))) {
            n -= count;
            atomicFetchAdd(int32, &nextseg->inuse, 1, AcqRel);
            atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);
            seg = nextseg;
            count = atomicLoad(uint32, &seg->count, Acquire);
        }
    }

retry:
    if((n - found) < count) {
        // scan for the requested element
        // A full scan is necessary instead of just adding (head + n) because there
        // may be holes in the array from partially completed pushes, and head may
        // also be lagging if there is contention.
        uint32 head = atomicLoad(uint32, &seg->head, Relaxed);
        uint32 slot;

        for(slot = head; slot < head + seg->size; slot++) {
            void *ptr = atomicLoad(ptr, &seg->buffer[slot % seg->size], Relaxed);
            if(ptr)
                found++;

            if(found == n) {
                // got it!
                ret = ptr;
                break;
            }
        }

        // If we didn't find it yet, another thread probably concurrently popped from the queue.
        // Try going to the next segment.
        if(!ret && dynamic) {
            PrqSegment *nextseg = atomicLoad(ptr, &seg->nextseg, Acquire);
            if(nextseg) {
                atomicFetchAdd(int32, &nextseg->inuse, 1, AcqRel);
                atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);
                seg = nextseg;
                count = atomicLoad(uint32, &seg->count, Acquire);
                goto retry;
            }
        }
    }

    if(dynamic)
        atomicFetchAdd(int32, &seg->inuse, -1, Relaxed);

    return ret;
}

_Use_decl_annotations_
bool prqDestroy(PrQueue *prq)
{
    bool ret = false;
    if (qdynamic(prq))
        mutexAcquire(&prq->gcmtx);

    uint64 totalcount = 0;
    bool writepending = false;
    // forcibly retire all active segments
    PrqSegment *seg = atomicLoad(ptr, &prq->current, Acquire);
    while(seg) {
        uint32 reserved = atomicFetchOr(uint32, &seg->reserved, RESERVED_RETIRING, AcqRel);
        if((reserved & RESERVED_MASK) > 0)
            writepending = true;
        totalcount += atomicLoad(uint32, &seg->count, Acquire);
        seg = atomicLoad(ptr, &seg->nextseg, Acquire);
    }

    if(totalcount > 0 || writepending)
        goto out;       // queue isn't empty and idle! abort!

    // this effectively shuts down the queue; no new segments can be added
    seg = atomicLoad(ptr, &prq->current, Acquire);
    if(!atomicCompareExchange(ptr, strong, &prq->current, (void**)&seg, NULL, Release, Relaxed))
        goto out;

    // move them all to retired list to be able to do it all in one go
    PrqSegment *retired;
    while(seg) {
        // Add to the end of the retired segment chain
        if(prq->retired) {
            retired = prq->retired;
            while(retired->nextretired) {
                retired = retired->nextretired;
            }
            retired->nextretired = seg;
        } else {
            prq->retired = seg;
        }

        INCSTAT(prq, seg_retired);
        seg = atomicLoad(ptr, &seg->nextseg, Acquire);
    }

    // now clear out the whole retired list
    ret = true;
    PrqSegment **rptr = &prq->retired;
    retired = prq->retired;
    while(retired) {
        PrqSegment *nextretired = retired->nextretired;
        uint32 access = atomicLoad(int32, &prq->access, Acquire);
        uint32 inuse = atomicLoad(int32, &retired->inuse, Acquire);
        if(access == 0 && inuse == 0) {
            *rptr = nextretired;
            xaFree(retired);
            INCSTAT(prq, seg_dealloc);
        } else {
            // Couldn't deallocate this segment, still in use.
            // Return failure, leave things in a state that could possibly succeed if
            // the caller tries again.
            rptr = &retired->nextretired;
            ret = false;
            INCSTAT(prq, seg_dealloc_failinuse);
        }

        retired = nextretired;
    }

out:
    if(qdynamic(prq)) {
        mutexRelease(&prq->gcmtx);
        if(ret) {
            mutexDestroy(&prq->gcmtx);
        }
    }

    return ret;
}
