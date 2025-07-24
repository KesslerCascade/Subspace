#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <iup.h>
#include "subspace.h"
#include "ui/page/settingspage.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SettingsWin SettingsWin;
typedef struct SettingsWin_WeakRef SettingsWin_WeakRef;
saDeclarePtr(SettingsWin);
saDeclarePtr(SettingsWin_WeakRef);

#define SETTINGSWIN_WIDTH 500
#define SETTINGSWIN_HEIGHT 400

typedef struct SettingsWin_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*showPage)(_In_ void* self, int num);
    void (*showPageByList)(_In_ void* self, int listid);
    bool (*make)(_In_ void* self);
    void (*show)(_In_ void* self);
    void (*updateList)(_In_ void* self);
    bool (*updatePage)(_In_ void* self, _In_opt_ strref name);
    void (*updateAll)(_In_ void* self);
    void (*finish)(_In_ void* self);
} SettingsWin_ClassIf;
extern SettingsWin_ClassIf SettingsWin_ClassIf_tmpl;

typedef struct SettingsWin {
    union {
        SettingsWin_ClassIf* _;
        void* _is_SettingsWin;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    SubspaceUI* ui;
    Ihandle* win;
    Ihandle* pagelist;
    Ihandle* pagezbox;
    sa_SettingsPage pages;
    sa_int32 zboxmap;
} SettingsWin;
extern ObjClassInfo SettingsWin_clsinfo;
#define SettingsWin(inst) ((SettingsWin*)(unused_noeval((inst) && &((inst)->_is_SettingsWin)), (inst)))
#define SettingsWinNone ((SettingsWin*)NULL)

typedef struct SettingsWin_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SettingsWin_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SettingsWin_WeakRef;
#define SettingsWin_WeakRef(inst) ((SettingsWin_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SettingsWin_WeakRef)), (inst)))

_objfactory_guaranteed SettingsWin* SettingsWin_create(SubspaceUI* ui);
// SettingsWin* settingswinCreate(SubspaceUI* ui);
#define settingswinCreate(ui) SettingsWin_create(SubspaceUI(ui))

int SettingsWin_onClose(Ihandle* ih);
// int settingswinOnClose(Ihandle* ih);
#define settingswinOnClose(ih) SettingsWin_onClose(ih)

// void settingswinShowPage(SettingsWin* self, int num);
#define settingswinShowPage(self, num) (self)->_->showPage(SettingsWin(self), num)
// void settingswinShowPageByList(SettingsWin* self, int listid);
#define settingswinShowPageByList(self, listid) (self)->_->showPageByList(SettingsWin(self), listid)
// bool settingswinMake(SettingsWin* self);
#define settingswinMake(self) (self)->_->make(SettingsWin(self))
// void settingswinShow(SettingsWin* self);
#define settingswinShow(self) (self)->_->show(SettingsWin(self))
// void settingswinUpdateList(SettingsWin* self);
#define settingswinUpdateList(self) (self)->_->updateList(SettingsWin(self))
// bool settingswinUpdatePage(SettingsWin* self, strref name);
#define settingswinUpdatePage(self, name) (self)->_->updatePage(SettingsWin(self), name)
// void settingswinUpdateAll(SettingsWin* self);
#define settingswinUpdateAll(self) (self)->_->updateAll(SettingsWin(self))
// void settingswinFinish(SettingsWin* self);
#define settingswinFinish(self) (self)->_->finish(SettingsWin(self))

