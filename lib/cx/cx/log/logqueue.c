// absolutely NEVER debug locks in this file because lock debugging calls log*
#ifdef CX_LOCK_DEBUG
#undef CX_LOCK_DEBUG
#endif

#include "log_private.h"
#include <cx/container/foreach.h>
#include <cx/utils/compare.h>
#include <cx/time/clock.h>
#include <cx/platform/os.h>

#define OVERFLOW_BATCH_MAXENT   65536

PrQueue _log_queue;

// per-thread overflow log batch if the main queue fills up or is under
// high contention
typedef struct LogOverflowTLS {
    LogEntry *head;
    LogEntry *tail;
    int count;
    bool lost;
} LogOverflowTLS;
static _Thread_local LogOverflowTLS _log_overflow;

static bool logQueueAddInternal(_In_ LogEntry *ent)
{
    bool ret = prqPush(&_log_queue, ent);
    if(ret && _log_thread)
        eventSignal(&_log_thread->notify);
    return ret;
}

static void logQueueOverflow(_In_ LogEntry *ent)
{
    if (_log_overflow.count >= OVERFLOW_BATCH_MAXENT) {
        // if the overflow is full, we have no choice but to drop events
        if (!_log_overflow.lost) {
            devAssert(_log_overflow.tail);
            if (!_log_overflow.tail)
                return;
            // insert a message that events were lost
            LogEntry *lostent = xaAlloc(sizeof(LogEntry), XA_Zero);
            strDup(&lostent->msg, _S"One or more log entries were lost due to log queue overflow.");
            lostent->timestamp = clockWall();
            lostent->level = LOG_Warn;
            _log_overflow.tail->_next = lostent;
            _log_overflow.tail = lostent;
            ++_log_overflow.count;
            _log_overflow.lost = 1;
        }

        while (ent) {
            LogEntry *next = ent->_next;
            logDestroyEnt(ent);
            ent = next;
        }

        return;
    }

    if (_log_overflow.tail) {
        _log_overflow.tail->_next = ent;
        _log_overflow.tail = ent;
    } else {
        _log_overflow.head = ent;
        _log_overflow.tail = ent;
    }

    ++_log_overflow.count;

    // this might have itself been a batch, so chase the tail if necessary
    while (_log_overflow.tail->_next) {
        _log_overflow.tail = _log_overflow.tail->_next;
        ++_log_overflow.count;
    }
}

static bool logQueueRetryOverflow()
{
    if (!_log_overflow.head)
        return true;

    if (logQueueAddInternal(_log_overflow.head)) {
        _log_overflow.head = NULL;
        _log_overflow.tail = NULL;
        _log_overflow.count = 0;
        _log_overflow.lost = false;
        return true;
    }

    return false;
}

// this should never block
_Use_decl_annotations_
void logQueueAdd(LogEntry *ent)
{
    if (!logQueueRetryOverflow() || !logQueueAddInternal(ent))
        logQueueOverflow(ent);
}
