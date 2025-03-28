// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqtestobj.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue.h>

_objfactory_guaranteed TQTest1* TQTest1_create(int num1, int num2, Event* notify)
{
    TQTest1 *self;
    self = objInstCreate(TQTest1);

    self->name = _S"TQTest1";
    self->num[0] = num1;
    self->num[1] = num2;
    self->notify = notify;

    objInstInit(self);

    return self;
}

uint32 TQTest1_run(_In_ TQTest1* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    self->total = self->num[0] + self->num[1];
    tcon->notifyev = self->notify;

    return TASK_Result_Success;
}

_objfactory_guaranteed TQTestFail* TQTestFail_create(int n, Event* notify)
{
    TQTestFail *self;
    self = objInstCreate(TQTestFail);

    self->name = _S"TQTestFail";
    self->n = n;
    self->notify = notify;

    objInstInit(self);

    return self;
}

uint32 TQTestFail_run(_In_ TQTestFail* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    tcon->notifyev = self->notify;
    return self->n & 1 ? TASK_Result_Success :
                         TASK_Result_Failure;   // odd will succeed, even will fail
}

_objfactory_guaranteed TQTestCC1* TQTestCC1_create(int num1, int num2, TaskQueue* destq, int* accum, int* counter, Event* notify)
{
    TQTestCC1 *self;
    self = objInstCreate(TQTestCC1);

    self->name = _S"TQTestCC1";
    self->num[0] = num1;
    self->num[1] = num2;
    self->destq = destq;
    self->accum = accum;
    self->counter = counter;
    self->notify = notify;

    objInstInit(self);

    return self;
}

uint32 TQTestCC1_run(_In_ TQTestCC1* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    TQTestCC2 *task = tqtestcc2Create(self->num[0] + self->num[1], self->accum, self->counter, self->notify);
    tqRun(self->destq, &task);
    return TASK_Result_Success;
}

_objfactory_guaranteed TQTestCC2* TQTestCC2_create(int total, int* accum, int* counter, Event* notify)
{
    TQTestCC2 *self;
    self = objInstCreate(TQTestCC2);

    self->name = _S"TQTestCC2";
    self->total = total;
    self->accum = accum;
    self->counter = counter;
    self->notify = notify;

    objInstInit(self);

    return self;
}

uint32 TQTestCC2_run(_In_ TQTestCC2* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    *self->accum += self->total;
    *self->counter += 1;
    tcon->notifyev = self->notify;
    return TASK_Result_Success;
}

extern atomic(int32) tqtests_order;

_objfactory_guaranteed TQTestS1* TQTestS1_create(int order, int64 dtime, Event* notify)
{
    TQTestS1 *self;
    self = objInstCreate(TQTestS1);

    self->name = _S"TQTestS1";
    self->order = order;
    self->dtime = dtime;
    self->notify = notify;

    objInstInit(self);

    return self;
}

uint32 TQTestS1_run(_In_ TQTestS1* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    if (self->rantime == 0) {
        self->rantime = clockTimer();
        tcon->delay   = self->dtime;
        return TASK_Result_Schedule;
    }

    tcon->notifyev = self->notify;
    if(self->rantime + self->dtime > clockTimer())
        return TASK_Result_Failure;   // was run early!!!

//    if(atomicLoad(int32, &tqtests_order, Acquire) > self->order)
//        return TASK_Result_Failure;

    atomicStore(int32, &tqtests_order, self->order, Release);

    return TASK_Result_Success;
}

_objfactory_guaranteed TQTestS2* TQTestS2_create(Task* waitfor, Event* notify)
{
    TQTestS2 *self;
    self = objInstCreate(TQTestS2);

    self->name = _S"TQTestS2";
    self->waitfor = objAcquire(waitfor);
    self->notify = notify;

    objInstInit(self);

    // Insert any post-initialization object construction here

    return self;
}

uint32 TQTestS2_run(_In_ TQTestS2* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    int state = btaskState(self->waitfor);
    if(state == TASK_Succeeded) {
        tcon->notifyev = self->notify;
        return TASK_Result_Success;
    }
    if(state == TASK_Failed) {
        tcon->notifyev = self->notify;
        return TASK_Result_Failure;
    }

    tcon->delay = -1;
    return TASK_Result_Schedule;
}

