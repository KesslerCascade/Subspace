// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqmdedicated.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

_objfactory_guaranteed TQDedicatedManager* TQDedicatedManager_create()
{
    TQDedicatedManager* self;
    self = objInstCreate(TQDedicatedManager);
    objInstInit(self);
    return self;
}

_objinit_guaranteed bool TQDedicatedManager_init(_In_ TQDedicatedManager* self)
{
    // Autogen begins -----
    eventInit(&self->mgrnotify);
    return true;
    // Autogen ends -------
}

static int tqManagerThread(Thread* thr)
{
    TQDedicatedManager* self = stvlNextObj(&thr->args, TQDedicatedManager);
    TaskQueue* tq            = stvlNextObj(&thr->args, TaskQueue);
    if (!(self && tq))
        return 1;

    while (thrLoop(thr)) {
        uint32 qstate = atomicLoad(uint32, &self->tq->state, Relaxed);
        if (!(qstate == TQState_Running || qstate == TQState_Starting))
            break;

        int64 waittime = timeForever;
        if (qstate == TQState_Running)
            waittime = tqdedicatedmanagerTick(self);

        eventWaitTimeout(&self->mgrnotify, clamphigh(waittime, MAX_MANAGER_INTERVAL));
    }

    return 0;
}

extern bool TQThreadPoolManager_start(_In_ TQThreadPoolManager* self, _In_ TaskQueue* tq);   // parent
#define parent_start(tq) TQThreadPoolManager_start((TQThreadPoolManager*)(self), tq)
bool TQDedicatedManager_start(_In_ TQDedicatedManager* self, _In_ TaskQueue* tq)
{
    if (!parent_start(tq))
        return false;

    string thrname = 0;
    strNConcat(&thrname, tq->name, _S" Manager");
    self->mgrthread = thrCreate(tqManagerThread, thrname, stvar(object, self), stvar(object, tq));
    strDestroy(&thrname);

    return self->mgrthread != NULL;
}

extern bool TQThreadPoolManager_stop(_In_ TQThreadPoolManager* self);   // parent
#define parent_stop() TQThreadPoolManager_stop((TQThreadPoolManager*)(self))
bool TQDedicatedManager_stop(_In_ TQDedicatedManager* self)
{
    if (!self->mgrthread)
        return false;

    thrRequestExit(self->mgrthread);
    eventSignal(&self->mgrnotify);
    if (!thrWait(self->mgrthread, timeS(10)))
        return false;

    thrRelease(&self->mgrthread);

    return parent_stop();
}

extern void TQManager_notify(_In_ TQManager* self, bool wakeup);   // parent
#define parent_notify(wakeup) TQManager_notify((TQManager*)(self), wakeup)
void TQDedicatedManager_notify(_In_ TQDedicatedManager* self, bool wakeup)
{
    eventSignal(&self->mgrnotify);
}

void TQDedicatedManager_destroy(_In_ TQDedicatedManager* self)
{
    // Autogen begins -----
    objRelease(&self->mgrthread);
    eventDestroy(&self->mgrnotify);
    // Autogen ends -------
}

// Autogen begins -----
#include "tqmdedicated.auto.inc"
// Autogen ends -------
