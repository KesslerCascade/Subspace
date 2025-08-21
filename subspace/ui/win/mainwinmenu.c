#include <cx/container.h>
#include "control/controlclient.h"
#include "feature/featureregistry.h"
#include "gamemgr/gameinst.h"
#include "ui/util/iupsetobj.h"
#include "ui/win/about.h"
#include "ui/win/mainwin.h"
#include "ui/win/runhistory.h"

static int menu_exit(Ihandle* ih)
{
    MainWin* win = iupGetParentObj(MainWin, ih);
    if (win) {
        mainwinOnClose(win->win);
    }

    return IUP_DEFAULT;
}

static int menu_about(Ihandle* ih)
{
    Subspace* ss = iupGetSubspace(ih);
    if (ss)
        showAbout(ss);
    return IUP_DEFAULT;
}

static int menu_runhist(Ihandle* ih)
{
    MainWin* win = iupGetParentObj(MainWin, ih);
    if (win) {
        SubspaceUI* ui = win->ss->ui;
        objRelease(&ui->runhistw);
        ui->runhistw = runhistorywinCreate(win->ss->ui);
        runhistorywinShow(ui->runhistw);
    }
    return IUP_DEFAULT;
}

static int menu_load_practice(Ihandle* ih)
{
    MainWin* win = iupGetParentObj(MainWin, ih);
    if (win) {
        Ihandle* idlg = IupFileDlg();
        IupSetAttributeHandle(idlg, "PARENTDIALOG", win->win);
        IupSetAttribute(idlg, "DIALOGTYPE", "OPEN");
        IupSetAttribute(idlg, "FILTER", "*.sav");
        IupSetStrAttribute(idlg, "FILTERINFO", langGetC(win->ss, "sav_files"));
        IupSetStrAttribute(idlg, "TITLE", langGetC(win->ss, "practice_browse_title"));

        if (strEmpty(win->lastPracticeSaveDir))
            vfsGetFSPath(&win->lastPracticeSaveDir, win->ss->fs, RUNDIR_FILENAME);

        if (!strEmpty(win->lastPracticeSaveDir)) {
            string rundirreal = 0;
            pathJoin(&rundirreal, win->lastPracticeSaveDir, _S"*.sav");
            pathToPlatform(&rundirreal, rundirreal);
            IupSetStrAttribute(idlg, "FILE", strC(rundirreal));
            strDestroy(&rundirreal);
        }

        IupPopup(idlg, IUP_CENTER, IUP_CENTER);

        if (IupGetInt(idlg, "STATUS") == 0) {
            string savepath = 0;
            const char* val = IupGetAttribute(idlg, "VALUE");
            pathFromPlatform(&savepath, (strref)val);
            pathNormalize(&savepath);
            pathParent(&win->lastPracticeSaveDir, savepath);

            GameInst* inst        = subspaceGame(win->ss);
            ControlClient* client = inst ? objAcquireFromWeak(ControlClient, inst->client) : NULL;

            if (client && vfsExist(win->ss->rootfs, savepath)) {
                ControlMsg* msg = controlNewMsg("LoadPractice", 1);
                controlMsgStr(msg, 0, "savepath", (strref)val);
                cclientQueue(client, msg);
            }

            objRelease(&client);
            objRelease(&inst);
            strDestroy(&savepath);
        }

        IupDestroy(idlg);
    }
    return IUP_DEFAULT;
}

