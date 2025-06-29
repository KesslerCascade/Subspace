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

    uint64 time   = cfieldValD(uint64, fields, _S"time", 0);
    int32 sector  = cfieldValD(int32, fields, _S"sector", 0);
    int32 beacons = cfieldValD(int32, fields, _S"beacons", 0);
    int32 visit   = cfieldValD(int32, fields, _S"visit", 0);
    int32 x       = cfieldValD(int32, fields, _S"x", 0);
    int32 y       = cfieldValD(int32, fields, _S"y", 0);
    strref event  = cfieldString(fields, _S"event");

    if (sector == 0 || beacons == 0 || time == 0)
        goto out;

#ifdef _PLATFORM_WIN
    // TODO: Move this somewhere under osdep
    FILETIME ft;
    ft.dwHighDateTime = time >> 32;
    ft.dwLowDateTime  = time & 0xFFFFFFFF;
    int64 beacontime  = timeFromFileTime(&ft);
#endif

    runinfoBeacon(run, sector, beacons, visit, x, y, beacontime, event);

out:
    objRelease(&run);
}
