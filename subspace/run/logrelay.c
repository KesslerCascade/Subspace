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
#include <cx/utils/murmur.h>
#include "ui/subspaceui.h"
#include "runinfo.h"

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
    htInit(&self->subscribers, object, none, 16);
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

static bool _LogRelay_subscribe(_In_ LogRelay* self, ObjInst* subscriber, _In_opt_ strref id,
                                bool ui)
{
    // check if the subscriber implements the proper interface
    if (!objInstIf(subscriber, LogSubscriber))
        return false;

    withWriteLock (&self->lock) {
        htelem elem = htFind(self->logsubs, strref, id, none, NULL);
        if (!elem) {
            sa_SubscriberInfo newlist;
            saInit(&newlist, object, 1);
            elem = htInsertC(&self->logsubs, strref, id, sarray, &newlist);
        }
        sa_SubscriberInfo* list = (sa_SubscriberInfo*)hteValPtr(self->logsubs, sarray, elem);

        SubscriberInfo* si = subscriberinfoCreate(subscriber, ui);
        saPush(list, object, si);

        // insert into set of subscribers (they might be subscribed to more than one log ID)
        htInsert(&self->subscribers, object, si, none, NULL);

        objRelease(&si);
    }

    return true;
}

bool LogRelay_subscribe(_In_ LogRelay* self, ObjInst* subscriber, _In_opt_ strref id)
{
    return _LogRelay_subscribe(self, subscriber, id, false);
}

bool LogRelay_subscribeUI(_In_ LogRelay* self, ObjInst* subscriber, _In_opt_ strref id)
{
    return _LogRelay_subscribe(self, subscriber, id, true);
}

bool LogRelay_send(_In_ LogRelay* self, LogEnt* ent, bool replay)
{
    bool ret = false;

    sa_SubscriberInfo removed;
    saInit(&removed, object, 1);

    withReadLock (&self->lock) {
        htelem elem = htFind(self->logsubs, strref, ent->id, none, NULL);
        if (elem) {
            sa_SubscriberInfo* list = (sa_SubscriberInfo*)hteValPtr(self->logsubs, sarray, elem);

            // dispatch the events in separate threads
            foreach (sarray, idx, SubscriberInfo*, sub, *list) {
                ObjInst* subscriber = objAcquireFromWeak(ObjInst, sub->subscriber);
                if (subscriber) {
                    LogDispatch* ndisp = logdispatchCreate(subscriber, ent, replay);

                    // If we're replaying, can't depend on temporal separation of events. Put them
                    // in a FIFO to ensure they're processed in order.
                    if (replay)
                        ctaskRequireResource(ndisp, self->replayqueue);

                    // put it in the correct queue based on of it's a UI subscriber or not
                    if (sub->ui)
                        tqRun(self->ss->ui->uiq, &ndisp);
                    else
                        tqRun(self->ss->workq, &ndisp);
                    ret = true;

                    objRelease(&subscriber);
                } else {
                    // remember subscribers that no longer exist to remove them
                    saPush(&removed, object, sub);
                }
            }
        }
    }

    // clean up any subscribers that don't exist anymore
    if (saSize(removed) > 0) {
        foreach (sarray, idx, SubscriberInfo*, sub, removed) {
            withWriteLock (&self->lock) {
                htelem elem = htFind(self->logsubs, strref, ent->id, none, NULL);
                if (elem) {
                    sa_SubscriberInfo* list = (sa_SubscriberInfo*)
                        hteValPtr(self->logsubs, sarray, elem);

                    saFindRemove(list, object, sub);
                    htRemove(&self->subscribers, object, sub);
                }
            }
        }
    }
    saDestroy(&removed);

    return ret;
}

