// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqmonitor.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

void TQMonitor_destroy(_In_ TQMonitor* self)
{
    // Autogen begins -----
    objRelease(&self->tq);
    // Autogen ends -------
}

// Autogen begins -----
#include "tqmonitor.auto.inc"
// Autogen ends -------
