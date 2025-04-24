// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "cmdgamestart.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gamemgr.h"

#ifdef _PLATFORM_WIN
#include <cx/platform/win/win_fs.h>

static _objfactory_guaranteed Task* CmdGameStart_factory(ControlClient* client, ControlMsg* msg)
{
    CmdGameStart* self;
    self = objInstCreate(CmdGameStart);
    objInstInit(self);

    self->client = objAcquire(client);
    self->msg    = msg;

    return Task(self);
}

void CmdGameStart_register(ControlServer* svr)
{
    cserverRegisterHandler(svr, _S"GameStart", CmdGameStart_factory);
}

uint32 CmdGameStart_run(_In_ CmdGameStart* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                        _Inout_ TaskControl* tcon)
{
    ControlClient* client = self->client;
    Subspace* ss          = client->ss;
    GameInst* inst        = NULL;

    uint32 cookie         = 0;
    ControlField* cookief = controlMsgFindField(self->msg, "cookie");
    if (cookief)
        cookie = cookief->d.cfd_uint;

    // link up the game instance and the cookie
    inst = cookie != 0 ? gmgrAcquireByCookie(ss->gmgr, cookie) : NULL;
    if (!inst && ss->devmode && cookie == 0xDEADBEEF) {
        // special cookie for manually running a client in dev mode.
        // in this case we create one when it connects
        inst = ginstCreateForClient(ss->gmgr, client, cookie);
        // use the default FTL executable for it
        ssdStringOut(ss->settings, _S"ftl/exe", &inst->exepath);
        gmgrReg(ss->gmgr, inst);
    }

    if (inst) {
        // link the instance and client together
        objDestroyWeak(&client->inst);
        objDestroyWeak(&inst->client);
        client->inst = objGetWeak(GameInst, inst);
        inst->client = objGetWeak(ControlClient, client);
    }

    if (inst && !strEmpty(inst->exepath)) {
        ControlMsg* msg = controlAllocMsg(4, CF_ALLOC_AUTO);
        string tmpstr   = 0;
        if (!msg) {
            return TASK_Result_Failure;
        }

        msg->hdr.replyid = self->msg->hdr.msgid;
        strcpy(msg->hdr.cmd, "Launch");
        strcpy(msg->fields[0]->h.name, "launchmode");
        msg->fields[0]->h.ftype   = CF_INT;
        msg->fields[0]->d.cfd_int = 0;

        strcpy(msg->fields[1]->h.name, "gamedir");
        msg->fields[1]->h.ftype = CF_STRING;
        pathParent(&tmpstr, inst->exepath);
        pathToPlatform(&msg->fields[1]->d.cfd_str, tmpstr);

        strcpy(msg->fields[2]->h.name, "gameprogram");
        msg->fields[2]->h.ftype = CF_STRING;
        pathFilename(&tmpstr, inst->exepath);
        pathToPlatform(&msg->fields[2]->d.cfd_str, tmpstr);

        strcpy(msg->fields[3]->h.name, "gamepath");
        msg->fields[3]->h.ftype = CF_STRING;
        pathToPlatform(&msg->fields[3]->d.cfd_str, inst->exepath);

        strDestroy(&tmpstr);

        cclientQueue(self->client, msg);
    } else {
        if (!inst) {
            logFmt(Warn,
                   _S"Game connected with invalid cookie ${uint(hex,min:8)}",
                   stvar(uint32, cookie));
        }

        // Something wrong with this client, tell it to exit
        ControlMsg* msg = controlAllocMsg(0, CF_ALLOC_AUTO);
        if (!msg)
            return TASK_Result_Failure;

        msg->hdr.replyid = self->msg->hdr.msgid;
        strcpy(msg->hdr.cmd, "Exit");

        cclientQueue(self->client, msg);
    }

    objRelease(&inst);

    return TASK_Result_Success;
}

// Autogen begins -----
#include "cmdgamestart.auto.inc"
// Autogen ends -------
