#include "control/cmds.h"
#include "run/runinfo.h"

#ifdef _PLATFORM_WIN
#include <cx/platform/win/win_time.h>
#endif

void cmdBeacon(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    RunInfo* run = ginstRun(inst);
    // can't do anything without a run in progress
    if (!run)
        return;

    uint32 timelow  = cfieldValD(uint32, fields, _S"timelow", 0);
    uint32 timehigh = cfieldValD(uint32, fields, _S"timehigh", 0);
    int32 sector    = cfieldValD(int32, fields, _S"sector", 0);
    int32 beacons   = cfieldValD(int32, fields, _S"beacons", 0);
    int32 visit = cfieldValD(int32, fields, _S"visit", 0);
    int32 x = cfieldValD(int32, fields, _S"x", 0);
    int32 y = cfieldValD(int32, fields, _S"y", 0);
    strref event       = cfieldString(fields, _S"event");

    if (sector == 0 || beacons == 0 || (timelow == 0 && timehigh == 0))
        goto out;

#ifdef _PLATFORM_WIN
    // TODO: Move this somewhere under osdep
    FILETIME ft;
    ft.dwHighDateTime = timehigh;
    ft.dwLowDateTime  = timelow;
    int64 beacontime     = timeFromFileTime(&ft);
#endif

    runinfoBeacon(run, sector, beacons, visit, x, y, beacontime, event);

out:
    objRelease(&run);
}
