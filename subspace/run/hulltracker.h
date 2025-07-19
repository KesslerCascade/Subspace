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
typedef struct HullTracker HullTracker;
typedef struct HullTracker_WeakRef HullTracker_WeakRef;
saDeclarePtr(HullTracker);
saDeclarePtr(HullTracker_WeakRef);

typedef struct HullTotals {
    int32 damage;           // damage from all sources below

    int32 projectile;       // damage from lasers and flak
    int32 missile;          // damage from missiles
    int32 beam;             // damage from beams
    int32 asteroid;         // damage from asteroids
    int32 asb;              // damage from ASB shots
    int32 solarflare;       // damage from solar flares
    int32 systemdestroyed;  // damage from a system being destroyed
    int32 event;            // damage from events
    int32 other;            // some other type of damage
    int32 repair;           // hull repairs
} HullTotals;

typedef struct HullTracker_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*getTotal)(_In_ void* self, HullTotals* out);
    bool (*getBeacon)(_In_ void* self, int64 savepoint, HullTotals* out);
    bool (*getSector)(_In_ void* self, int64 sectorpoint, HullTotals* out);
    void (*reset)(_In_ void* self);
    void (*logNotify)(_In_ void* self, LogEnt* ent, bool replay);
    void (*logReset)(_In_ void* self);
    void (*logReplayComplete)(_In_ void* self);
} HullTracker_ClassIf;
extern HullTracker_ClassIf HullTracker_ClassIf_tmpl;

typedef struct HullTracker {
    union {
        HullTracker_ClassIf* _;
        void* _is_HullTracker;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(RunInfo)* run;
    RWLock lock;
    HullTotals total;        // total scrap over the run
    hashtable sectors;        // per-sector totals
    hashtable beacons;        // per-beacon totals
} HullTracker;
extern ObjClassInfo HullTracker_clsinfo;
#define HullTracker(inst) ((HullTracker*)(unused_noeval((inst) && &((inst)->_is_HullTracker)), (inst)))
#define HullTrackerNone ((HullTracker*)NULL)

typedef struct HullTracker_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_HullTracker_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} HullTracker_WeakRef;
#define HullTracker_WeakRef(inst) ((HullTracker_WeakRef*)(unused_noeval((inst) && &((inst)->_is_HullTracker_WeakRef)), (inst)))

_objfactory_guaranteed HullTracker* HullTracker_create(RunInfo* run);
// HullTracker* hulltrackerCreate(RunInfo* run);
#define hulltrackerCreate(run) HullTracker_create(RunInfo(run))

// bool hulltrackerGetTotal(HullTracker* self, HullTotals* out);
#define hulltrackerGetTotal(self, out) (self)->_->getTotal(HullTracker(self), out)
// bool hulltrackerGetBeacon(HullTracker* self, int64 savepoint, HullTotals* out);
#define hulltrackerGetBeacon(self, savepoint, out) (self)->_->getBeacon(HullTracker(self), savepoint, out)
// bool hulltrackerGetSector(HullTracker* self, int64 sectorpoint, HullTotals* out);
#define hulltrackerGetSector(self, sectorpoint, out) (self)->_->getSector(HullTracker(self), sectorpoint, out)
// void hulltrackerReset(HullTracker* self);
#define hulltrackerReset(self) (self)->_->reset(HullTracker(self))
// void hulltrackerLogNotify(HullTracker* self, LogEnt* ent, bool replay);
#define hulltrackerLogNotify(self, ent, replay) (self)->_->logNotify(HullTracker(self), LogEnt(ent), replay)
// void hulltrackerLogReset(HullTracker* self);
#define hulltrackerLogReset(self) (self)->_->logReset(HullTracker(self))
// void hulltrackerLogReplayComplete(HullTracker* self);
#define hulltrackerLogReplayComplete(self) (self)->_->logReplayComplete(HullTracker(self))

