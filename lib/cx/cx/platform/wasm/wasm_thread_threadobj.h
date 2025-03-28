#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/thread/threadobj.h>
#include <pthread.h>

typedef struct UnixThread UnixThread;
typedef struct UnixThread_WeakRef UnixThread_WeakRef;
saDeclarePtr(UnixThread);
saDeclarePtr(UnixThread_WeakRef);

typedef struct UnixThread {
    union {
        ObjIface* _;
        void* _is_UnixThread;
        void* _is_Thread;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    threadFunc entry;
    string name;
    int exitCode;        // only valid once 'running' become false
    stvlist args;
    sa_stvar _argsa;        // should use the stvlist instead where possible
    atomic(bool) running;
    atomic(bool) requestExit;
    Event notify;
    pthread_t pthr;
    int id;
    bool joined;
} UnixThread;
extern ObjClassInfo UnixThread_clsinfo;
#define UnixThread(inst) ((UnixThread*)(unused_noeval((inst) && &((inst)->_is_UnixThread)), (inst)))
#define UnixThreadNone ((UnixThread*)NULL)

typedef struct UnixThread_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_UnixThread_WeakRef;
        void* _is_Thread_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} UnixThread_WeakRef;
#define UnixThread_WeakRef(inst) ((UnixThread_WeakRef*)(unused_noeval((inst) && &((inst)->_is_UnixThread_WeakRef)), (inst)))

_objfactory_guaranteed UnixThread* UnixThread_create();
// UnixThread* _unixthrobjCreate();
#define _unixthrobjCreate() UnixThread_create()


