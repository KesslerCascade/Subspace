#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "image.h"
#include "imagedrivers.h"
#include <iup.h>

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
typedef struct ImageLoad ImageLoad;
typedef struct ImageLoad_WeakRef ImageLoad_WeakRef;
saDeclarePtr(ImageLoad);
saDeclarePtr(ImageLoad_WeakRef);

typedef struct ImageLoad_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
    uint32 (*hash)(_In_ void* self, uint32 flags);
    // Called once all phases (including fail phases) have completed. May be overridden to perform
    // additional cleanup or change the final result.
    uint32 (*finish)(_In_ void* self, uint32 result, TaskControl* tcon);
} ImageLoad_ClassIf;
extern ImageLoad_ClassIf ImageLoad_ClassIf_tmpl;

typedef struct ImageLoad {
    union {
        ImageLoad_ClassIf* _;
        void* _is_ImageLoad;
        void* _is_MultiphaseTask;
        void* _is_ComplexTask;
        void* _is_Task;
        void* _is_BasicTask;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    atomic(uint32) state;
    string name;        // task name to be shown in monitor output
    int64 last;        // the last time this task was moved between queues and/or run
    cchain oncomplete;        // functions that are called when this task has completed
    int64 nextrun;        // next time for this task to run when scheduled
    int64 lastprogress;        // timestamp of last progress change
    Weak(ComplexTaskQueue)* lastq;        // The last queue this task ran on before it was deferred
    sa_TaskRequires _requires;        // list of requirements that must be satisfied
    uint16 flags;        // flags to customize task behavior
    uint16 _intflags;        // internal flags reserved for use by the scheduler
    atomic(uint32) _advcount;        // number of times this task has been advanced
    sa_MPTPhaseFunc phases;        // phases that are run during normal task execution
    sa_MPTPhaseFunc failphases;        // phases that only run on the event of failure
    uint32 _phase;
    bool _fail;
    int driver;
    VFSFile* srcfile;
    uint64 srcfilesz;
    uint8* srcbuf;
    uint8* srcbufp;
    size_t srcbufsz;
    int preferred_width;
    int preferred_height;
    int preferred_dpi;
    Image* image;
} ImageLoad;
extern ObjClassInfo ImageLoad_clsinfo;
#define ImageLoad(inst) ((ImageLoad*)(unused_noeval((inst) && &((inst)->_is_ImageLoad)), (inst)))
#define ImageLoadNone ((ImageLoad*)NULL)

typedef struct ImageLoad_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ImageLoad_WeakRef;
        void* _is_MultiphaseTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ImageLoad_WeakRef;
#define ImageLoad_WeakRef(inst) ((ImageLoad_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ImageLoad_WeakRef)), (inst)))

_objfactory_check ImageLoad* ImageLoad_loadFile(_In_opt_ strref driver, VFS* vfs, _In_opt_ strref fname);
// ImageLoad* imageloadLoadFile(strref driver, VFS* vfs, strref fname);
#define imageloadLoadFile(driver, vfs, fname) ImageLoad_loadFile(driver, vfs, fname)

_objfactory_check ImageLoad* ImageLoad_loadBuffer(_In_opt_ strref driver, uint8* buf, size_t sz);
// ImageLoad* imageloadLoadBuffer(strref driver, uint8* buf, size_t sz);
#define imageloadLoadBuffer(driver, buf, sz) ImageLoad_loadBuffer(driver, buf, sz)

uint32 ImageLoad_loadPhase(_In_ ImageLoad* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
// uint32 imageloadLoadPhase(ImageLoad* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define imageloadLoadPhase(self, tq, worker, tcon) ImageLoad_loadPhase(ImageLoad(self), TaskQueue(tq), TQWorker(worker), tcon)

uint32 ImageLoad_finishPhase(_In_ ImageLoad* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
// uint32 imageloadFinishPhase(ImageLoad* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define imageloadFinishPhase(self, tq, worker, tcon) ImageLoad_finishPhase(ImageLoad(self), TaskQueue(tq), TQWorker(worker), tcon)

// void imageload_addPhases(ImageLoad* self, int32 num, MPTPhaseFunc parr[], bool fail);
//
// Adds phases from a static array.
#define imageload_addPhases(self, num, parr, fail) MultiphaseTask__addPhases(MultiphaseTask(self), num, parr, fail)

// void imageloadRequireTask(ImageLoad* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define imageloadRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void imageloadRequireTaskTimeout(ImageLoad* self, Task* dep, bool failok, int64 timeout);
#define imageloadRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void imageloadRequireResource(ImageLoad* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define imageloadRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void imageloadRequireResourceTimeout(ImageLoad* self, TaskResource* res, int64 timeout);
#define imageloadRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void imageloadRequireGate(ImageLoad* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define imageloadRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void imageloadRequireGateTimeout(ImageLoad* self, TRGate* gate, int64 timeout);
#define imageloadRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void imageloadRequire(ImageLoad* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define imageloadRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool imageloadAdvance(ImageLoad* self);
//
// advance a deferred task to run as soon as possible
#define imageloadAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 imageloadCheckRequires(ImageLoad* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define imageloadCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void imageloadCancelRequires(ImageLoad* self);
//
// cascade a task cancellation to any requirements
#define imageloadCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool imageloadAcquireRequires(ImageLoad* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define imageloadAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool imageloadReleaseRequires(ImageLoad* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define imageloadReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool imageload_setState(ImageLoad* self, uint32 newstate);
#define imageload_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 imageloadRun(ImageLoad* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define imageloadRun(self, tq, worker, tcon) (self)->_->run(ImageLoad(self), TaskQueue(tq), TQWorker(worker), tcon)
// void imageloadRunCancelled(ImageLoad* self, TaskQueue* tq, TQWorker* worker);
#define imageloadRunCancelled(self, tq, worker) (self)->_->runCancelled(ImageLoad(self), TaskQueue(tq), TQWorker(worker))
// bool imageloadCancel(ImageLoad* self);
#define imageloadCancel(self) (self)->_->cancel(ImageLoad(self))
// bool imageloadReset(ImageLoad* self);
#define imageloadReset(self) (self)->_->reset(ImageLoad(self))
// bool imageloadWait(ImageLoad* self, int64 timeout);
#define imageloadWait(self, timeout) (self)->_->wait(ImageLoad(self), timeout)
// intptr imageloadCmp(ImageLoad* self, ImageLoad* other, uint32 flags);
#define imageloadCmp(self, other, flags) (self)->_->cmp(ImageLoad(self), other, flags)
// uint32 imageloadHash(ImageLoad* self, uint32 flags);
#define imageloadHash(self, flags) (self)->_->hash(ImageLoad(self), flags)
// uint32 imageloadFinish(ImageLoad* self, uint32 result, TaskControl* tcon);
//
// Called once all phases (including fail phases) have completed. May be overridden to perform
// additional cleanup or change the final result.
#define imageloadFinish(self, result, tcon) (self)->_->finish(ImageLoad(self), result, tcon)

