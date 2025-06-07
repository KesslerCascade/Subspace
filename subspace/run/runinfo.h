#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"
#include "sectorinfo.h"

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

#define SPOINT(num, extra) (((num) << 8) | ((extra) & 0xff))
#define SPOINT_NUM(spoint) ((spoint) >> 8)
#define SPOINT_EXTRA(spoint) ((spoint) & 0xff)

typedef struct RunInfo_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*newGame)(_In_ void* self, int seed, _In_opt_ strref shipType, _In_opt_ strref shipName, int difficulty);
    void (*loadGame)(_In_ void* self, int seed, _In_opt_ strref shipType, _In_opt_ strref shipName, int difficulty, int beacons);
    void (*enterSector)(_In_ void* self, int num, int seed, _In_opt_ strref type, bool secret);
    void (*abandon)(_In_ void* self);
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
// void runinfoEnterSector(RunInfo* self, int num, int seed, strref type, bool secret);
#define runinfoEnterSector(self, num, seed, type, secret) (self)->_->enterSector(RunInfo(self), num, seed, type, secret)
// void runinfoAbandon(RunInfo* self);
#define runinfoAbandon(self) (self)->_->abandon(RunInfo(self))

