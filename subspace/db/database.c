// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "database.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed Database* Database_create(Subspace* ss)
{
    Database* self;
    self = objInstCreate(Database);

    // Insert any pre-initialization object construction here

    objInstInit(self);

    // Insert any post-initialization object construction here

    return self;
}

bool Database_open(_In_ Database* self)
{
    return false;
}

void Database_close(_In_ Database* self)
{
    return;
}

void Database_destroy(_In_ Database* self)
{
    // Autogen begins -----
    objRelease(&self->fifo);
    // Autogen ends -------
}

// Autogen begins -----
#include "database.auto.inc"
// Autogen ends -------
