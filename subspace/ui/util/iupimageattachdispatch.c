// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "iupimageattachdispatch.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed IupImageAttachDispatch*
IupImageAttachDispatch_create(Ihandle* hdl, _In_opt_ strref attr, Image* img)
{
    IupImageAttachDispatch* self;
    self = objInstCreate(IupImageAttachDispatch);

    self->h = hdl;
    strDup(&self->attr, attr);
    self->img = objAcquire(img);

    self->name = _S"IupImageAttachDispatch";
    objInstInit(self);
    return self;
}

uint32 IupImageAttachDispatch_run(_In_ IupImageAttachDispatch* self, _In_ TaskQueue* tq,
                                  _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    Ihandle* imgh = imageIupImage(self->img);
    if (!imgh)
        return TASK_Result_Failure;

    IupSetAttributeHandle(self->h, strC(self->attr), imgh);
    IupRefresh(self->h);

    return TASK_Result_Success;
}

void IupImageAttachDispatch_destroy(_In_ IupImageAttachDispatch* self)
{
    // Autogen begins -----
    strDestroy(&self->attr);
    objRelease(&self->img);
    // Autogen ends -------
}

// Autogen begins -----
#include "iupimageattachdispatch.auto.inc"
// Autogen ends -------
