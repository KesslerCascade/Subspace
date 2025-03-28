// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "complextask.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue.h>
#include <cx/taskqueue/queue/tqcomplex.h>
#include <cx/taskqueue/requires/taskrequirestask.h>
#include <cx/taskqueue/requires/taskrequiresgate.h>
#include <cx/taskqueue/requires/taskrequiresresource.h>
#include "cx/utils/murmur.h"

bool ComplexTask_advance(_In_ ComplexTask* self)
{
    // If the task was previously in a queue, it will keep a weak reference to it.
    ComplexTaskQueue* tq = objAcquireFromWeak(ComplexTaskQueue, self->lastq);
    bool ret             = false;

    if (tq) {
        atomicFetchAdd(uint32, &self->_advcount, 1, Relaxed);
        ret = ctaskqueueAdvance(tq, self);
        objRelease(&tq);
    }

    return ret;
}

extern bool Task_reset(_In_ Task* self);   // parent
#define parent_reset() Task_reset((Task*)(self))
bool ComplexTask_reset(_In_ ComplexTask* self)
{
    if (!parent_reset())
        return false;

    self->lastprogress = 0;
    self->nextrun      = 0;
    self->_intflags    = 0;
    objDestroyWeak(&self->lastq);
    saClear(&self->_requires);

    return true;
}

void ComplexTask_destroy(_In_ ComplexTask* self)
{
    // failsafe in case any resources were acquired in retain mode and we got deferred,
    // then failed a dependency check or something

    ctaskReleaseRequires(self, self->_requires);

    // Autogen begins -----
    objDestroyWeak(&self->lastq);
    saDestroy(&self->_requires);
    // Autogen ends -------
}

intptr ComplexTask_cmp(_In_ ComplexTask* self, ComplexTask* other, uint32 flags)
{
    intptr nrcmp = stCmp(int64, self->nextrun, other->nextrun);
    if (nrcmp != 0)
        return nrcmp;

    return stCmp(ptr, self, other);
}

uint32 ComplexTask_hash(_In_ ComplexTask* self, uint32 flags)
{
    // just hash the pointer
    return hashMurmur3((uint8*)&self, sizeof(void*));
}

uint32 ComplexTask_checkRequires(_In_ ComplexTask* self, bool updateProgress,
                                 _Out_opt_ int64* expires)
{
    int64 now        = clockTimer();
    uint32 ret       = TASK_Requires_Ok;
    bool done        = false;
    int64 minexpires = timeForever;

    for (int i = saSize(self->_requires) - 1; i >= 0 && !done; --i) {
        TaskRequires* req = self->_requires.a[i];
        uint32 state      = taskrequiresState(req, self);

        // if this is a state that is going to have to wait, check for expiration
        if ((state == TASK_Requires_Wait || state == TASK_Requires_Acquire) && req->expires > 0) {
            if (now >= req->expires) {
                // expire time has passed, fail this dependency, code below will take care of
                // removing it
                state = TASK_Requires_Fail_Permanent;

                // if we're cancelling expired requirements, do so now
                if (self->flags & TASK_Cancel_Expired)
                    taskrequiresCancel(req);
            } else {
                minexpires = min(minexpires, req->expires);
            }
        }

        switch (state) {
        case TASK_Requires_Ok:
            self->lastprogress = now;
            break;

        case TASK_Requires_Ok_Permanent:
            self->lastprogress = now;
            saRemove(&self->_requires, i);
            break;

        case TASK_Requires_Fail_Permanent:
            saRemove(&self->_requires, i);

            if (self->flags & TASK_Soft_Requires)
                self->flags |= TASK_Require_Failed;   // just flag it as failed but otherwise treat
                                                      // as permanent Ok
            else
                ret = TASK_Requires_Fail_Permanent;
            break;

        case TASK_Requires_Wait:
            if (ret != TASK_Requires_Fail_Permanent)
                ret = TASK_Requires_Wait;
            if (updateProgress) {
                int64 progress = taskrequiresProgress(req);
                if (progress > 0)
                    self->lastprogress = max(self->lastprogress, progress);
            } else {
                // we don't care about updating progress, so no need to keep checking once one that
                // isn't complete is found
                done = true;
            }
            break;

        case TASK_Requires_Acquire:
            // resources need to be acquired, but we prefer to try to do that right before the task
            // is run, but only if we're not waiting on anything else
            if (ret == TASK_Requires_Ok)
                ret = TASK_Requires_Acquire;
            break;
        }
    }

    if (expires)
        *expires = minexpires;
    return ret;
}

void ComplexTask_cancelRequires(_In_ ComplexTask* self)
{
    for (int i = saSize(self->_requires) - 1; i >= 0; --i) {
        taskrequiresCancel(self->_requires.a[i]);
    }
}

