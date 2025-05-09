#include <cx/container.h>
#include "ui/util/iupsetobj.h"
#include "mainwin.h"

static int split_valuechange(Ihandle* ih)
{
    MainWin* mainwin = iupGetParentObj(MainWin, ih);
    if (!mainwin)
        return IUP_DEFAULT;

    mainwinSetLayoutDirty(mainwin);

    return IUP_DEFAULT;
}

Ihandle* MainWin_createSplit(_In_ MainWin* self, bool vertical)
{
    Ihandle* ret = IupSplit(NULL, NULL);
    IupSetAttribute(ret, "ORIENTATION", vertical ? "VERTICAL" : "HORIZONTAL");
    IupSetAttribute(ret, "ORIGORIENTATION", vertical ? "VERTICAL" : "HORIZONTAL");
    IupSetAttribute(ret, "VALUE", "500");
    IupSetAttribute(ret, "MINMAX", "200:800");
    IupSetAttribute(ret, "COLOR", "128 128 128");
    IupSetAttribute(ret, "BARSIZE", "4");
    Ihandle* sep = IupGetChild(ret, 0);
    IupSetAttribute(sep, "STYLE", "FILL");
    iupSetObj(ret, ObjNone, self, self->ui);
    IupSetCallback(ret, "VALUECHANGED_CB", split_valuechange);
    return ret;
}

static int tab_dragbegin(Ihandle* ih, int x, int y)
{
    return 0;
}

static int tab_dragdatasize(Ihandle* ih, char* type)
{
    char* val = IupGetAttribute(ih, "VALUE");
    if (val)
        return strlen(val) + 1;
    return 0;
}

static int tab_dragdata(Ihandle* ih, char* type, void* data, int size)
{
    char* val = IupGetAttribute(ih, "VALUE");
    if (val && strlen(val) < size) {
        strcpy((char*)data, val);
        return 0;
    }
    return IUP_IGNORE;
}

static int tab_rightclick(Ihandle* ih, int pos)
{
    Ihandle* tab   = IupGetChild(ih, pos);
    SubspaceUI* ui = iupGetUI(ih);
    if (ui && tab) {
        int mx, my;
        IupGetIntInt(NULL, "CURSORPOS", &mx, &my);
        mainwinShowLayoutMenu(ui->mainw, ih, tab, mx, my);
    }

    return IUP_DEFAULT;
}

static int placeholder_buttoncb(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
    SubspaceUI* ui = iupGetUI(ih);
    if (ui && button == IUP_BUTTON3 && pressed == 0) {
        int mx, my;
        IupGetIntInt(NULL, "CURSORPOS", &mx, &my);
        mainwinShowLayoutMenu(ui->mainw, NULL, ih, mx, my);
    }

    return IUP_DEFAULT;
}

typedef int (*iftcxytp_t)(Ihandle* ih, int x, int y);

void MainWin_replaceSplitChild(_In_ MainWin* self, Ihandle* split, Ihandle* oh, Ihandle* nh,
                               bool destroy)
{
    // special handling for if this is actually the root and not a child of a split.
    // better to do it here than duplicate it everywhere.
    if (split == self->zbox) {
        devAssert(oh == self->root);

        if (destroy)
            IupDestroy(oh);
        else
            IupDetach(oh);
        IupAppend(split, nh);
        IupMap(nh);

        self->root = nh;
        IupSetAttributeHandle(self->zbox, "VALUE", self->root);
        IupRefresh(self->root);
        return;
    }

    int oldpos = IupGetChildPos(split, oh);
    iupSetObj(nh, ObjNone, self, self->ui);
    if (oldpos == 1) {
        // have to do some fancy footwork to keep the ordering correct
        if (destroy)
            IupDestroy(oh);
        else
            IupDetach(oh);
        Ihandle* p1 = IupGetChild(split, 1);
        IupInsert(split, p1, nh);
        IupMap(nh);
    } else {
        // can just remove and append
        if (destroy)
            IupDestroy(oh);
        else
            IupDetach(oh);
        IupAppend(split, nh);
        IupMap(nh);
    }
    IupRefresh(split);
}

