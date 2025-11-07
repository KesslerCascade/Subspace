// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "hook/moduleinfo.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objinit_guaranteed bool ModuleInfo_init(_In_ ModuleInfo* self)
{
    // Autogen begins -----
    htInit(&self->exporthash, string, uintptr, 16);
    htInit(&self->importhash, string, hashtable, 16, HT_CaseInsensitive);
    htInit(&self->relcallhash, uintptr, sarray, 16);
    htInit(&self->stringhash, string, sarray, 16);
    htInit(&self->stringlochash, uintptr, ptr, 16);
    htInit(&self->stringrefhash, uintptr, sarray, 16);
    htInit(&self->relochash, uintptr, uintptr, 16);
    htInit(&self->ptrhash, uintptr, sarray, 16);
    htInit(&self->ptrrefhash, uintptr, sarray, 16);
    htInit(&self->funccallhash, uintptr, sarray, 16);
    saInit(&self->funclist, uintptr, 1, SA_Sorted);
    return true;
    // Autogen ends -------
}

void ModuleInfo_destroy(_In_ ModuleInfo* self)
{
    // Autogen begins -----
    htDestroy(&self->exporthash);
    htDestroy(&self->importhash);
    htDestroy(&self->relcallhash);
    htDestroy(&self->stringhash);
    htDestroy(&self->stringlochash);
    htDestroy(&self->stringrefhash);
    htDestroy(&self->relochash);
    htDestroy(&self->ptrhash);
    htDestroy(&self->ptrrefhash);
    htDestroy(&self->funccallhash);
    saDestroy(&self->funclist);
    // Autogen ends -------
}

_objfactory_guaranteed ModuleInfo* ModuleInfo_create()
{
    ModuleInfo* self;
    self = objInstCreate(ModuleInfo);

    objInstInit(self);
    return self;
}

// Autogen begins -----
// clang-format off
#include "hook/moduleinfo.auto.inc"
// clang-format on
// Autogen ends -------
