#include "control/cmds.h"
#include "feature/featureregistry.h"
#include "run/runinfo.h"

void cmdStats(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    int ships     = cfieldValD(int32, fields, _S"ships", -1);
    int beacons    = cfieldValD(int32, fields, _S"beacons", -1);
    int scrap    = cfieldValD(int32, fields, _S"scrap", -1);
    int crew    = cfieldValD(int32, fields, _S"crew", -1);

    RunInfo* cur = NULL;
    withReadLock (&inst->lock) {
        cur = objAcquire(inst->currentRun);
    }

    if (cur) {
        runinfoUpdateStats(cur, ships, beacons, scrap, crew);
        objRelease(&cur);
    }
}
