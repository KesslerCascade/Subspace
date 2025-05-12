#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "feature.h"
#include "subspace.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct FeatureRegistry FeatureRegistry;
typedef struct FeatureRegistry_WeakRef FeatureRegistry_WeakRef;
saDeclarePtr(FeatureRegistry);
saDeclarePtr(FeatureRegistry_WeakRef);

#define fregGet(fname, registry) objDynCast(fname, fregFind(registry, _S #fname))

typedef struct FeatureRegistry_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    SubspaceFeature* (*find)(_In_ void* self, _In_opt_ strref name);
} FeatureRegistry_ClassIf;
extern FeatureRegistry_ClassIf FeatureRegistry_ClassIf_tmpl;

typedef struct FeatureRegistry {
    union {
        FeatureRegistry_ClassIf* _;
        void* _is_FeatureRegistry;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    // This doesn't need to be locked because it's populated during
    // program load and never changes.
    hashtable features;
} FeatureRegistry;
extern ObjClassInfo FeatureRegistry_clsinfo;
#define FeatureRegistry(inst) ((FeatureRegistry*)(unused_noeval((inst) && &((inst)->_is_FeatureRegistry)), (inst)))
#define FeatureRegistryNone ((FeatureRegistry*)NULL)

typedef struct FeatureRegistry_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_FeatureRegistry_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} FeatureRegistry_WeakRef;
#define FeatureRegistry_WeakRef(inst) ((FeatureRegistry_WeakRef*)(unused_noeval((inst) && &((inst)->_is_FeatureRegistry_WeakRef)), (inst)))

_objfactory_guaranteed FeatureRegistry* FeatureRegistry_create(Subspace* ss);
// FeatureRegistry* fregCreate(Subspace* ss);
#define fregCreate(ss) FeatureRegistry_create(ss)

// SubspaceFeature* fregFind(FeatureRegistry* self, strref name);
#define fregFind(self, name) (self)->_->find(FeatureRegistry(self), name)