bool ComplexTask_acquireRequires(_In_ ComplexTask* self, sa_TaskRequires* acquired)
{
    for (int i = saSize(self->_requires) - 1; i >= 0; --i) {
        TaskRequires* req = self->_requires.a[i];
        uint32 state      = taskrequiresState(req, self);
        if (state == TASK_Requires_Acquire) {
            if (taskrequiresTryAcquire(req, self)) {
                saPush(acquired, object, req);
                self->_intflags |= TASK_INTERNAL_Owns_Resources;
            } else {
                return false;
            }
        } else if (state != TASK_Requires_Ok && state != TASK_Requires_Ok_Permanent) {
            // We have some other dependency that is no longer satisfied, need to go back into defer queue
            return false;
        }
    }

    return true;
}

bool ComplexTask_releaseRequires(_In_ ComplexTask* self, sa_TaskRequires resources)
{
    bool ret = true;
    for (int i = saSize(resources) - 1; i >= 0; --i) {
        TaskRequires* req = resources.a[i];
        if (!taskrequiresRelease(req, self))
            ret = false;
    }

    self->_intflags &= ~TASK_INTERNAL_Owns_Resources;

    return ret;
}

bool ComplexTask_advanceCallback(stvlist* cvars, stvlist* args)
{
    Weak(ComplexTask)* wref = NULL;
    if (!stvlNext(cvars, weakref, &wref))
        return false;

    ComplexTask* ctask = objAcquireFromWeak(ComplexTask, wref);
    if (ctask) {
        // advance the task so that it can re-check its dependencies
        ctaskAdvance(ctask);
        objRelease(&ctask);
    }

    return true;
}

void ComplexTask_require(_In_ ComplexTask* self, _In_ TaskRequires* req)
{
    taskrequiresRegisterTask(req, self);
    saPush(&self->_requires, object, req);
}

void ComplexTask_requireTask(_In_ ComplexTask* self, _In_ Task* dep, bool failok)
{
    TaskRequiresTask* trt = taskrequirestaskCreate(dep, failok);
    taskrequirestaskRegisterTask(trt, self);
    saPushC(&self->_requires, object, &trt);
}

void ComplexTask_requireTaskTimeout(_In_ ComplexTask* self, _In_ Task* dep, bool failok,
                                    int64 timeout)
{
    TaskRequiresTask* trt = taskrequirestaskCreate(dep, failok);
    trt->expires          = (timeout < timeForever) ? clockTimer() + timeout : timeForever;
    taskrequirestaskRegisterTask(trt, self);
    saPushC(&self->_requires, object, &trt);
}

void ComplexTask_requireResource(_In_ ComplexTask* self, _In_ TaskResource* res)
{
    TaskRequiresResource* trr = taskrequiresresourceCreate(res);
    taskrequiresresourceRegisterTask(trr, self);
    saPushC(&self->_requires, object, &trr);
}

void ComplexTask_requireResourceTimeout(_In_ ComplexTask* self, _In_ TaskResource* res,
                                        int64 timeout)
{
    TaskRequiresResource* trr = taskrequiresresourceCreate(res);
    trr->expires              = (timeout < timeForever) ? clockTimer() + timeout : timeForever;
    taskrequiresresourceRegisterTask(trr, self);
    saPushC(&self->_requires, object, &trr);
}

void ComplexTask_requireGate(_In_ ComplexTask* self, _In_ TRGate* gate)
{
    TaskRequiresGate* trg = taskrequiresgateCreate(gate);
    taskrequiresgateRegisterTask(trg, self);
    saPushC(&self->_requires, object, &trg);
}

void ComplexTask_requireGateTimeout(_In_ ComplexTask* self, _In_ TRGate* gate, int64 timeout)
{
    TaskRequiresGate* trg = taskrequiresgateCreate(gate);
    trg->expires          = (timeout < timeForever) ? clockTimer() + timeout : timeForever;
    taskrequiresgateRegisterTask(trg, self);
    saPushC(&self->_requires, object, &trg);
}

extern bool BasicTask_cancel(_In_ BasicTask* self);   // parent
#define parent_cancel() BasicTask_cancel((BasicTask*)(self))
bool ComplexTask_cancel(_In_ ComplexTask* self)
{
    // Advancing on cancel serves two purposes:
    // 1. If the task is deferred or scheduled, immediately allows the runCancelled() method to run
    // and fails the task.
    // 2. Removes dependencies amd cascades the cancellation if the flag is set.
    ctaskAdvance(self);

    return parent_cancel();
}

// Autogen begins -----
#include "complextask.auto.inc"
// Autogen ends -------
