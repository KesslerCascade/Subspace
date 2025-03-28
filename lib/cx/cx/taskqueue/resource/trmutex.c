// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "trmutex.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue/taskqueue.h>

void TRMutex_wakeup(_In_ TRMutex* self)
{
    // Get a task from the waitlist (hashtables retain insertion order)
    // and advance it so it can try to acquire the mutex.
    withMutex (&self->_wlmtx) {
        htiter hti;
        htiInit(&hti, self->_waitlist);
        while(htiValid(&hti)) {
            if (ctaskAdvance((ComplexTask*)htiKey(object, hti)))
                break;
            htiNext(&hti);
        }
        htiFinish(&hti);
    }
}

bool TRMutex_registerTask(_In_ TRMutex* self, ComplexTask* task)
{
    htelem ret = 0;

    withMutex(&self->_wlmtx) {
        ret = htInsert(&self->_waitlist, object, task, none, NULL);
    }

    return ret != 0;
}

bool TRMutex_canAcquire(_In_ TRMutex* self, ComplexTask* task)
{
    return true;
}

bool TRMutex_tryAcquire(_In_ TRMutex* self, ComplexTask* task)
{
    bool ret = mutexTryAcquire(&self->mtx);

    if (ret) {
        withMutex(&self->_wlmtx) {
            // only remove from waitlist if the task successfully acquires the mutex
            htRemove(&self->_waitlist, object, task);
        }
    }

    return ret;
}

void TRMutex_release(_In_ TRMutex* self, ComplexTask* task)
{
    mutexRelease(&self->mtx);
    TRMutex_wakeup(self);
}

_objinit_guaranteed bool TRMutex_init(_In_ TRMutex* self)
{
    // Autogen begins -----
    mutexInit(&self->mtx);
    mutexInit(&self->_wlmtx);
    htInit(&self->_waitlist, object, none, 16);
    return true;
    // Autogen ends -------
}

void TRMutex_destroy(_In_ TRMutex* self)
{
    // Autogen begins -----
    mutexDestroy(&self->mtx);
    mutexDestroy(&self->_wlmtx);
    htDestroy(&self->_waitlist);
    // Autogen ends -------
}

_objfactory_guaranteed TRMutex* TRMutex_create()
{
    TRMutex* self;
    self = objInstCreate(TRMutex);
    objInstInit(self);
    return self;
}

// Autogen begins -----
#include "trmutex.auto.inc"
// Autogen ends -------
