#pragma once

#include <cx/cx.h>
#include <cx/thread/atomic.h>
#include <cx/thread/aspin.h>
#include <cx/thread/mutex.h>

// Pointer-ring FIFO queue
// Lock-free thread-safe expandable ringbuffer implementation.
//
// This is in thread/ rather than container/ because it is a low-level structure intended for use
// to implement either containers or other modules, such as the MPMC work queue.
//
// In abstract, it is a truly lock-free* structure in general operation. It makes a few design
// tradeoffs to achieve this and may not be as performant as a specialized implementation that
// does not provide full lock-free guarantees, but aims to be solidly fast for most general use
// cases.
//
// * with some small caveats which are documented below
//
// It is fully thread-safe and can be used with multiple threads both pushing and popping
// simultaneously.
//
// NULL pointers may NOT be stored in this queue -- it as considered an error to try to insert one.
//
// Lock-free guarantees
//
// The core algorithm is lock-free in the classical computer science sense. That is, if a thread
// using it suspends or terminates unexpectedly while it is in the middle of an operation on the
// queue, it will not irreperably break the structure and block other threads. It does not guarantee
// full performance if this happens, however.
//
// Additionally, the lock-free guarantee does not extend to the garbage collection operation. This
// operation does indeed use a lock that limits it to only exectuing on a single thread at once,
// though it IS guaranteed that the collect operation will never block, but instead return immediately
// if it cannot lock the structure, with the intent that it be run periodically so that it is
// eventually able to succeed.
//
// Threads which stall or terminate in the middle of a push operation will likewise block the ability
// of the garbage collector to prune the structure, causing a performance hit if the buffer has been
// expanded multiple times.
//
// Despite all these warnings, garbage collection is not an essential function and the queue continues
// to function even if GC is never able to run, albeit not optimally.
//
// Ordering guarantees
//
// Push operations within a single thread are guaranteed to be popped in order, provided a single
// thread (not necessarily the same one) pops them sequentially.
//
// Push operations spread across multiple threads are ordered in a best effort fashion and will
// generally be inserted into the queue in the order that the operations finish in real time. However
// this is not a strong guarantee, and there may be minor reordering among entries that were pushed
// at roughly the same time.
//
// Similarlty, simultaneous pop operations in multiple threads will return the items in the order
// they were inserted, but variations in timing among threads may cause them to not be processed
// in the exact order.

#if DEBUG_LEVEL >= 2 && _64BIT
#define PRQ_PERF_STATS
#endif
CX_C_BEGIN

typedef enum PrqGrowthEnum
{
    PRQ_Grow_None = 1,
    PRQ_Grow_25,
    PRQ_Grow_50,
    PRQ_Grow_100,       // Default
    PRQ_Grow_150,
    PRQ_Grow_200,
} PrqGrowth;

typedef struct PrqSegment PrqSegment;

#ifdef PRQ_PERF_STATS
typedef struct PrqPerfStats {
    atomic(uint64) grow;
    atomic(uint64) grow_collision;
    atomic(uint64) shrink;
    atomic(uint64) shrink_collision;
    atomic(uint64) head_contention;
    atomic(uint64) reserved_contention;
    atomic(uint64) push;
    atomic(uint64) push_optimal;
    atomic(uint64) push_fast;
    atomic(uint64) push_slow;
    atomic(uint64) push_appeared_full;
    atomic(uint64) push_actually_full;
    atomic(uint64) push_collision;
    atomic(uint64) push_retry;
    atomic(uint64) push_full_retry;
    atomic(uint64) push_noreserve_retiring;
    atomic(uint64) pop;
    atomic(uint64) pop_optimal;
    atomic(uint64) pop_fast;
    atomic(uint64) pop_slow;
    atomic(uint64) pop_nonobvious_empty;
    atomic(uint64) pop_assist;
    atomic(uint64) pop_assist_fail;
    atomic(uint64) pop_segtraverse;
    atomic(uint64) pop_collision;
    atomic(uint64) gc_run;
    atomic(uint64) seg_retired;
    atomic(uint64) seg_dealloc;
    atomic(uint64) seg_dealloc_failinuse;
} PrqPerfStats;
#endif

