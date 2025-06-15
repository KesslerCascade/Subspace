#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/panel/panel.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct RunSummaryPanel RunSummaryPanel;
typedef struct RunSummaryPanel_WeakRef RunSummaryPanel_WeakRef;
saDeclarePtr(RunSummaryPanel);
saDeclarePtr(RunSummaryPanel_WeakRef);

typedef struct RunSummaryPanel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    void (*remap)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} RunSummaryPanel_ClassIf;
extern RunSummaryPanel_ClassIf RunSummaryPanel_ClassIf_tmpl;

typedef struct RunSummaryPanel {
    union {
        RunSummaryPanel_ClassIf* _;
        void* _is_RunSummaryPanel;
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
} RunSummaryPanel;
extern ObjClassInfo RunSummaryPanel_clsinfo;
#define RunSummaryPanel(inst) ((RunSummaryPanel*)(unused_noeval((inst) && &((inst)->_is_RunSummaryPanel)), (inst)))
#define RunSummaryPanelNone ((RunSummaryPanel*)NULL)

typedef struct RunSummaryPanel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_RunSummaryPanel_WeakRef;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} RunSummaryPanel_WeakRef;
#define RunSummaryPanel_WeakRef(inst) ((RunSummaryPanel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_RunSummaryPanel_WeakRef)), (inst)))

_objfactory_guaranteed RunSummaryPanel* RunSummaryPanel_create(SubspaceUI* ui);
// RunSummaryPanel* runsummarypanelCreate(SubspaceUI* ui);
#define runsummarypanelCreate(ui) RunSummaryPanel_create(SubspaceUI(ui))

// bool runsummarypanelMake(RunSummaryPanel* self);
#define runsummarypanelMake(self) (self)->_->make(RunSummaryPanel(self))
// bool runsummarypanelUpdate(RunSummaryPanel* self);
#define runsummarypanelUpdate(self) (self)->_->update(RunSummaryPanel(self))
// void runsummarypanelRemap(RunSummaryPanel* self);
#define runsummarypanelRemap(self) (self)->_->remap(RunSummaryPanel(self))
// intptr runsummarypanelCmp(RunSummaryPanel* self, RunSummaryPanel* other, uint32 flags);
#define runsummarypanelCmp(self, other, flags) (self)->_->cmp(RunSummaryPanel(self), other, flags)

