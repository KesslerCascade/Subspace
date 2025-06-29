#include "control/cmds.h"
#include "run/runinfo.h"
#include "logentspec.h"

#ifdef _PLATFORM_WIN
#include <cx/platform/win/win_time.h>
#endif

void cmdRunLog(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    RunInfo* run = ginstRun(inst);
    // can't do anything without a run in progress
    if (!run)
        return;

    strref id     = cfieldString(fields, _S"id");
    uint64 time   = cfieldValD(uint64, fields, _S"time", 0);
    int32 sector  = cfieldValD(int32, fields, _S"sector", 0);
    int32 beacons = cfieldValD(int32, fields, _S"beacons", 0);

    if (strEmpty(id) || sector == 0 || beacons == 0 || time == 0)
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

#ifdef _PLATFORM_WIN
    // TODO: Move this somewhere under osdep
    FILETIME ft;
    ft.dwHighDateTime = time >> 32;
    ft.dwLowDateTime  = time & 0xFFFFFFFF;
    int64 logtime     = timeFromFileTime(&ft);
#endif

    runinfoRunLog(run, sector, beacons, logtime, id, params);

out:
    objRelease(&run);
}
