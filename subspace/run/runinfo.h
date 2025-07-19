#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"
#include "gamedata/gamedata.h"
#include "sectorinfo.h"
#include "logentspec.h"
#include <cx/taskqueue.h>

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
typedef struct VFSDir VFSDir;
typedef struct TaskControl TaskControl;
typedef struct RunInfo RunInfo;
typedef struct RunInfo_WeakRef RunInfo_WeakRef;
typedef struct LogReplay LogReplay;
typedef struct LogReplay_WeakRef LogReplay_WeakRef;
saDeclarePtr(RunInfo);
saDeclarePtr(RunInfo_WeakRef);
saDeclarePtr(LogReplay);
saDeclarePtr(LogReplay_WeakRef);

typedef enum {
    RUN_Active = 0,
    RUN_Victory = 1,
    RUN_Defeat = 2,
    RUN_Abandoned = 3
} RunResult;

#define SPOINT(num, extra) ((((int64)(num)) << 8) | ((extra) & 0xff))
#define SPOINT_NUM(spoint) (int32)(((spoint) >> 8))
#define SPOINT_EXTRA(spoint) ((int32)((spoint) & 0xff))

typedef struct RunInfo_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*newGame)(_In_ void* self, int seed, _In_opt_ strref shipType, _In_opt_ strref shipName, int difficulty);
    void (*loadGame)(_In_ void* self, int seed, _In_opt_ strref shipType, _In_opt_ strref shipName, int difficulty, int beacons);
    bool (*loadHistoric)(_In_ void* self, int64 runid);
    void (*replayLog)(_In_ void* self, bool combat, int64 savepoint, int64 sectorpoint);
    void (*enterSector)(_In_ void* self, int num, int seed, _In_opt_ strref type, bool secret);
    void (*updateStats)(_In_ void* self, int ships, int beacons, int scrap, int crew);
    void (*runLog)(_In_ void* self, int sector, int beacons, int64 time, float64 gametime, _In_opt_ strref id, stvar params[LOG_MAX_PARAMS]);
    void (*beacon)(_In_ void* self, int sector, int beacons, int visit, int x, int y, int64 time, _In_opt_ strref event);
    void (*processLog)(_In_ void* self, LogEnt* ent);
    void (*processScrap)(_In_ void* self, _In_opt_ strref src, int amount, int rawamount);
    void (*processHullDamage)(_In_ void* self, _In_opt_ strref src, int amount);
    void (*processShip)(_In_ void* self, _In_opt_ strref name);
    void (*setFocused)(_In_ void* self, bool focused);
    SectorInfo* (*getSector)(_In_ void* self, int64 sectorpoint);
    HullTracker* (*getHull)(_In_ void* self);
    ScrapTracker* (*getScrap)(_In_ void* self);
    int32 (*score)(_In_ void* self);
    void (*finish)(_In_ void* self, RunResult result);
} RunInfo_ClassIf;
extern RunInfo_ClassIf RunInfo_ClassIf_tmpl;

typedef struct LogReplay_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
    uint32 (*hash)(_In_ void* self, uint32 flags);
} LogReplay_ClassIf;
extern LogReplay_ClassIf LogReplay_ClassIf_tmpl;

typedef struct RunInfo {
    union {
        RunInfo_ClassIf* _;
        void* _is_RunInfo;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    RWLock lock;
    int64 runid;        // -1 if not tracked
    int32 seed;
    string shipType;
    string shipName;
    int32 difficulty;
    RunResult result;
    int64 startTime;
    int64 endTime;
    int64 savepoint;
    int64 sectorpoint;
    int32 shipsDefeated;
    int32 beaconsExplored;
    int32 scrapCollected;
    int32 crewHired;
    int32 scrapActual;
    int32 damageTaken;
    bool updatedBeaconShip;
    int64 modified;
    string savePath;
    sa_SectorInfo sectors;
    bool recording;        // is this run being currently recorded (i.e. not a historical run)
    bool focused;        // is focused by the UI
    HullTracker* hull;        // Trackers
    ScrapTracker* scrap;
} RunInfo;
extern ObjClassInfo RunInfo_clsinfo;
#define RunInfo(inst) ((RunInfo*)(unused_noeval((inst) && &((inst)->_is_RunInfo)), (inst)))
#define RunInfoNone ((RunInfo*)NULL)

typedef struct RunInfo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_RunInfo_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} RunInfo_WeakRef;
#define RunInfo_WeakRef(inst) ((RunInfo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_RunInfo_WeakRef)), (inst)))

