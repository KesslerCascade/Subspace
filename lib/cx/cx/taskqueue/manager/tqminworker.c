// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqminworker.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

_objfactory_guaranteed TQInWorkerManager* TQInWorkerManager_create()
{
    TQInWorkerManager* self;
    self = objInstCreate(TQInWorkerManager);
    objInstInit(self);
    return self;
}

_objinit_guaranteed bool TQInWorkerManager_init(_In_ TQInWorkerManager* self)
{
    self->needsWorkerTick = true;
    // Autogen begins -----
    mutexInit(&self->mgrlock);
    return true;
    // Autogen ends -------
}

extern void TQManager_notify(_In_ TQManager* self, bool wakeup);   // parent
#define parent_notify(wakeup) TQManager_notify((TQManager*)(self), wakeup)
void TQInWorkerManager_notify(_In_ TQInWorkerManager* self, bool wakeup)
{
    if (self->tq && !atomicLoad(bool, &self->needrun, Relaxed)) {
        atomicStore(bool, &self->needrun, true, Relaxed);
        if (wakeup)
            eventSignal(&self->tq->workev);
    }
}

extern int64 TQThreadPoolManager_tick(_In_ TQThreadPoolManager* self);   // parent
#define parent_tick() TQThreadPoolManager_tick((TQThreadPoolManager*)(self))
int64 TQInWorkerManager_tick(_In_ TQInWorkerManager* self)
{
    int64 ret = MAX_MANAGER_INTERVAL;

    // This will be called by many worker threads, so only process if it the mutex can be acquired.
    if (mutexTryAcquire(&self->mgrlock)) {
        if (self->tq && self->runner) {
            atomicStore(bool, &self->needrun, false, Relaxed);
            ret = parent_tick();
        }
        mutexRelease(&self->mgrlock);
    }

    // if another thread needed the manager in the meantime, tell the caller to run it again
    if (atomicLoad(bool, &self->needrun, Relaxed))
         return 0;

    return ret;
}

extern void TQManager_pretask(_In_ TQManager* self);   // parent
#define parent_pretask() TQManager_pretask((TQManager*)(self))
void TQInWorkerManager_pretask(_In_ TQInWorkerManager* self)
{
    if (mutexTryAcquire(&self->mgrlock)) {
        tqthreadpoolmanagerUpdatePoolSize(self);
        mutexRelease(&self->mgrlock);
    }
}

extern bool TQThreadPoolManager_stop(_In_ TQThreadPoolManager* self);   // parent
#define parent_stop() TQThreadPoolManager_stop((TQThreadPoolManager*)(self))
bool TQInWorkerManager_stop(_In_ TQInWorkerManager* self)
{
    bool ret = false;

    // ensure that no worker is running in the manager while we stop it,
    // so that the runner reference isn't pulled out from under it
    withMutex (&self->mgrlock) {
        ret = parent_stop();
    }

    return ret;
}

void TQInWorkerManager_destroy(_In_ TQInWorkerManager* self)
{
    // Autogen begins -----
    mutexDestroy(&self->mgrlock);
    // Autogen ends -------
}

// Autogen begins -----
#include "tqminworker.auto.inc"
// Autogen ends -------
