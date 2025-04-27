// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "cmdlog.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/log.h>
#include "gamemgr/gameinst.h"
#include "process.h"

static _objfactory_guaranteed Task* CmdLog_factory(ControlClient* client, ControlMsg* msg)
{
    CmdLog* self;
    self = objInstCreate(CmdLog);
    objInstInit(self);

    self->client = objAcquire(client);
    self->msg    = msg;

    return Task(self);
}

void CmdLog_register(ControlServer* svr)
{
    cserverRegisterHandler(svr, _S"Log", CmdLog_factory);
}

uint32 CmdLog_run(_In_ CmdLog* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    int level       = LOG_Count;
    string msg      = 0;
    for (int i = 0; i < self->msg->hdr.nfields; i++)
    {
        ControlField* f = self->msg->fields[i];
        if (!strcmp(f->h.name, "level")) {
            level = f->d.cfd_int;
        } else if (!strcmp(f->h.name, "msg")) {
            strDup(&msg, f->d.cfd_str);
        }
    }

    GameInst* inst = objAcquireFromWeak(GameInst, self->client->inst);
    uint32 pid;
    if (inst && inst->process && procGetID(&inst->process, &pid)) {
        string pidstr = 0;
        strFromUInt32(&pidstr, pid, 10);
        strNConcat(&msg, _S"[", pidstr, _S"] ", msg);
        strDestroy(&pidstr);
    } else {
        strPrepend(_S"[Game] ", &msg);
    }
    objRelease(&inst);

    if (level < LOG_Count)
        _logStr(level, gamecat, msg);

    strDestroy(&msg);
    return TASK_Result_Success;
}

// Autogen begins -----
#include "cmdlog.auto.inc"
// Autogen ends -------
