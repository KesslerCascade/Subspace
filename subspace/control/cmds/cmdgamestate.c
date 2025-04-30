// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "cmdgamestate.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gameinst.h"

ControlCmd_impl("GameState", CmdGameState);

uint32 CmdGameState_run(_In_ CmdGameState* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    GameInst* inst = objAcquireFromWeak(GameInst, self->client->inst);
    if (inst) {
        ControlField* statef = controlMsgFindField(self->msg, "state");
        if (statef)
            ginstSetState(inst, statef->d.cfd_int);
        objRelease(&inst);
    }

    return TASK_Result_Success;
}

// Autogen begins -----
void CmdGameState_register(ControlServer* svr);
#include "cmdgamestate.auto.inc"
// Autogen ends -------
