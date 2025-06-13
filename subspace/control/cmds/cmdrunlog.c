#include "control/cmds.h"
#include "run/runinfo.h"
#include "logentspec.h"

#ifdef _PLATFORM_WIN
#include <cx/platform/win/win_time.h>
#endif

void cmdRunLog(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    RunInfo* run = subspaceRun(client->ss);
    // can't do anything without a run in progress
    if (!run)
        return;

    strref id       = cfieldString(fields, _S"id");
    uint32 timelow  = cfieldValD(uint32, fields, _S"timelow", 0);
    uint32 timehigh = cfieldValD(uint32, fields, _S"timehigh", 0);
    int32 sector    = cfieldValD(int32, fields, _S"sector", 0);
    int32 beacons   = cfieldValD(int32, fields, _S"beacons", 0);

    if (strEmpty(id) || sector == 0 || beacons == 0 || (timelow == 0 && timehigh == 0))
        return;

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
    ft.dwHighDateTime = timehigh;
    ft.dwLowDateTime  = timelow;
    int64 logtime = timeFromFileTime(&ft);
#endif

    runinfoRunLog(run, sector, beacons, logtime, id, params);

    objRelease(&run);
}
