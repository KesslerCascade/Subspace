// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "fmttestobj.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed FmtTestClass* FmtTestClass_create(int32 ival, string sval)
{
    FmtTestClass *ret;
    ret = objInstCreate(FmtTestClass);

    ret->iv = ival;
    strDup(&ret->sv, sval);

    objInstInit(ret);
    return ret;
}

bool FmtTestClass_format(_In_ FmtTestClass* self, FMTVar* v, string* out)
{
    string snum;
    switch (self->iv) {
    case 1:
        snum = _S"One";
        break;
    case 2:
        snum = _S"Two";
        break;
    case 3:
        snum = _S"Three";
        break;
    case 4:
        snum = _S"Four";
        break;
    case 5:
        snum = _S"Five";
        break;
    default:
        return false;
    }

    strNConcat(out, _S"Object(", self->sv, _S":", snum, _S")");

    return true;
}

void FmtTestClass_destroy(_In_ FmtTestClass* self)
{
    // Autogen begins -----
    strDestroy(&self->sv);
    // Autogen ends -------
}

_objfactory_guaranteed FmtTestClass2* FmtTestClass2_create(int32 ival, string sval)
{
    FmtTestClass2 *self;
    self = objInstCreate(FmtTestClass2);

    self->iv = ival;
    strDup(&self->sv, sval);

    objInstInit(self);
    return self;
}

bool FmtTestClass2_convert(_In_ FmtTestClass2* self, stype st, stgeneric* dest, uint32 flags)
{
    if (!stEq(st, stType(string)))
        return false;

    string snum;
    switch (self->iv) {
    case 1:
        snum = _S"One";
        break;
    case 2:
        snum = _S"Two";
        break;
    case 3:
        snum = _S"Three";
        break;
    case 4:
        snum = _S"Four";
        break;
    case 5:
        snum = _S"Five";
        break;
    default:
        return false;
    }

    strNConcat(&dest->st_string, _S"Object(", self->sv, _S":", snum, _S")");
    return true;
}

void FmtTestClass2_destroy(_In_ FmtTestClass2* self)
{
    // Autogen begins -----
    strDestroy(&self->sv);
    // Autogen ends -------
}

// Autogen begins -----
#include "fmttestobj.auto.inc"
// Autogen ends -------
