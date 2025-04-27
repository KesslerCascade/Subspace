#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "image.h"

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
typedef struct ImageResample ImageResample;
typedef struct ImageResample_WeakRef ImageResample_WeakRef;
saDeclarePtr(ImageResample);
saDeclarePtr(ImageResample_WeakRef);

typedef struct ImageResample_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} ImageResample_ClassIf;
extern ImageResample_ClassIf ImageResample_ClassIf_tmpl;

typedef struct ImageResample {
    union {
        ImageResample_ClassIf* _;
        void* _is_ImageResample;
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
    int targetWidth;
    int targetHeight;
    int targetDPI;
    Image* image;
} ImageResample;
extern ObjClassInfo ImageResample_clsinfo;
#define ImageResample(inst) ((ImageResample*)(unused_noeval((inst) && &((inst)->_is_ImageResample)), (inst)))
#define ImageResampleNone ((ImageResample*)NULL)

typedef struct ImageResample_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ImageResample_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ImageResample_WeakRef;
#define ImageResample_WeakRef(inst) ((ImageResample_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ImageResample_WeakRef)), (inst)))

_objfactory_check ImageResample* ImageResample_toSize(Image* image, int width, int height);
// ImageResample* imageresampleToSize(Image* image, int width, int height);
#define imageresampleToSize(image, width, height) ImageResample_toSize(Image(image), width, height)

_objfactory_check ImageResample* ImageResample_toDPI(Image* image, int dpi);
// ImageResample* imageresampleToDPI(Image* image, int dpi);
#define imageresampleToDPI(image, dpi) ImageResample_toDPI(Image(image), dpi)

// bool imageresample_setState(ImageResample* self, uint32 newstate);
#define imageresample_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 imageresampleRun(ImageResample* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define imageresampleRun(self, tq, worker, tcon) (self)->_->run(ImageResample(self), TaskQueue(tq), TQWorker(worker), tcon)
// void imageresampleRunCancelled(ImageResample* self, TaskQueue* tq, TQWorker* worker);
#define imageresampleRunCancelled(self, tq, worker) (self)->_->runCancelled(ImageResample(self), TaskQueue(tq), TQWorker(worker))
// bool imageresampleCancel(ImageResample* self);
#define imageresampleCancel(self) (self)->_->cancel(ImageResample(self))
// bool imageresampleReset(ImageResample* self);
#define imageresampleReset(self) (self)->_->reset(ImageResample(self))
// bool imageresampleWait(ImageResample* self, int64 timeout);
#define imageresampleWait(self, timeout) (self)->_->wait(ImageResample(self), timeout)

