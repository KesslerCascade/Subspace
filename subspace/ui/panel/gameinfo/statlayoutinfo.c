// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ui/panel/gameinfo/statlayoutinfo.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>

_objfactory_guaranteed StatLayoutInfo* StatLayoutInfo_create(_In_opt_ strref title, _In_opt_ strref value, bool span)
{
    StatLayoutInfo* self;
    self = objInstCreate(StatLayoutInfo);

    strDup(&self->title, title);
    strDup(&self->value, value);
    self->span = span;

    objInstInit(self);

    return self;
}

void StatLayoutInfo_update(_In_ StatLayoutInfo* self, Ihandle* mtx)
{
    string temp = 0;
    if (self->span) {
        strFormat(&temp, _S"${int}:${int}", stvar(int32, self->lin), stvar(int32, 4));
        IupSetAttributeId2(mtx, "MERGE", self->lin, 2, strC(temp));
    } else {
        // try to split any previously merged cells
        strFormat(&temp, _S"${int}:${int}", stvar(int32, self->lin), stvar(int32, self->col + 1));
        IupSetAttribute(mtx, "MERGESPLIT", strC(temp));
    }
        strDestroy(&temp);

    IupSetStrAttributeId2(mtx, "", self->lin, self->col, strC(self->title));
    IupSetStrAttributeId2(mtx, "", self->lin, self->col + 1, strC(self->value));

    return;
}

void StatLayoutInfo_destroy(_In_ StatLayoutInfo* self)
{
    // Autogen begins -----
    strDestroy(&self->title);
    strDestroy(&self->value);
    // Autogen ends -------
}

// Autogen begins -----
#include "ui/panel/gameinfo/statlayoutinfo.auto.inc"
// Autogen ends -------
