#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/location.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

int WorldManager_CreateLocation_pre(WorldManager* self, Location* loc)
{
    gs.waitInProgress = false;

    if (RunTracker_feature.enabled) {
        LocationEvent* event = loc ? Location_event(loc) : NULL;
        basic_string* eventname = event ? LocationEvent_eventName(event) : NULL;
        if (eventname) {
            runLogSend(&Log_Event, eventname->buf, 1);
        }
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, WorldManager_CreateLocation, WorldManager_CreateLocation_pre, NULL);
}

Patch patch_WorldManager_CreateLocation = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_CreateLocation),
                        &SYM(Location_event_offset),
                        &SYM(LocationEvent_eventName_offset),
                        0 }
};
