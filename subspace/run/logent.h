#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "logentspec.h"

typedef struct LogEnt LogEnt;
typedef struct LogEnt_WeakRef LogEnt_WeakRef;
saDeclarePtr(LogEnt);
saDeclarePtr(LogEnt_WeakRef);

typedef struct LogEnt {
    union {
        ObjIface* _;
        void* _is_LogEnt;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int64 sectorpoint;
    int64 savepoint;
    int64 time;
    float64 gametime;
    string id;
    LogEntSpec* spec;
    sa_stvar rawparams;
    hashtable params;
} LogEnt;
extern ObjClassInfo LogEnt_clsinfo;
#define LogEnt(inst) ((LogEnt*)(unused_noeval((inst) && &((inst)->_is_LogEnt)), (inst)))
#define LogEntNone ((LogEnt*)NULL)

typedef struct LogEnt_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LogEnt_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LogEnt_WeakRef;
#define LogEnt_WeakRef(inst) ((LogEnt_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LogEnt_WeakRef)), (inst)))

_objfactory_check LogEnt* LogEnt_create(int64 sectorpoint, int64 savepoint, int64 time, float64 gametime, _In_opt_ strref id, stvar params[LOG_MAX_PARAMS]);
// LogEnt* logentCreate(int64 sectorpoint, int64 savepoint, int64 time, float64 gametime, strref id, stvar params[LOG_MAX_PARAMS]);
#define logentCreate(sectorpoint, savepoint, time, gametime, id, params) LogEnt_create(sectorpoint, savepoint, time, gametime, id, params)


