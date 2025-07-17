#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"
#include "ui/subspaceui.h"
#include "logrelay.h"

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
typedef struct RunInfo RunInfo;
typedef struct RunInfo_WeakRef RunInfo_WeakRef;
typedef struct RunInfo RunInfo;
typedef struct RunInfo_WeakRef RunInfo_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct ScrapTracker ScrapTracker;
typedef struct ScrapTracker_WeakRef ScrapTracker_WeakRef;
saDeclarePtr(ScrapTracker);
saDeclarePtr(ScrapTracker_WeakRef);

typedef struct ScrapTotals {
    int32 rewards;          // scrap rewards from events
    int32 rawrewards;       // raw rewards (without scrap recovery / repair arm)
    int32 eventloss;        // scrap spent or lost through events

    int32 dropvalue;        // value of inventory drops (retroactively adjusted if the item is later sold, discarded, or placed in storage)
    int32 rawdropvalue;     // raw value of inventory drops -- NOT retroactively adjusted

    int32 storesold;        // scrap gotten from selling items to stores
    int32 storebought;      // scrap spent buying items from stores
    int32 storerepair;      // scrap spent repairing at stores
    
    int32 sysupg;           // scrap spent upgrading systems
    int32 powerupg;         // scrap spent upgrading power
} ScrapTotals;

typedef struct ScrapTracker_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*getTotal)(_In_ void* self, ScrapTotals* out);
    bool (*getBeacon)(_In_ void* self, int64 savepoint, ScrapTotals* out);
    bool (*getSector)(_In_ void* self, int64 sectorpoint, ScrapTotals* out);
    void (*reset)(_In_ void* self);
    void (*logNotify)(_In_ void* self, LogEnt* ent, bool replay);
    void (*logReset)(_In_ void* self);
    void (*logReplayComplete)(_In_ void* self);
} ScrapTracker_ClassIf;
extern ScrapTracker_ClassIf ScrapTracker_ClassIf_tmpl;

typedef struct ScrapTracker {
    union {
        ScrapTracker_ClassIf* _;
        void* _is_ScrapTracker;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(RunInfo)* run;
    RWLock lock;
    ScrapTotals total;        // total scrap over the run
    hashtable sectors;        // per-sector totals
    hashtable beacons;        // per-beacon totals
} ScrapTracker;
extern ObjClassInfo ScrapTracker_clsinfo;
#define ScrapTracker(inst) ((ScrapTracker*)(unused_noeval((inst) && &((inst)->_is_ScrapTracker)), (inst)))
#define ScrapTrackerNone ((ScrapTracker*)NULL)

typedef struct ScrapTracker_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ScrapTracker_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ScrapTracker_WeakRef;
#define ScrapTracker_WeakRef(inst) ((ScrapTracker_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ScrapTracker_WeakRef)), (inst)))

_objfactory_guaranteed ScrapTracker* ScrapTracker_create(RunInfo* run);
// ScrapTracker* scraptrackerCreate(RunInfo* run);
#define scraptrackerCreate(run) ScrapTracker_create(RunInfo(run))

// bool scraptrackerGetTotal(ScrapTracker* self, ScrapTotals* out);
#define scraptrackerGetTotal(self, out) (self)->_->getTotal(ScrapTracker(self), out)
// bool scraptrackerGetBeacon(ScrapTracker* self, int64 savepoint, ScrapTotals* out);
#define scraptrackerGetBeacon(self, savepoint, out) (self)->_->getBeacon(ScrapTracker(self), savepoint, out)
// bool scraptrackerGetSector(ScrapTracker* self, int64 sectorpoint, ScrapTotals* out);
#define scraptrackerGetSector(self, sectorpoint, out) (self)->_->getSector(ScrapTracker(self), sectorpoint, out)
// void scraptrackerReset(ScrapTracker* self);
#define scraptrackerReset(self) (self)->_->reset(ScrapTracker(self))
// void scraptrackerLogNotify(ScrapTracker* self, LogEnt* ent, bool replay);
#define scraptrackerLogNotify(self, ent, replay) (self)->_->logNotify(ScrapTracker(self), LogEnt(ent), replay)
// void scraptrackerLogReset(ScrapTracker* self);
#define scraptrackerLogReset(self) (self)->_->logReset(ScrapTracker(self))
// void scraptrackerLogReplayComplete(ScrapTracker* self);
#define scraptrackerLogReplayComplete(self) (self)->_->logReplayComplete(ScrapTracker(self))

