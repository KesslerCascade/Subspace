// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "uiupdatedispatch.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/mainwin.h"

_objfactory_guaranteed UIUpdateDispatch*
UIUpdateDispatch_create(SubspaceUI* ui, _In_opt_ strref panelname)
{
    UIUpdateDispatch* self;
    self = objInstCreate(UIUpdateDispatch);

    self->ui = objAcquire(ui);
    strDup(&self->panelname, panelname);

    self->name = _S"UIUpdateDispatch";
    objInstInit(self);
    return self;
}

uint32 UIUpdateDispatch_run(_In_ UIUpdateDispatch* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                            _Inout_ TaskControl* tcon)
{
    if (strEmpty(self->panelname))
        mainwinUpdate(self->ui->main);
    else
        mainwinUpdatePanel(self->ui->main, self->panelname);

    return TASK_Result_Success;
}

void UIUpdateDispatch_destroy(_In_ UIUpdateDispatch* self)
{
    // Autogen begins -----
    objRelease(&self->ui);
    strDestroy(&self->panelname);
    // Autogen ends -------
}

// Autogen begins -----
#include "uiupdatedispatch.auto.inc"
// Autogen ends -------
