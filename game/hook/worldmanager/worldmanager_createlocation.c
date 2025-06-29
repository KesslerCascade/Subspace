#include "control/controlclient.h"
#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/location.h"
#include "ftl/scorekeeper.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

void WorldManager_CreateLocation_post(WorldManager* self, Location* loc)
{
    gs.waitInProgress = false;

    if (gc.loadingGame)
        return;   // don't do this while loading the game

    LocationEvent* event    = loc ? Location_event(loc) : NULL;
    basic_string* eventname = event ? LocationEvent_eventName(event) : NULL;

    if (RunTracker_feature.enabled || SaveManager_feature.enabled) {
        // send beacon details first so savepoint can be updated if needed
        ControlMsg* msg     = controlNewMsg("Beacon", 6);
        WorldManager* world = CApp_world(theApp);
        int visits          = Location_visited(loc);
        Pointf* pos         = (Pointf*)loc;   // always the first field in Location

        controlMsgInt(msg, 0, "sector", WorldManager_worldLevel(world) + 1);
        controlMsgInt(msg, 1, "beacons", ScoreKeeper_stats(SKeeper)[1].current);
        controlMsgInt(msg, 2, "visit", MAX(visits, 1));
        controlMsgInt(msg, 3, "x", (int)pos->x);
        controlMsgInt(msg, 4, "y", (int)pos->y);
        controlMsgStr(msg, 5, "event", eventname ? eventname->buf : "");

        msg->priority = 2;   // ensure this comes after NewGame and Sector
        controlClientQueue(msg);
    }

    if (RunTracker_feature.enabled) {
        if (eventname) {
            int visits = Location_visited(loc);
            runLogSend(&Log_Event, eventname->buf, 1, MAX(visits, 1));
        }
    }
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, WorldManager_CreateLocation, NULL, WorldManager_CreateLocation_post);
}

Patch patch_WorldManager_CreateLocation = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_CreateLocation),
                        &SYM(Location_event_offset),
                        &SYM(Location_visited_offset),
                        &SYM(LocationEvent_eventName_offset),
                        0 }
};
