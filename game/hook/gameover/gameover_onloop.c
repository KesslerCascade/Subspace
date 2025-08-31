#include "feature/screenshot.h"
#include "ftl/gameover.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static bool doneVictoryScreenshot;

// ---- Hooks ----------------

int subspace_GameOver_OnLoop_pre(GameOver* self)
{
    gc.inGameOverLoop = true;
    return 1;
}

void subspace_GameOver_OnLoop_post(GameOver* self)
{
    gc.inGameOverLoop = false;

    if (GameOver_bVictory(self)) {
        if (!doneVictoryScreenshot && !GameOver_bShowingCredits(self) &&
            screenshotAuto(SSEvent_Victory)) {
            gs.screenshotNowAuto  = true;
            doneVictoryScreenshot = true;
        }
    } else {
        doneVictoryScreenshot = false;
    }
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
    .requiredSymbols = { &SYM(GameOver_OnLoop),
                        &SYM(GameOver_bVictory_offset),
                        &SYM(GameOver_bShowingCredits_offset),
                        0 }
};
