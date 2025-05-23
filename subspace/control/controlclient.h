#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "control.h"
#include "subspace.h"
#include <cx/thread/prqueue.h>

typedef struct ControlServer ControlServer;
typedef struct ControlServer_WeakRef ControlServer_WeakRef;
typedef struct GameInst GameInst;
typedef struct GameInst_WeakRef GameInst_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
saDeclarePtr(ControlClient);
saDeclarePtr(ControlClient_WeakRef);

typedef struct ControlClient_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*queue)(_In_ void* self, ControlMsg* msg);
    void (*send)(_In_ void* self);
    void (*recv)(_In_ void* self);
} ControlClient_ClassIf;
extern ControlClient_ClassIf ControlClient_ClassIf_tmpl;

typedef struct ControlClient {
    union {
        ControlClient_ClassIf* _;
        void* _is_ControlClient;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(ControlServer)* svr;
    Weak(GameInst)* inst;
    ControlState state;
    PrQueue outbound;
} ControlClient;
extern ObjClassInfo ControlClient_clsinfo;
#define ControlClient(inst) ((ControlClient*)(unused_noeval((inst) && &((inst)->_is_ControlClient)), (inst)))
#define ControlClientNone ((ControlClient*)NULL)

typedef struct ControlClient_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ControlClient_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ControlClient_WeakRef;
#define ControlClient_WeakRef(inst) ((ControlClient_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ControlClient_WeakRef)), (inst)))

_objfactory_guaranteed ControlClient* ControlClient_create(ControlServer* svr, socket_t sock);
// ControlClient* cclientCreate(ControlServer* svr, socket_t sock);
#define cclientCreate(svr, sock) ControlClient_create(ControlServer(svr), sock)

socket_t ControlClient_sock(_In_ ControlClient* self);
// socket_t cclientSock(ControlClient* self);
#define cclientSock(self) ControlClient_sock(ControlClient(self))

bool ControlClient_closed(_In_ ControlClient* self);
// bool cclientClosed(ControlClient* self);
#define cclientClosed(self) ControlClient_closed(ControlClient(self))

bool ControlClient_sendPending(_In_ ControlClient* self);
// bool cclientSendPending(ControlClient* self);
#define cclientSendPending(self) ControlClient_sendPending(ControlClient(self))

// void cclientQueue(ControlClient* self, ControlMsg* msg);
#define cclientQueue(self, msg) (self)->_->queue(ControlClient(self), msg)
// void cclientSend(ControlClient* self);
#define cclientSend(self) (self)->_->send(ControlClient(self))
// void cclientRecv(ControlClient* self);
#define cclientRecv(self) (self)->_->recv(ControlClient(self))

