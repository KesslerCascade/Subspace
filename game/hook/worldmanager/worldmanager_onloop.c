#include "ftl/cfps.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

int WorldManager_OnLoop_pre(WorldManager* self)
{
    // this is essentially the same thing CFPS::OnLoopUnpaused does, but it's our private copy that
    // resets with each jump; since it would have no way of persisting across save/load anyway
    gs.gameTime += (double)CFPS_SpeedFactor(FPSControl) * (double)0.0625;

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, WorldManager_OnLoop, WorldManager_OnLoop_pre, NULL);
}

Patch patch_WorldManager_OnLoop = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_OnLoop),
                        &SYM(CFPS_FPSControl),
                        &SYM(CFPS_SpeedFactor_offset),
                        0 }
};