typedef struct PrQueue
{
    // Initial size of the queue as well as minimum size.
    uint32 minsz;

    // Ideal size of the queue.
    uint32 targetsz;

    // Maximum size of the queue.
    uint32 maxsz;

    // How much to grow the queue at a time.
    PrqGrowth growth;

    // How much to shrink the queue at a time.
    PrqGrowth shrink;

    // Concurrence factor. Defaults to the number of logcal CPUs in the system. This value is
    // used to determine how many queue entries there must be before threads start assisting
    // each other to complete operations.
    uint32 concurrence;

    // Buffer segment that is currently the target for queue pushes and pops. The head of a linked
    // list of buffer segments when the queue is being grown.
    atomic(ptr) current;

    // Linked list of buffer segments that have been retired. These are segments that have been
    // fully emptied and no longer have any valid queue entries, nor can they have any entries
    // pushed into them, but are being held to deallocate later until it can be guaranteed that
    // no threads attempting a pop operation still have a pointer to the segment.
    PrqSegment *retired;

    // Access counter. This is used internally to close a small gap between when a thread retrieves
    // the current segment pointers and when it increments the use counter, since it cannot do that
    // atomically while another thread is retiring the segment.
    atomic(int32) access;

    // Lower 32 bits of timestamp of the last time a segment was added to grow or shrink the queue,
    // because this needs to be atomic and 64-bit atomics don't exist on all platforms.
    atomic(uint32) chgtime;

    // Minimum number of milliseconds the queue must wait to shrink after growing or shrinking (default 500ms).
    uint32 shrinkms;

    // Running average to track the total queue size across GC cycles for possible shrinking.
    uint32 avgcount;
    uint32 avgcount_num;

    // Only 1 thread may run the garbage collection operation at a time. It's recommended to try to
    // run GC optimistcally when a thread has nothing else to do. For example, a consumer thread
    // that is about to sleep.
    Mutex gcmtx;

#ifdef PRQ_PERF_STATS
    // Performance stats for debugging
    PrqPerfStats stats;
#endif
} PrQueue;

typedef struct PrqSegment
{
    // Next segment in the chain. When the queue grows, it allocates a larger segment which is
    // temporarily chained to from the original segment in order to handle the transition while
    // many other threads may be still using the original.
    atomic(ptr) nextseg;

    // Next retired segment in retired chain.
    // NOTE: We cannot reuse nextseg for this. nextseg needs to continue to point to the actual
    // segment that replaced this one, so that any threads which grabbed a pointer to this segment
    // before it was retired can still follow it.
    PrqSegment *nextretired;

    // Atomic counter of how many threads are actively using this segment. This, along with the
    // access counter, act as a non-blocking optimistic lock similar to a reader-writer lock but
    // less intrusive. They block garbage collection from deallocating this segment if there is
    // a chance that a thread may still be reading from it (or about to read from it).
    atomic(int32) inuse;

    // Total number of queue slots in this buffer.
    uint32 size;

    // Number of queue slots in this buffer that are used. This number may be slightly higher
    // than the actual number of slots that has been written to. This is the authoritative
    // source for how much of the queue is used.
    atomic(uint32) count;

    // Head of the queue; points at the slot that is first in line to be read. This is cached
    // information for performance optimization only and is not authoritative.
    atomic(uint32) head;

    // Number of write reservations on this segment. Only used when the buffer is expandable, to
    // prevent GC from retiring the segment while there are pending write operations. The high
    // bit is used to signal that the segment is transitioning to the retired status and further
    // writes may not be started.
    atomic(uint32) reserved;

    // The actual ringbuffer
    atomic(ptr) buffer[];
} PrqSegment;

// Initialize a fixed-sized ringbuffer. Guaranteed to succeed, or assert.
void prqInitFixed(_Out_ PrQueue *prq, uint32 sz);

// Initialize a dynamic buffer chain. Guaranteed to succeed, or assert.
// minsz: Minimum & initial size
// targetsz: Ideal size the buffer should try to reach
// maxsz: Maximum size
// growth: How much to grow at a time
// shrink: How much to shrink at a time
void prqInitDynamic(_Out_ PrQueue *prq, uint32 minsz, uint32 targetsz, uint32 maxsz,
                      PrqGrowth growth, PrqGrowth shrink);

// Attempts to destroy the queue. Will fail if there are still entries in the queue, because
// this is a low-level API that has no idea what the pointers stored in it point to or what
// kind of cleanup needs to be done on them. It is the caller's responsibility to ensure that
// all entires have been popped and no threads are still trying to push new ones!
_Success_(return)
bool prqDestroy(_Pre_valid_ _Post_invalid_ PrQueue *prq);

// Attempt to push a pointer into the queue. Returns true on success, false if the queue
// is full and cannot be grown. Upon success, the pointer should be considered to be
// owned by the queue and not touched again.
_Success_(return)
bool prqPush(_Inout_ PrQueue *prq, _Pre_notnull_ _Post_invalid_ void *ptr);

// Attempt to pop a pointer from the queue. If one is available, it is returned. Otherwise,
// a NULL return indicates that the queue is empty.
_Must_inspect_result_ _Ret_maybenull_
void *prqPop(_Inout_ PrQueue *prq);

// Attempt to run a garbage collection cycle on the queue. Returns true if the cycle runs,
// whether or not anything was collected.
bool prqCollect(_Inout_ PrQueue *prq);

// Retrieves an estimated count of the number of valid items in the queue. Accuracy
// varies depending on how busy the queue is.
uint32 prqCount(_In_ PrQueue *prq);

// Attempt to fetch a copy of the nth pointer from the queue.
// EXERCISE EXTREME CAUTION!
// This is very dangerous and tricky to use safely! It is likely the pointer returned
// by this function is being actively processed by another thread and may have already
// been removed from the queue by the time you examine its contents. It is almost
// certain to cause a crash unless you take precautions to prevent the pointed-to
// data from being destroyed after being processed, and your underlying data must be
// thread-safe.
// This function is intended for use only in controlled situations where guarantees can
// be made about which threads pop items from the queue and what they do with those items.
void *prqPeek(_In_ PrQueue *prq, uint32 n);

CX_C_END
