#include "quitconfirm.h"
#include <iup.h>
#include "control/controlserver.h"
#include "gamemgr/gameinst.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

static bool shouldquit = false;

static int stay_action(Ihandle* ih)
{
    Ihandle* quitconfirm = IupGetAttributeHandle(ih, "QUITCONFIRMWIN");
    if (quitconfirm)
        IupHide(quitconfirm);

    shouldquit = false;

    return IUP_DEFAULT;
}

static int save_action(Ihandle* ih)
{
    Subspace* ss         = iupGetSubspace(ih);
    Ihandle* quitconfirm = IupGetAttributeHandle(ih, "QUITCONFIRMWIN");
    if (quitconfirm)
        IupHide(quitconfirm);

    // fallback for if something goes wrong
    shouldquit = false;

    GameInst* inst        = ss ? subspaceGame(ss) : NULL;
    ControlClient* client = inst ? objAcquireFromWeak(ControlClient, inst->client) : NULL;
    if (client) {
        ControlMsg* msg = controlMsgCreate(_S"SaveAndQuit");
        cclientQueue(client, msg);

        shouldquit = true;
    }
    objRelease(&inst);

    return IUP_DEFAULT;
}

static int quit_action(Ihandle* ih)
{
    Ihandle* quitconfirm = IupGetAttributeHandle(ih, "QUITCONFIRMWIN");
    if (quitconfirm)
        IupHide(quitconfirm);

    shouldquit = true;

    return IUP_DEFAULT;
}

bool confirmQuit(Subspace* ss)
{
    Ihandle* quittext = IupLabel(langGetC(ss, "quitconfirm"));

    Ihandle* spc1 = IupSpace();
    IupSetAttribute(spc1, "SIZE", "1x12");

    Ihandle* staybutton = IupButton(langGetC(ss, "quitconfirm_stay"), NULL);
    IupSetAttribute(staybutton, "CPADDING", "8x4");
    iupSetObj(staybutton, ObjNone, ObjNone, ss->ui);
    IupSetCallback(staybutton, "ACTION", (Icallback)stay_action);

    Ihandle* savebutton = IupButton(langGetC(ss, "quitconfirm_save"), NULL);
    IupSetAttribute(savebutton, "CPADDING", "8x4");
    iupSetObj(savebutton, ObjNone, ObjNone, ss->ui);
    IupSetCallback(savebutton, "ACTION", (Icallback)save_action);

    Ihandle* quitbutton = IupButton(langGetC(ss, "quitconfirm_quit"), NULL);
    IupSetAttribute(quitbutton, "CPADDING", "8x4");
    iupSetObj(quitbutton, ObjNone, ObjNone, ss->ui);
    IupSetCallback(quitbutton, "ACTION", (Icallback)quit_action);

    Ihandle* buttonhbox = IupHbox(staybutton, savebutton, quitbutton, NULL);
    IupSetAttribute(buttonhbox, "CMARGIN", "0x0");

    Ihandle* vbox = IupVbox(quittext, spc1, buttonhbox, NULL);

    IupSetAttribute(vbox, "ALIGNMENT", "ACENTER");
    IupSetAttribute(vbox, "CMARGIN", "10x10");
    IupSetAttribute(vbox, "CGAP", "3");

    Ihandle* dlg = IupDialog(vbox);
    IupSetAttribute(dlg, "PARENTDIALOG", "SUBSPACE_MAINWIN");
    IupSetAttributeHandle(staybutton, "QUITCONFIRMWIN", dlg);
    IupSetAttributeHandle(savebutton, "QUITCONFIRMWIN", dlg);
    IupSetAttributeHandle(quitbutton, "QUITCONFIRMWIN", dlg);
    IupSetStrAttribute(dlg, "TITLE", langGetC(ss, "quitconfirm_title"));
    IupSetAttribute(dlg, "ICON", "SUBSPACE_ICON");
    IupSetAttribute(dlg, "RESIZE", "NO");
    IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

    return shouldquit;
}
