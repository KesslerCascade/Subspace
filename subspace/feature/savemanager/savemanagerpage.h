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
typedef struct SaveManager SaveManager;
typedef struct SaveManager_WeakRef SaveManager_WeakRef;
typedef struct VFSDir VFSDir;
typedef struct TaskControl TaskControl;
typedef struct SaveManagerPage SaveManagerPage;
typedef struct SaveManagerPage_WeakRef SaveManagerPage_WeakRef;
saDeclarePtr(SaveManagerPage);
saDeclarePtr(SaveManagerPage_WeakRef);

typedef struct SaveManagerPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} SaveManagerPage_ClassIf;
extern SaveManagerPage_ClassIf SaveManagerPage_ClassIf_tmpl;

typedef struct SaveManagerPage {
    union {
        SaveManagerPage_ClassIf* _;
        void* _is_SaveManagerPage;
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
    SaveManager* feature;
    Ihandle* savenametext;
} SaveManagerPage;
extern ObjClassInfo SaveManagerPage_clsinfo;
#define SaveManagerPage(inst) ((SaveManagerPage*)(unused_noeval((inst) && &((inst)->_is_SaveManagerPage)), (inst)))
#define SaveManagerPageNone ((SaveManagerPage*)NULL)

typedef struct SaveManagerPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SaveManagerPage_WeakRef;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SaveManagerPage_WeakRef;
#define SaveManagerPage_WeakRef(inst) ((SaveManagerPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SaveManagerPage_WeakRef)), (inst)))

_objfactory_guaranteed SaveManagerPage* SaveManagerPage_create(SaveManager* feature, SubspaceUI* ui);
// SaveManagerPage* savemanagerpageCreate(SaveManager* feature, SubspaceUI* ui);
#define savemanagerpageCreate(feature, ui) SaveManagerPage_create(SaveManager(feature), SubspaceUI(ui))

// bool savemanagerpageMake(SaveManagerPage* self, Ihandle* list);
#define savemanagerpageMake(self, list) (self)->_->make(SaveManagerPage(self), list)
// bool savemanagerpageUpdate(SaveManagerPage* self);
#define savemanagerpageUpdate(self) (self)->_->update(SaveManagerPage(self))