_objfactory_guaranteed RunInfo* RunInfo_create(Subspace* ss);
// RunInfo* runinfoCreate(Subspace* ss);
#define runinfoCreate(ss) RunInfo_create(ss)

// void runinfoNewGame(RunInfo* self, int seed, strref shipType, strref shipName, int difficulty);
#define runinfoNewGame(self, seed, shipType, shipName, difficulty) (self)->_->newGame(RunInfo(self), seed, shipType, shipName, difficulty)
// void runinfoLoadGame(RunInfo* self, int seed, strref shipType, strref shipName, int difficulty, int beacons);
#define runinfoLoadGame(self, seed, shipType, shipName, difficulty, beacons) (self)->_->loadGame(RunInfo(self), seed, shipType, shipName, difficulty, beacons)
// bool runinfoLoadHistoric(RunInfo* self, int64 runid);
#define runinfoLoadHistoric(self, runid) (self)->_->loadHistoric(RunInfo(self), runid)
// void runinfoReplayLog(RunInfo* self, bool combat, int64 savepoint, int64 sectorpoint);
#define runinfoReplayLog(self, combat, savepoint, sectorpoint) (self)->_->replayLog(RunInfo(self), combat, savepoint, sectorpoint)
// void runinfoEnterSector(RunInfo* self, int num, int seed, strref type, bool secret);
#define runinfoEnterSector(self, num, seed, type, secret) (self)->_->enterSector(RunInfo(self), num, seed, type, secret)
// void runinfoUpdateStats(RunInfo* self, int ships, int beacons, int scrap, int crew);
#define runinfoUpdateStats(self, ships, beacons, scrap, crew) (self)->_->updateStats(RunInfo(self), ships, beacons, scrap, crew)
// void runinfoRunLog(RunInfo* self, int sector, int beacons, int64 time, float64 gametime, strref id, stvar params[LOG_MAX_PARAMS]);
#define runinfoRunLog(self, sector, beacons, time, gametime, id, params) (self)->_->runLog(RunInfo(self), sector, beacons, time, gametime, id, params)
// void runinfoBeacon(RunInfo* self, int sector, int beacons, int visit, int x, int y, int64 time, strref event);
#define runinfoBeacon(self, sector, beacons, visit, x, y, time, event) (self)->_->beacon(RunInfo(self), sector, beacons, visit, x, y, time, event)
// void runinfoProcessLog(RunInfo* self, LogEnt* ent);
#define runinfoProcessLog(self, ent) (self)->_->processLog(RunInfo(self), LogEnt(ent))
// void runinfoProcessScrap(RunInfo* self, strref src, int amount, int rawamount);
#define runinfoProcessScrap(self, src, amount, rawamount) (self)->_->processScrap(RunInfo(self), src, amount, rawamount)
// void runinfoProcessHullDamage(RunInfo* self, strref src, int amount);
#define runinfoProcessHullDamage(self, src, amount) (self)->_->processHullDamage(RunInfo(self), src, amount)
// void runinfoProcessShip(RunInfo* self, strref name);
#define runinfoProcessShip(self, name) (self)->_->processShip(RunInfo(self), name)
// void runinfoSetFocused(RunInfo* self, bool focused);
#define runinfoSetFocused(self, focused) (self)->_->setFocused(RunInfo(self), focused)
// SectorInfo* runinfoGetSector(RunInfo* self, int64 sectorpoint);
#define runinfoGetSector(self, sectorpoint) (self)->_->getSector(RunInfo(self), sectorpoint)
// HullTracker* runinfoGetHull(RunInfo* self);
#define runinfoGetHull(self) (self)->_->getHull(RunInfo(self))
// ScrapTracker* runinfoGetScrap(RunInfo* self);
#define runinfoGetScrap(self) (self)->_->getScrap(RunInfo(self))
// int32 runinfoScore(RunInfo* self);
#define runinfoScore(self) (self)->_->score(RunInfo(self))
// void runinfoFinish(RunInfo* self, RunResult result);
#define runinfoFinish(self, result) (self)->_->finish(RunInfo(self), result)

