#include "feature/screenshot.h"
#include "ftl/creditscreen.h"
#include "ftl/gameover.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static bool doneVictoryScreenshot;
static bool doneCreditsScreenshot;

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

        if (GameOver_bShowingCredits(self)) {
            CreditScreen* credits = GameOver_credits(self);
            float pausing         = credits ? CreditScreen_pausing(credits) : 0;
            if (!doneCreditsScreenshot && pausing > 0 && pausing < 4 &&
                screenshotAuto(SSEvent_Credits)) {
                gs.screenshotNowAuto  = true;
                doneCreditsScreenshot = true;
            }
        }
    } else {
        doneVictoryScreenshot = false;
        doneCreditsScreenshot = false;
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
                        &SYM(GameOver_credits_offset),
                        &SYM(CreditScreen_pausing_offset),
                        0 }
};
