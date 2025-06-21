#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"
#include "sectorinfo.h"
#include "logentspec.h"

typedef struct LogEnt LogEnt;
typedef struct LogEnt_WeakRef LogEnt_WeakRef;
typedef struct RunInfo RunInfo;
typedef struct RunInfo_WeakRef RunInfo_WeakRef;
saDeclarePtr(RunInfo);
saDeclarePtr(RunInfo_WeakRef);

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
    void (*runLog)(_In_ void* self, int sector, int beacons, int64 time, _In_opt_ strref id, stvar params[LOG_MAX_PARAMS]);
    void (*beacon)(_In_ void* self, int sector, int beacons, int visit, int x, int y, int64 time, _In_opt_ strref event);
    void (*processLog)(_In_ void* self, LogEnt* ent);
    void (*processScrap)(_In_ void* self, _In_opt_ strref src, int amount, int rawamount);
    void (*processHullDamage)(_In_ void* self, _In_opt_ strref src, int amount);
    void (*processShip)(_In_ void* self, _In_opt_ strref name);
    int32 (*score)(_In_ void* self);
    void (*finish)(_In_ void* self, RunResult result);
} RunInfo_ClassIf;
extern RunInfo_ClassIf RunInfo_ClassIf_tmpl;

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
// void runinfoRunLog(RunInfo* self, int sector, int beacons, int64 time, strref id, stvar params[LOG_MAX_PARAMS]);
#define runinfoRunLog(self, sector, beacons, time, id, params) (self)->_->runLog(RunInfo(self), sector, beacons, time, id, params)
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
// int32 runinfoScore(RunInfo* self);
#define runinfoScore(self) (self)->_->score(RunInfo(self))
// void runinfoFinish(RunInfo* self, RunResult result);
#define runinfoFinish(self, result) (self)->_->finish(RunInfo(self), result)