static int tab_dropdata(Ihandle* ih, char* type, void* data, int size, int x, int y)
{
    Ihandle* mtab    = IupGetHandle((char*)data);
    MainWin* mainwin = iupGetParentObj(MainWin, ih);
    if (!mtab || !mainwin)
        return IUP_IGNORE;
    Ihandle* origparent = IupGetParent(mtab);

    const char* cls = IupGetClassName(ih);
    int tab         = -1;
    if (!strcmp(cls, "flattabs")) {
        // we're dragging onto another tab collection
        iftcxytp_t cvt = (iftcxytp_t)IupGetCallback(ih, "_IUP_XY2POS_CB");
        if (cvt)
            tab = cvt(ih, x, y);
        // if it was dropped onto a tab, it gets inserted before that tab
        Ihandle* ref = NULL;
        if (tab > -1)
            ref = IupGetChild(ih, tab);

        IupReparent(mtab, ih, ref);
    } else if (!strcmp(cls, "backgroundbox")) {
        // destination is a placeholder, need to delete it and replace with tabs
        Ihandle* tabs = mainwinCreateTabs(mainwin);
        mainwinReplaceSplitChild(mainwin, IupGetParent(ih), ih, tabs, true);
        IupReparent(mtab, tabs, NULL);
        IupRefresh(tabs);
        ih = tabs;
    }

    // if the source is empty now, replace it with a placeholder
    if (IupGetChildCount(origparent) == 0) {
        Ihandle* ph = mainwinCreatePlaceholder(mainwin);
        mainwinReplaceSplitChild(mainwin, IupGetParent(origparent), origparent, ph, true);
    }

    mainwinSetLayoutDirty(mainwin);

    return IUP_DEFAULT;
}

Ihandle* MainWin_createTabs(_In_ MainWin* self)
{
    Ihandle* tab = IupFlatTabs(NULL);
    IupSetAttribute(tab, "BGCOLOR", panelbg);
    IupSetAttribute(tab, "FORECOLOR", "255 255 255");
    IupSetAttribute(tab, "TABSBACKCOLOR", "64 64 64");
    IupSetAttribute(tab, "TABSFORECOLOR", "192 192 192");
    IupSetAttribute(tab, "TABSHIGHCOLOR", "64 64 244");
    IupSetAttribute(tab, "DRAGSOURCE", "YES");
    IupSetAttribute(tab, "DRAGTYPES", "TABDATA");
    IupSetAttribute(tab, "DROPTARGET", "YES");
    IupSetAttribute(tab, "DROPTYPES", "TABDATA");
    IupSetCallback(tab, "DRAGBEGIN_CB", (Icallback)tab_dragbegin);
    IupSetCallback(tab, "DRAGDATASIZE_CB", (Icallback)tab_dragdatasize);
    IupSetCallback(tab, "DRAGDATA_CB", (Icallback)tab_dragdata);
    IupSetCallback(tab, "DROPDATA_CB", (Icallback)tab_dropdata);
    IupSetCallback(tab, "RIGHTCLICK_CB", (Icallback)tab_rightclick);
    iupSetObj(tab, ObjNone, self, self->ui);
    return tab;
}

Ihandle* MainWin_createPlaceholder(_In_ MainWin* self)
{
    Ihandle* ih = IupBackgroundBox(NULL);
    IupSetAttribute(ih, "BGCOLOR", panelbg);
    IupSetAttribute(ih, "CANVASBOX", "YES");
    IupSetAttribute(ih, "DROPTARGET", "YES");
    IupSetAttribute(ih, "DROPTYPES", "TABDATA");
    IupSetCallback(ih, "DROPDATA_CB", (Icallback)tab_dropdata);
    IupSetCallback(ih, "BUTTON_CB", (Icallback)placeholder_buttoncb);
    iupSetObj(ih, ObjNone, self, self->ui);
    return ih;
}

static void loadLayoutNode(MainWin* self, SSDNode* node, Ihandle** out,
                           SSDLockState* _ssdCurrentLockState);
static void splitAppendChild(MainWin* self, Ihandle* split, SSDNode* cnode,
                             SSDLockState* _ssdCurrentLockState)
{
    Ihandle* ci = NULL;
    if (cnode)
        loadLayoutNode(self, cnode, &ci, _ssdCurrentLockState);
    if (ci)
        IupAppend(split, ci);
    else
        IupAppend(split, mainwinCreatePlaceholder(self));
}

