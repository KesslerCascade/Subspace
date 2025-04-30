// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "cmdloading.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gameinst.h"

ControlCmd_impl("Loading", CmdLoading);

uint32 CmdLoading_run(_In_ CmdLoading* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    GameInst* inst = objAcquireFromWeak(GameInst, self->client->inst);
    if (inst) {
        ControlField* pctf = controlMsgFindField(self->msg, "pct");
        withWriteLock(&inst->lock) {
            if (pctf)
                inst->loadPct = pctf->d.cfd_float32;
            ginstSetStateLocked(inst, GI_Loading);
        }
        objRelease(&inst);
    }

    return TASK_Result_Success;
}

// Autogen begins -----
void CmdLoading_register(ControlServer* svr);
#include "cmdloading.auto.inc"
// Autogen ends -------
