#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/globals.h"
#include "ftl/settings.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static const char* difftext[3] = { "_EASY", "_NORMAL", "_HARD" };

int WorldManager_CreateShip_pre(WorldManager* self, ShipEvent* ship, bool boss)
{
    if (RunTracker_feature.enabled) {
        if (boss) {
            // This is contructed when CreateShip calls BossShip::StartStage. It's Easier to just
            // grab the blueprint name from BlueprintManager::GetShipBlueprint
            gc.loadingBossShip = true;
        } else {
            // TODO: This offset should really be found through a DisasmTrace.
            // However, it's not referenced in any convenient places and would be a big PITA, plus
            // it's the same in all supported versions, so just hardcoded it for now.
            basic_string* shipname = ship ? (basic_string*)((char*)ship + 4) : NULL;
            if (shipname && shipname->len > 0) {
                runLogSend(&Log_Ship, shipname->buf);
            }
        }
    }

    return 1;
}

CompleteShip*
WorldManager_CreateShip_post(CompleteShip* ret, WorldManager* self, ShipEvent* ship, bool boss)
{
    gc.loadingBossShip = false;
    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WorldManager_CreateShip,
                        WorldManager_CreateShip_pre,
                        WorldManager_CreateShip_post);
}

Patch patch_WorldManager_CreateShip = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_CreateShip), 0 }
};