static void loadLayoutNode(MainWin* self, SSDNode* node, Ihandle** out,
                           SSDLockState* _ssdCurrentLockState)
{
    string ltype = 0;
    ssdLockedTransaction(node)
    {
        ssdStringOut(node, _S"type", &ltype);
        if (strEq(ltype, _S"split")) {
            *out       = mainwinCreateSplit(self, ssdVal(bool, node, _S"vertical", false));
            int32 size = ssdVal(int32, node, _S"size", 0);
            if (size > 0) {
                string tmp = 0;
                strFromInt32(&tmp, size, 10);
                IupSetAttribute(*out, "VALUE", strC(tmp));
                strDestroy(&tmp);
            }

            splitAppendChild(self, *out, ssdSubtreeB(node, _S"first"), _ssdCurrentLockState);
            splitAppendChild(self, *out, ssdSubtreeB(node, _S"second"), _ssdCurrentLockState);
        } else if (strEq(ltype, _S"tabs")) {
            *out = mainwinCreateTabs(self);

            sa_string tabnames;
            saInit(&tabnames, string, 8);
            ssdExportTypedArray(node, _S"panels", string, &tabnames, true);

            foreach (sarray, idx, string, tabname, tabnames) {
                Panel* panel;
                if (htFind(self->panels, string, tabname, object, &panel)) {
                    IupAppend(*out, panel->h);
                    objRelease(&panel);
                }
            }

            saDestroy(&tabnames);
        } else if (strEq(ltype, _S"empty")) {
            *out = mainwinCreatePlaceholder(self);
        }
    }
}

void MainWin_loadLayout(_In_ MainWin* self)
{
    SSDNode* layout = ssdSubtree(self->ss->settings, _S"ui/layout", SSD_Create_None);
    if (layout) {
        loadLayoutNode(self, layout, &self->root, NULL);
        objRelease(&layout);
        return;
    }

    // create default layout

    self->root = mainwinCreateSplit(self, true);

    Ihandle* left = mainwinCreateSplit(self, false);
    IupSetAttribute(left, "VALUE", "400");
    Panel* panel;
    Ihandle* tab;
    if (htFind(self->panels, string, _S"gameinfo", object, &panel)) {
        tab = mainwinCreateTabs(self);
        IupAppend(tab, panel->h);
        objRelease(&panel);
    } else {
        tab = mainwinCreatePlaceholder(self);
    }
    IupAppend(left, tab);

    if (htFind(self->panels, string, _S"scrapgraph", object, &panel)) {
        tab = mainwinCreateTabs(self);
        IupAppend(tab, panel->h);
        objRelease(&panel);

        if (htFind(self->panels, string, _S"sectordetail", object, &panel)) {
            IupAppend(tab, panel->h);
            objRelease(&panel);
        }
    } else {
        tab = mainwinCreatePlaceholder(self);
    }
    IupAppend(left, tab);

    Ihandle* right = mainwinCreateSplit(self, false);
    IupSetAttribute(right, "VALUE", "750");

    if (htFind(self->panels, string, _S"notableevent", object, &panel)) {
        tab = mainwinCreateTabs(self);
        IupAppend(tab, panel->h);
        objRelease(&panel);
    } else {
        tab = mainwinCreatePlaceholder(self);
    }
    IupAppend(right, tab);
    IupAppend(right, mainwinCreatePlaceholder(self));

    IupAppend(self->root, left);
    IupAppend(self->root, right);
    IupSetAttribute(self->root, "VALUE", "633");
}

static void saveLayoutNode(MainWin* self, SSDNode* node, Ihandle* in,
                           SSDLockState* _ssdCurrentLockState)
{
    ssdLockedTransaction(node)
    {
        const char* cls = NULL;
        if (in)
            cls = IupGetClassName(in);
        if (cls && !strcmp(cls, "split")) {
            ssdSet(node, _S"type", false, stvar(strref, _S"split"));

            char* orientstr = IupGetAttribute(in, "ORIGORIENTATION");
            if (orientstr && !strcmp(orientstr, "VERTICAL"))
                ssdSet(node, _S"vertical", false, stvar(bool, true));

            int size = IupGetInt(in, "VALUE");
            if (size > 0)
                ssdSet(node, _S"size", false, stvar(int32, size));

            SSDNode* chld = ssdSubtree(node, _S"first", true);
            saveLayoutNode(self, chld, IupGetChild(in, 1), _ssdCurrentLockState);
            objRelease(&chld);
            chld = ssdSubtree(node, _S"second", true);
            saveLayoutNode(self, chld, IupGetChild(in, 2), _ssdCurrentLockState);
            objRelease(&chld);
        } else if (cls && !strcmp(cls, "flattabs")) {
            ssdSet(node, _S"type", false, stvar(strref, _S"tabs"));

            sa_string tabnames;
            saInit(&tabnames, string, 8);

            for (int i = 0; i < IupGetChildCount(in); i++) {
                Ihandle* ci  = IupGetChild(in, i);
                Panel* panel = ci ? iupGetObj(Panel, ci) : NULL;
                if (panel && !strEmpty(panel->name))
                    saPush(&tabnames, strref, panel->name);
            }
            ssdImportTypedArray(node, _S"panels", string, tabnames);
            saDestroy(&tabnames);
        } else {
            ssdSet(node, _S"type", false, stvar(strref, _S"empty"));
        }
    }
}

