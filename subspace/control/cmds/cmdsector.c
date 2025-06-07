#include "control/cmds.h"
#include "feature/featureregistry.h"
#include "run/runinfo.h"

void cmdSector(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    int num     = cfieldValD(int32, fields, _S"num", 0);
    int seed    = cfieldValD(int32, fields, _S"seed", 0);
    bool secret = cfieldValD(bool, fields, _S"secret", false);
    strref type = cfieldString(fields, _S"type");

    RunInfo* cur = NULL;
    withReadLock (&inst->lock) {
        cur = objAcquire(inst->currentRun);
    }

    if (cur) {
        runinfoEnterSector(cur, num, seed, type, secret);
        objRelease(&cur);
    }
}
