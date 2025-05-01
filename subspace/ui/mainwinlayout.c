#include <cx/container.h>
#include "ui/util/iupsetobj.h"
#include "mainwin.h"

static int split_valuechange(Ihandle *ih)
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

typedef int (*iftcxytp_t)(Ihandle* ih, int x, int y);

void MainWin_replaceSplitChild(_In_ MainWin* self, Ihandle* split, Ihandle* oh, Ihandle* nh)
{
    int oldpos = IupGetChildPos(split, oh);
    iupSetObj(nh, ObjNone, self, self->ui);
    if (oldpos == 1) {
        // have to do some fancy footwork to keep the ordering correct
        IupDetach(oh);
        IupDestroy(oh);
        Ihandle* p1 = IupGetChild(split, 1);
        IupInsert(split, p1, nh);
        IupMap(nh);
        IupRefresh(split);
    } else {
        // can just remove and append
        IupDetach(oh);
        IupDestroy(oh);
        IupAppend(split, nh);
        IupMap(nh);
        IupRefresh(split);
    }
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
        mainwinReplaceSplitChild(mainwin, IupGetParent(ih), ih, tabs);
        IupReparent(mtab, tabs, NULL);
        IupRefresh(tabs);
        ih = tabs;
    }

    // if the source is empty now, replace it with a placeholder
    if (IupGetChildCount(origparent) == 0) {
        Ihandle* ph = mainwinCreatePlaceholder(mainwin);
        mainwinReplaceSplitChild(mainwin, IupGetParent(origparent), origparent, ph);
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
    IupSetAttribute(self->root, "VALUE", "500");

    Ihandle* left = mainwinCreateSplit(self, false);
    IupSetAttribute(left, "VALUE", "500");
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
    IupAppend(left, mainwinCreatePlaceholder(self));

    Ihandle* right = mainwinCreateSplit(self, false);
    IupSetAttribute(right, "VALUE", "750");
    IupAppend(right, mainwinCreatePlaceholder(self));
    IupAppend(right, mainwinCreatePlaceholder(self));

    IupAppend(self->root, left);
    IupAppend(self->root, right);
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

            int size     = IupGetInt(in, "VALUE");
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
