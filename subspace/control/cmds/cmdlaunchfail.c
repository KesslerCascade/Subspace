// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "cmdlaunchfail.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gamemgr.h"

static _objfactory_guaranteed Task* CmdLaunchFail_factory(ControlClient* client, ControlMsg* msg)
{
    CmdLaunchFail* self;
    self = objInstCreate(CmdLaunchFail);
    objInstInit(self);

    self->client = objAcquire(client);
    self->msg    = msg;

    return Task(self);
}

void CmdLaunchFail_register(ControlServer* svr)
{
    cserverRegisterHandler(svr, _S"LaunchFail", CmdLaunchFail_factory);
}

uint32 CmdLaunchFail_run(_In_ CmdLaunchFail* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    GameInst* inst = objAcquireFromWeak(GameInst, self->client->inst);
    if (inst) {
        ControlField* reasonf = controlMsgFindField(self->msg, "reason");
        if (reasonf) {
            withWriteLock(&inst->lock) {
                inst->failReason = reasonf->d.cfd_int;
            }
        }
        ginstSetState(inst, GI_Failed);
        objRelease(&inst);
    }

    return TASK_Result_Success;
}

// Autogen begins -----
#include "cmdlaunchfail.auto.inc"
// Autogen ends -------
