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
typedef struct ControlHandler ControlHandler;
typedef struct ControlHandler_WeakRef ControlHandler_WeakRef;
saDeclarePtr(ControlServer);
saDeclarePtr(ControlServer_WeakRef);
saDeclarePtr(ControlHandler);
saDeclarePtr(ControlHandler_WeakRef);

typedef struct GameInst GameInst;
typedef void (*controlservercb_t)(GameInst *inst, ControlClient *client, ControlMsg* msg, hashtable fields);

#define DECL_CFIELDVAL(type) bool _cfieldVal_##type(hashtable fields, strref name, type *out)
#define DECL_CFIELDVALD(type) type _cfieldValD_##type(hashtable fields, strref name, type def)
DECL_CFIELDVAL(bool);
DECL_CFIELDVALD(bool);
DECL_CFIELDVAL(int32);
DECL_CFIELDVALD(int32);
DECL_CFIELDVAL(uint32);
DECL_CFIELDVALD(uint32);
DECL_CFIELDVAL(float32);
DECL_CFIELDVALD(float32);
DECL_CFIELDVAL(float64);
DECL_CFIELDVALD(float64);

#define cfieldVal(type, fields, name, out) _cfieldVal_##type(fields, name, out)
#define cfieldValD(type, fields, name, def) _cfieldValD_##type(fields, name, def)
strref cfieldString(hashtable fields, strref name);

typedef struct ControlServer_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self);
    void (*stop)(_In_ void* self);
    void (*notify)(_In_ void* self);
    bool (*registerHandler)(_In_ void* self, _In_opt_ strref cmd, controlservercb_t cb, bool needinst, TaskResource* require);
    ControlHandler* (*getHandler)(_In_ void* self, _In_opt_ strref cmd);
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
    TRFifo* preGameReady;
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
// bool cserverRegisterHandler(ControlServer* self, strref cmd, controlservercb_t cb, bool needinst, TaskResource* require);
#define cserverRegisterHandler(self, cmd, cb, needinst, require) (self)->_->registerHandler(ControlServer(self), cmd, cb, needinst, TaskResource(require))
// ControlHandler* cserverGetHandler(ControlServer* self, strref cmd);
#define cserverGetHandler(self, cmd) (self)->_->getHandler(ControlServer(self), cmd)
// int cserverPort(ControlServer* self);
#define cserverPort(self) (self)->_->port(ControlServer(self))

typedef struct ControlHandler {
    union {
        ObjIface* _;
        void* _is_ControlHandler;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    controlservercb_t cb;
    TaskResource* require;
    bool needinst;
} ControlHandler;
extern ObjClassInfo ControlHandler_clsinfo;
#define ControlHandler(inst) ((ControlHandler*)(unused_noeval((inst) && &((inst)->_is_ControlHandler)), (inst)))
#define ControlHandlerNone ((ControlHandler*)NULL)

typedef struct ControlHandler_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ControlHandler_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ControlHandler_WeakRef;
#define ControlHandler_WeakRef(inst) ((ControlHandler_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ControlHandler_WeakRef)), (inst)))

_objfactory_guaranteed ControlHandler* ControlHandler_create(controlservercb_t cb, bool needinst, TaskResource* require);
// ControlHandler* controlhandlerCreate(controlservercb_t cb, bool needinst, TaskResource* require);
#define controlhandlerCreate(cb, needinst, require) ControlHandler_create(cb, needinst, TaskResource(require))