void LogRelay_reset(_In_ LogRelay* self)
{
    withReadLock (&self->lock) {
        foreach (hashtable, iter, self->subscribers) {
            SubscriberInfo* sub = (SubscriberInfo*)htiKey(object, iter);

            // tell all subscribers to reset their info; either for a fresh run or a replay
            ObjInst* subscriber = objAcquireFromWeak(ObjInst, sub->subscriber);
            if (subscriber) {
                LogDispatch* ndisp = logdispatchCreateReplay(subscriber, true, false);
                ctaskRequireResource(ndisp, self->replayqueue);

                // put it in the correct queue based on of it's a UI subscriber or not
                if (sub->ui)
                    tqRun(self->ss->ui->uiq, &ndisp);
                else
                    tqRun(self->ss->workq, &ndisp);
                objRelease(&subscriber);
            }
        }
    }
}

void LogRelay_replayComplete(_In_ LogRelay* self)
{
    withReadLock (&self->lock) {
        foreach (hashtable, iter, self->subscribers) {
            SubscriberInfo* sub = (SubscriberInfo*)htiKey(object, iter);

            // tell all subscribers to reset their info; either for a fresh run or a replay
            ObjInst* subscriber = objAcquireFromWeak(ObjInst, sub->subscriber);
            if (subscriber) {
                LogDispatch* ndisp = logdispatchCreateReplay(subscriber, false, true);
                ctaskRequireResource(ndisp, self->replayqueue);

                // put it in the correct queue based on of it's a UI subscriber or not
                if (sub->ui)
                    tqRun(self->ss->ui->uiq, &ndisp);
                else
                    tqRun(self->ss->workq, &ndisp);
                objRelease(&subscriber);
            }
        }
    }
}

// -------- Log Dispatcher

_objfactory_guaranteed LogDispatch*
LogDispatch_create(ObjInst* subscriber, LogEnt* ent, bool replay)
{
    LogDispatch* self;
    self = objInstCreate(LogDispatch);

    self->subscriber = objAcquire(subscriber);
    self->ent        = objAcquire(ent);
    self->replay     = replay;

    objInstInit(self);
    return self;
}

_objfactory_guaranteed LogDispatch*
LogDispatch_createReplay(ObjInst* subscriber, bool reset, bool complete)
{
    LogDispatch* self;
    self = objInstCreate(LogDispatch);

    self->subscriber = objAcquire(subscriber);
    self->replay     = true;   // implied
    self->reset      = reset;
    self->complete   = complete;

    objInstInit(self);
    return self;
}

uint32 LogDispatch_run(_In_ LogDispatch* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                       _Inout_ TaskControl* tcon)
{
    LogSubscriber* subif = objInstIf(self->subscriber, LogSubscriber);
    if (!subif)
        return TASK_Result_Failure;

    if (!self->reset && !self->complete)
        subif->logNotify(self->subscriber, self->ent, self->replay);
    else if (self->reset)
        subif->logReset(self->subscriber);
    else if (self->complete)
        subif->logReplayComplete(self->subscriber);

    return TASK_Result_Success;
}

void LogDispatch_destroy(_In_ LogDispatch* self)
{
    // Autogen begins -----
    objRelease(&self->subscriber);
    objRelease(&self->ent);
    // Autogen ends -------
}

_objfactory_guaranteed SubscriberInfo* SubscriberInfo_create(ObjInst* subscriber, bool ui)
{
    SubscriberInfo* self;
    self = objInstCreate(SubscriberInfo);

    self->subscriber = objGetWeak(ObjInst, subscriber);
    self->ui         = ui;

    objInstInit(self);
    return self;
}

intptr SubscriberInfo_cmp(_In_ SubscriberInfo* self, SubscriberInfo* other, uint32 flags)
{
    devAssert(objClsInfo(self) == objClsInfo(other));
    return self->subscriber - other->subscriber;
}

void SubscriberInfo_destroy(_In_ SubscriberInfo* self)
{
    // Autogen begins -----
    objDestroyWeak(&self->subscriber);
    // Autogen ends -------
}

uint32 SubscriberInfo_hash(_In_ SubscriberInfo* self, uint32 flags)
{
    return hashMurmur3((uint8*)&self->subscriber, sizeof(void*));
}

// Autogen begins -----
#include "logrelay.auto.inc"
// Autogen ends -------
