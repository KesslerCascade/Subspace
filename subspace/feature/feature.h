#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"

typedef struct OptionsPage OptionsPage;
typedef struct OptionsPage_WeakRef OptionsPage_WeakRef;
typedef struct SubspaceFeature SubspaceFeature;
typedef struct SubspaceFeature_WeakRef SubspaceFeature_WeakRef;
typedef struct ClientFeature ClientFeature;
typedef struct ClientFeature_WeakRef ClientFeature_WeakRef;
saDeclarePtr(SubspaceFeature);
saDeclarePtr(SubspaceFeature_WeakRef);
saDeclarePtr(ClientFeature);
saDeclarePtr(ClientFeature_WeakRef);

typedef struct SubspaceFeature_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    OptionsPage* (*getOptions)(_In_ void* self);
    void (*enable)(_In_ void* self, bool enabled);
} SubspaceFeature_ClassIf;
extern SubspaceFeature_ClassIf SubspaceFeature_ClassIf_tmpl;

typedef struct SubspaceFeature {
    union {
        SubspaceFeature_ClassIf* _;
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

// OptionsPage* featureGetOptions(SubspaceFeature* self);
#define featureGetOptions(self) (self)->_->getOptions(SubspaceFeature(self))
// void featureEnable(SubspaceFeature* self, bool enabled);
#define featureEnable(self, enabled) (self)->_->enable(SubspaceFeature(self), enabled)

typedef struct ClientFeature {
    union {
        ObjIface* _;
        void* _is_ClientFeature;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string name;
    bool available;
    bool enabled;
} ClientFeature;
extern ObjClassInfo ClientFeature_clsinfo;
#define ClientFeature(inst) ((ClientFeature*)(unused_noeval((inst) && &((inst)->_is_ClientFeature)), (inst)))
#define ClientFeatureNone ((ClientFeature*)NULL)

typedef struct ClientFeature_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ClientFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ClientFeature_WeakRef;
#define ClientFeature_WeakRef(inst) ((ClientFeature_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ClientFeature_WeakRef)), (inst)))

_objfactory_guaranteed ClientFeature* ClientFeature_create(_In_opt_ strref name);
// ClientFeature* clientfeatureCreate(strref name);
#define clientfeatureCreate(name) ClientFeature_create(name)


