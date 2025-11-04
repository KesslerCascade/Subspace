#include "runlog.h"
#include <stdarg.h>
#include "ftl/capp.h"
#include "ftl/scorekeeper.h"
#include "ftl/worldmanager.h"
#include "control.h"
#include "controlclient.h"
#include "osdep.h"
#include "subspacegame.h"

bool runLogSend(LogEntSpec* spec, ...)
{
    if (gs.practiceMode)
        return false;

    // if disconnected, just drop combat events so that we don't queue up a ton of events
    if (spec->combat && !controlClientConnected())
        return false;

    ControlMsg* msg = controlMsgCreate(_S"RunLog");
    cfieldSet(msg, _S"id", strref, (strref)spec->id);

    WorldManager* world = CApp_world(theApp);
    cfieldSet(msg, _S"sector", int32, WorldManager_worldLevel(world) + 1);
    int beacons = ScoreKeeper_stats(SKeeper)[1].current;

    // special case because the hook that detects a new sector happens before the number of beacons
    // explored stat is updated
    if (spec == &Log_Sector)
        beacons++;

    cfieldSet(msg, _S"beacons", int32, max(beacons, 1));
    cfieldSet(msg, _S"gametime", float64, gs.gameTime);

    va_list args;
    va_start(args, spec);
    char tmpbuf[3] = { 'p', 0, 0 };
    for (int i = 0; i < spec->numParams; i++) {
        tmpbuf[1] = '1' + i;
        if (spec->paramTypes[i] == LP_INT)
            cfieldSet(msg, (strref)tmpbuf, int32, va_arg(args, int));
        else if (spec->paramTypes[i] == LP_STRING)
            cfieldSet(msg, (strref)tmpbuf, strref, (strref)va_arg(args, const char*));
        else
            va_arg(args, int);
    }
    va_end(args);

    msg->priority = spec->priority;

    controlClientQueue(msg);
    return true;
}
