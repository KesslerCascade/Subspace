// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "iupsetimagedispatch.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed IupSetImageDispatch* IupSetImageDispatch_create(_In_opt_ strref iupname, Image* img)
{
    IupSetImageDispatch* self;
    self = objInstCreate(IupSetImageDispatch);

    strDup(&self->iupname, iupname);
    self->img = objAcquire(img);

    self->name = _S"IupSetImageDispatch";
    objInstInit(self);
    return self;
}

uint32 IupSetImageDispatch_run(_In_ IupSetImageDispatch* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    Ihandle* imgh = imageIupImage(self->img);
    if (!imgh)
        return TASK_Result_Failure;

    const char* name = strC(self->iupname);
    Ihandle* oldh    = IupGetHandle(name);
    if (oldh)
        IupDestroy(oldh);
    IupSetHandle(name, imgh);

    return TASK_Result_Success;
}

void IupSetImageDispatch_destroy(_In_ IupSetImageDispatch* self)
{
    // Autogen begins -----
    strDestroy(&self->iupname);
    objRelease(&self->img);
    // Autogen ends -------
}

// Autogen begins -----
#include "iupsetimagedispatch.auto.inc"
// Autogen ends -------
