#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <iup.h>
#include "subspace.h"
#include "ui/panel/panel.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct MainWin MainWin;
typedef struct MainWin_WeakRef MainWin_WeakRef;
saDeclarePtr(MainWin);
saDeclarePtr(MainWin_WeakRef);

#define MAINWIN_DEFAULT_WIDTH 600
#define MAINWIN_DEFAULT_HEIGHT 300

typedef struct MainWin_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    void (*show)(_In_ void* self);
    void (*update)(_In_ void* self);
    void (*setLayoutDirty)(_In_ void* self);
} MainWin_ClassIf;
extern MainWin_ClassIf MainWin_ClassIf_tmpl;

typedef struct MainWin {
    union {
        MainWin_ClassIf* _;
        void* _is_MainWin;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(SubspaceUI)* ui;
    Ihandle* win;
    Ihandle* sidebar;
    Ihandle* zbox;
    Ihandle* root;        // root of dynamic layout
    Ihandle* timer;
    uint32 activeInst;        // cookie of game instance that's being tracked through the UI
    hashtable panels;        // all panels (except welcome)
    Panel* welcomepanel;
    int width;
    int height;
    bool layoutDirty;
} MainWin;
extern ObjClassInfo MainWin_clsinfo;
#define MainWin(inst) ((MainWin*)(unused_noeval((inst) && &((inst)->_is_MainWin)), (inst)))
#define MainWinNone ((MainWin*)NULL)

typedef struct MainWin_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_MainWin_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} MainWin_WeakRef;
#define MainWin_WeakRef(inst) ((MainWin_WeakRef*)(unused_noeval((inst) && &((inst)->_is_MainWin_WeakRef)), (inst)))

_objfactory_guaranteed MainWin* MainWin_create(SubspaceUI* ui);
// MainWin* mainwinCreate(SubspaceUI* ui);
#define mainwinCreate(ui) MainWin_create(SubspaceUI(ui))

void MainWin_loadLayout(_In_ MainWin* self);
// void mainwinLoadLayout(MainWin* self);
#define mainwinLoadLayout(self) MainWin_loadLayout(MainWin(self))

void MainWin_saveLayout(_In_ MainWin* self);
// void mainwinSaveLayout(MainWin* self);
#define mainwinSaveLayout(self) MainWin_saveLayout(MainWin(self))

Ihandle* MainWin_createPlaceholder(_In_ MainWin* self);
// Ihandle* mainwinCreatePlaceholder(MainWin* self);
#define mainwinCreatePlaceholder(self) MainWin_createPlaceholder(MainWin(self))

Ihandle* MainWin_createTabs(_In_ MainWin* self);
// Ihandle* mainwinCreateTabs(MainWin* self);
#define mainwinCreateTabs(self) MainWin_createTabs(MainWin(self))

Ihandle* MainWin_createSplit(_In_ MainWin* self, bool vertical);
// Ihandle* mainwinCreateSplit(MainWin* self, bool vertical);
#define mainwinCreateSplit(self, vertical) MainWin_createSplit(MainWin(self), vertical)

void MainWin_replaceSplitChild(_In_ MainWin* self, Ihandle* split, Ihandle* oh, Ihandle* nh);
// void mainwinReplaceSplitChild(MainWin* self, Ihandle* split, Ihandle* oh, Ihandle* nh);
#define mainwinReplaceSplitChild(self, split, oh, nh) MainWin_replaceSplitChild(MainWin(self), split, oh, nh)

int MainWin_onClose(Ihandle* ih);
// int mainwinOnClose(Ihandle* ih);
#define mainwinOnClose(ih) MainWin_onClose(ih)

int MainWin_onResize(Ihandle* ih, int width, int height);
// int mainwinOnResize(Ihandle* ih, int width, int height);
#define mainwinOnResize(ih, width, height) MainWin_onResize(ih, width, height)

int MainWin_onTimer(Ihandle* ih);
// int mainwinOnTimer(Ihandle* ih);
#define mainwinOnTimer(ih) MainWin_onTimer(ih)

// bool mainwinMake(MainWin* self);
#define mainwinMake(self) (self)->_->make(MainWin(self))
// void mainwinShow(MainWin* self);
#define mainwinShow(self) (self)->_->show(MainWin(self))
// void mainwinUpdate(MainWin* self);
#define mainwinUpdate(self) (self)->_->update(MainWin(self))
// void mainwinSetLayoutDirty(MainWin* self);
#define mainwinSetLayoutDirty(self) (self)->_->setLayoutDirty(MainWin(self))

