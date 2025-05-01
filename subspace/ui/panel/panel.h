#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <iup.h>
#include "subspace.h"
#include "lang/lang.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct Panel Panel;
typedef struct Panel_WeakRef Panel_WeakRef;
saDeclarePtr(Panel);
saDeclarePtr(Panel_WeakRef);

extern const char *panelbg;

typedef struct Panel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} Panel_ClassIf;
extern Panel_ClassIf Panel_ClassIf_tmpl;

typedef struct Panel {
    union {
        Panel_ClassIf* _;
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
} Panel;
extern ObjClassInfo Panel_clsinfo;
#define Panel(inst) ((Panel*)(unused_noeval((inst) && &((inst)->_is_Panel)), (inst)))
#define PanelNone ((Panel*)NULL)

typedef struct Panel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Panel_WeakRef;
#define Panel_WeakRef(inst) ((Panel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Panel_WeakRef)), (inst)))

// bool panelMake(Panel* self);
#define panelMake(self) (self)->_->make(Panel(self))
// bool panelUpdate(Panel* self);
#define panelUpdate(self) (self)->_->update(Panel(self))
// intptr panelCmp(Panel* self, Panel* other, uint32 flags);
#define panelCmp(self, other, flags) (self)->_->cmp(Panel(self), other, flags)

