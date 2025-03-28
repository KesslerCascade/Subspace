#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/ssdtree/ssdshared.h>
#include <cx/thread/mutex.h>

typedef struct SSDTree SSDTree;
typedef struct SSDTree_WeakRef SSDTree_WeakRef;
saDeclarePtr(SSDTree);
saDeclarePtr(SSDTree_WeakRef);

#ifdef SSD_LOCK_DEBUG
typedef struct SSDTreeDebug {
    Mutex mtx;
    sa_SSDLockDebug readlocks;
    sa_SSDLockDebug writelocks;
} SSDTreeDebug;
#else
typedef struct SSDTreeDebug {
    uint32 _dummy;
} SSDTreeDebug;
#endif

typedef struct SSDTree_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    _objfactory_guaranteed SSDNode* (*createNode)(_In_ void* self, _In_range_(SSD_Create_None+1, SSD_Create_Count-1) SSDCreateType crtype);
} SSDTree_ClassIf;
extern SSDTree_ClassIf SSDTree_ClassIf_tmpl;

typedef struct SSDTree {
    union {
        SSDTree_ClassIf* _;
        void* _is_SSDTree;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    RWLock lock;
    SSDTreeDebug dbg;
    uint32 flags;
    int64 modified;        // The most recent last-modified timestamp of any node in the tree
    SSDNodeFactory factories[SSD_Create_Count];        // Factory functions for if this tree wants to use derived node classes
} SSDTree;
extern ObjClassInfo SSDTree_clsinfo;
#define SSDTree(inst) ((SSDTree*)(unused_noeval((inst) && &((inst)->_is_SSDTree)), (inst)))
#define SSDTreeNone ((SSDTree*)NULL)

typedef struct SSDTree_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SSDTree_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SSDTree_WeakRef;
#define SSDTree_WeakRef(inst) ((SSDTree_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SSDTree_WeakRef)), (inst)))

_objfactory_guaranteed SSDTree* SSDTree_create(uint32 flags);
// SSDTree* ssdtreeCreate(uint32 flags);
#define ssdtreeCreate(flags) SSDTree_create(flags)

// SSDNode* ssdtreeCreateNode(SSDTree* self, SSDCreateType crtype);
#define ssdtreeCreateNode(self, crtype) (self)->_->createNode(SSDTree(self), crtype)

