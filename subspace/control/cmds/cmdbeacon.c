#include "control/cmds.h"
#include "run/runinfo.h"

void cmdBeacon(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    RunInfo* run = ginstRun(inst);
    // can't do anything without a run in progress
    if (!run)
        return;

    int32 sector  = cfieldValD(int32, fields, _S"sector", 0);
    int32 beacons = cfieldValD(int32, fields, _S"beacons", 0);
    int32 visit   = cfieldValD(int32, fields, _S"visit", 0);
    int32 x       = cfieldValD(int32, fields, _S"x", 0);
    int32 y       = cfieldValD(int32, fields, _S"y", 0);
    strref event  = cfieldString(fields, _S"event");

    if (sector == 0 || beacons == 0)
        goto out;

    runinfoBeacon(run, sector, beacons, visit, x, y, msg->hdr.timestamp, event);

out:
    objRelease(&run);
}
