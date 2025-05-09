// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "welcomepanel.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/settingswin.h"
#include "ui/subspaceui.h"
#include "ui/util/iuploadimage.h"
#include "ui/util/iupsetobj.h"

saDeclarePtr(Ihandle);

_objfactory_guaranteed WelcomePanel* WelcomePanel_create(SubspaceUI* ui)
{
    WelcomePanel* self;
    self = objInstCreate(WelcomePanel);
    self->ss = ui->ss;
    self->ui   = ui;
    self->name = _S"welcome";
    objInstInit(self);
    return self;
}

static int settingsbtn_action(Ihandle* ih)
{
    SubspaceUI* ui = iupGetUI(ih);
    if (!ui)
        return IUP_IGNORE;

    settingswinShow(ui->settingsw);
    return IUP_DEFAULT;
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool WelcomePanel_make(_In_ WelcomePanel* self)
{
    sa_Ihandle welcometxt;
    sa_Ihandle welcomefirst;
    sa_Ihandle welcomesubheader;
    sa_Ihandle welcomeopt1;
    sa_Ihandle welcomeopt2;

    sa_string words;
    saInit(&words, string, 16);
    saInit(&welcometxt, ptr, 16);
    saInit(&welcomesubheader, ptr, 16);
    saInit(&welcomefirst, ptr, 16);
    saInit(&welcomeopt1, ptr, 8);
    saInit(&welcomeopt2, ptr, 16);

    strSplit(&words, langGet(self->ss, _S"welcome"), _S" ", false);
    for (int i = 0; i < saSize(words); i++) {
        Ihandle* lbl = IupFlatLabel(strC(words.a[i]));
        IupSetAttribute(lbl, "FONT", "Helvetica, Bold 14");
        IupSetAttribute(lbl, "FGCOLOR", "255 255 255");
        if (i == saSize(words) - 1)
            IupSetAttribute(lbl, "LINEBREAK", "Yes");
        saPush(&welcometxt, ptr, lbl);
    }

    strSplit(&words, langGet(self->ss, _S"welcomesubheader"), _S" ", false);
    for (int i = 0; i < saSize(words); i++) {
        Ihandle* lbl = IupFlatLabel(strC(words.a[i]));
        IupSetAttribute(lbl, "FONT", "Helvetica, 10");
        IupSetAttribute(lbl, "FGCOLOR", "255 255 255");
        if (i == saSize(words) - 1)
            IupSetAttribute(lbl, "LINEBREAK", "Yes");
        saPush(&welcomesubheader, ptr, lbl);
    }

    strSplit(&words, langGet(self->ss, _S"welcomefirst"), _S" ", false);
    for (int i = 0; i < saSize(words); i++) {
        Ihandle* lbl = IupFlatLabel(strC(words.a[i]));
        IupSetAttribute(lbl, "FONT", "Helvetica, 12");
        IupSetAttribute(lbl, "FGCOLOR", "255 255 255");
        if (i == saSize(words) - 1)
            IupSetAttribute(lbl, "LINEBREAK", "Yes");
        saPush(&welcomefirst, ptr, lbl);
    }

    strSplit(&words, langGet(self->ss, _S"welcomeopt1"), _S" ", false);
    for (int i = 0; i < saSize(words); i++) {
        Ihandle* lbl = IupFlatLabel(strC(words.a[i]));
        IupSetAttribute(lbl, "FONT", "Helvetica, 12");
        IupSetAttribute(lbl, "FGCOLOR", "255 255 255");
        saPush(&welcomeopt1, ptr, lbl);
    }
    strSplit(&words, langGet(self->ss, _S"welcomeopt2"), _S" ", false);
    for (int i = 0; i < saSize(words); i++) {
        Ihandle* lbl = IupFlatLabel(strC(words.a[i]));
        IupSetAttribute(lbl, "FONT", "Helvetica, 12");
        IupSetAttribute(lbl, "FGCOLOR", "255 255 255");
        saPush(&welcomeopt2, ptr, lbl);
    }

    Ihandle* setbtn = IupFlatButton(langGetC(self->ss, _S"settings"));
    IupSetAttribute(setbtn, "BGCOLOR", "85 44 92");
    IupSetAttribute(setbtn, "HLCOLOR", "139 73 131");
    IupSetAttribute(setbtn, "PSCOLOR", "48 25 52");
    IupSetAttribute(setbtn, "BORDERCOLOR", "139 73 131");
    IupSetAttribute(setbtn, "BORDERHLCOLOR", "174 101 165");
    IupSetAttribute(setbtn, "BORDERPSCOLOR", "85 44 92");
    IupSetAttribute(setbtn, "FGCOLOR", "255 255 255");
    IupSetAttribute(setbtn, "IMAGE", "IMAGE_WRENCH_SMALL");
    IupSetAttribute(setbtn, "FONT", "Helvetica, 10");
    IupSetAttribute(setbtn, "CPADDING", "4x1");
    IupSetAttribute(setbtn, "CSPACING", "3");
    IupSetAttribute(setbtn, "FOCUSFEEDBACK", "No");
    iupLoadImage(self->ss, _S"IMAGE_WRENCH_SMALL", _S"svg", _S"subspace:/wrench-small.svg", setbtn);
    iupSetObj(setbtn, ObjNone, self, self->ui);
    IupSetCallback(setbtn, "FLAT_ACTION", settingsbtn_action);

    Ihandle* multibox = IupMultiBox(NULL);
    IupSetAttribute(multibox, "CGAPHORIZ", "2");
    IupSetAttribute(multibox, "NCMARGIN", "10x10");
    for (int i = 0; i < saSize(welcometxt); i++) {
        IupAppend(multibox, welcometxt.a[i]);
    }
    for (int i = 0; i < saSize(welcomesubheader); i++) {
        IupAppend(multibox, welcomesubheader.a[i]);
    }

    Ihandle* spacer = IupSpace();
    IupSetAttribute(spacer, "SIZE", "8x10");
    IupSetAttribute(spacer, "LINEBREAK", "Yes");
    IupAppend(multibox, spacer);

    for (int i = 0; i < saSize(welcomefirst); i++) {
        IupAppend(multibox, welcomefirst.a[i]);
    }

    spacer = IupSpace();
    IupSetAttribute(spacer, "SIZE", "8x10");
    IupSetAttribute(spacer, "LINEBREAK", "Yes");
    IupAppend(multibox, spacer);
    
    for (int i = 0; i < saSize(welcomeopt1); i++) {
        IupAppend(multibox, welcomeopt1.a[i]);
    }
    IupAppend(multibox, setbtn);
    for (int i = 0; i < saSize(welcomeopt2); i++) {
        IupAppend(multibox, welcomeopt2.a[i]);
    }
    self->h = IupBackgroundBox(multibox);
    IupSetAttribute(self->h, "BGCOLOR", panelbg);

    saDestroy(&welcometxt);
    saDestroy(&welcomeopt1);
    saDestroy(&welcomeopt2);
    saDestroy(&words);

    return true;
}

// Autogen begins -----
#include "welcomepanel.auto.inc"
// Autogen ends -------
