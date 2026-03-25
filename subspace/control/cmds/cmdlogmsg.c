#include "control/cmds.h"
#include "control/controlclient.h"

static void GameLogMsg_dtor(stype st, stgeneric* gen, flags_t flags)
{
    GameLogMsg* msg = (GameLogMsg*)gen->st_opaque;
    strDestroy(&msg->msg);
}

STypeOps GameLogMsg_ops = { .dtor = GameLogMsg_dtor };

void cmdLogMsg(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    int32 level     = cfieldValD(int32, fields, _S"level", LOG_Count);
    uint32 batchid  = cfieldValD(int32, fields, _S"batchid", 0);
    int64 timestamp = cfieldValD(int64, fields, _S"timestamp", -1);
    string logmsg   = 0;
    strDup(&logmsg, cfieldString(fields, _S"msg"));

    uint32 pid;
    if (inst && inst->process && procGetID(&inst->process, &pid)) {
        string pidstr = 0;
        strFromUInt32(&pidstr, pid, 10);
        strNConcat(&logmsg, _S"[", pidstr, _S"] ", logmsg);
        strDestroy(&pidstr);
    } else {
        strPrepend(_S"[Game] ", &logmsg);
    }

    if (level < LOG_Count && !strEmpty(logmsg)) {
        htelem elem = htFind(client->logbatch, uint32, batchid, none, NULL);
        if (!elem) {
            sa_GameLogMsg nbatch;
            saInit(&nbatch, custom(opaque(GameLogMsg), GameLogMsg_ops), 4);
            elem = htInsertC(&client->logbatch, uint32, batchid, sarray, &nbatch);
        }

        sa_GameLogMsg* batch = (sa_GameLogMsg*)hteValPtr(client->logbatch, sarray, elem);
        GameLogMsg nmsg      = { .timestamp = timestamp, .level = level, .msg = logmsg };
        saPushC(batch, opaque, &nmsg);
    } else {
        strDestroy(&logmsg);
    }
}