void TQTestS2_destroy(_In_ TQTestS2* self)
{
    // Autogen begins -----
    objRelease(&self->waitfor);
    // Autogen ends -------
}

_objfactory_guaranteed TQDelayTest* TQDelayTest_create(int64 len)
{
    TQDelayTest *self;
    self = objInstCreate(TQDelayTest);

    self->name = _S"TQDelayTest";
    self->len = len;

    objInstInit(self);

    return self;
}

uint32 TQDelayTest_run(_In_ TQDelayTest* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    osSleep(self->len);
    return TASK_Result_Success;
}

_objfactory_guaranteed TQMTest* TQMTest_create(Event* notify)
{
    TQMTest *self;
    self = objInstCreate(TQMTest);

    self->notify = notify;

    objInstInit(self);

    return self;
}

uint32 TQMTest_run(_In_ TQMTest* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    tcon->notifyev = self->notify;

    return TASK_Result_Success;
}

_objfactory_guaranteed TQRTestMtx* TQRTestMtx_create(ReqTestState* rts, int num)
{
    TQRTestMtx* self;
    self = objInstCreate(TQRTestMtx);

    self->rts = rts;
    self->num = num;

    objInstInit(self);

    return self;
}

uint32 TQRTestMtx_run(_In_ TQRTestMtx* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    ReqTestState* rts = self->rts;
    if (atomicLoad(bool, &rts->running, Relaxed)) {
        atomicStore(bool, &rts->fail, true, Relaxed);
    }

    atomicStore(bool, &rts->running, true, Relaxed);
    rts->sum += self->num;
    rts->xor ^= self->num;
    rts->count++;
    atomicStore(bool, &rts->running, false, Relaxed);

    if (rts->count == rts->target_count)
        eventSignal(&rts->notify);

    return TASK_Result_Success;
}

_objfactory_guaranteed TQRTestFifo* TQRTestFifo_create(ReqTestState* rts, int seq, int num)
{
    TQRTestFifo* self;
    self = objInstCreate(TQRTestFifo);

    self->rts = rts;
    self->seq = seq;
    self->num = num;

    objInstInit(self);
    return self;
}

uint32 TQRTestFifo_run(_In_ TQRTestFifo* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    ReqTestState* rts = self->rts;
    if (atomicLoad(bool, &rts->running, Relaxed)) {
        atomicStore(bool, &rts->fail, true, Relaxed);
    }

    atomicStore(bool, &rts->running, true, Relaxed);
    if (rts->seq != self->seq - 1)
        atomicStore(bool, &rts->fail, true, Relaxed);
    
    rts->sum += self->num;
    rts->product = (rts->product * self->num) % 0xfffffffe + 1;
    rts->count++;

    rts->seq = self->seq;
    atomicStore(bool, &rts->running, false, Relaxed);

    if (rts->count == rts->target_count)
        eventSignal(&rts->notify);

    return TASK_Result_Success;
}

_objfactory_guaranteed TQRTestLifo* TQRTestLifo_create(ReqTestState* rts, int seq, int num)
{
    TQRTestLifo* self;
    self = objInstCreate(TQRTestLifo);

    self->rts = rts;
    self->seq = seq;
    self->num = num;

    objInstInit(self);
    return self;
}

uint32 TQRTestLifo_run(_In_ TQRTestLifo* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    ReqTestState* rts = self->rts;
    if (atomicLoad(bool, &rts->running, Relaxed)) {
        atomicStore(bool, &rts->fail, true, Relaxed);
    }

    atomicStore(bool, &rts->running, true, Relaxed);
    if (rts->seq != self->seq + 1)
        atomicStore(bool, &rts->fail, true, Relaxed);

    rts->sum += self->num;
    rts->xor ^= self->num;
    rts->count++;

    rts->seq = self->seq;
    atomicStore(bool, &rts->running, false, Relaxed);

    if (rts->count == rts->target_count)
        eventSignal(&rts->notify);

    return TASK_Result_Success;
}

