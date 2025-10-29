#include "control/runlog.h"
#include "feature/feature.h"
#include "feature/runtracker.h"
#include "ftl/location.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static EventSource eventsrc_damage;
static EventSource eventsrc_scrap;
static EventSource eventsrc_inv;

int WorldManager_UpdateLocation_pre(WorldManager* self, LocationEvent* event)
{
    gs.waitInProgress = false;

    if (RunTracker_feature.enabled) {
        eventSourceSet(Damage, &eventsrc_damage, "Event");
        eventSourceSet(Scrap, &eventsrc_scrap, "Event");
        eventSourceSet(Inv, &eventsrc_inv, "Event");

        basic_string* eventname = event ? LocationEvent_eventName(event) : NULL;
        if (eventname) {
            runLogSend(&Log_Event, eventname->buf, 0, 0);
        }
    }

    return 1;
}

void WorldManager_UpdateLocation_post(WorldManager* self, LocationEvent* event)
{
    eventSourceFinish(Damage, &eventsrc_damage);
    eventSourceFinish(Scrap, &eventsrc_scrap);
    eventSourceFinish(Inv, &eventsrc_inv);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WorldManager_UpdateLocation,
                        WorldManager_UpdateLocation_pre,
                        WorldManager_UpdateLocation_post);
}

Patch patch_WorldManager_UpdateLocation = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_UpdateLocation),
                        &SYM(Location_event_offset),
                        &SYM(LocationEvent_eventName_offset),
                        0 }
};
