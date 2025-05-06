#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/panel/panel.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct NotableEventPanel NotableEventPanel;
typedef struct NotableEventPanel_WeakRef NotableEventPanel_WeakRef;
saDeclarePtr(NotableEventPanel);
saDeclarePtr(NotableEventPanel_WeakRef);

typedef struct NotableEventPanel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} NotableEventPanel_ClassIf;
extern NotableEventPanel_ClassIf NotableEventPanel_ClassIf_tmpl;

typedef struct NotableEventPanel {
    union {
        NotableEventPanel_ClassIf* _;
        void* _is_NotableEventPanel;
        void* _is_Panel;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    SubspaceUI* ui;
    Ihandle* h;
    Ihandle* parent;
    strref name;
    string title;
} NotableEventPanel;
extern ObjClassInfo NotableEventPanel_clsinfo;
#define NotableEventPanel(inst) ((NotableEventPanel*)(unused_noeval((inst) && &((inst)->_is_NotableEventPanel)), (inst)))
#define NotableEventPanelNone ((NotableEventPanel*)NULL)

typedef struct NotableEventPanel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_NotableEventPanel_WeakRef;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} NotableEventPanel_WeakRef;
#define NotableEventPanel_WeakRef(inst) ((NotableEventPanel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_NotableEventPanel_WeakRef)), (inst)))

_objfactory_guaranteed NotableEventPanel* NotableEventPanel_create(SubspaceUI* ui);
// NotableEventPanel* notableeventpanelCreate(SubspaceUI* ui);
#define notableeventpanelCreate(ui) NotableEventPanel_create(SubspaceUI(ui))

// bool notableeventpanelMake(NotableEventPanel* self);
#define notableeventpanelMake(self) (self)->_->make(NotableEventPanel(self))
// bool notableeventpanelUpdate(NotableEventPanel* self);
#define notableeventpanelUpdate(self) (self)->_->update(NotableEventPanel(self))
// intptr notableeventpanelCmp(NotableEventPanel* self, NotableEventPanel* other, uint32 flags);
#define notableeventpanelCmp(self, other, flags) (self)->_->cmp(NotableEventPanel(self), other, flags)

