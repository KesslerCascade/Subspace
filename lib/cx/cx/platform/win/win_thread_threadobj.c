// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "win_thread_threadobj.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed WinThread *WinThread_create()
{
    WinThread *self;
    self = objInstCreate(WinThread);

    objInstInit(self);

    return self;
}

void WinThread_destroy(_In_ WinThread* self)
{
     CloseHandle(self->handle);
}

// Autogen begins -----
#include "win_thread_threadobj.auto.inc"
// Autogen ends -------