typedef struct LogReplay {
    union {
        LogReplay_ClassIf* _;
        void* _is_LogReplay;
        void* _is_ComplexTask;
        void* _is_Task;
        void* _is_BasicTask;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    atomic(uint32) state;
    string name;        // task name to be shown in monitor output
    int64 last;        // the last time this task was moved between queues and/or run
    cchain oncomplete;        // functions that are called when this task has completed
    int64 nextrun;        // next time for this task to run when scheduled
    int64 lastprogress;        // timestamp of last progress change
    Weak(ComplexTaskQueue)* lastq;        // The last queue this task ran on before it was deferred
    sa_TaskRequires _requires;        // list of requirements that must be satisfied
    uint16 flags;        // flags to customize task behavior
    uint16 _intflags;        // internal flags reserved for use by the scheduler
    atomic(uint32) _advcount;        // number of times this task has been advanced
    Subspace* ss;
    RunInfo* run;
    bool combat;
    int64 savepoint;
    int64 sectorpoint;
} LogReplay;
extern ObjClassInfo LogReplay_clsinfo;
#define LogReplay(inst) ((LogReplay*)(unused_noeval((inst) && &((inst)->_is_LogReplay)), (inst)))
#define LogReplayNone ((LogReplay*)NULL)

typedef struct LogReplay_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LogReplay_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LogReplay_WeakRef;
#define LogReplay_WeakRef(inst) ((LogReplay_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LogReplay_WeakRef)), (inst)))

_objfactory_guaranteed LogReplay* LogReplay_create(Subspace* ss, RunInfo* run, bool combat, int64 savepoint, int64 sectorpoint);
// LogReplay* logreplayCreate(Subspace* ss, RunInfo* run, bool combat, int64 savepoint, int64 sectorpoint);
#define logreplayCreate(ss, run, combat, savepoint, sectorpoint) LogReplay_create(ss, RunInfo(run), combat, savepoint, sectorpoint)

// void logreplayRequireTask(LogReplay* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define logreplayRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void logreplayRequireTaskTimeout(LogReplay* self, Task* dep, bool failok, int64 timeout);
#define logreplayRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void logreplayRequireResource(LogReplay* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define logreplayRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void logreplayRequireResourceTimeout(LogReplay* self, TaskResource* res, int64 timeout);
#define logreplayRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void logreplayRequireGate(LogReplay* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define logreplayRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void logreplayRequireGateTimeout(LogReplay* self, TRGate* gate, int64 timeout);
#define logreplayRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void logreplayRequire(LogReplay* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define logreplayRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool logreplayAdvance(LogReplay* self);
//
// advance a deferred task to run as soon as possible
#define logreplayAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 logreplayCheckRequires(LogReplay* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define logreplayCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void logreplayCancelRequires(LogReplay* self);
//
// cascade a task cancellation to any requirements
#define logreplayCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool logreplayAcquireRequires(LogReplay* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define logreplayAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool logreplayReleaseRequires(LogReplay* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define logreplayReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool logreplay_setState(LogReplay* self, uint32 newstate);
#define logreplay_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 logreplayRun(LogReplay* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define logreplayRun(self, tq, worker, tcon) (self)->_->run(LogReplay(self), TaskQueue(tq), TQWorker(worker), tcon)
// void logreplayRunCancelled(LogReplay* self, TaskQueue* tq, TQWorker* worker);
#define logreplayRunCancelled(self, tq, worker) (self)->_->runCancelled(LogReplay(self), TaskQueue(tq), TQWorker(worker))
// bool logreplayCancel(LogReplay* self);
#define logreplayCancel(self) (self)->_->cancel(LogReplay(self))
// bool logreplayReset(LogReplay* self);
#define logreplayReset(self) (self)->_->reset(LogReplay(self))
// bool logreplayWait(LogReplay* self, int64 timeout);
#define logreplayWait(self, timeout) (self)->_->wait(LogReplay(self), timeout)
// intptr logreplayCmp(LogReplay* self, LogReplay* other, uint32 flags);
#define logreplayCmp(self, other, flags) (self)->_->cmp(LogReplay(self), other, flags)
// uint32 logreplayHash(LogReplay* self, uint32 flags);
#define logreplayHash(self, flags) (self)->_->hash(LogReplay(self), flags)

