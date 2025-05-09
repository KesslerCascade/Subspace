#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "feature/feature.h"

typedef struct OptionsPage OptionsPage;
typedef struct OptionsPage_WeakRef OptionsPage_WeakRef;
typedef struct NumericHull NumericHull;
typedef struct NumericHull_WeakRef NumericHull_WeakRef;
saDeclarePtr(NumericHull);
saDeclarePtr(NumericHull_WeakRef);

typedef struct NumericHull_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    OptionsPage* (*getOptions)(_In_ void* self);
} NumericHull_ClassIf;
extern NumericHull_ClassIf NumericHull_ClassIf_tmpl;

typedef struct NumericHull {
    union {
        NumericHull_ClassIf* _;
        void* _is_NumericHull;
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
} NumericHull;
extern ObjClassInfo NumericHull_clsinfo;
#define NumericHull(inst) ((NumericHull*)(unused_noeval((inst) && &((inst)->_is_NumericHull)), (inst)))
#define NumericHullNone ((NumericHull*)NULL)

typedef struct NumericHull_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_NumericHull_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} NumericHull_WeakRef;
#define NumericHull_WeakRef(inst) ((NumericHull_WeakRef*)(unused_noeval((inst) && &((inst)->_is_NumericHull_WeakRef)), (inst)))

_objfactory_guaranteed NumericHull* NumericHull_create(Subspace* ss);
// NumericHull* numerichullCreate(Subspace* ss);
#define numerichullCreate(ss) NumericHull_create(ss)

// OptionsPage* numerichullGetOptions(NumericHull* self);
#define numerichullGetOptions(self) (self)->_->getOptions(NumericHull(self))

