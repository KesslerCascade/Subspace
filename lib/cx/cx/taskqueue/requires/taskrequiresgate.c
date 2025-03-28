// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "taskrequiresgate.h"
// clang-format on
// ==================== Auto-generated section ends ======================

enum GateStateEnum
{
    GATE_Closed = 0,
    GATE_Open = 1,
    GATE_Sealed = 2
};

_objfactory_guaranteed TRGate* TRGate_create(_In_opt_ strref name)
{
    TRGate* self;
    self = objInstCreate(TRGate);

    strDup(&self->name, name);
    objInstInit(self);
    return self;
}

static bool TRGate_transition(TRGate* self, uint32 nstate)
{
    uint32 state = atomicLoad(uint32, &self->state, Relaxed);
    do {
        if (state == nstate)
            return true;
        if (state != GATE_Closed)
            return false;
    } while (!atomicCompareExchange(uint32, weak, &self->state, &state, nstate, Release, Relaxed));
    return true;
}

static void TRGate_advanceWaitList(TRGate *self)
{
    withMutex (&self->_wlmtx) {
        foreach (sarray, idx, ComplexTask*, task, self->_waitlist) {
            ctaskAdvance(task);
        }
        saClear(&self->_waitlist);
    }
}

bool TRGate_open(_In_ TRGate* self)
{
    if (!TRGate_transition(self, GATE_Open))
        return false;

    TRGate_advanceWaitList(self);
    return true;
}

bool TRGate_seal(_In_ TRGate* self)
{
    if (!TRGate_transition(self, GATE_Sealed))
        return false;

    TRGate_advanceWaitList(self);
    return true;
}

void TRGate_progress(_In_ TRGate* self)
{
    int64 now = clockTimer();
    withMutex(&self->_wlmtx) {
        self->lastprogress = now;
    }
}

bool TRGate_registerTask(_In_ TRGate* self, ComplexTask* task)
{
    // if the gate is not closed, no sense allowing new registrations
    if (atomicLoad(uint32, &self->state, Acquire) != GATE_Closed)
        return false;

    withMutex(&self->_wlmtx) {
        saPush(&self->_waitlist, object, task);
    }

    return true;
}

_objinit_guaranteed bool TRGate_init(_In_ TRGate* self)
{
    // Autogen begins -----
    mutexInit(&self->_wlmtx);
    saInit(&self->_waitlist, object, 1);
    return true;
    // Autogen ends -------
}

void TRGate_destroy(_In_ TRGate* self)
{
    // Autogen begins -----
    strDestroy(&self->name);
    mutexDestroy(&self->_wlmtx);
    saDestroy(&self->_waitlist);
    // Autogen ends -------
}

// ---------------- TaskRequiresGate ----------------

_objfactory_guaranteed TaskRequiresGate* TaskRequiresGate_create(_In_ TRGate* gate)
{
    TaskRequiresGate* self;
    self = objInstCreate(TaskRequiresGate);

    self->gate = objAcquire(gate);

    objInstInit(self);
    return self;
}

uint32 TaskRequiresGate_state(_In_ TaskRequiresGate* self, ComplexTask* task)
{
    uint32 state = atomicLoad(uint32, &self->gate->state, Acquire);
    if (state == GATE_Open)
        return TASK_Requires_Ok_Permanent;
    if (state == GATE_Sealed)
        return TASK_Requires_Fail_Permanent;
    return TASK_Requires_Wait;
}

int64 TaskRequiresGate_progress(_In_ TaskRequiresGate* self)
{
    int64 ret = -1;
    withMutex(&self->gate->_wlmtx) {
        ret = self->gate->lastprogress;
    }

    return ret;
}

bool TaskRequiresGate_tryAcquire(_In_ TaskRequiresGate* self, ComplexTask* task)
{
    return false;
}

bool TaskRequiresGate_release(_In_ TaskRequiresGate* self, ComplexTask* task)
{
    return false;
}

void TaskRequiresGate_cancel(_In_ TaskRequiresGate* self)
{
}

bool TaskRequiresGate_registerTask(_In_ TaskRequiresGate* self, _In_ ComplexTask* task)
{
    return trgateRegisterTask(self->gate, task);
}

void TaskRequiresGate_destroy(_In_ TaskRequiresGate* self)
{
    // Autogen begins -----
    objRelease(&self->gate);
    // Autogen ends -------
}

// Autogen begins -----
#include "taskrequiresgate.auto.inc"
// Autogen ends -------
