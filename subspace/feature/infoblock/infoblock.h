#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "feature/feature.h"

typedef struct InfoBlock InfoBlock;
typedef struct InfoBlock_WeakRef InfoBlock_WeakRef;
saDeclarePtr(InfoBlock);
saDeclarePtr(InfoBlock_WeakRef);

typedef struct InfoBlock {
    union {
        ObjIface* _;
        void* _is_InfoBlock;
        void* _is_SubspaceFeature;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    RWLock lock;
    string name;
    bool available;
    bool enabled;
    bool optional;        // Features that are expected to be unavailable, e.g. version-specific
} InfoBlock;
extern ObjClassInfo InfoBlock_clsinfo;
#define InfoBlock(inst) ((InfoBlock*)(unused_noeval((inst) && &((inst)->_is_InfoBlock)), (inst)))
#define InfoBlockNone ((InfoBlock*)NULL)

typedef struct InfoBlock_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_InfoBlock_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} InfoBlock_WeakRef;
#define InfoBlock_WeakRef(inst) ((InfoBlock_WeakRef*)(unused_noeval((inst) && &((inst)->_is_InfoBlock_WeakRef)), (inst)))

_objfactory_guaranteed InfoBlock* InfoBlock_create(Subspace* ss);
// InfoBlock* infoblockCreate(Subspace* ss);
#define infoblockCreate(ss) InfoBlock_create(ss)


