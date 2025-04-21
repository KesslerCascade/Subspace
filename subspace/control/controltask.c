// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "controltask.h"
// clang-format on
// ==================== Auto-generated section ends ======================

void ControlTask_destroy(_In_ ControlTask* self)
{
    // Autogen begins -----
    objRelease(&self->client);
    // Autogen ends -------
}

// Autogen begins -----
#include "controltask.auto.inc"
// Autogen ends -------
