// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "iupattrdispatch.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed IupAttrDispatch*
IupAttrDispatch_create(Ihandle* hdl, _In_opt_ strref attr, _In_opt_ strref val)
{
    IupAttrDispatch* self;
    self = objInstCreate(IupAttrDispatch);

    self->h = hdl;
    strDup(&self->attr, attr);
    strDup(&self->val, val);

    self->name = _S"IupAttrDispatch";
    objInstInit(self);
    return self;
}

_objfactory_guaranteed IupAttrDispatch*
IupAttrDispatch_createHandle(Ihandle* hdl, _In_opt_ strref attr, Ihandle* vhdl)
{
    IupAttrDispatch* self;
    self = objInstCreate(IupAttrDispatch);

    self->h = hdl;
    strDup(&self->attr, attr);
    self->vhdl = vhdl;

    self->name = _S"IupAttrDispatch";
    objInstInit(self);
    return self;
}

uint32 IupAttrDispatch_run(_In_ IupAttrDispatch* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                           _Inout_ TaskControl* tcon)
{
    if (self->vhdl)
        IupSetAttributeHandle(self->h, strC(self->attr), self->vhdl);
    else
        IupSetAttribute(self->h, strC(self->attr), strC(self->val));
    return TASK_Result_Success;
}

void IupAttrDispatch_destroy(_In_ IupAttrDispatch* self)
{
    // Autogen begins -----
    strDestroy(&self->attr);
    strDestroy(&self->val);
    // Autogen ends -------
}

// Autogen begins -----
#include "iupattrdispatch.auto.inc"
// Autogen ends -------
