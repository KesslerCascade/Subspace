#include <cx/container.h>
#include "ui/util/iupsetobj.h"
#include "mainwin.h"

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
    return IUP_DEFAULT;
}

void MainWin_makeMenu(_In_ MainWin* self)
{
    SubspaceUI* ui = self->ui;

    Ihandle* about = IupItem(langGetC(self->ss,_S"menu_about"), NULL);
    iupSetObj(about, ObjNone, self, ui);
    IupSetCallback(about, "ACTION", menu_about);

    Ihandle* exit = IupItem(langGetC(self->ss,_S"menu_exit"), NULL);
    iupSetObj(exit, ObjNone, self, ui);
    IupSetCallback(exit, "ACTION", menu_exit);

    self->menu = IupMenu(about, exit, NULL);
}

void MainWin_showMenu(_In_ MainWin* self, int x, int y)
{
    // dynamically create a submenu for any panels that have been removed
    if (self->showpanelmenu) {
        IupUnmap(self->menu);
        IupDestroy(self->showpanelmenu);
        IupDestroy(self->showpanelsep);
        self->showpanelmenu = NULL;
        self->showpanelsep  = NULL;
    }

    Ihandle *smenu = IupMenu(NULL);

    int count = 0;
    foreach (hashtable, hti, self->panels) {
        Panel* panel = (Panel*)htiVal(object, hti);
        if (!mainwinIsPanelInLayout(self, panel->name)) {
            Ihandle* itm = IupItem(strC(panel->title), NULL);
            iupSetObj(itm, ObjNone, self, self->ui);
            IupSetAttribute(itm, "PANELNAME", strC(panel->name));
            IupAppend(smenu, itm);
            count++;
        }
    }

    if (count > 0) {
        self->showpanelmenu = IupSubmenu(langGetC(self->ss,_S"menu_show"), smenu);
        self->showpanelsep = IupSeparator();
        Ihandle* c1         = IupGetChild(self->menu, 0);
        IupInsert(self->menu, c1, self->showpanelmenu);
        IupInsert(self->menu, c1, self->showpanelsep);
    } else {
        IupDestroy(smenu);
    }

    IupPopup(self->menu, x, y);
}
