// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqrunner.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

void TQRunner_destroy(_In_ TQRunner* self)
{
    // Autogen begins -----
    objRelease(&self->tq);
    // Autogen ends -------
}

bool TQRunner_start(_In_ TQRunner* self, _In_ TaskQueue* tq)
{
    // circular reference keeps the queue alive so long as the runner is running
    self->tq = objAcquire(tq);
    return true;
}

bool TQRunner_stop(_In_ TQRunner* self)
{
    objRelease(&self->tq);
    return true;
}

// Autogen begins -----
#include "tqrunner.auto.inc"
// Autogen ends -------
