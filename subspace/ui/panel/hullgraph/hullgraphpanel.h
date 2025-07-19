#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "ui/subspaceui.h"
#include "ui/panel/panel.h"
#include "run/logrelay.h"

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
typedef struct MainWin MainWin;
typedef struct MainWin_WeakRef MainWin_WeakRef;
typedef struct SettingsWin SettingsWin;
typedef struct SettingsWin_WeakRef SettingsWin_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct RunInfo RunInfo;
typedef struct RunInfo_WeakRef RunInfo_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct HullGraphPanel HullGraphPanel;
typedef struct HullGraphPanel_WeakRef HullGraphPanel_WeakRef;
saDeclarePtr(HullGraphPanel);
saDeclarePtr(HullGraphPanel_WeakRef);

typedef struct HullGraphPanel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    void (*remap)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
    void (*clear)(_In_ void* self);
    void (*handleUpdate)(_In_ void* self, int64 sectorpoint, bool redraw);
    void (*uiNotify)(_In_ void* self, _In_opt_ strref event, stvlist* params);
} HullGraphPanel_ClassIf;
extern HullGraphPanel_ClassIf HullGraphPanel_ClassIf_tmpl;

typedef struct HullGraphPanel {
    union {
        HullGraphPanel_ClassIf* _;
        void* _is_HullGraphPanel;
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
    Ihandle* plot;
    int ds[9];
    int repairds;
    sa_int64 sectoridx;
} HullGraphPanel;
extern ObjClassInfo HullGraphPanel_clsinfo;
#define HullGraphPanel(inst) ((HullGraphPanel*)(unused_noeval((inst) && &((inst)->_is_HullGraphPanel)), (inst)))
#define HullGraphPanelNone ((HullGraphPanel*)NULL)

typedef struct HullGraphPanel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_HullGraphPanel_WeakRef;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} HullGraphPanel_WeakRef;
#define HullGraphPanel_WeakRef(inst) ((HullGraphPanel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_HullGraphPanel_WeakRef)), (inst)))

_objfactory_guaranteed HullGraphPanel* HullGraphPanel_create(SubspaceUI* ui);
// HullGraphPanel* hullgraphpanelCreate(SubspaceUI* ui);
#define hullgraphpanelCreate(ui) HullGraphPanel_create(SubspaceUI(ui))

// bool hullgraphpanelMake(HullGraphPanel* self);
#define hullgraphpanelMake(self) (self)->_->make(HullGraphPanel(self))
// bool hullgraphpanelUpdate(HullGraphPanel* self);
#define hullgraphpanelUpdate(self) (self)->_->update(HullGraphPanel(self))
// void hullgraphpanelRemap(HullGraphPanel* self);
#define hullgraphpanelRemap(self) (self)->_->remap(HullGraphPanel(self))
// intptr hullgraphpanelCmp(HullGraphPanel* self, HullGraphPanel* other, uint32 flags);
#define hullgraphpanelCmp(self, other, flags) (self)->_->cmp(HullGraphPanel(self), other, flags)
// void hullgraphpanelClear(HullGraphPanel* self);
#define hullgraphpanelClear(self) (self)->_->clear(HullGraphPanel(self))
// void hullgraphpanelHandleUpdate(HullGraphPanel* self, int64 sectorpoint, bool redraw);
#define hullgraphpanelHandleUpdate(self, sectorpoint, redraw) (self)->_->handleUpdate(HullGraphPanel(self), sectorpoint, redraw)
// void hullgraphpanelUiNotify(HullGraphPanel* self, strref event, stvlist* params);
#define hullgraphpanelUiNotify(self, event, params) (self)->_->uiNotify(HullGraphPanel(self), event, params)

