#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

int WorldManager_CreateShip_pre(WorldManager* self, ShipEvent* ship, bool boss)
{
    if (RunTracker_feature.enabled) {
        // TODO: This offset should really be found through a DisasmTrace.
        // However, it's not referenced in any convenient places and would be a big PITA, plus it's
        // the same in all supported versions, so just hardcoded it for now.
        basic_string* shipname = ship ? (basic_string*)((char*)ship + 4) : NULL;
        if (shipname) {
            runLogSend(&Log_Ship, shipname->buf);
        }
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, WorldManager_CreateShip, WorldManager_CreateShip_pre, NULL);
}

Patch patch_WorldManager_CreateShip = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_CreateShip), 0 }
};
