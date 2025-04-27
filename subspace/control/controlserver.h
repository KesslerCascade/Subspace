#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "control.h"
#include "subspace.h"
#include "net.h"
#include "controlclient.h"
#include <cx/thread/threadobj.h>
#include <cx/taskqueue.h>

typedef struct ControlServer ControlServer;
typedef struct ControlServer_WeakRef ControlServer_WeakRef;
typedef struct GameInst GameInst;
typedef struct GameInst_WeakRef GameInst_WeakRef;
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
typedef struct ControlServer ControlServer;
typedef struct ControlServer_WeakRef ControlServer_WeakRef;
saDeclarePtr(ControlServer);
saDeclarePtr(ControlServer_WeakRef);

typedef _objfactory_guaranteed Task* (*ctask_factory_t)(ControlClient* client, ControlMsg* msg);

typedef struct ControlServer_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self);
    void (*stop)(_In_ void* self);
    void (*notify)(_In_ void* self);
    bool (*registerHandler)(_In_ void* self, _In_opt_ strref cmd, ctask_factory_t handler);
    ctask_factory_t (*getHandler)(_In_ void* self, _In_opt_ strref cmd);
    int (*port)(_In_ void* self);
} ControlServer_ClassIf;
extern ControlServer_ClassIf ControlServer_ClassIf_tmpl;

typedef struct ControlServer {
    union {
        ControlServer_ClassIf* _;
        void* _is_ControlServer;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Thread* thread;
    sa_ControlClient clients;
    socket_t svrsock;
    int port;
    socket_t notifysock;
    int notifyport;
    RWLock handler_lock;
    hashtable handlers;
} ControlServer;
extern ObjClassInfo ControlServer_clsinfo;
#define ControlServer(inst) ((ControlServer*)(unused_noeval((inst) && &((inst)->_is_ControlServer)), (inst)))
#define ControlServerNone ((ControlServer*)NULL)

typedef struct ControlServer_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ControlServer_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ControlServer_WeakRef;
#define ControlServer_WeakRef(inst) ((ControlServer_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ControlServer_WeakRef)), (inst)))

_objfactory_guaranteed ControlServer* ControlServer_create(Subspace* subspace);
// ControlServer* cserverCreate(Subspace* subspace);
#define cserverCreate(subspace) ControlServer_create(subspace)

// bool cserverStart(ControlServer* self);
#define cserverStart(self) (self)->_->start(ControlServer(self))
// void cserverStop(ControlServer* self);
#define cserverStop(self) (self)->_->stop(ControlServer(self))
// void cserverNotify(ControlServer* self);
#define cserverNotify(self) (self)->_->notify(ControlServer(self))
// bool cserverRegisterHandler(ControlServer* self, strref cmd, ctask_factory_t handler);
#define cserverRegisterHandler(self, cmd, handler) (self)->_->registerHandler(ControlServer(self), cmd, handler)
// ctask_factory_t cserverGetHandler(ControlServer* self, strref cmd);
#define cserverGetHandler(self, cmd) (self)->_->getHandler(ControlServer(self), cmd)
// int cserverPort(ControlServer* self);
#define cserverPort(self) (self)->_->port(ControlServer(self))

