#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/task/mptask.h>
#include <cx/thread/event.h>

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
typedef struct TQTest1 TQTest1;
typedef struct TQTest1_WeakRef TQTest1_WeakRef;
typedef struct TQTestFail TQTestFail;
typedef struct TQTestFail_WeakRef TQTestFail_WeakRef;
typedef struct TQTestCC1 TQTestCC1;
typedef struct TQTestCC1_WeakRef TQTestCC1_WeakRef;
typedef struct TQTestCC2 TQTestCC2;
typedef struct TQTestCC2_WeakRef TQTestCC2_WeakRef;
typedef struct TQTestSched TQTestSched;
typedef struct TQTestSched_WeakRef TQTestSched_WeakRef;
typedef struct TQTestS1 TQTestS1;
typedef struct TQTestS1_WeakRef TQTestS1_WeakRef;
typedef struct TQTestS2 TQTestS2;
typedef struct TQTestS2_WeakRef TQTestS2_WeakRef;
typedef struct TQDelayTest TQDelayTest;
typedef struct TQDelayTest_WeakRef TQDelayTest_WeakRef;
typedef struct TQMTest TQMTest;
typedef struct TQMTest_WeakRef TQMTest_WeakRef;
typedef struct TQRTestMtx TQRTestMtx;
typedef struct TQRTestMtx_WeakRef TQRTestMtx_WeakRef;
typedef struct TQRTestFifo TQRTestFifo;
typedef struct TQRTestFifo_WeakRef TQRTestFifo_WeakRef;
typedef struct TQRTestLifo TQRTestLifo;
typedef struct TQRTestLifo_WeakRef TQRTestLifo_WeakRef;
typedef struct TQRTestGate TQRTestGate;
typedef struct TQRTestGate_WeakRef TQRTestGate_WeakRef;
typedef struct TQMPTest TQMPTest;
typedef struct TQMPTest_WeakRef TQMPTest_WeakRef;
typedef struct TQTimeoutTest1 TQTimeoutTest1;
typedef struct TQTimeoutTest1_WeakRef TQTimeoutTest1_WeakRef;
typedef struct TQTimeoutTest2 TQTimeoutTest2;
typedef struct TQTimeoutTest2_WeakRef TQTimeoutTest2_WeakRef;
saDeclarePtr(TQTest1);
saDeclarePtr(TQTest1_WeakRef);
saDeclarePtr(TQTestFail);
saDeclarePtr(TQTestFail_WeakRef);
saDeclarePtr(TQTestCC1);
saDeclarePtr(TQTestCC1_WeakRef);
saDeclarePtr(TQTestCC2);
saDeclarePtr(TQTestCC2_WeakRef);
saDeclarePtr(TQTestSched);
saDeclarePtr(TQTestSched_WeakRef);
saDeclarePtr(TQTestS1);
saDeclarePtr(TQTestS1_WeakRef);
saDeclarePtr(TQTestS2);
saDeclarePtr(TQTestS2_WeakRef);
saDeclarePtr(TQDelayTest);
saDeclarePtr(TQDelayTest_WeakRef);
saDeclarePtr(TQMTest);
saDeclarePtr(TQMTest_WeakRef);
saDeclarePtr(TQRTestMtx);
saDeclarePtr(TQRTestMtx_WeakRef);
saDeclarePtr(TQRTestFifo);
saDeclarePtr(TQRTestFifo_WeakRef);
saDeclarePtr(TQRTestLifo);
saDeclarePtr(TQRTestLifo_WeakRef);
saDeclarePtr(TQRTestGate);
saDeclarePtr(TQRTestGate_WeakRef);
saDeclarePtr(TQMPTest);
saDeclarePtr(TQMPTest_WeakRef);
saDeclarePtr(TQTimeoutTest1);
saDeclarePtr(TQTimeoutTest1_WeakRef);
saDeclarePtr(TQTimeoutTest2);
saDeclarePtr(TQTimeoutTest2_WeakRef);

typedef struct ReqTestState
{
    int sum;
    int xor;
    int product;
    int count;
    int target_count;
    int seq;
    atomic(bool) running;
    atomic(bool) fail;
    Event notify;
} ReqTestState;

typedef struct ReqTestState2
{
    atomic(int32) sum;
    atomic(int32) count;
    int target_count;
    atomic(bool) fail;
    Event notify;
} ReqTestState2;
typedef struct MPTestState {
    atomic(int32) sum;
    atomic(bool) fail;
    atomic(int32) count;
    int target_count;
    Event notify;
} MPTestState;

typedef struct TQTest1_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} TQTest1_ClassIf;
extern TQTest1_ClassIf TQTest1_ClassIf_tmpl;

typedef struct TQTestFail_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} TQTestFail_ClassIf;
extern TQTestFail_ClassIf TQTestFail_ClassIf_tmpl;

typedef struct TQTestCC1_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} TQTestCC1_ClassIf;
extern TQTestCC1_ClassIf TQTestCC1_ClassIf_tmpl;

typedef struct TQTestCC2_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} TQTestCC2_ClassIf;
extern TQTestCC2_ClassIf TQTestCC2_ClassIf_tmpl;

typedef struct TQTestSched_ClassIf {
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
} TQTestSched_ClassIf;
extern TQTestSched_ClassIf TQTestSched_ClassIf_tmpl;

typedef struct TQTestS1_ClassIf {
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
} TQTestS1_ClassIf;
extern TQTestS1_ClassIf TQTestS1_ClassIf_tmpl;

typedef struct TQTestS2_ClassIf {
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
} TQTestS2_ClassIf;
extern TQTestS2_ClassIf TQTestS2_ClassIf_tmpl;

typedef struct TQDelayTest_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} TQDelayTest_ClassIf;
extern TQDelayTest_ClassIf TQDelayTest_ClassIf_tmpl;

typedef struct TQMTest_ClassIf {
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
} TQMTest_ClassIf;
extern TQMTest_ClassIf TQMTest_ClassIf_tmpl;

typedef struct TQRTestMtx_ClassIf {
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
} TQRTestMtx_ClassIf;
extern TQRTestMtx_ClassIf TQRTestMtx_ClassIf_tmpl;

typedef struct TQRTestFifo_ClassIf {
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
} TQRTestFifo_ClassIf;
extern TQRTestFifo_ClassIf TQRTestFifo_ClassIf_tmpl;

typedef struct TQRTestLifo_ClassIf {
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
} TQRTestLifo_ClassIf;
extern TQRTestLifo_ClassIf TQRTestLifo_ClassIf_tmpl;

typedef struct TQRTestGate_ClassIf {
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
} TQRTestGate_ClassIf;
extern TQRTestGate_ClassIf TQRTestGate_ClassIf_tmpl;

typedef struct TQMPTest_ClassIf {
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
} TQMPTest_ClassIf;
extern TQMPTest_ClassIf TQMPTest_ClassIf_tmpl;

typedef struct TQTimeoutTest1_ClassIf {
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
} TQTimeoutTest1_ClassIf;
extern TQTimeoutTest1_ClassIf TQTimeoutTest1_ClassIf_tmpl;

