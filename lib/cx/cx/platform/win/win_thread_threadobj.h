#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/thread/threadobj.h>
#include <cx/platform/win.h>

typedef struct WinThread WinThread;
typedef struct WinThread_WeakRef WinThread_WeakRef;
saDeclarePtr(WinThread);
saDeclarePtr(WinThread_WeakRef);

typedef struct WinThread {
    union {
        ObjIface* _;
        void* _is_WinThread;
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
    bool background;
    HANDLE handle;
    DWORD id;
} WinThread;
extern ObjClassInfo WinThread_clsinfo;
#define WinThread(inst) ((WinThread*)(unused_noeval((inst) && &((inst)->_is_WinThread)), (inst)))
#define WinThreadNone ((WinThread*)NULL)

typedef struct WinThread_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_WinThread_WeakRef;
        void* _is_Thread_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} WinThread_WeakRef;
#define WinThread_WeakRef(inst) ((WinThread_WeakRef*)(unused_noeval((inst) && &((inst)->_is_WinThread_WeakRef)), (inst)))

_objfactory_guaranteed WinThread* WinThread_create();
// WinThread* _winthrobjCreate();
#define _winthrobjCreate() WinThread_create()


