#include "control/cmds.h"
#include "run/runinfo.h"
#include "logentspec.h"

void cmdRunLog(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    RunInfo* run = ginstRun(inst);
    // can't do anything without a run in progress
    if (!run)
        return;

    strref id     = cfieldString(fields, _S"id");
    int32 sector  = cfieldValD(int32, fields, _S"sector", 0);
    int32 beacons = cfieldValD(int32, fields, _S"beacons", 0);

    if (strEmpty(id) || sector == 0 || beacons == 0)
        goto out;

    stvar params[LOG_MAX_PARAMS] = { 0 };
    foreach (hashtable, hti, fields) {
        strref key = htiKey(strref, hti);
        if (strLen(key) == 2 && strGetChar(key, 0) == 'p') {
            int n = strGetChar(key, 1) - '1';
            if (n >= 0 && n < LOG_MAX_PARAMS) {
                params[n] = htiVal(stvar, hti);
            }
        }
    }

    runinfoRunLog(run, sector, beacons, msg->hdr.timestamp, id, params);

out:
    objRelease(&run);
}
