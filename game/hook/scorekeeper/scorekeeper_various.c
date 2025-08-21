#include "control/runlog.h"
#include "ftl/scorekeeper.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// hook a lot of these together, we don't need separate files for each one

int subspace_ScoreKeeper_Save_pre(ScoreKeeper* self, bool newHighscore)
{
    if (gs.practiceMode)
        return 0;
    return 1;
}

int subspace_ScoreKeeper_SetVictory_pre(ScoreKeeper* self, bool victory)
{
    if (gs.practiceMode)
        return 0;
    return 1;
}

int subspace_ScoreKeeper_AddCrew_pre(ScoreKeeper* self)
{
    if (gs.practiceMode)
        return 0;
    return 1;
}

int subspace_ScoreKeeper_AddDefeatedShips_pre(ScoreKeeper* self)
{
    if (gs.practiceMode)
        return 0;
    return 1;
}
int subspace_ScoreKeeper_AddExploredLocations_pre(ScoreKeeper* self)
{
    if (gs.practiceMode)
        return 0;
    return 1;
}

int subspace_ScoreKeeper_AddScrapCollected_pre(ScoreKeeper* self, int amount)
{
    if (gs.practiceMode)
        return 0;
    return 1;
}

int subspace_ScoreKeeper_UnlockShip_pre(ScoreKeeper* self, int shipId, int layout, bool save,
                                        bool hidePopup)
{
    if (gs.practiceMode)
        return 0;
    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, ScoreKeeper_Save, subspace_ScoreKeeper_Save_pre, NULL) &&
        hookFunction(base, ScoreKeeper_SetVictory, subspace_ScoreKeeper_SetVictory_pre, NULL) &&
        hookFunction(base, ScoreKeeper_AddCrew, subspace_ScoreKeeper_AddCrew_pre, NULL) &&
        hookFunction(base,
                     ScoreKeeper_AddDefeatedShips,
                     subspace_ScoreKeeper_AddDefeatedShips_pre,
                     NULL) &&
        hookFunction(base,
                     ScoreKeeper_AddExploredLocations,
                     subspace_ScoreKeeper_AddExploredLocations_pre,
                     NULL) &&
        hookFunction(base,
                     ScoreKeeper_AddScrapCollected,
                     subspace_ScoreKeeper_AddScrapCollected_pre,
                     NULL) &&
        hookFunction(base, ScoreKeeper_UnlockShip, subspace_ScoreKeeper_UnlockShip_pre, NULL);
}

Patch patch_ScoreKeeper_Various = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ScoreKeeper_Save),
                        &SYM(ScoreKeeper_SetVictory),
                        &SYM(ScoreKeeper_AddCrew),
                        &SYM(ScoreKeeper_AddDefeatedShips),
                        &SYM(ScoreKeeper_AddExploredLocations),
                        &SYM(ScoreKeeper_AddScrapCollected),
                        &SYM(ScoreKeeper_UnlockShip),
                        0 }
};
