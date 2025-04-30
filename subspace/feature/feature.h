#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>

typedef struct SubspaceFeature SubspaceFeature;
typedef struct SubspaceFeature_WeakRef SubspaceFeature_WeakRef;
saDeclarePtr(SubspaceFeature);
saDeclarePtr(SubspaceFeature_WeakRef);

typedef struct SubspaceFeature {
    union {
        ObjIface* _;
        void* _is_SubspaceFeature;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    RWLock lock;
} SubspaceFeature;
extern ObjClassInfo SubspaceFeature_clsinfo;
#define SubspaceFeature(inst) ((SubspaceFeature*)(unused_noeval((inst) && &((inst)->_is_SubspaceFeature)), (inst)))
#define SubspaceFeatureNone ((SubspaceFeature*)NULL)

typedef struct SubspaceFeature_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SubspaceFeature_WeakRef;
#define SubspaceFeature_WeakRef(inst) ((SubspaceFeature_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SubspaceFeature_WeakRef)), (inst)))


