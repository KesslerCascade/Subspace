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
#include "ui/optionswin.h"
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

static int optionsbtn_action(Ihandle* ih)
{
    SubspaceUI* ui = iupGetUI(ih);
    if (!ui)
        return IUP_IGNORE;

    optionswinShow(ui->options);
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

    Ihandle* optbtn = IupFlatButton(langGetC(self->ss, _S"options"));
    IupSetAttribute(optbtn, "BGCOLOR", "85 44 92");
    IupSetAttribute(optbtn, "HLCOLOR", "139 73 131");
    IupSetAttribute(optbtn, "PSCOLOR", "48 25 52");
    IupSetAttribute(optbtn, "BORDERCOLOR", "139 73 131");
    IupSetAttribute(optbtn, "BORDERHLCOLOR", "174 101 165");
    IupSetAttribute(optbtn, "BORDERPSCOLOR", "85 44 92");
    IupSetAttribute(optbtn, "FGCOLOR", "255 255 255");
    IupSetAttribute(optbtn, "IMAGE", "IMAGE_WRENCH_SMALL");
    IupSetAttribute(optbtn, "FONT", "Helvetica, 10");
    IupSetAttribute(optbtn, "CPADDING", "4x1");
    IupSetAttribute(optbtn, "CSPACING", "3");
    IupSetAttribute(optbtn, "FOCUSFEEDBACK", "No");
    iupLoadImage(self->ss, _S"IMAGE_WRENCH_SMALL", _S"svg", _S"subspace:/wrench-small.svg", optbtn);
    iupSetObj(optbtn, ObjNone, self, self->ui);
    IupSetCallback(optbtn, "FLAT_ACTION", optionsbtn_action);

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
    IupAppend(multibox, optbtn);
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