_objfactory_guaranteed TQRTestGate* TQRTestGate_create(ReqTestState2* rts, int num)
{
    TQRTestGate* self;
    self = objInstCreate(TQRTestGate);

    self->rts = rts;
    self->num = num;

    objInstInit(self);
    return self;
}

uint32 TQRTestGate_run(_In_ TQRTestGate* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    ReqTestState2* rts = self->rts;
    atomicFetchAdd(int32, &rts->sum, self->num, Relaxed);
    int32 lastcount = atomicFetchAdd(int32, &rts->count, 1, Relaxed);
    if (lastcount == rts->target_count - 1)
        eventSignal(&rts->notify);

    return TASK_Result_Success;
}

static uint32 mptest1(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchAdd(int32, &self->mps->sum, self->idx, Relaxed);
    return TASK_Result_Success;
}

static uint32 mptest2(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchAdd(int32, &self->mps->sum, self->idx * 2, Relaxed);
    return TASK_Result_Success;
}

static uint32 mptest3(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchAdd(int32, &self->mps->sum, self->idx * 3, Relaxed);
    return TASK_Result_Success;
}

static uint32 mptest4(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchAdd(int32, &self->mps->sum, self->idx * 4, Relaxed);
    return TASK_Result_Success;
}

static uint32 mptestFAIL(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    return TASK_Result_Failure;
}

static uint32 mpftest1(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchSub(int32, &self->mps->sum, self->idx, Relaxed);
    return TASK_Result_Success;
}

static uint32 mpftest2(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchSub(int32, &self->mps->sum, self->idx * 2, Relaxed);
    return TASK_Result_Success;
}

static uint32 mpftest3(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchSub(int32, &self->mps->sum, self->idx * 3, Relaxed);
    return TASK_Result_Failure;
}

static uint32 mpftest4(TQMPTest *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon)
{
    atomicFetchSub(int32, &self->mps->sum, self->idx * 4, Relaxed);
    return TASK_Result_Success;
}

static MPTPhaseFunc mpvariant1[] = { (MPTPhaseFunc)mptest1 };
static MPTPhaseFunc mpvariant2[] = { (MPTPhaseFunc)mptest1, (MPTPhaseFunc)mptest2 };
static MPTPhaseFunc mpvariant3[] = { (MPTPhaseFunc)mptest1,
                                     (MPTPhaseFunc)mptest2,
                                     (MPTPhaseFunc)mptest3 };
static MPTPhaseFunc mpvariant4[] = { (MPTPhaseFunc)mptest1,
                                     (MPTPhaseFunc)mptest2,
                                     (MPTPhaseFunc)mptest3,
                                     (MPTPhaseFunc)mptest4 };
static MPTPhaseFunc mpvariant5[] = {
    (MPTPhaseFunc)mptestFAIL,
    (MPTPhaseFunc)mptest1,
    (MPTPhaseFunc)mptest2,
    (MPTPhaseFunc)mptest3,
    (MPTPhaseFunc)mptest4
};
static MPTPhaseFunc mpvariant6[] = {
    (MPTPhaseFunc)mptest1,
    (MPTPhaseFunc)mptestFAIL,
    (MPTPhaseFunc)mptest2,
    (MPTPhaseFunc)mptest3,
    (MPTPhaseFunc)mptest4
};
static MPTPhaseFunc mpvariant7[] = {
    (MPTPhaseFunc)mptest1,
    (MPTPhaseFunc)mptest2,
    (MPTPhaseFunc)mptest3,
    (MPTPhaseFunc)mptestFAIL,
    (MPTPhaseFunc)mptest4
};
static MPTPhaseFunc mpvariant8[] = {
    (MPTPhaseFunc)mptest1, (MPTPhaseFunc)mptest2,    (MPTPhaseFunc)mptest3,
    (MPTPhaseFunc)mptest4, (MPTPhaseFunc)mptestFAIL,
};
static MPTPhaseFunc mpvariant9[] = {
    (MPTPhaseFunc)mptest1, (MPTPhaseFunc)mptest2, (MPTPhaseFunc)mptestFAIL,
    (MPTPhaseFunc)mptest3, (MPTPhaseFunc)mptest4,
};

