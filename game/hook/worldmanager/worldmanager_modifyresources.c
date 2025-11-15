#include "feature/runtracker.h"
#include "ftl/worldmanager.h"
#include "inventory/invscan.h"

#include "hook/hook.h"
#include "patch/patchlist.h"

static EventSource eventsrc_scrap;
static EventSource eventsrc_inv;

// ---- Hooks ----------------

int subspace_WorldManager_ModifyResources_pre(WorldManager* self, LocationEvent* event)
{
    if (RunTracker_feature.enabled)
        invScan();

    eventSourceSet(Scrap, &eventsrc_scrap, _S"Event");
    eventSourceSet(Inv, &eventsrc_inv, _S"Event");

    return 1;
}

LocationEvent* subspace_WorldManager_ModifyResources_post(LocationEvent* ret, WorldManager* self,
                                                          LocationEvent* event)
{
    if (RunTracker_feature.enabled)
        invScan();

    eventSourceFinish(Scrap, &eventsrc_scrap);
    eventSourceFinish(Inv, &eventsrc_inv);

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WorldManager_ModifyResources,
                        subspace_WorldManager_ModifyResources_pre,
                        subspace_WorldManager_ModifyResources_post);
}

Patch patch_WorldManager_ModifyResources = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_ModifyResources), 0 }
};
