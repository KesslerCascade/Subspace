// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "converttestobj.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/stype/stconvert.h>

_objfactory_guaranteed ConvertTestClass* ConvertTestClass_create(int32 ival, float64 fval, string sval)
{
    ConvertTestClass *self;
    self = objInstCreate(ConvertTestClass);

    self->ival = ival;
    self->fval = fval;
    strDup(&self->sval, sval);

    objInstInit(self);
    return self;
}

bool ConvertTestClass_convert(_In_ ConvertTestClass* self, stype st, stgeneric* dest, uint32 flags)
{
    if (STYPE_CLASS(st) == STCLASS_INT || STYPE_CLASS(st) == STCLASS_UINT)
        return stConvert_int(st, dest, stCheckedArg(int32, self->ival), flags);
    if (STYPE_CLASS(st) == STCLASS_FLOAT)
        return stConvert_float64(st, dest, stCheckedArg(float64, self->fval), flags);

    if (stEq(st, stType(string))) {
        strDup(&dest->st_string, self->sval);
        return true;
    }

    return false;
}

void ConvertTestClass_destroy(_In_ ConvertTestClass* self)
{
    // Autogen begins -----
    strDestroy(&self->sval);
    // Autogen ends -------
}

// Autogen begins -----
#include "converttestobj.auto.inc"
// Autogen ends -------
