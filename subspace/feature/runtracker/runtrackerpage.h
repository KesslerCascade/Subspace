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
typedef struct ScrapTracker ScrapTracker;
typedef struct ScrapTracker_WeakRef ScrapTracker_WeakRef;
typedef struct HullTracker HullTracker;
typedef struct HullTracker_WeakRef HullTracker_WeakRef;
typedef struct LogEnt LogEnt;
typedef struct LogEnt_WeakRef LogEnt_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct RunTracker RunTracker;
typedef struct RunTracker_WeakRef RunTracker_WeakRef;
typedef struct VFSDir VFSDir;
typedef struct TaskControl TaskControl;
typedef struct RunTrackerPage RunTrackerPage;
typedef struct RunTrackerPage_WeakRef RunTrackerPage_WeakRef;
saDeclarePtr(RunTrackerPage);
saDeclarePtr(RunTrackerPage_WeakRef);

typedef struct RunTrackerPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} RunTrackerPage_ClassIf;
extern RunTrackerPage_ClassIf RunTrackerPage_ClassIf_tmpl;

typedef struct RunTrackerPage {
    union {
        RunTrackerPage_ClassIf* _;
        void* _is_RunTrackerPage;
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
    RunTracker* feature;
    Ihandle* rundirtext;
} RunTrackerPage;
extern ObjClassInfo RunTrackerPage_clsinfo;
#define RunTrackerPage(inst) ((RunTrackerPage*)(unused_noeval((inst) && &((inst)->_is_RunTrackerPage)), (inst)))
#define RunTrackerPageNone ((RunTrackerPage*)NULL)

typedef struct RunTrackerPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_RunTrackerPage_WeakRef;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} RunTrackerPage_WeakRef;
#define RunTrackerPage_WeakRef(inst) ((RunTrackerPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_RunTrackerPage_WeakRef)), (inst)))

_objfactory_guaranteed RunTrackerPage* RunTrackerPage_create(RunTracker* feature, SubspaceUI* ui);
// RunTrackerPage* runtrackerpageCreate(RunTracker* feature, SubspaceUI* ui);
#define runtrackerpageCreate(feature, ui) RunTrackerPage_create(RunTracker(feature), SubspaceUI(ui))

// bool runtrackerpageMake(RunTrackerPage* self, Ihandle* list);
#define runtrackerpageMake(self, list) (self)->_->make(RunTrackerPage(self), list)
// bool runtrackerpageUpdate(RunTrackerPage* self);
#define runtrackerpageUpdate(self) (self)->_->update(RunTrackerPage(self))