void MainWin_makeMenu(_In_ MainWin* self)
{
    SubspaceUI* ui = self->ui;

    Ihandle* runhist = IupItem(langGetC(self->ss, "menu_runhistory"), NULL);
    iupSetObj(runhist, ObjNone, self, ui);
    IupSetCallback(runhist, "ACTION", menu_runhist);

    self->m_practicesep  = IupSeparator();
    self->m_loadpractice = IupItem(langGetC(self->ss, "practicemode_load"), NULL);
    iupSetObj(self->m_loadpractice, ObjNone, self, ui);
    IupSetAttribute(self->m_loadpractice, "VISIBLE", "NO");
    IupSetCallback(self->m_loadpractice, "ACTION", menu_load_practice);

    self->m_sep1   = IupSeparator();
    Ihandle* about = IupItem(langGetC(self->ss, "menu_about"), NULL);
    iupSetObj(about, ObjNone, self, ui);
    IupSetCallback(about, "ACTION", menu_about);

    Ihandle* exit = IupItem(langGetC(self->ss, "menu_exit"), NULL);
    iupSetObj(exit, ObjNone, self, ui);
    IupSetCallback(exit, "ACTION", menu_exit);

    self->menu = IupMenu(runhist, self->m_sep1, about, exit, NULL);
}

void MainWin_showMenu(_In_ MainWin* self, int mx, int my)
{
    int x, y, w, h;
    IupGetIntInt(self->menubtn, "SCREENPOSITION", &x, &y);
    IupGetIntInt(self->menubtn, "RASTERSIZE", &w, &h);

    // If the provided coordinates are within the button, use those.
    // Otherwise pop up the menu in the center of the button,
    // i.e. if it was activated using the keyboard.

    bool pmenabled = fregIsEnabled(self->ss->freg, _S"PracticeMode");
    if (pmenabled & !self->practiceInMenu) {
        IupUnmap(self->menu);
        IupInsert(self->menu, self->m_sep1, self->m_loadpractice);
        IupInsert(self->menu, self->m_loadpractice, self->m_practicesep);
        self->practiceInMenu = true;
    } else if (!pmenabled && self->practiceInMenu) {
        IupUnmap(self->menu);
        IupDetach(self->m_practicesep);
        IupDetach(self->m_loadpractice);
        self->practiceInMenu = false;
    }

    if (pmenabled) {
        GameInst* inst      = subspaceGame(self->ss);
        GameInstState state = GI_Init;
        if (inst)
            state = ginstGetState(inst);
        objRelease(&inst);

        bool canload = (state == GI_Menu) || (state == GI_Practice);
        IupSetAttribute(self->m_loadpractice, "ACTIVE", canload ? "YES" : "NO");
    }

    if (mx < x || mx > x + w || my < y || my > y + h) {
        mx = x + w / 2;
        my = y + h / 2;
    }
    IupPopup(self->menu, mx, my);
}

static int layoutmenu_addtab(Ihandle* ih)
{
    Ihandle* attachto  = (Ihandle*)IupGetAttribute(ih, "ATTACHTO");
    Ihandle* addbefore = (Ihandle*)IupGetAttribute(ih, "ADDBEFORE");
    strref panelname   = (strref)IupGetAttribute(ih, "PANELNAME");
    MainWin* win       = iupGetParentObj(MainWin, ih);
    if (win && attachto) {
        mainwinAddTab(win, attachto, addbefore, panelname);
    }
    return IUP_DEFAULT;
}

static int layoutmenu_removetab(Ihandle* ih)
{
    Ihandle* tabparent = (Ihandle*)IupGetAttribute(ih, "TABPARENT");
    Ihandle* toremove  = (Ihandle*)IupGetAttribute(ih, "TOREMOVE");
    MainWin* win       = iupGetParentObj(MainWin, ih);
    if (win && tabparent && toremove) {
        mainwinRemoveTab(win, tabparent, toremove);
    }
    return IUP_DEFAULT;
}

static int layoutmenu_splithoriz(Ihandle* ih)
{
    Ihandle* splitat = (Ihandle*)IupGetAttribute(ih, "SPLITAT");
    MainWin* win     = iupGetParentObj(MainWin, ih);
    if (win && splitat) {
        mainwinAddSplit(win, splitat, true);   // splitting horontally creates a vertical splitter
    }
    return IUP_DEFAULT;
}

static int layoutmenu_splitvert(Ihandle* ih)
{
    Ihandle* splitat = (Ihandle*)IupGetAttribute(ih, "SPLITAT");
    MainWin* win     = iupGetParentObj(MainWin, ih);
    if (win && splitat) {
        mainwinAddSplit(win,
                        splitat,
                        false);   // splitting vertically creates a horizontal splitter
    }
    return IUP_DEFAULT;
}

