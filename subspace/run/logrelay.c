// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "logrelay.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "runinfo.h"

saDeclarePtr(ObjInst);

_objfactory_guaranteed LogRelay* LogRelay_create(Subspace* ss)
{
    LogRelay* self;
    self = objInstCreate(LogRelay);

    self->ss          = ss;
    self->replayqueue = trfifoCreate();

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool LogRelay_init(_In_ LogRelay* self)
{
    // Autogen begins -----
    rwlockInit(&self->lock);
    htInit(&self->logsubs, string, sarray, 16);
    htInit(&self->subscribers, ptr, none, 16);
    return true;
    // Autogen ends -------
}

void LogRelay_destroy(_In_ LogRelay* self)
{
    // Autogen begins -----
    objRelease(&self->replayqueue);
    rwlockDestroy(&self->lock);
    htDestroy(&self->logsubs);
    htDestroy(&self->subscribers);
    // Autogen ends -------
}

bool LogRelay_subscribe(_In_ LogRelay* self, ObjInst* subscriber, _In_opt_ strref id)
{
    // check if the subscriber implements the proper interface
    if (!objInstIf(subscriber, LogSubscriber))
        return false;

    withWriteLock (&self->lock) {
        htelem elem = htFind(self->logsubs, strref, id, none, NULL);
        if (!elem) {
            sa_ObjInst newlist;
            saInit(&newlist, object, 1);
            elem = htInsertC(&self->logsubs, strref, id, sarray, &newlist);
        }
        sa_ObjInst* list = (sa_ObjInst*)hteValPtr(self->logsubs, sarray, elem);
        saPush(list, object, subscriber);

        // insert into set of subscribers (they might be subscribed to more than one log ID)
        htInsert(&self->subscribers, ptr, subscriber, none, NULL);
    }

    return true;
}

bool LogRelay_send(_In_ LogRelay* self, LogEnt* ent, bool replay)
{
    bool ret = false;

    withReadLock (&self->lock) {
        htelem elem = htFind(self->logsubs, strref, ent->id, none, NULL);
        if (elem) {
            sa_ObjInst* list = (sa_ObjInst*)hteValPtr(self->logsubs, sarray, elem);

            // dispatch the events in separate threads
            foreach (sarray, idx, ObjInst*, sub, *list) {
                LogDispatcher* ndisp = logdispatcherCreate(sub, ent, replay);

                // If we're replaying, can't depend on temporal separation of events. Put them in a
                // FIFO to ensure they're processed in order.
                if (replay)
                    ctaskRequireResource(ndisp, self->replayqueue);

                tqRun(self->ss->workq, &ndisp);
                ret = true;
            }
        }
    }

    return ret;
}

void LogRelay_reset(_In_ LogRelay* self)
{
    withReadLock (&self->lock) {
        foreach (hashtable, iter, self->subscribers) {
            ObjInst* sub = htiKey(ptr, iter);

            // tell all subscribers to reset their info; either for a fresh run or a replay
            LogSubscriber* subif = objInstIf(sub, LogSubscriber);
            subif->logReset(sub);
        }
    }
}

// -------- Event Dispatcher

_objfactory_guaranteed LogDispatcher* LogDispatcher_create(ObjInst* subscriber, LogEnt* ent, bool replay)
{
    LogDispatcher* self;
    self = objInstCreate(LogDispatcher);

    self->subscriber = objAcquire(subscriber);
    self->ent         = objAcquire(ent);
    self->replay     = replay;

    objInstInit(self);

    return self;
}

uint32 LogDispatcher_run(_In_ LogDispatcher* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    LogSubscriber* subif = objInstIf(self->subscriber, LogSubscriber);
    if (!subif)
        return TASK_Result_Failure;
    subif->logNotify(self->subscriber, self->ent, self->replay);
    return TASK_Result_Success;
}

void LogDispatcher_destroy(_In_ LogDispatcher* self)
{
    // Autogen begins -----
    objRelease(&self->subscriber);
    objRelease(&self->ent);
    // Autogen ends -------
}

// Autogen begins -----
#include "logrelay.auto.inc"
// Autogen ends -------
