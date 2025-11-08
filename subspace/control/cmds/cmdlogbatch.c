#include <cx/log.h>
#include "control/cmds.h"
#include "control/controlclient.h"

void cmdLogBatch(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    uint32 batchid = cfieldValD(int32, fields, _S"batchid", 0);

    sa_GameLogMsg batch;
    if (htExtract(&client->logbatch, uint32, batchid, sarray, &batch)) {
        logBatchBegin();
        for (int i = 0; i < saSize(batch); i++) {
            _logStr(batch.a[i].level, batch.a[i].timestamp, gamecat, batch.a[i].msg);
        }
        logBatchEnd();

        saDestroy(&batch);
    }
}
