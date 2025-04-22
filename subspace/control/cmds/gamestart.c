// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "gamestart.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/ssdtree.h>
#include <cx/fs.h>
#include "control/controlserver.h"

static _objfactory_guaranteed Task* CmdGameStart_factory(ControlClient* client, ControlMsg* msg)
{
    CmdGameStart* self;
    self = objInstCreate(CmdGameStart);
    objInstInit(self);

    self->client = objAcquire(client);
    self->msg    = msg;

    return Task(self);
}

void CmdGameStart_register()
{
    controlServerRegisterHandler("GameStart", CmdGameStart_factory);
}

uint32 CmdGameStart_run(_In_ CmdGameStart* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                        _Inout_ TaskControl* tcon)
{
    int nfields   = 4;
    string ftlexe = 0;

    if (ssdStringOut(self->client->subspace->settings, _S"ftl/exe", &ftlexe)) {
        ControlMsg* msg = controlAllocMsg(4, CF_ALLOC_AUTO);
        string tmpstr   = 0;
        if (!msg) {
            strDestroy(&ftlexe);
            return TASK_Result_Failure;
        }

        msg->hdr.replyid = self->msg->hdr.msgid;
        strcpy(msg->hdr.cmd, "Launch");
        strcpy(msg->fields[0]->h.name, "launchmode");
        msg->fields[0]->h.ftype = CF_INT;
        msg->fields[0]->d.cfd_int = 0;

        strcpy(msg->fields[1]->h.name, "gamedir");
        msg->fields[1]->h.ftype = CF_STRING;
        pathParent(&tmpstr, ftlexe);
        pathToPlatform(&msg->fields[1]->d.cfd_str, tmpstr);

        strcpy(msg->fields[2]->h.name, "gameprogram");
        msg->fields[2]->h.ftype = CF_STRING;
        pathFilename(&tmpstr, ftlexe);
        pathToPlatform(&msg->fields[2]->d.cfd_str, tmpstr);

        strcpy(msg->fields[3]->h.name, "gamepath");
        msg->fields[3]->h.ftype = CF_STRING;
        pathToPlatform(&msg->fields[3]->d.cfd_str, ftlexe);

        strDestroy(&tmpstr);
        strDestroy(&ftlexe);

        cclientQueue(self->client, msg);
    } else {
        ControlMsg* msg = controlAllocMsg(0, CF_ALLOC_AUTO);
        if (!msg)
            return TASK_Result_Failure;

        msg->hdr.replyid = self->msg->hdr.msgid;
        strcpy(msg->hdr.cmd, "Exit");

        cclientQueue(self->client, msg);
    }

    return TASK_Result_Success;
}

// Autogen begins -----
#include "gamestart.auto.inc"
// Autogen ends -------
