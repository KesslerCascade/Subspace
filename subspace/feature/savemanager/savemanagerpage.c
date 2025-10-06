// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/savemanager/savemanagerpage.h"
// clang-format on
// ==================== Auto-generated section ends ======================

#include "feature/savemanager/savemanager.h"

_objfactory_guaranteed SaveManagerPage* SaveManagerPage_create(SaveManager* feature, SubspaceUI* ui)
{
    SaveManagerPage* self;
    self = objInstCreate(SaveManagerPage);

    self->feature = feature;
    self->owner   = objGetWeak(SubspaceFeature, feature);
    self->ui      = ui;
    self->ss      = ui->ss;

    self->name = _S"savemanager";
    strDup(&self->title, langGet(self->ss, _S"feature_savemanager"));
    strDup(&self->imgname, _S"IMAGE_SAVEMANAGER");

    objInstInit(self);
    return self;
}

static int savenametextchange(Ihandle* ih, int c, char* new_value)
{
    SaveManagerPage* sp = iupGetParentObj(SaveManagerPage, ih);
    if (!sp)
        return IUP_IGNORE;

    string savename = 0;
    strCopy(&savename, (strref)new_value);
    if (strEmpty(savename))
        strDup(&savename, SAVENAME_DEFAULT);

    ssdSet(sp->feature->settings, _S"savename", true, stvar(string, savename));
    strDestroy(&savename);

    return IUP_DEFAULT;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool SaveManagerPage_make(_In_ SaveManagerPage* self, Ihandle* list)
{
    Ihandle* savenamelabel = IupLabel(langGetC(self->ss, "settings_savenamefmt"));

        IupSetAttribute(savenamelabel, "FONT", "Helvetica, Bold 10");
    Ihandle* savenamelonglabel = IupLabel(langGetC(self->ss, "settings_savenamefmt_long"));
    IupSetAttribute(savenamelonglabel, "SIZE", "1x22");
    IupSetAttribute(savenamelonglabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(savenamelonglabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(savenamelonglabel, "WORDWRAP", "YES");

    self->savenametext = IupText(NULL);
    IupSetAttribute(self->savenametext, "EXPAND", "HORIZONTAL");
    iupSetObj(self->savenametext, ObjNone, self, self->ui);
    IupSetCallback(self->savenametext, "ACTION", (Icallback)savenametextchange);

    Ihandle* savenamevarlistlabel = IupLabel(langGetC(self->ss, "settings_savenamevar_list"));
    IupSetAttribute(savenamevarlistlabel, "SIZE", "1x24");
    IupSetAttribute(savenamevarlistlabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(savenamevarlistlabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(savenamevarlistlabel, "WORDWRAP", "YES");

    Ihandle* vbox = IupVbox(savenamelabel,
                            savenamelonglabel,
                            self->savenametext,
                            savenamevarlistlabel,
                            IupFill(),
                            NULL);
    IupSetAttribute(vbox, "CMARGIN", "6x6");
    IupSetAttribute(vbox, "CGAP", "4");

    self->h = IupBackgroundBox(vbox);

    iupLoadImage(self->ss, _S"IMAGE_SAVEMANAGER", _S"svg", _S"subspace:/save.svg", list);

    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool SaveManagerPage_update(_In_ SaveManagerPage* self)
{
    string temp = 0;
    ssdStringOutD(self->feature->settings, _S"savename", &temp, SAVENAME_DEFAULT);
    IupSetStrAttribute(self->savenametext, "VALUE", strC(temp));

    strDestroy(&temp);
    return true;}

// Autogen begins -----
// clang-format off
#include "feature/savemanager/savemanagerpage.auto.inc"
// clang-format on
// Autogen ends -------
