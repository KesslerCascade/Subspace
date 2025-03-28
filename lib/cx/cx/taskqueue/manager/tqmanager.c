// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqmanager.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

void TQManager_destroy(_In_ TQManager* self)
{
    // Autogen begins -----
    objRelease(&self->tq);
    // Autogen ends -------
}

bool TQManager_start(_In_ TQManager* self, _In_ TaskQueue* tq)
{
    self->tq = objAcquire(tq);
    return true;
}

bool TQManager_stop(_In_ TQManager* self)
{
    objRelease(&self->tq);
    return true;
}

int64 TQManager_tick(_In_ TQManager* self)
{
    return timeForever;
}

void TQManager_notify(_In_ TQManager* self, bool wakeup)
{
}

void TQManager_pretask(_In_ TQManager* self)
{
}

// Autogen begins -----
#include "tqmanager.auto.inc"
// Autogen ends -------