void MainWin_saveLayout(_In_ MainWin* self)
{
    SSDNode* sets = self->ss->settings;
    if (self->width != MAINWIN_DEFAULT_WIDTH || self->height != MAINWIN_DEFAULT_HEIGHT) {
        ssdSet(sets, _S"ui/size/width", true, stvar(int32, self->width));
        ssdSet(sets, _S"ui/size/height", true, stvar(int32, self->height));
    }

    ssdRemove(sets, _S"ui/layout");
    SSDNode* layout = ssdSubtree(sets, _S"ui/layout", true);
    if (!layout)
        return;
    saveLayoutNode(self, layout, self->root, NULL);
}

static bool findPanelNode(MainWin* self, Ihandle* in, strref name)
{
    const char* cls = NULL;
    if (!in)
        return false;
    cls = IupGetClassName(in);
    if (!strcmp(cls, "split")) {
        if (findPanelNode(self, IupGetChild(in, 1), name))
            return true;
        if (findPanelNode(self, IupGetChild(in, 2), name))
            return true;
    } else if (cls && !strcmp(cls, "flattabs")) {
        for (int i = 0; i < IupGetChildCount(in); i++) {
            Ihandle* ci  = IupGetChild(in, i);
            Panel* panel = ci ? iupGetObj(Panel, ci) : NULL;
            if (panel && strEq(panel->name, name))
                return true;
        }
    }

    return false;
}

bool MainWin_isPanelInLayout(_In_ MainWin* self, _In_opt_ strref name)
{
    return findPanelNode(self, self->root, name);
}

void MainWin_addTab(_In_ MainWin* self, Ihandle* attachto, Ihandle* addbefore, _In_opt_ strref name)
{
    const char* cls = IupGetClassName(attachto);
    Panel* panel    = NULL;

    htFind(self->panels, strref, name, object, &panel);

    if (panel && IupGetParent(panel->h) == NULL) {
        if (!strcmp(cls, "flattabs")) {
            IupInsert(attachto, addbefore, panel->h);
        } else if (!strcmp(cls, "backgroundbox")) {
            // swap out placeholder for tabs
            Ihandle* tabs = mainwinCreateTabs(self);
            IupAppend(tabs, panel->h);
            mainwinReplaceSplitChild(self, IupGetParent(attachto), attachto, tabs, true);
        }
    }

    IupMap(panel->h);
    IupRefresh(panel->h);
    mainwinSetLayoutDirty(self);

    objRelease(&panel);

    return;
}

void MainWin_removeTab(_In_ MainWin* self, Ihandle* tabparent, Ihandle* toremove)
{
    IupDetach(toremove);

    // if the parent is empty now, replace it with a placeholder
    if (IupGetChildCount(tabparent) == 0) {
        Ihandle* ph = mainwinCreatePlaceholder(self);
        mainwinReplaceSplitChild(self, IupGetParent(tabparent), tabparent, ph, true);
    }

    mainwinSetLayoutDirty(self);

    return;
}

void MainWin_addSplit(_In_ MainWin* self, Ihandle* at, bool vertical)
{
    Ihandle* parent = IupGetParent(at);
    Ihandle* nsplit = mainwinCreateSplit(self, vertical);

    mainwinReplaceSplitChild(self, parent, at, nsplit, false);
    IupAppend(nsplit, at);
    Ihandle* ph = mainwinCreatePlaceholder(self);
    IupAppend(nsplit, ph);
    IupMap(at);
    IupMap(ph);
    IupRefresh(nsplit);
    mainwinSetLayoutDirty(self);

    return;
}

void MainWin_removePlaceholder(_In_ MainWin* self, Ihandle* ph)
{
    Ihandle* parent = IupGetParent(ph);   // this is the split we're deleting

    if (ph == self->root)
        return;   // not valid to remove the last placeholder

    IupDestroy(ph);
    Ihandle* other = IupGetChild(parent, 1);
    if (!other)
        return;   // shouldn't happen?

    IupDetach(other);
    mainwinReplaceSplitChild(self, IupGetParent(parent), parent, other, true);
    mainwinSetLayoutDirty(self);

    return;
}
