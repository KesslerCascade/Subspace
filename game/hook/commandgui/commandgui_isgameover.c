#include "ftl/commandgui.h"

#include "feature/feature.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

bool subspace_CommandGui_IsGameOver_post(bool ret, CommandGui* self)
{
    // if tweaks is doing a post-game save, briefly pretend that the game isn't over
    if (Tweaks_feature.enabled && gs.postGameSaveInProgress)
        return false;

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CommandGui_IsGameOver, NULL, subspace_CommandGui_IsGameOver_post);
}

Patch patch_CommandGui_IsGameOver = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CommandGui_IsGameOver), 0 }
};
