#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/page/settingspage.h"
#include "gamemgr/gameinst.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct KeybindsPage KeybindsPage;
typedef struct KeybindsPage_WeakRef KeybindsPage_WeakRef;
saDeclarePtr(KeybindsPage);
saDeclarePtr(KeybindsPage_WeakRef);

typedef struct KeybindsPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} KeybindsPage_ClassIf;
extern KeybindsPage_ClassIf KeybindsPage_ClassIf_tmpl;

typedef struct KeybindsPage {
    union {
        KeybindsPage_ClassIf* _;
        void* _is_KeybindsPage;
        void* _is_SettingsPage;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    SubspaceUI* ui;
    Weak(SubspaceFeature)* owner;
    Ihandle* h;
    Ihandle* parent;
    strref name;
    string imgname;
    string title;
    Ihandle* kbgrid;
    Ihandle* tip;
} KeybindsPage;
extern ObjClassInfo KeybindsPage_clsinfo;
#define KeybindsPage(inst) ((KeybindsPage*)(unused_noeval((inst) && &((inst)->_is_KeybindsPage)), (inst)))
#define KeybindsPageNone ((KeybindsPage*)NULL)

typedef struct KeybindsPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_KeybindsPage_WeakRef;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} KeybindsPage_WeakRef;
#define KeybindsPage_WeakRef(inst) ((KeybindsPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_KeybindsPage_WeakRef)), (inst)))

_objfactory_guaranteed KeybindsPage* KeybindsPage_create(SubspaceUI* ui);
// KeybindsPage* keybindspageCreate(SubspaceUI* ui);
#define keybindspageCreate(ui) KeybindsPage_create(SubspaceUI(ui))

void KeybindsPage_populateGrid(_In_ KeybindsPage* self);
// void keybindspagePopulateGrid(KeybindsPage* self);
#define keybindspagePopulateGrid(self) KeybindsPage_populateGrid(KeybindsPage(self))

// bool keybindspageMake(KeybindsPage* self, Ihandle* list);
#define keybindspageMake(self, list) (self)->_->make(KeybindsPage(self), list)
// bool keybindspageUpdate(KeybindsPage* self);
#define keybindspageUpdate(self) (self)->_->update(KeybindsPage(self))

