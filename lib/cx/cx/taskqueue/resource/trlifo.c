// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "trlifo.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue/taskqueue.h>

bool TRLifo_registerTask(_In_ TRLifo* self, ComplexTask* task)
{
    withMutex (&self->_lifomtx) {
        // new tasks go to the end of the list
        saPush(&self->_lifo, object, task);
    }

    return true;
}

bool TRLifo_canAcquire(_In_ TRLifo* self, ComplexTask* task)
{
    bool ret = false;

    withMutex (&self->_lifomtx) {
        if (!self->cur) {
            // in order to acquire, task must be the LAST tast on the list (most recent)
            int32 sz = saSize(self->_lifo);
            if (sz > 0 && self->_lifo.a[sz - 1] == task)
                ret = true;
        }
    }

    return ret;
}

bool TRLifo_tryAcquire(_In_ TRLifo* self, ComplexTask* task)
{
    bool ret = false;

    withMutex (&self->_lifomtx) {
        if (!self->cur) {
            // in order to acquire, task must be the LAST tast on the list (most recent)

            int32 sz = saSize(self->_lifo);
            if (sz > 0 && self->_lifo.a[sz - 1] == task) {
                saRemove(&self->_lifo, sz - 1);
                self->cur = task;
                ret       = true;
            }
        } else {
            ret = (self->cur == task);
        }
    }

    return ret;
}

void TRLifo_release(_In_ TRLifo* self, ComplexTask* task)
{
    ComplexTask* release = NULL;

    withMutex (&self->_lifomtx) {
        if (self->cur == task)
            self->cur = NULL;

        if (self->cur == NULL) {
            int32 sz = saSize(self->_lifo);
            release  = sz > 0 ? self->_lifo.a[sz - 1] : NULL;
        }
    }

    if (release)
        ctaskAdvance(release);
}

_objinit_guaranteed bool TRLifo_init(_In_ TRLifo* self)
{
    // Autogen begins -----
    mutexInit(&self->_lifomtx);
    saInit(&self->_lifo, object, 1);
    return true;
    // Autogen ends -------
}

void TRLifo_destroy(_In_ TRLifo* self)
{
    // Autogen begins -----
    mutexDestroy(&self->_lifomtx);
    saDestroy(&self->_lifo);
    // Autogen ends -------
}

_objfactory_guaranteed TRLifo* TRLifo_create()
{
    TRLifo* self;
    self = objInstCreate(TRLifo);
    objInstInit(self);
    return self;
}

// Autogen begins -----
#include "trlifo.auto.inc"
// Autogen ends -------
