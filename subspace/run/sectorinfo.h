#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"

typedef struct SectorInfo SectorInfo;
typedef struct SectorInfo_WeakRef SectorInfo_WeakRef;
saDeclarePtr(SectorInfo);
saDeclarePtr(SectorInfo_WeakRef);

typedef struct SectorInfo_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} SectorInfo_ClassIf;
extern SectorInfo_ClassIf SectorInfo_ClassIf_tmpl;

typedef struct SectorInfo {
    union {
        SectorInfo_ClassIf* _;
        void* _is_SectorInfo;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int64 sectorpoint;
    int64 time;        // time first entered
    string type;
    int32 seed;
} SectorInfo;
extern ObjClassInfo SectorInfo_clsinfo;
#define SectorInfo(inst) ((SectorInfo*)(unused_noeval((inst) && &((inst)->_is_SectorInfo)), (inst)))
#define SectorInfoNone ((SectorInfo*)NULL)

typedef struct SectorInfo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SectorInfo_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SectorInfo_WeakRef;
#define SectorInfo_WeakRef(inst) ((SectorInfo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SectorInfo_WeakRef)), (inst)))

_objfactory_guaranteed SectorInfo* SectorInfo_create();
// SectorInfo* sectorinfoCreate();
#define sectorinfoCreate() SectorInfo_create()

// intptr sectorinfoCmp(SectorInfo* self, SectorInfo* other, uint32 flags);
#define sectorinfoCmp(self, other, flags) (self)->_->cmp(SectorInfo(self), other, flags)