static int layoutmenu_removeph(Ihandle* ih)
{
    Ihandle* toremove = (Ihandle*)IupGetAttribute(ih, "TOREMOVE");
    MainWin* win      = iupGetParentObj(MainWin, ih);
    if (win && toremove) {
        mainwinRemovePlaceholder(win, toremove);
    }
    return IUP_DEFAULT;
}

void MainWin_showLayoutMenu(_In_ MainWin* self, Ihandle* tabparent, Ihandle* ih, int mx, int my)
{
    if (self->layoutmenu)
        IupDestroy(self->layoutmenu);
    self->layoutmenu = IupMenu(NULL);

    // tabs can be added anywhere, so start with that
    Ihandle* addtabmenu = IupMenu(NULL);

    int count = 0;
    foreach (hashtable, hti, self->panels) {
        Panel* panel = (Panel*)htiVal(object, hti);
        if (!mainwinIsPanelInLayout(self, panel->name)) {
            Ihandle* itm = IupItem(strC(panel->title), NULL);
            iupSetObj(itm, ObjNone, self, self->ui);
            if (tabparent) {
                IupSetAttribute(itm, "ATTACHTO", (char*)tabparent);
                IupSetAttribute(itm, "ADDBEFORE", (char*)ih);
            } else {
                IupSetAttribute(itm, "ATTACHTO", (char*)ih);
            }
            IupSetAttribute(itm, "PANELNAME", (char*)panel->name);
            IupSetCallback(itm, "ACTION", (Icallback)layoutmenu_addtab);
            IupAppend(addtabmenu, itm);
            count++;
        }
    }

    if (count > 0) {
        Ihandle* addtabsub = IupSubmenu(langGetC(self->ss, "layoutmenu_addtab"), addtabmenu);
        IupAppend(self->layoutmenu, addtabsub);
    } else {
        IupDestroy(addtabmenu);
    }

    if (tabparent) {
        // if this is a tab, add an option to remove it
        Ihandle* removetab = IupItem(langGetC(self->ss, "layoutmenu_removetab"), NULL);
        iupSetObj(removetab, ObjNone, self, self->ui);
        IupSetAttribute(removetab, "TABPARENT", (char*)tabparent);
        IupSetAttribute(removetab, "TOREMOVE", (char*)ih);
        IupSetCallback(removetab, "ACTION", (Icallback)layoutmenu_removetab);
        IupAppend(self->layoutmenu, removetab);
    }

    // we can always add more splits
    Ihandle* splithoriz = IupItem(langGetC(self->ss, "layoutmenu_split_horiz"), NULL);
    iupSetObj(splithoriz, ObjNone, self, self->ui);
    IupSetAttribute(splithoriz, "SPLITAT", (char*)(tabparent ? tabparent : ih));
    IupSetCallback(splithoriz, "ACTION", (Icallback)layoutmenu_splithoriz);
    IupAppend(self->layoutmenu, splithoriz);

    Ihandle* splitvert = IupItem(langGetC(self->ss, "layoutmenu_split_vert"), NULL);
    iupSetObj(splitvert, ObjNone, self, self->ui);
    IupSetAttribute(splitvert, "SPLITAT", (char*)(tabparent ? tabparent : ih));
    IupSetCallback(splitvert, "ACTION", (Icallback)layoutmenu_splitvert);
    IupAppend(self->layoutmenu, splitvert);

    if (!tabparent && ih) {
        // if this is a placeholder, add an option to remove it
        Ihandle* remove = IupItem(langGetC(self->ss, "layoutmenu_remove"), NULL);
        iupSetObj(remove, ObjNone, self, self->ui);
        IupSetAttribute(remove, "TOREMOVE", (char*)ih);
        IupSetCallback(remove, "ACTION", (Icallback)layoutmenu_removeph);
        IupAppend(self->layoutmenu, remove);
    }

    IupPopup(self->layoutmenu, mx, my);

    return;
}
