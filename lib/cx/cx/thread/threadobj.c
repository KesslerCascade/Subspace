// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "threadobj.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "thread_private.h"

_objfactory_guaranteed Thread* Thread_create(threadFunc func, _In_opt_ strref name, int n, stvar args[], bool ui)
{
    Thread *self;
    self = _thrPlatformCreate();
    //self = objInstCreate(Thread);

    strDup(&self->name, name);
    self->entry = func;
    saInit(&self->_argsa, stvar, 1);
    for (int i = 0; i < n; i++) {
        saPush(&self->_argsa, stvar, args[i]);
    }

    eventInit(&self->notify, ui ? EV_UIEvent: 0);

    objInstInit(self);

    return self;
}

_objinit_guaranteed bool Thread_init(_In_ Thread* self)
{
    stvlInitSA(&self->args, self->_argsa);

    // Autogen begins -----
    return true;
    // Autogen ends -------
}

void Thread_destroy(_In_ Thread* self)
{
    // Autogen begins -----
    strDestroy(&self->name);
    saDestroy(&self->_argsa);
    eventDestroy(&self->notify);
    // Autogen ends -------
}

// Autogen begins -----
#include "threadobj.auto.inc"
// Autogen ends -------
