// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "unix_thread_threadobj.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed UnixThread* UnixThread_create()
{
    UnixThread *self;
    self = objInstCreate(UnixThread);

    objInstInit(self);

    return self;
}

// Autogen begins -----
#include "unix_thread_threadobj.auto.inc"
// Autogen ends -------
