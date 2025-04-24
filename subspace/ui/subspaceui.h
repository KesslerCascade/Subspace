#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "subspace.h"

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
saDeclarePtr(SubspaceUI);
saDeclarePtr(SubspaceUI_WeakRef);

typedef struct SubspaceUI_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*init)(_In_ void* self);
    bool (*shutdown)(_In_ void* self);
    void (*start)(_In_ void* self);
    void (*stop)(_In_ void* self);
} SubspaceUI_ClassIf;
extern SubspaceUI_ClassIf SubspaceUI_ClassIf_tmpl;

typedef struct SubspaceUI {
    union {
        SubspaceUI_ClassIf* _;
        void* _is_SubspaceUI;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    TaskQueue* uiq;
    TaskQueue* uiworkers;
    bool started;
} SubspaceUI;
extern ObjClassInfo SubspaceUI_clsinfo;
#define SubspaceUI(inst) ((SubspaceUI*)(unused_noeval((inst) && &((inst)->_is_SubspaceUI)), (inst)))
#define SubspaceUINone ((SubspaceUI*)NULL)

typedef struct SubspaceUI_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SubspaceUI_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SubspaceUI_WeakRef;
#define SubspaceUI_WeakRef(inst) ((SubspaceUI_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SubspaceUI_WeakRef)), (inst)))

_objfactory_guaranteed SubspaceUI* SubspaceUI_create(Subspace* subspace);
// SubspaceUI* ssuiCreate(Subspace* subspace);
#define ssuiCreate(subspace) SubspaceUI_create(subspace)

// bool ssuiInit(SubspaceUI* self);
#define ssuiInit(self) (self)->_->init(SubspaceUI(self))
// bool ssuiShutdown(SubspaceUI* self);
#define ssuiShutdown(self) (self)->_->shutdown(SubspaceUI(self))
// void ssuiStart(SubspaceUI* self);
#define ssuiStart(self) (self)->_->start(SubspaceUI(self))
// void ssuiStop(SubspaceUI* self);
#define ssuiStop(self) (self)->_->stop(SubspaceUI(self))

