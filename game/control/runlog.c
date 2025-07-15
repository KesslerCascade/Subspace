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
    // if disconnected, just drop combat events so that we don't queue up a ton of events
    if (spec->combat && !controlClientConnected())
        return false;

    ControlMsg* msg = controlNewMsg("RunLog", 4 + spec->numParams);
    controlMsgStr(msg, 0, "id", spec->id);

    WorldManager* world = CApp_world(theApp);
    controlMsgInt(msg, 1, "sector", WorldManager_worldLevel(world) + 1);
    int beacons = ScoreKeeper_stats(SKeeper)[1].current;
    controlMsgInt(msg, 2, "beacons", MAX(beacons, 1));
    controlMsgFloat64(msg, 3, "gametime", gs.gameTime);

    va_list args;
    va_start(args, spec);
    char tmpbuf[3] = { 'p', 0, 0 };
    for (int i = 0; i < spec->numParams; i++) {
        tmpbuf[1] = '1' + i;
        if (spec->paramTypes[i] == LP_INT)
            controlMsgInt(msg, 4 + i, tmpbuf, va_arg(args, int));
        else if (spec->paramTypes[i] == LP_STRING)
            controlMsgStr(msg, 4 + i, tmpbuf, va_arg(args, const char*));
        else
            va_arg(args, int);
    }
    va_end(args);

    msg->priority = spec->priority;

    controlClientQueue(msg);
    return true;
}