typedef struct TQTimeoutTest2_ClassIf {
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
} TQTimeoutTest2_ClassIf;
extern TQTimeoutTest2_ClassIf TQTimeoutTest2_ClassIf_tmpl;

typedef struct TQTest1 {
    union {
        TQTest1_ClassIf* _;
        void* _is_TQTest1;
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
    int num[2];
    int total;
    Event* notify;
} TQTest1;
extern ObjClassInfo TQTest1_clsinfo;
#define TQTest1(inst) ((TQTest1*)(unused_noeval((inst) && &((inst)->_is_TQTest1)), (inst)))
#define TQTest1None ((TQTest1*)NULL)

typedef struct TQTest1_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTest1_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTest1_WeakRef;
#define TQTest1_WeakRef(inst) ((TQTest1_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTest1_WeakRef)), (inst)))

_objfactory_guaranteed TQTest1* TQTest1_create(int num1, int num2, Event* notify);
// TQTest1* tqtest1Create(int num1, int num2, Event* notify);
#define tqtest1Create(num1, num2, notify) TQTest1_create(num1, num2, notify)

// bool tqtest1_setState(TQTest1* self, uint32 newstate);
#define tqtest1_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtest1Run(TQTest1* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtest1Run(self, tq, worker, tcon) (self)->_->run(TQTest1(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtest1RunCancelled(TQTest1* self, TaskQueue* tq, TQWorker* worker);
#define tqtest1RunCancelled(self, tq, worker) (self)->_->runCancelled(TQTest1(self), TaskQueue(tq), TQWorker(worker))
// bool tqtest1Cancel(TQTest1* self);
#define tqtest1Cancel(self) (self)->_->cancel(TQTest1(self))
// bool tqtest1Reset(TQTest1* self);
#define tqtest1Reset(self) (self)->_->reset(TQTest1(self))
// bool tqtest1Wait(TQTest1* self, int64 timeout);
#define tqtest1Wait(self, timeout) (self)->_->wait(TQTest1(self), timeout)

typedef struct TQTestFail {
    union {
        TQTestFail_ClassIf* _;
        void* _is_TQTestFail;
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
    int n;
    Event* notify;
} TQTestFail;
extern ObjClassInfo TQTestFail_clsinfo;
#define TQTestFail(inst) ((TQTestFail*)(unused_noeval((inst) && &((inst)->_is_TQTestFail)), (inst)))
#define TQTestFailNone ((TQTestFail*)NULL)

typedef struct TQTestFail_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTestFail_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTestFail_WeakRef;
#define TQTestFail_WeakRef(inst) ((TQTestFail_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTestFail_WeakRef)), (inst)))

_objfactory_guaranteed TQTestFail* TQTestFail_create(int n, Event* notify);
// TQTestFail* tqtestfailCreate(int n, Event* notify);
#define tqtestfailCreate(n, notify) TQTestFail_create(n, notify)

// bool tqtestfail_setState(TQTestFail* self, uint32 newstate);
#define tqtestfail_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtestfailRun(TQTestFail* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtestfailRun(self, tq, worker, tcon) (self)->_->run(TQTestFail(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtestfailRunCancelled(TQTestFail* self, TaskQueue* tq, TQWorker* worker);
#define tqtestfailRunCancelled(self, tq, worker) (self)->_->runCancelled(TQTestFail(self), TaskQueue(tq), TQWorker(worker))
// bool tqtestfailCancel(TQTestFail* self);
#define tqtestfailCancel(self) (self)->_->cancel(TQTestFail(self))
// bool tqtestfailReset(TQTestFail* self);
#define tqtestfailReset(self) (self)->_->reset(TQTestFail(self))
// bool tqtestfailWait(TQTestFail* self, int64 timeout);
#define tqtestfailWait(self, timeout) (self)->_->wait(TQTestFail(self), timeout)

typedef struct TQTestCC1 {
    union {
        TQTestCC1_ClassIf* _;
        void* _is_TQTestCC1;
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
    int num[2];
    TaskQueue* destq;
    int* accum;
    int* counter;
    Event* notify;
} TQTestCC1;
extern ObjClassInfo TQTestCC1_clsinfo;
#define TQTestCC1(inst) ((TQTestCC1*)(unused_noeval((inst) && &((inst)->_is_TQTestCC1)), (inst)))
#define TQTestCC1None ((TQTestCC1*)NULL)

typedef struct TQTestCC1_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTestCC1_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTestCC1_WeakRef;
#define TQTestCC1_WeakRef(inst) ((TQTestCC1_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTestCC1_WeakRef)), (inst)))

_objfactory_guaranteed TQTestCC1* TQTestCC1_create(int num1, int num2, TaskQueue* destq, int* accum, int* counter, Event* notify);
// TQTestCC1* tqtestcc1Create(int num1, int num2, TaskQueue* destq, int* accum, int* counter, Event* notify);
#define tqtestcc1Create(num1, num2, destq, accum, counter, notify) TQTestCC1_create(num1, num2, TaskQueue(destq), accum, counter, notify)

// bool tqtestcc1_setState(TQTestCC1* self, uint32 newstate);
#define tqtestcc1_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtestcc1Run(TQTestCC1* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtestcc1Run(self, tq, worker, tcon) (self)->_->run(TQTestCC1(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtestcc1RunCancelled(TQTestCC1* self, TaskQueue* tq, TQWorker* worker);
#define tqtestcc1RunCancelled(self, tq, worker) (self)->_->runCancelled(TQTestCC1(self), TaskQueue(tq), TQWorker(worker))
// bool tqtestcc1Cancel(TQTestCC1* self);
#define tqtestcc1Cancel(self) (self)->_->cancel(TQTestCC1(self))
// bool tqtestcc1Reset(TQTestCC1* self);
#define tqtestcc1Reset(self) (self)->_->reset(TQTestCC1(self))
// bool tqtestcc1Wait(TQTestCC1* self, int64 timeout);
#define tqtestcc1Wait(self, timeout) (self)->_->wait(TQTestCC1(self), timeout)

typedef struct TQTestCC2 {
    union {
        TQTestCC2_ClassIf* _;
        void* _is_TQTestCC2;
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
    int total;
    int* accum;
    int* counter;
    Event* notify;
} TQTestCC2;
extern ObjClassInfo TQTestCC2_clsinfo;
#define TQTestCC2(inst) ((TQTestCC2*)(unused_noeval((inst) && &((inst)->_is_TQTestCC2)), (inst)))
#define TQTestCC2None ((TQTestCC2*)NULL)

typedef struct TQTestCC2_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTestCC2_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTestCC2_WeakRef;
#define TQTestCC2_WeakRef(inst) ((TQTestCC2_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTestCC2_WeakRef)), (inst)))

_objfactory_guaranteed TQTestCC2* TQTestCC2_create(int total, int* accum, int* counter, Event* notify);
// TQTestCC2* tqtestcc2Create(int total, int* accum, int* counter, Event* notify);
#define tqtestcc2Create(total, accum, counter, notify) TQTestCC2_create(total, accum, counter, notify)

// bool tqtestcc2_setState(TQTestCC2* self, uint32 newstate);
#define tqtestcc2_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtestcc2Run(TQTestCC2* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtestcc2Run(self, tq, worker, tcon) (self)->_->run(TQTestCC2(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtestcc2RunCancelled(TQTestCC2* self, TaskQueue* tq, TQWorker* worker);
#define tqtestcc2RunCancelled(self, tq, worker) (self)->_->runCancelled(TQTestCC2(self), TaskQueue(tq), TQWorker(worker))
// bool tqtestcc2Cancel(TQTestCC2* self);
#define tqtestcc2Cancel(self) (self)->_->cancel(TQTestCC2(self))
// bool tqtestcc2Reset(TQTestCC2* self);
#define tqtestcc2Reset(self) (self)->_->reset(TQTestCC2(self))
// bool tqtestcc2Wait(TQTestCC2* self, int64 timeout);
#define tqtestcc2Wait(self, timeout) (self)->_->wait(TQTestCC2(self), timeout)

typedef struct TQTestSched {
    union {
        TQTestSched_ClassIf* _;
        void* _is_TQTestSched;
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
    Event* notify;
} TQTestSched;
extern ObjClassInfo TQTestSched_clsinfo;
#define TQTestSched(inst) ((TQTestSched*)(unused_noeval((inst) && &((inst)->_is_TQTestSched)), (inst)))
#define TQTestSchedNone ((TQTestSched*)NULL)

typedef struct TQTestSched_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTestSched_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTestSched_WeakRef;
#define TQTestSched_WeakRef(inst) ((TQTestSched_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTestSched_WeakRef)), (inst)))

// void tqtestschedRequireTask(TQTestSched* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqtestschedRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqtestschedRequireTaskTimeout(TQTestSched* self, Task* dep, bool failok, int64 timeout);
#define tqtestschedRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqtestschedRequireResource(TQTestSched* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqtestschedRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqtestschedRequireResourceTimeout(TQTestSched* self, TaskResource* res, int64 timeout);
#define tqtestschedRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqtestschedRequireGate(TQTestSched* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqtestschedRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqtestschedRequireGateTimeout(TQTestSched* self, TRGate* gate, int64 timeout);
#define tqtestschedRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqtestschedRequire(TQTestSched* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqtestschedRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqtestschedAdvance(TQTestSched* self);
//
// advance a deferred task to run as soon as possible
#define tqtestschedAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqtestschedCheckRequires(TQTestSched* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqtestschedCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqtestschedCancelRequires(TQTestSched* self);
//
// cascade a task cancellation to any requirements
#define tqtestschedCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqtestschedAcquireRequires(TQTestSched* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqtestschedAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqtestschedReleaseRequires(TQTestSched* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqtestschedReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqtestsched_setState(TQTestSched* self, uint32 newstate);
#define tqtestsched_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtestschedRun(TQTestSched* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtestschedRun(self, tq, worker, tcon) (self)->_->run(TQTestSched(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtestschedRunCancelled(TQTestSched* self, TaskQueue* tq, TQWorker* worker);
#define tqtestschedRunCancelled(self, tq, worker) (self)->_->runCancelled(TQTestSched(self), TaskQueue(tq), TQWorker(worker))
// bool tqtestschedCancel(TQTestSched* self);
#define tqtestschedCancel(self) (self)->_->cancel(TQTestSched(self))
// bool tqtestschedReset(TQTestSched* self);
#define tqtestschedReset(self) (self)->_->reset(TQTestSched(self))
// bool tqtestschedWait(TQTestSched* self, int64 timeout);
#define tqtestschedWait(self, timeout) (self)->_->wait(TQTestSched(self), timeout)
// intptr tqtestschedCmp(TQTestSched* self, TQTestSched* other, uint32 flags);
#define tqtestschedCmp(self, other, flags) (self)->_->cmp(TQTestSched(self), other, flags)
// uint32 tqtestschedHash(TQTestSched* self, uint32 flags);
#define tqtestschedHash(self, flags) (self)->_->hash(TQTestSched(self), flags)

typedef struct TQTestS1 {
    union {
        TQTestS1_ClassIf* _;
        void* _is_TQTestS1;
        void* _is_TQTestSched;
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
    Event* notify;
    int order;
    int64 dtime;
    int64 rantime;
} TQTestS1;
extern ObjClassInfo TQTestS1_clsinfo;
#define TQTestS1(inst) ((TQTestS1*)(unused_noeval((inst) && &((inst)->_is_TQTestS1)), (inst)))
#define TQTestS1None ((TQTestS1*)NULL)

typedef struct TQTestS1_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTestS1_WeakRef;
        void* _is_TQTestSched_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTestS1_WeakRef;
#define TQTestS1_WeakRef(inst) ((TQTestS1_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTestS1_WeakRef)), (inst)))

_objfactory_guaranteed TQTestS1* TQTestS1_create(int order, int64 dtime, Event* notify);
// TQTestS1* tqtests1Create(int order, int64 dtime, Event* notify);
#define tqtests1Create(order, dtime, notify) TQTestS1_create(order, dtime, notify)

// void tqtests1RequireTask(TQTestS1* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqtests1RequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqtests1RequireTaskTimeout(TQTestS1* self, Task* dep, bool failok, int64 timeout);
#define tqtests1RequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqtests1RequireResource(TQTestS1* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqtests1RequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqtests1RequireResourceTimeout(TQTestS1* self, TaskResource* res, int64 timeout);
#define tqtests1RequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqtests1RequireGate(TQTestS1* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqtests1RequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqtests1RequireGateTimeout(TQTestS1* self, TRGate* gate, int64 timeout);
#define tqtests1RequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqtests1Require(TQTestS1* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqtests1Require(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqtests1Advance(TQTestS1* self);
//
// advance a deferred task to run as soon as possible
#define tqtests1Advance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqtests1CheckRequires(TQTestS1* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqtests1CheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqtests1CancelRequires(TQTestS1* self);
//
// cascade a task cancellation to any requirements
#define tqtests1CancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqtests1AcquireRequires(TQTestS1* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqtests1AcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqtests1ReleaseRequires(TQTestS1* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqtests1ReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqtests1_setState(TQTestS1* self, uint32 newstate);
#define tqtests1_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtests1Run(TQTestS1* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtests1Run(self, tq, worker, tcon) (self)->_->run(TQTestS1(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtests1RunCancelled(TQTestS1* self, TaskQueue* tq, TQWorker* worker);
#define tqtests1RunCancelled(self, tq, worker) (self)->_->runCancelled(TQTestS1(self), TaskQueue(tq), TQWorker(worker))
// bool tqtests1Cancel(TQTestS1* self);
#define tqtests1Cancel(self) (self)->_->cancel(TQTestS1(self))
// bool tqtests1Reset(TQTestS1* self);
#define tqtests1Reset(self) (self)->_->reset(TQTestS1(self))
// bool tqtests1Wait(TQTestS1* self, int64 timeout);
#define tqtests1Wait(self, timeout) (self)->_->wait(TQTestS1(self), timeout)
// intptr tqtests1Cmp(TQTestS1* self, TQTestS1* other, uint32 flags);
#define tqtests1Cmp(self, other, flags) (self)->_->cmp(TQTestS1(self), other, flags)
// uint32 tqtests1Hash(TQTestS1* self, uint32 flags);
#define tqtests1Hash(self, flags) (self)->_->hash(TQTestS1(self), flags)

typedef struct TQTestS2 {
    union {
        TQTestS2_ClassIf* _;
        void* _is_TQTestS2;
        void* _is_TQTestSched;
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
    Event* notify;
    Task* waitfor;
} TQTestS2;
extern ObjClassInfo TQTestS2_clsinfo;
#define TQTestS2(inst) ((TQTestS2*)(unused_noeval((inst) && &((inst)->_is_TQTestS2)), (inst)))
#define TQTestS2None ((TQTestS2*)NULL)

typedef struct TQTestS2_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTestS2_WeakRef;
        void* _is_TQTestSched_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTestS2_WeakRef;
#define TQTestS2_WeakRef(inst) ((TQTestS2_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTestS2_WeakRef)), (inst)))

_objfactory_guaranteed TQTestS2* TQTestS2_create(Task* waitfor, Event* notify);
// TQTestS2* tqtests2Create(Task* waitfor, Event* notify);
#define tqtests2Create(waitfor, notify) TQTestS2_create(Task(waitfor), notify)

// void tqtests2RequireTask(TQTestS2* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqtests2RequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqtests2RequireTaskTimeout(TQTestS2* self, Task* dep, bool failok, int64 timeout);
#define tqtests2RequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqtests2RequireResource(TQTestS2* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqtests2RequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqtests2RequireResourceTimeout(TQTestS2* self, TaskResource* res, int64 timeout);
#define tqtests2RequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqtests2RequireGate(TQTestS2* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqtests2RequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqtests2RequireGateTimeout(TQTestS2* self, TRGate* gate, int64 timeout);
#define tqtests2RequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqtests2Require(TQTestS2* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqtests2Require(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqtests2Advance(TQTestS2* self);
//
// advance a deferred task to run as soon as possible
#define tqtests2Advance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqtests2CheckRequires(TQTestS2* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqtests2CheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqtests2CancelRequires(TQTestS2* self);
//
// cascade a task cancellation to any requirements
#define tqtests2CancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqtests2AcquireRequires(TQTestS2* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqtests2AcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqtests2ReleaseRequires(TQTestS2* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqtests2ReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqtests2_setState(TQTestS2* self, uint32 newstate);
#define tqtests2_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtests2Run(TQTestS2* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtests2Run(self, tq, worker, tcon) (self)->_->run(TQTestS2(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtests2RunCancelled(TQTestS2* self, TaskQueue* tq, TQWorker* worker);
#define tqtests2RunCancelled(self, tq, worker) (self)->_->runCancelled(TQTestS2(self), TaskQueue(tq), TQWorker(worker))
// bool tqtests2Cancel(TQTestS2* self);
#define tqtests2Cancel(self) (self)->_->cancel(TQTestS2(self))
// bool tqtests2Reset(TQTestS2* self);
#define tqtests2Reset(self) (self)->_->reset(TQTestS2(self))
// bool tqtests2Wait(TQTestS2* self, int64 timeout);
#define tqtests2Wait(self, timeout) (self)->_->wait(TQTestS2(self), timeout)
// intptr tqtests2Cmp(TQTestS2* self, TQTestS2* other, uint32 flags);
#define tqtests2Cmp(self, other, flags) (self)->_->cmp(TQTestS2(self), other, flags)
// uint32 tqtests2Hash(TQTestS2* self, uint32 flags);
#define tqtests2Hash(self, flags) (self)->_->hash(TQTestS2(self), flags)

typedef struct TQDelayTest {
    union {
        TQDelayTest_ClassIf* _;
        void* _is_TQDelayTest;
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
    int64 len;
} TQDelayTest;
extern ObjClassInfo TQDelayTest_clsinfo;
#define TQDelayTest(inst) ((TQDelayTest*)(unused_noeval((inst) && &((inst)->_is_TQDelayTest)), (inst)))
#define TQDelayTestNone ((TQDelayTest*)NULL)

typedef struct TQDelayTest_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQDelayTest_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQDelayTest_WeakRef;
#define TQDelayTest_WeakRef(inst) ((TQDelayTest_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQDelayTest_WeakRef)), (inst)))

_objfactory_guaranteed TQDelayTest* TQDelayTest_create(int64 len);
// TQDelayTest* tqdelaytestCreate(int64 len);
#define tqdelaytestCreate(len) TQDelayTest_create(len)

// bool tqdelaytest_setState(TQDelayTest* self, uint32 newstate);
#define tqdelaytest_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqdelaytestRun(TQDelayTest* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqdelaytestRun(self, tq, worker, tcon) (self)->_->run(TQDelayTest(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqdelaytestRunCancelled(TQDelayTest* self, TaskQueue* tq, TQWorker* worker);
#define tqdelaytestRunCancelled(self, tq, worker) (self)->_->runCancelled(TQDelayTest(self), TaskQueue(tq), TQWorker(worker))
// bool tqdelaytestCancel(TQDelayTest* self);
#define tqdelaytestCancel(self) (self)->_->cancel(TQDelayTest(self))
// bool tqdelaytestReset(TQDelayTest* self);
#define tqdelaytestReset(self) (self)->_->reset(TQDelayTest(self))
// bool tqdelaytestWait(TQDelayTest* self, int64 timeout);
#define tqdelaytestWait(self, timeout) (self)->_->wait(TQDelayTest(self), timeout)

typedef struct TQMTest {
    union {
        TQMTest_ClassIf* _;
        void* _is_TQMTest;
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
    Event* notify;
} TQMTest;
extern ObjClassInfo TQMTest_clsinfo;
#define TQMTest(inst) ((TQMTest*)(unused_noeval((inst) && &((inst)->_is_TQMTest)), (inst)))
#define TQMTestNone ((TQMTest*)NULL)

typedef struct TQMTest_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQMTest_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQMTest_WeakRef;
#define TQMTest_WeakRef(inst) ((TQMTest_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQMTest_WeakRef)), (inst)))

_objfactory_guaranteed TQMTest* TQMTest_create(Event* notify);
// TQMTest* tqmtestCreate(Event* notify);
#define tqmtestCreate(notify) TQMTest_create(notify)

// void tqmtestRequireTask(TQMTest* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqmtestRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqmtestRequireTaskTimeout(TQMTest* self, Task* dep, bool failok, int64 timeout);
#define tqmtestRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqmtestRequireResource(TQMTest* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqmtestRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqmtestRequireResourceTimeout(TQMTest* self, TaskResource* res, int64 timeout);
#define tqmtestRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqmtestRequireGate(TQMTest* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqmtestRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqmtestRequireGateTimeout(TQMTest* self, TRGate* gate, int64 timeout);
#define tqmtestRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqmtestRequire(TQMTest* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqmtestRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqmtestAdvance(TQMTest* self);
//
// advance a deferred task to run as soon as possible
#define tqmtestAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqmtestCheckRequires(TQMTest* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqmtestCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqmtestCancelRequires(TQMTest* self);
//
// cascade a task cancellation to any requirements
#define tqmtestCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqmtestAcquireRequires(TQMTest* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqmtestAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqmtestReleaseRequires(TQMTest* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqmtestReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqmtest_setState(TQMTest* self, uint32 newstate);
#define tqmtest_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqmtestRun(TQMTest* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqmtestRun(self, tq, worker, tcon) (self)->_->run(TQMTest(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqmtestRunCancelled(TQMTest* self, TaskQueue* tq, TQWorker* worker);
#define tqmtestRunCancelled(self, tq, worker) (self)->_->runCancelled(TQMTest(self), TaskQueue(tq), TQWorker(worker))
// bool tqmtestCancel(TQMTest* self);
#define tqmtestCancel(self) (self)->_->cancel(TQMTest(self))
// bool tqmtestReset(TQMTest* self);
#define tqmtestReset(self) (self)->_->reset(TQMTest(self))
// bool tqmtestWait(TQMTest* self, int64 timeout);
#define tqmtestWait(self, timeout) (self)->_->wait(TQMTest(self), timeout)
// intptr tqmtestCmp(TQMTest* self, TQMTest* other, uint32 flags);
#define tqmtestCmp(self, other, flags) (self)->_->cmp(TQMTest(self), other, flags)
// uint32 tqmtestHash(TQMTest* self, uint32 flags);
#define tqmtestHash(self, flags) (self)->_->hash(TQMTest(self), flags)

typedef struct TQRTestMtx {
    union {
        TQRTestMtx_ClassIf* _;
        void* _is_TQRTestMtx;
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
    ReqTestState* rts;
    int num;
} TQRTestMtx;
extern ObjClassInfo TQRTestMtx_clsinfo;
#define TQRTestMtx(inst) ((TQRTestMtx*)(unused_noeval((inst) && &((inst)->_is_TQRTestMtx)), (inst)))
#define TQRTestMtxNone ((TQRTestMtx*)NULL)

typedef struct TQRTestMtx_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQRTestMtx_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQRTestMtx_WeakRef;
#define TQRTestMtx_WeakRef(inst) ((TQRTestMtx_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQRTestMtx_WeakRef)), (inst)))

_objfactory_guaranteed TQRTestMtx* TQRTestMtx_create(ReqTestState* rts, int num);
// TQRTestMtx* tqrtestmtxCreate(ReqTestState* rts, int num);
#define tqrtestmtxCreate(rts, num) TQRTestMtx_create(rts, num)

// void tqrtestmtxRequireTask(TQRTestMtx* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqrtestmtxRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqrtestmtxRequireTaskTimeout(TQRTestMtx* self, Task* dep, bool failok, int64 timeout);
#define tqrtestmtxRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqrtestmtxRequireResource(TQRTestMtx* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqrtestmtxRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqrtestmtxRequireResourceTimeout(TQRTestMtx* self, TaskResource* res, int64 timeout);
#define tqrtestmtxRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqrtestmtxRequireGate(TQRTestMtx* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqrtestmtxRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqrtestmtxRequireGateTimeout(TQRTestMtx* self, TRGate* gate, int64 timeout);
#define tqrtestmtxRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqrtestmtxRequire(TQRTestMtx* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqrtestmtxRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqrtestmtxAdvance(TQRTestMtx* self);
//
// advance a deferred task to run as soon as possible
#define tqrtestmtxAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqrtestmtxCheckRequires(TQRTestMtx* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqrtestmtxCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqrtestmtxCancelRequires(TQRTestMtx* self);
//
// cascade a task cancellation to any requirements
#define tqrtestmtxCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqrtestmtxAcquireRequires(TQRTestMtx* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqrtestmtxAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqrtestmtxReleaseRequires(TQRTestMtx* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqrtestmtxReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqrtestmtx_setState(TQRTestMtx* self, uint32 newstate);
#define tqrtestmtx_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqrtestmtxRun(TQRTestMtx* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqrtestmtxRun(self, tq, worker, tcon) (self)->_->run(TQRTestMtx(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqrtestmtxRunCancelled(TQRTestMtx* self, TaskQueue* tq, TQWorker* worker);
#define tqrtestmtxRunCancelled(self, tq, worker) (self)->_->runCancelled(TQRTestMtx(self), TaskQueue(tq), TQWorker(worker))
// bool tqrtestmtxCancel(TQRTestMtx* self);
#define tqrtestmtxCancel(self) (self)->_->cancel(TQRTestMtx(self))
// bool tqrtestmtxReset(TQRTestMtx* self);
#define tqrtestmtxReset(self) (self)->_->reset(TQRTestMtx(self))
// bool tqrtestmtxWait(TQRTestMtx* self, int64 timeout);
#define tqrtestmtxWait(self, timeout) (self)->_->wait(TQRTestMtx(self), timeout)
// intptr tqrtestmtxCmp(TQRTestMtx* self, TQRTestMtx* other, uint32 flags);
#define tqrtestmtxCmp(self, other, flags) (self)->_->cmp(TQRTestMtx(self), other, flags)
// uint32 tqrtestmtxHash(TQRTestMtx* self, uint32 flags);
#define tqrtestmtxHash(self, flags) (self)->_->hash(TQRTestMtx(self), flags)

typedef struct TQRTestFifo {
    union {
        TQRTestFifo_ClassIf* _;
        void* _is_TQRTestFifo;
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
    ReqTestState* rts;
    int seq;
    int num;
} TQRTestFifo;
extern ObjClassInfo TQRTestFifo_clsinfo;
#define TQRTestFifo(inst) ((TQRTestFifo*)(unused_noeval((inst) && &((inst)->_is_TQRTestFifo)), (inst)))
#define TQRTestFifoNone ((TQRTestFifo*)NULL)

typedef struct TQRTestFifo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQRTestFifo_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQRTestFifo_WeakRef;
#define TQRTestFifo_WeakRef(inst) ((TQRTestFifo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQRTestFifo_WeakRef)), (inst)))

_objfactory_guaranteed TQRTestFifo* TQRTestFifo_create(ReqTestState* rts, int seq, int num);
// TQRTestFifo* tqrtestfifoCreate(ReqTestState* rts, int seq, int num);
#define tqrtestfifoCreate(rts, seq, num) TQRTestFifo_create(rts, seq, num)

// void tqrtestfifoRequireTask(TQRTestFifo* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqrtestfifoRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqrtestfifoRequireTaskTimeout(TQRTestFifo* self, Task* dep, bool failok, int64 timeout);
#define tqrtestfifoRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqrtestfifoRequireResource(TQRTestFifo* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqrtestfifoRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqrtestfifoRequireResourceTimeout(TQRTestFifo* self, TaskResource* res, int64 timeout);
#define tqrtestfifoRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqrtestfifoRequireGate(TQRTestFifo* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqrtestfifoRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqrtestfifoRequireGateTimeout(TQRTestFifo* self, TRGate* gate, int64 timeout);
#define tqrtestfifoRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqrtestfifoRequire(TQRTestFifo* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqrtestfifoRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqrtestfifoAdvance(TQRTestFifo* self);
//
// advance a deferred task to run as soon as possible
#define tqrtestfifoAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqrtestfifoCheckRequires(TQRTestFifo* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqrtestfifoCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqrtestfifoCancelRequires(TQRTestFifo* self);
//
// cascade a task cancellation to any requirements
#define tqrtestfifoCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqrtestfifoAcquireRequires(TQRTestFifo* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqrtestfifoAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqrtestfifoReleaseRequires(TQRTestFifo* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqrtestfifoReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqrtestfifo_setState(TQRTestFifo* self, uint32 newstate);
#define tqrtestfifo_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqrtestfifoRun(TQRTestFifo* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqrtestfifoRun(self, tq, worker, tcon) (self)->_->run(TQRTestFifo(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqrtestfifoRunCancelled(TQRTestFifo* self, TaskQueue* tq, TQWorker* worker);
#define tqrtestfifoRunCancelled(self, tq, worker) (self)->_->runCancelled(TQRTestFifo(self), TaskQueue(tq), TQWorker(worker))
// bool tqrtestfifoCancel(TQRTestFifo* self);
#define tqrtestfifoCancel(self) (self)->_->cancel(TQRTestFifo(self))
// bool tqrtestfifoReset(TQRTestFifo* self);
#define tqrtestfifoReset(self) (self)->_->reset(TQRTestFifo(self))
// bool tqrtestfifoWait(TQRTestFifo* self, int64 timeout);
#define tqrtestfifoWait(self, timeout) (self)->_->wait(TQRTestFifo(self), timeout)
// intptr tqrtestfifoCmp(TQRTestFifo* self, TQRTestFifo* other, uint32 flags);
#define tqrtestfifoCmp(self, other, flags) (self)->_->cmp(TQRTestFifo(self), other, flags)
// uint32 tqrtestfifoHash(TQRTestFifo* self, uint32 flags);
#define tqrtestfifoHash(self, flags) (self)->_->hash(TQRTestFifo(self), flags)

typedef struct TQRTestLifo {
    union {
        TQRTestLifo_ClassIf* _;
        void* _is_TQRTestLifo;
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
    ReqTestState* rts;
    int seq;
    int num;
} TQRTestLifo;
extern ObjClassInfo TQRTestLifo_clsinfo;
#define TQRTestLifo(inst) ((TQRTestLifo*)(unused_noeval((inst) && &((inst)->_is_TQRTestLifo)), (inst)))
#define TQRTestLifoNone ((TQRTestLifo*)NULL)

typedef struct TQRTestLifo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQRTestLifo_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQRTestLifo_WeakRef;
#define TQRTestLifo_WeakRef(inst) ((TQRTestLifo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQRTestLifo_WeakRef)), (inst)))

_objfactory_guaranteed TQRTestLifo* TQRTestLifo_create(ReqTestState* rts, int seq, int num);
// TQRTestLifo* tqrtestlifoCreate(ReqTestState* rts, int seq, int num);
#define tqrtestlifoCreate(rts, seq, num) TQRTestLifo_create(rts, seq, num)

// void tqrtestlifoRequireTask(TQRTestLifo* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqrtestlifoRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqrtestlifoRequireTaskTimeout(TQRTestLifo* self, Task* dep, bool failok, int64 timeout);
#define tqrtestlifoRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqrtestlifoRequireResource(TQRTestLifo* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqrtestlifoRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqrtestlifoRequireResourceTimeout(TQRTestLifo* self, TaskResource* res, int64 timeout);
#define tqrtestlifoRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqrtestlifoRequireGate(TQRTestLifo* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqrtestlifoRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqrtestlifoRequireGateTimeout(TQRTestLifo* self, TRGate* gate, int64 timeout);
#define tqrtestlifoRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqrtestlifoRequire(TQRTestLifo* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqrtestlifoRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqrtestlifoAdvance(TQRTestLifo* self);
//
// advance a deferred task to run as soon as possible
#define tqrtestlifoAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqrtestlifoCheckRequires(TQRTestLifo* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqrtestlifoCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqrtestlifoCancelRequires(TQRTestLifo* self);
//
// cascade a task cancellation to any requirements
#define tqrtestlifoCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqrtestlifoAcquireRequires(TQRTestLifo* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqrtestlifoAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqrtestlifoReleaseRequires(TQRTestLifo* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqrtestlifoReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqrtestlifo_setState(TQRTestLifo* self, uint32 newstate);
#define tqrtestlifo_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqrtestlifoRun(TQRTestLifo* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqrtestlifoRun(self, tq, worker, tcon) (self)->_->run(TQRTestLifo(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqrtestlifoRunCancelled(TQRTestLifo* self, TaskQueue* tq, TQWorker* worker);
#define tqrtestlifoRunCancelled(self, tq, worker) (self)->_->runCancelled(TQRTestLifo(self), TaskQueue(tq), TQWorker(worker))
// bool tqrtestlifoCancel(TQRTestLifo* self);
#define tqrtestlifoCancel(self) (self)->_->cancel(TQRTestLifo(self))
// bool tqrtestlifoReset(TQRTestLifo* self);
#define tqrtestlifoReset(self) (self)->_->reset(TQRTestLifo(self))
// bool tqrtestlifoWait(TQRTestLifo* self, int64 timeout);
#define tqrtestlifoWait(self, timeout) (self)->_->wait(TQRTestLifo(self), timeout)
// intptr tqrtestlifoCmp(TQRTestLifo* self, TQRTestLifo* other, uint32 flags);
#define tqrtestlifoCmp(self, other, flags) (self)->_->cmp(TQRTestLifo(self), other, flags)
// uint32 tqrtestlifoHash(TQRTestLifo* self, uint32 flags);
#define tqrtestlifoHash(self, flags) (self)->_->hash(TQRTestLifo(self), flags)

typedef struct TQRTestGate {
    union {
        TQRTestGate_ClassIf* _;
        void* _is_TQRTestGate;
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
    ReqTestState2* rts;
    int num;
} TQRTestGate;
extern ObjClassInfo TQRTestGate_clsinfo;
#define TQRTestGate(inst) ((TQRTestGate*)(unused_noeval((inst) && &((inst)->_is_TQRTestGate)), (inst)))
#define TQRTestGateNone ((TQRTestGate*)NULL)

typedef struct TQRTestGate_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQRTestGate_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQRTestGate_WeakRef;
#define TQRTestGate_WeakRef(inst) ((TQRTestGate_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQRTestGate_WeakRef)), (inst)))

_objfactory_guaranteed TQRTestGate* TQRTestGate_create(ReqTestState2* rts, int num);
// TQRTestGate* tqrtestgateCreate(ReqTestState2* rts, int num);
#define tqrtestgateCreate(rts, num) TQRTestGate_create(rts, num)

// void tqrtestgateRequireTask(TQRTestGate* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqrtestgateRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqrtestgateRequireTaskTimeout(TQRTestGate* self, Task* dep, bool failok, int64 timeout);
#define tqrtestgateRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqrtestgateRequireResource(TQRTestGate* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqrtestgateRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqrtestgateRequireResourceTimeout(TQRTestGate* self, TaskResource* res, int64 timeout);
#define tqrtestgateRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqrtestgateRequireGate(TQRTestGate* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqrtestgateRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqrtestgateRequireGateTimeout(TQRTestGate* self, TRGate* gate, int64 timeout);
#define tqrtestgateRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqrtestgateRequire(TQRTestGate* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqrtestgateRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqrtestgateAdvance(TQRTestGate* self);
//
// advance a deferred task to run as soon as possible
#define tqrtestgateAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqrtestgateCheckRequires(TQRTestGate* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqrtestgateCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqrtestgateCancelRequires(TQRTestGate* self);
//
// cascade a task cancellation to any requirements
#define tqrtestgateCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqrtestgateAcquireRequires(TQRTestGate* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqrtestgateAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqrtestgateReleaseRequires(TQRTestGate* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqrtestgateReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqrtestgate_setState(TQRTestGate* self, uint32 newstate);
#define tqrtestgate_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqrtestgateRun(TQRTestGate* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqrtestgateRun(self, tq, worker, tcon) (self)->_->run(TQRTestGate(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqrtestgateRunCancelled(TQRTestGate* self, TaskQueue* tq, TQWorker* worker);
#define tqrtestgateRunCancelled(self, tq, worker) (self)->_->runCancelled(TQRTestGate(self), TaskQueue(tq), TQWorker(worker))
// bool tqrtestgateCancel(TQRTestGate* self);
#define tqrtestgateCancel(self) (self)->_->cancel(TQRTestGate(self))
// bool tqrtestgateReset(TQRTestGate* self);
#define tqrtestgateReset(self) (self)->_->reset(TQRTestGate(self))
// bool tqrtestgateWait(TQRTestGate* self, int64 timeout);
#define tqrtestgateWait(self, timeout) (self)->_->wait(TQRTestGate(self), timeout)
// intptr tqrtestgateCmp(TQRTestGate* self, TQRTestGate* other, uint32 flags);
#define tqrtestgateCmp(self, other, flags) (self)->_->cmp(TQRTestGate(self), other, flags)
// uint32 tqrtestgateHash(TQRTestGate* self, uint32 flags);
#define tqrtestgateHash(self, flags) (self)->_->hash(TQRTestGate(self), flags)

typedef struct TQMPTest {
    union {
        TQMPTest_ClassIf* _;
        void* _is_TQMPTest;
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
    MPTestState* mps;
    int idx;
    int variant;
} TQMPTest;
extern ObjClassInfo TQMPTest_clsinfo;
#define TQMPTest(inst) ((TQMPTest*)(unused_noeval((inst) && &((inst)->_is_TQMPTest)), (inst)))
#define TQMPTestNone ((TQMPTest*)NULL)

typedef struct TQMPTest_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQMPTest_WeakRef;
        void* _is_MultiphaseTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQMPTest_WeakRef;
#define TQMPTest_WeakRef(inst) ((TQMPTest_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQMPTest_WeakRef)), (inst)))

_objfactory_guaranteed TQMPTest* TQMPTest_create(int variant, int idx, MPTestState* mps);
// TQMPTest* tqmptestCreate(int variant, int idx, MPTestState* mps);
#define tqmptestCreate(variant, idx, mps) TQMPTest_create(variant, idx, mps)

// void tqmptest_addPhases(TQMPTest* self, int32 num, MPTPhaseFunc parr[], bool fail);
//
// Adds phases from a static array.
#define tqmptest_addPhases(self, num, parr, fail) MultiphaseTask__addPhases(MultiphaseTask(self), num, parr, fail)

// void tqmptestRequireTask(TQMPTest* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqmptestRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqmptestRequireTaskTimeout(TQMPTest* self, Task* dep, bool failok, int64 timeout);
#define tqmptestRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqmptestRequireResource(TQMPTest* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqmptestRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqmptestRequireResourceTimeout(TQMPTest* self, TaskResource* res, int64 timeout);
#define tqmptestRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqmptestRequireGate(TQMPTest* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqmptestRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqmptestRequireGateTimeout(TQMPTest* self, TRGate* gate, int64 timeout);
#define tqmptestRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqmptestRequire(TQMPTest* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqmptestRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqmptestAdvance(TQMPTest* self);
//
// advance a deferred task to run as soon as possible
#define tqmptestAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqmptestCheckRequires(TQMPTest* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqmptestCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqmptestCancelRequires(TQMPTest* self);
//
// cascade a task cancellation to any requirements
#define tqmptestCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqmptestAcquireRequires(TQMPTest* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqmptestAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqmptestReleaseRequires(TQMPTest* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqmptestReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqmptest_setState(TQMPTest* self, uint32 newstate);
#define tqmptest_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqmptestRun(TQMPTest* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqmptestRun(self, tq, worker, tcon) (self)->_->run(TQMPTest(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqmptestRunCancelled(TQMPTest* self, TaskQueue* tq, TQWorker* worker);
#define tqmptestRunCancelled(self, tq, worker) (self)->_->runCancelled(TQMPTest(self), TaskQueue(tq), TQWorker(worker))
// bool tqmptestCancel(TQMPTest* self);
#define tqmptestCancel(self) (self)->_->cancel(TQMPTest(self))
// bool tqmptestReset(TQMPTest* self);
#define tqmptestReset(self) (self)->_->reset(TQMPTest(self))
// bool tqmptestWait(TQMPTest* self, int64 timeout);
#define tqmptestWait(self, timeout) (self)->_->wait(TQMPTest(self), timeout)
// intptr tqmptestCmp(TQMPTest* self, TQMPTest* other, uint32 flags);
#define tqmptestCmp(self, other, flags) (self)->_->cmp(TQMPTest(self), other, flags)
// uint32 tqmptestHash(TQMPTest* self, uint32 flags);
#define tqmptestHash(self, flags) (self)->_->hash(TQMPTest(self), flags)
// uint32 tqmptestFinish(TQMPTest* self, uint32 result, TaskControl* tcon);
//
// Called once all phases (including fail phases) have completed. May be overridden to perform
// additional cleanup or change the final result.
#define tqmptestFinish(self, result, tcon) (self)->_->finish(TQMPTest(self), result, tcon)

typedef struct TQTimeoutTest1 {
    union {
        TQTimeoutTest1_ClassIf* _;
        void* _is_TQTimeoutTest1;
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
    int count;
} TQTimeoutTest1;
extern ObjClassInfo TQTimeoutTest1_clsinfo;
#define TQTimeoutTest1(inst) ((TQTimeoutTest1*)(unused_noeval((inst) && &((inst)->_is_TQTimeoutTest1)), (inst)))
#define TQTimeoutTest1None ((TQTimeoutTest1*)NULL)

typedef struct TQTimeoutTest1_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTimeoutTest1_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTimeoutTest1_WeakRef;
#define TQTimeoutTest1_WeakRef(inst) ((TQTimeoutTest1_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTimeoutTest1_WeakRef)), (inst)))

_objfactory_guaranteed TQTimeoutTest1* TQTimeoutTest1_create();
// TQTimeoutTest1* tqtimeouttest1Create();
#define tqtimeouttest1Create() TQTimeoutTest1_create()

// void tqtimeouttest1RequireTask(TQTimeoutTest1* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqtimeouttest1RequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqtimeouttest1RequireTaskTimeout(TQTimeoutTest1* self, Task* dep, bool failok, int64 timeout);
#define tqtimeouttest1RequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqtimeouttest1RequireResource(TQTimeoutTest1* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqtimeouttest1RequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqtimeouttest1RequireResourceTimeout(TQTimeoutTest1* self, TaskResource* res, int64 timeout);
#define tqtimeouttest1RequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqtimeouttest1RequireGate(TQTimeoutTest1* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqtimeouttest1RequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqtimeouttest1RequireGateTimeout(TQTimeoutTest1* self, TRGate* gate, int64 timeout);
#define tqtimeouttest1RequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqtimeouttest1Require(TQTimeoutTest1* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqtimeouttest1Require(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqtimeouttest1Advance(TQTimeoutTest1* self);
//
// advance a deferred task to run as soon as possible
#define tqtimeouttest1Advance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqtimeouttest1CheckRequires(TQTimeoutTest1* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqtimeouttest1CheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqtimeouttest1CancelRequires(TQTimeoutTest1* self);
//
// cascade a task cancellation to any requirements
#define tqtimeouttest1CancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqtimeouttest1AcquireRequires(TQTimeoutTest1* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqtimeouttest1AcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqtimeouttest1ReleaseRequires(TQTimeoutTest1* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqtimeouttest1ReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqtimeouttest1_setState(TQTimeoutTest1* self, uint32 newstate);
#define tqtimeouttest1_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtimeouttest1Run(TQTimeoutTest1* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtimeouttest1Run(self, tq, worker, tcon) (self)->_->run(TQTimeoutTest1(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtimeouttest1RunCancelled(TQTimeoutTest1* self, TaskQueue* tq, TQWorker* worker);
#define tqtimeouttest1RunCancelled(self, tq, worker) (self)->_->runCancelled(TQTimeoutTest1(self), TaskQueue(tq), TQWorker(worker))
// bool tqtimeouttest1Cancel(TQTimeoutTest1* self);
#define tqtimeouttest1Cancel(self) (self)->_->cancel(TQTimeoutTest1(self))
// bool tqtimeouttest1Reset(TQTimeoutTest1* self);
#define tqtimeouttest1Reset(self) (self)->_->reset(TQTimeoutTest1(self))
// bool tqtimeouttest1Wait(TQTimeoutTest1* self, int64 timeout);
#define tqtimeouttest1Wait(self, timeout) (self)->_->wait(TQTimeoutTest1(self), timeout)
// intptr tqtimeouttest1Cmp(TQTimeoutTest1* self, TQTimeoutTest1* other, uint32 flags);
#define tqtimeouttest1Cmp(self, other, flags) (self)->_->cmp(TQTimeoutTest1(self), other, flags)
// uint32 tqtimeouttest1Hash(TQTimeoutTest1* self, uint32 flags);
#define tqtimeouttest1Hash(self, flags) (self)->_->hash(TQTimeoutTest1(self), flags)

typedef struct TQTimeoutTest2 {
    union {
        TQTimeoutTest2_ClassIf* _;
        void* _is_TQTimeoutTest2;
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
    ReqTestState* rts;
} TQTimeoutTest2;
extern ObjClassInfo TQTimeoutTest2_clsinfo;
#define TQTimeoutTest2(inst) ((TQTimeoutTest2*)(unused_noeval((inst) && &((inst)->_is_TQTimeoutTest2)), (inst)))
#define TQTimeoutTest2None ((TQTimeoutTest2*)NULL)

typedef struct TQTimeoutTest2_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQTimeoutTest2_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQTimeoutTest2_WeakRef;
#define TQTimeoutTest2_WeakRef(inst) ((TQTimeoutTest2_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQTimeoutTest2_WeakRef)), (inst)))

_objfactory_guaranteed TQTimeoutTest2* TQTimeoutTest2_create(ReqTestState* rts);
// TQTimeoutTest2* tqtimeouttest2Create(ReqTestState* rts);
#define tqtimeouttest2Create(rts) TQTimeoutTest2_create(rts)

// void tqtimeouttest2RequireTask(TQTimeoutTest2* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define tqtimeouttest2RequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void tqtimeouttest2RequireTaskTimeout(TQTimeoutTest2* self, Task* dep, bool failok, int64 timeout);
#define tqtimeouttest2RequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void tqtimeouttest2RequireResource(TQTimeoutTest2* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define tqtimeouttest2RequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void tqtimeouttest2RequireResourceTimeout(TQTimeoutTest2* self, TaskResource* res, int64 timeout);
#define tqtimeouttest2RequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void tqtimeouttest2RequireGate(TQTimeoutTest2* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define tqtimeouttest2RequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void tqtimeouttest2RequireGateTimeout(TQTimeoutTest2* self, TRGate* gate, int64 timeout);
#define tqtimeouttest2RequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void tqtimeouttest2Require(TQTimeoutTest2* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define tqtimeouttest2Require(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool tqtimeouttest2Advance(TQTimeoutTest2* self);
//
// advance a deferred task to run as soon as possible
#define tqtimeouttest2Advance(self) ComplexTask_advance(ComplexTask(self))

// uint32 tqtimeouttest2CheckRequires(TQTimeoutTest2* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define tqtimeouttest2CheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void tqtimeouttest2CancelRequires(TQTimeoutTest2* self);
//
// cascade a task cancellation to any requirements
#define tqtimeouttest2CancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool tqtimeouttest2AcquireRequires(TQTimeoutTest2* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define tqtimeouttest2AcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool tqtimeouttest2ReleaseRequires(TQTimeoutTest2* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define tqtimeouttest2ReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool tqtimeouttest2_setState(TQTimeoutTest2* self, uint32 newstate);
#define tqtimeouttest2_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 tqtimeouttest2Run(TQTimeoutTest2* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define tqtimeouttest2Run(self, tq, worker, tcon) (self)->_->run(TQTimeoutTest2(self), TaskQueue(tq), TQWorker(worker), tcon)
// void tqtimeouttest2RunCancelled(TQTimeoutTest2* self, TaskQueue* tq, TQWorker* worker);
#define tqtimeouttest2RunCancelled(self, tq, worker) (self)->_->runCancelled(TQTimeoutTest2(self), TaskQueue(tq), TQWorker(worker))
// bool tqtimeouttest2Cancel(TQTimeoutTest2* self);
#define tqtimeouttest2Cancel(self) (self)->_->cancel(TQTimeoutTest2(self))
// bool tqtimeouttest2Reset(TQTimeoutTest2* self);
#define tqtimeouttest2Reset(self) (self)->_->reset(TQTimeoutTest2(self))
// bool tqtimeouttest2Wait(TQTimeoutTest2* self, int64 timeout);
#define tqtimeouttest2Wait(self, timeout) (self)->_->wait(TQTimeoutTest2(self), timeout)
// intptr tqtimeouttest2Cmp(TQTimeoutTest2* self, TQTimeoutTest2* other, uint32 flags);
#define tqtimeouttest2Cmp(self, other, flags) (self)->_->cmp(TQTimeoutTest2(self), other, flags)
// uint32 tqtimeouttest2Hash(TQTimeoutTest2* self, uint32 flags);
#define tqtimeouttest2Hash(self, flags) (self)->_->hash(TQTimeoutTest2(self), flags)