static MPTPhaseFunc mpfvariant5[] = { (MPTPhaseFunc)mpftest1, (MPTPhaseFunc)mpftest2, (MPTPhaseFunc)mpftest3, (MPTPhaseFunc)mpftest4 };
static MPTPhaseFunc mpfvariant6[] = { (MPTPhaseFunc)mpftest1, (MPTPhaseFunc)mpftest2, (MPTPhaseFunc)mpftest3 };
static MPTPhaseFunc mpfvariant7[] = { (MPTPhaseFunc)mpftest1, (MPTPhaseFunc)mpftest2 };
static MPTPhaseFunc mpfvariant8[] = { (MPTPhaseFunc)mpftest1 };

_objfactory_guaranteed TQMPTest* TQMPTest_create(int variant, int idx, MPTestState* mps)
{
    TQMPTest* self;
    self          = objInstCreate(TQMPTest);
    self->idx     = idx;
    self->mps     = mps;
    self->variant = variant;

    objInstInit(self);

    switch (variant) {
    case 1:
        mptaskAddPhases(self, mpvariant1);
        break;
    case 2:
        mptaskAddPhases(self, mpvariant2);
        break;
    case 3:
        mptaskAddPhases(self, mpvariant3);
        break;
    case 4:
        mptaskAddPhases(self, mpvariant4);
        break;
    case 5:
        mptaskAddPhases(self, mpvariant5);
        mptaskAddFailPhases(self, mpfvariant5);
        break;
    case 6:
        mptaskAddPhases(self, mpvariant6);
        mptaskAddFailPhases(self, mpfvariant6);
        break;
    case 7:
        mptaskAddPhases(self, mpvariant7);
        mptaskAddFailPhases(self, mpfvariant7);
        break;
    case 8:
        mptaskAddPhases(self, mpvariant8);
        mptaskAddFailPhases(self, mpfvariant8);
        break;
    case 9:
        mptaskAddPhases(self, mpvariant9);
        break;
    }

    return self;
}

extern uint32 MultiphaseTask_finish(_In_ MultiphaseTask* self, uint32 result, TaskControl* tcon);   // parent
#define parent_finish(result, tcon) MultiphaseTask_finish((MultiphaseTask*)(self), result, tcon)
uint32 TQMPTest_finish(_In_ TQMPTest* self, uint32 result, TaskControl* tcon)
{
    if (self->variant < 5 && result != TASK_Result_Success)
        atomicStore(bool, &self->mps->fail, true, Relaxed);

    if (self->variant >= 5 && result != TASK_Result_Failure)
        atomicStore(bool, &self->mps->fail, true, Relaxed);

    int32 oldcount = atomicFetchAdd(int32, &self->mps->count, 1, Relaxed);
    if (oldcount == self->mps->target_count - 1)
        eventSignal(&self->mps->notify);

    return parent_finish(result, tcon);
}

_objfactory_guaranteed TQTimeoutTest1* TQTimeoutTest1_create()
{
    TQTimeoutTest1* self;
    self = objInstCreate(TQTimeoutTest1);

    objInstInit(self);
    return self;
}

uint32 TQTimeoutTest1_run(_In_ TQTimeoutTest1* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                          _Inout_ TaskControl* tcon)
{
    if (taskCancelled(self))
        return TASK_Result_Failure;

    if (self->count < 100) {
        self->count++;
        osSleep(timeMS(100));
        return TASK_Result_Schedule_Progress;
    }
    return TASK_Result_Success;
}

_objfactory_guaranteed TQTimeoutTest2* TQTimeoutTest2_create(ReqTestState* rts)
{
    TQTimeoutTest2* self;
    self = objInstCreate(TQTimeoutTest2);

    self->rts = rts;

    objInstInit(self);
    return self;
}

uint32 TQTimeoutTest2_run(_In_ TQTimeoutTest2* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    self->rts->count = 1;
    return TASK_Result_Success;
}

// Autogen begins -----
#include "tqtestobj.auto.inc"
// Autogen ends -------
