// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "optionswin.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

_objfactory_guaranteed OptionsWin* OptionsWin_create(SubspaceUI* ui)
{
    OptionsWin* self;
    self = objInstCreate(OptionsWin);

    self->ss = ui->ss;
    self->ui = ui;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool OptionsWin_init(_In_ OptionsWin* self)
{
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

bool OptionsWin_make(_In_ OptionsWin* self)
{
    self->win = IupDialog(NULL);
    string tmp = 0;
    strFormat(&tmp, _S"${int}x${int}", stvar(int32, OPTIONSWIN_WIDTH), stvar(int32, OPTIONSWIN_HEIGHT));
    IupSetAttribute(self->win, "SIZE", strC(tmp));
    IupSetAttribute(self->win, "TITLE", langGetC(self->ss, _S"options_title"));
    iupSetObj(self->win, self, ObjNone, self->ui);
    IupSetAttribute(self->win, "PARENTDIALOG", "SUBSPACE_MAINWIN");

    IupSetCallback(self->win, "CLOSE_CB", (Icallback)OptionsWin_onClose);

    strDestroy(&tmp);
    return true;
}

void OptionsWin_show(_In_ OptionsWin* self)
{
    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
    IupSetAttribute(self->win, "SIMULATEMODAL", "YES");
    char* rsz = IupGetAttribute(self->win, "RASTERSIZE");
    IupSetAttribute(self->win, "MINSIZE", rsz);
}

void OptionsWin_update(_In_ OptionsWin* self)
{
    return;
}

int OptionsWin_onClose(Ihandle* ih)
{
    IupSetAttribute(ih, "SIMULATEMODAL", "NO");
    return IUP_DEFAULT;
}

void OptionsWin_finish(_In_ OptionsWin* self)
{
    if (self->win)
        IupDestroy(self->win);
    self->win = NULL;
}

void OptionsWin_destroy(_In_ OptionsWin* self)
{
    OptionsWin_finish(self);
}

// Autogen begins -----
#include "optionswin.auto.inc"
// Autogen ends -------
