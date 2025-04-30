#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/panel/panel.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct WelcomePanel WelcomePanel;
typedef struct WelcomePanel_WeakRef WelcomePanel_WeakRef;
saDeclarePtr(WelcomePanel);
saDeclarePtr(WelcomePanel_WeakRef);

typedef struct WelcomePanel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} WelcomePanel_ClassIf;
extern WelcomePanel_ClassIf WelcomePanel_ClassIf_tmpl;

typedef struct WelcomePanel {
    union {
        WelcomePanel_ClassIf* _;
        void* _is_WelcomePanel;
        void* _is_Panel;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(SubspaceUI)* ui;
    Ihandle* h;
    Ihandle* parent;
    strref name;
    string title;
} WelcomePanel;
extern ObjClassInfo WelcomePanel_clsinfo;
#define WelcomePanel(inst) ((WelcomePanel*)(unused_noeval((inst) && &((inst)->_is_WelcomePanel)), (inst)))
#define WelcomePanelNone ((WelcomePanel*)NULL)

typedef struct WelcomePanel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_WelcomePanel_WeakRef;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} WelcomePanel_WeakRef;
#define WelcomePanel_WeakRef(inst) ((WelcomePanel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_WelcomePanel_WeakRef)), (inst)))

_objfactory_guaranteed WelcomePanel* WelcomePanel_create(SubspaceUI* ui);
// WelcomePanel* welcomepanelCreate(SubspaceUI* ui);
#define welcomepanelCreate(ui) WelcomePanel_create(SubspaceUI(ui))

// bool welcomepanelMake(WelcomePanel* self);
#define welcomepanelMake(self) (self)->_->make(WelcomePanel(self))
// bool welcomepanelUpdate(WelcomePanel* self);
#define welcomepanelUpdate(self) (self)->_->update(WelcomePanel(self))
// intptr welcomepanelCmp(WelcomePanel* self, WelcomePanel* other, uint32 flags);
#define welcomepanelCmp(self, other, flags) (self)->_->cmp(WelcomePanel(self), other, flags)

