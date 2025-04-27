// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "iuprefreshdispatch.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed IupRefreshDispatch* IupRefreshDispatch_create(Ihandle* hdl)
{
    IupRefreshDispatch* self;
    self = objInstCreate(IupRefreshDispatch);

    self->h = hdl;

    self->name = _S"IupRefreshDispatch";
    objInstInit(self);
    return self;
}

uint32 IupRefreshDispatch_run(_In_ IupRefreshDispatch* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    IupRefresh(self->h);
    return TASK_Result_Success;
}

// Autogen begins -----
#include "iuprefreshdispatch.auto.inc"
// Autogen ends -------
