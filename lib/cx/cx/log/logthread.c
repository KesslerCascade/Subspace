// absolutely NEVER debug locks in this file because lock debugging calls log*
#ifdef CX_LOCK_DEBUG
#undef CX_LOCK_DEBUG
#endif

#include "log_private.h"
#include <cx/container/foreach.h>
#include <cx/platform/os.h>

Thread *_log_thread;
static Event _log_done_event;

#define LOG_BATCH_SIZE 256

static int logthread_func(_Inout_ Thread *self)
{
    sa_LogEntry ents;
    uint32 batchid = 0;
    saInit(&ents, ptr, 16);

    while (thrLoop(self)) {
        bool empty = false;
        // grab some available log entries
        for(int i = 0; i < LOG_BATCH_SIZE; i++) {
            LogEntry *ent = (LogEntry *)prqPop(&_log_queue);
            if(ent) {
                saPush(&ents, ptr, ent);
            } else {
                empty = true;
                break;              // queue empty
            }
        }

        // now that we have a bunch of log entries and batches, process them
        mutexAcquire(&_log_op_lock);
        foreach(sarray, ent__idx, LogEntry*, ent, ents) {
            ++batchid;
            while (ent) {
                LogEntry *next = ent->_next;

                // verify that this log entry is using a category that was registered to the log system
                if (ent->cat == LogDefault || htHasKey(_log_categories, ptr, ent->cat)) {
                    // send it to all relevant destinations
                    foreach (sarray, dest_idx, LogDest*, dest, _log_dests) {
                        if (ent->level <= dest->maxlevel &&
                            applyCatFilter(dest->catfilter, ent->cat)) {
                            // dispatch to log destination
                            dest->func(ent->level,
                                       ent->cat,
                                       ent->timestamp,
                                       ent->msg,
                                       batchid,
                                       dest->userdata);
                        }
                    }
                }
                logDestroyEnt(ent);
                ent = next;         // process the rest of the batch
            }
        }
        mutexRelease(&_log_op_lock);

        saClear(&ents);

        if(empty) {
            prqCollect(&_log_queue);            // run GC before event signal so it's not running during shutdown
            eventSignal(&_log_done_event);
            eventWait(&self->notify);
        }
    }

    saDestroy(&ents);

    return 0;
}

void logThreadCreate(void)
{
    devAssert(!_log_thread);
    eventInit(&_log_done_event);
    _log_thread = thrCreate(logthread_func, _S"CX Log Writer", stvNone);
    if (!_log_thread)
        relFatalError("Failed to create log thread");
    thrRegisterSysThread(_log_thread);
}

void logFlush(void)
{
    if (!_log_thread)
        return;

    eventReset(&_log_done_event);
    eventSignal(&_log_thread->notify);
    eventWait(&_log_done_event);

    // signal the thread twice because the event above may be from a partially-complete run
    // that was already processing when this function was called

    eventReset(&_log_done_event);
    eventSignal(&_log_thread->notify);
    eventWait(&_log_done_event);
}
