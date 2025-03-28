// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqrmanual.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

_objfactory_guaranteed TQManualRunner* TQManualRunner_create()
{
    TQManualRunner* self;
    self = objInstCreate(TQManualRunner);
    objInstInit(self);
    return self;
}

int64 TQManualRunner_tick(_In_ TQManualRunner* self)
{
    if (self->worker)
        return tqmanualworkerTick(self->worker, self->tq);

    return timeForever;
}

void TQManualRunner_destroy(_In_ TQManualRunner* self)
{
    // Autogen begins -----
    objRelease(&self->worker);
    // Autogen ends -------
}

extern bool TQRunner_start(_In_ TQRunner* self, _In_ TaskQueue* tq);   // parent
#define parent_start(tq) TQRunner_start((TQRunner*)(self), tq)
bool TQManualRunner_start(_In_ TQManualRunner* self, _In_ TaskQueue* tq)
{
    if (!parent_start(tq))
        return false;

    self->worker = tqmanualworkerCreate();
    return true;
}

extern bool TQRunner_stop(_In_ TQRunner* self);   // parent
#define parent_stop() TQRunner_stop((TQRunner*)(self))
bool TQManualRunner_stop(_In_ TQManualRunner* self)
{
    objRelease(&self->worker);
    return parent_stop();
}

// Autogen begins -----
#include "tqrmanual.auto.inc"
// Autogen ends -------
