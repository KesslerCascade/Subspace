// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "basictask.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue/taskqueue.h>

bool BasicTask_reset(_In_ BasicTask* self)
{
    uint32 oldval   = atomicLoad(uint32, &self->state, Relaxed);
    uint32 oldstate = oldval & TASK_State_Mask;
    if (oldstate != TASK_Succeeded && oldstate != TASK_Failed)
        return false;

    // this also resets the cancelled flag
    while (!atomicCompareExchange(uint32,
                                  weak,
                                  &self->state,
                                  &oldval,
                                  TASK_Created,
                                  AcqRel,
                                  Relaxed)) {
        oldstate = oldval & TASK_State_Mask;
        if (oldstate != TASK_Succeeded && oldstate != TASK_Failed)
            return false;
    }

    return true;
}

bool BasicTask_cancel(_In_ BasicTask* self)
{
    uint32 oldval   = atomicLoad(uint32, &self->state, Relaxed);
    uint32 oldstate = oldval & TASK_State_Mask;

    do {
        // can't cancel if it's already been cancelled, or if it's already finished
        if ((oldval & TASK_Cancelled) || oldstate == TASK_Succeeded || oldstate == TASK_Failed)
            return false;
    } while (!atomicCompareExchange(uint32,
                                    weak,
                                    &self->state,
                                    &oldval,
                                    oldval | TASK_Cancelled,
                                    AcqRel,
                                    Relaxed));

    return true;
}

bool BasicTask__setState(_In_ BasicTask* self, uint32 newstate)
{
    uint32 oldval     = atomicLoad(uint32, &self->state, Relaxed);
    uint32 oldstate   = oldval & TASK_State_Mask;
    uint32 cancelflag = oldval & TASK_Cancelled;

    if (oldstate == newstate)
        return true;

    do {
        // validate state transition
        switch (newstate) {
        case TASK_Created:
            // Really should be using btaskReset for this, but go ahead and allow it
            if (!(oldstate == TASK_Succeeded || oldstate == TASK_Failed))
                return false;
            break;
        case TASK_Waiting:
            // must be inserted into run queue from one of these states
            if (!(oldstate == TASK_Created || oldstate == TASK_Scheduled ||
                  oldstate == TASK_Deferred))
                return false;
            break;
        case TASK_Running:
            // can only go into running state if in the run queue
            if (!(oldstate == TASK_Waiting))
                return false;
            break;
        case TASK_Scheduled:
            // can be scheduled directly when added, or as a return from a task tick
            if (!(oldstate == TASK_Created || oldstate == TASK_Running))
                return false;
            break;
        case TASK_Deferred:
            // can be deferred directly when added, as a return from a task tick, or when waiting
            // if a requirement is not met
            if (!(oldstate == TASK_Created || oldstate == TASK_Running || oldstate == TASK_Waiting))
                return false;
            break;
        case TASK_Succeeded:
            // can't succeed unless we ran!
            if (!(oldstate == TASK_Running))
                return false;
            break;
        case TASK_Failed:
            // can fail anytime, unless already succeeded
            if (oldstate == TASK_Succeeded)
                return false;
            break;
        }
    } while (!atomicCompareExchange(uint32,
                                    weak,
                                    &self->state,
                                    &oldval,
                                    newstate | cancelflag,
                                    AcqRel,
                                    Relaxed));
    return true;
}

void BasicTask_runCancelled(_In_ BasicTask* self, _In_ TaskQueue* tq, _In_ TQWorker* worker)
{
    return;
}

// Autogen begins -----
#include "basictask.auto.inc"
// Autogen ends -------
