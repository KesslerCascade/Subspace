#include "ftl/gameover.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_GameOver_OnLoop_pre(GameOver* self)
{
    gc.inGameOverLoop = true;
    return 1;
}

void subspace_GameOver_OnLoop_post(GameOver* self)
{
    gc.inGameOverLoop = false;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        GameOver_OnLoop,
                        subspace_GameOver_OnLoop_pre,
                        subspace_GameOver_OnLoop_post);
}

Patch patch_GameOver_OnLoop = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(GameOver_OnLoop), 0 }
};
