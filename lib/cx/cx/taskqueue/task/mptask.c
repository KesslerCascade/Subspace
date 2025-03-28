// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "mptask.h"
// clang-format on
// ==================== Auto-generated section ends ======================

static void mptaskSetFail(_Inout_ MultiphaseTask* self)
{
    self->_fail  = true;
    self->_phase = 0;
}

_objinit_guaranteed bool MultiphaseTask_init(_In_ MultiphaseTask* self)
{
    self->flags |= TASK_Soft_Requires;   // ensure that finish is always called
    saInit(&self->phases, ptr, 1);
    saInit(&self->failphases, ptr, 1);
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

void MultiphaseTask_destroy(_In_ MultiphaseTask* self)
{
    // Autogen begins -----
    saDestroy(&self->phases);
    saDestroy(&self->failphases);
    // Autogen ends -------
}

uint32 MultiphaseTask_run(_In_ MultiphaseTask* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    bool done = false;
    uint32 ret;

    if (!self->_fail && (self->flags & TASK_Require_Failed)) {
        // a requirement failed, but we still want to activate the fail phases and run finish
        mptaskSetFail(self);
    }

    do {
        if (!self->_fail && self->_phase < (uint32)saSize(self->phases)) {
            uint32 pret = self->phases.a[self->_phase](self, tq, worker, tcon);

            switch (pret) {
            case TASK_Result_Success:
                self->_phase++;
                ret = TASK_Result_Schedule_Progress;
                break;
            case TASK_Result_Defer:
            case TASK_Result_Defer_Progress:
            case TASK_Result_Schedule:
            case TASK_Result_Schedule_Progress:
                ret = pret;
                break;
            case TASK_Result_Failure:
            default:
                // catch all for unknown status codes
                mptaskSetFail(self);
                // need to run again to run through the fail phases
                ret = TASK_Result_Schedule;
            }
        } else if (self->_fail && self->_phase < (uint32)saSize(self->failphases)) {
            uint32 pret = self->failphases.a[self->_phase](self, tq, worker, tcon);
            switch (pret) {
            case TASK_Result_Success:
                self->_phase++;
                ret = TASK_Result_Schedule_Progress;
                break;
            case TASK_Result_Defer:
            case TASK_Result_Defer_Progress:
            case TASK_Result_Schedule:
            case TASK_Result_Schedule_Progress:
                ret = pret;
                break;
            case TASK_Result_Failure:
            default:
                // if a fail phase fails we skip the rest and go straight to finishing
                self->_phase = saSize(self->failphases);
                ret          = TASK_Result_Schedule;
            }
        } else {
            ret = mptaskFinish(self, self->_fail ? TASK_Result_Failure : TASK_Result_Success, tcon);
            done = true;
        }
        // repeat if this is not a greedy task, otherwise return schedule or defer
    } while (!done && (self->flags & MPTASK_Greedy));

    return ret;
}

uint32 MultiphaseTask_finish(_In_ MultiphaseTask* self, uint32 result, TaskControl* tcon)
{
    return result;
}

void MultiphaseTask__addPhases(_In_ MultiphaseTask* self, int32 num, MPTPhaseFunc parr[], bool fail)
{
    sa_MPTPhaseFunc* phasearray = fail ? &self->failphases : &self->phases;
    int32 csize                 = saSize(*phasearray);

    saSetSize(phasearray, csize + num);
    memcpy(&phasearray->a[csize], parr, sizeof(MPTPhaseFunc) * num);
}

// Autogen begins -----
#include "mptask.auto.inc"
// Autogen ends -------
