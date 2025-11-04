#include "control/controlclient.h"
#include "control/runlog.h"
#include "feature/feature.h"
#include "feature/runtracker.h"
#include "ftl/capp.h"
#include "ftl/location.h"
#include "ftl/scorekeeper.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static EventSource eventsrc;

int WorldManager_CreateLocation_pre(WorldManager* self, Location* loc)
{
    gs.waitInProgress = false;
    gs.gameTime       = 0;

    if (RunTracker_feature.enabled && !gc.loadingGame) {
        // any damage that happens during this call is event damage
        eventSourceSet(Damage, &eventsrc, "Event");
    }

    return 1;
}

void WorldManager_CreateLocation_post(WorldManager* self, Location* loc)
{
    if (gc.loadingGame)
        return;   // don't do this while loading the game

    eventSourceFinish(Damage, &eventsrc);

    LocationEvent* event    = loc ? Location_event(loc) : NULL;
    basic_string* eventname = event ? LocationEvent_eventName(event) : NULL;

    if (RunTracker_feature.enabled || SaveManager_feature.enabled) {
        // send beacon details first so savepoint can be updated if needed
        ControlMsg* msg     = controlMsgCreate(_S"Beacon");
        WorldManager* world = CApp_world(theApp);
        int visits          = Location_visited(loc);
        Pointf* pos         = (Pointf*)loc;   // always the first field in Location

        cfieldSet(msg, _S"sector", int32, WorldManager_worldLevel(world) + 1);
        cfieldSet(msg, _S"beacons", int32, ScoreKeeper_stats(SKeeper)[1].current);
        cfieldSet(msg, _S"visit", int32, max(visits, 1));
        cfieldSet(msg, _S"x", int32, (int)pos->x);
        cfieldSet(msg, _S"y", int32, (int)pos->y);
        cfieldSet(msg, _S"event", strref, eventname ? (strref)eventname->buf : (strref)"");

        msg->priority = 2;   // ensure this comes after NewGame and Sector
        controlClientQueue(msg);
    }

    if (RunTracker_feature.enabled) {
        if (eventname) {
            int visits = Location_visited(loc);
            runLogSend(&Log_Event, eventname->buf, 1, max(visits, 1));
        }
    }

    // if we're at a new location, flag the next autosave to be archived
    if (SaveManager_feature.enabled)
        gs.autoSaveNow = true;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WorldManager_CreateLocation,
                        WorldManager_CreateLocation_pre,
                        WorldManager_CreateLocation_post);
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
