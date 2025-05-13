// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "keybind.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed KeyBind* KeyBind_create(SubspaceFeature* owner, _In_opt_ strref name)
{
    KeyBind* self;
    self = objInstCreate(KeyBind);

    self->owner = objGetWeak(SubspaceFeature, owner);
    strDup(&self->name, name);

    objInstInit(self);
    return self;
}

void KeyBind_send(_In_ KeyBind* self, ControlClient* client)
{
    ControlMsg* msg = controlNewMsg("BindKey", 2);
    controlMsgStr(msg, 0, "name", self->name);
    controlMsgInt(msg, 1, "key", self->ftlkey);
    cclientQueue(client, msg);
}

void KeyBind_destroy(_In_ KeyBind* self)
{
    // Autogen begins -----
    objDestroyWeak(&self->owner);
    strDestroy(&self->name);
    // Autogen ends -------
}

// Autogen begins -----
#include "keybind.auto.inc"
// Autogen ends -------
