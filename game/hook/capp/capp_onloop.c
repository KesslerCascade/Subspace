#include "control/controlclient.h"
#include "feature/feature.h"
#include "feature/practicemode.h"
#include "feature/savemanager.h"
#include "feature/screenshot.h"
#include "feature/tweaks.h"
#include "ftl/capp.h"
#include "ftl/scorekeeper.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

#define NUM_STATS 4
static int lastStats[NUM_STATS];

// ---- Hooks ----------------

int subspace_CApp_OnLoop_pre(CApp* self)
{
    osNextFrame();                   // record the current time for anything that uses it this frame
    controlClientProcessInbound();   // process any incoming control messages

    return 1;                        // we do want to execute the original CApp::OnLoop
}

void subspace_CApp_OnLoop_post(CApp* self)
{
    StatTracker* stats = ScoreKeeper_stats(SKeeper);
    int statschanged   = 0;

    // we actually have hooks for the stat update functions, but it's super cheap to simply check
    // them each loop anyway, since for stat tracking we don't care about WHY it changed

    for (int i = 0; i < NUM_STATS; i++) {
        if (stats[i].current != lastStats[i])
            statschanged++;
    }

    if (gs.sendAllStats)
        statschanged = NUM_STATS;

    if (statschanged) {
        ControlMsg* msg = controlMsgCreate(_S"Stats");

        if (gs.sendAllStats || stats[0].current != lastStats[0])
            cfieldSet(msg, _S"ships", int32, stats[0].current);
        if (gs.sendAllStats || stats[1].current != lastStats[1])
            cfieldSet(msg, _S"beacons", int32, stats[1].current);
        if (gs.sendAllStats || stats[2].current != lastStats[2])
            cfieldSet(msg, _S"scrap", int32, stats[2].current);
        if (gs.sendAllStats || stats[3].current != lastStats[3])
            cfieldSet(msg, _S"crew", int32, stats[3].current);
        controlClientQueue(msg);

        for (int i = 0; i < NUM_STATS; i++) {
            lastStats[i] = stats[i].current;
        }
    }

    gs.sendAllStats = false;

    if (RunTracker_feature.enabled) {
        gc.curScrapSource  = NULL;
        gc.curDamageSource = NULL;
    }

    if (SaveManager_feature.enabled && gs.autoSaveNow)
        saveManagerAutoSave(CApp_world(self));
    gs.autoSaveNow = false;

    if (Tweaks_feature.enabled && gs.postGameSaveNow)
        tweaksPostGameSave();
    gs.postGameSaveNow = false;

    if (gs.practiceLoadSave)
        practiceLoad();
    gs.practiceLoadSave = false;

    if (Screenshot_feature.enabled) {
        screenshotCheckDestroyed();
        screenshotCheckSound();
    }

    controlClientProcessOutbound();
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnLoop, subspace_CApp_OnLoop_pre, subspace_CApp_OnLoop_post);
}

Patch patch_CApp_OnLoop = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CApp_OnLoop), &SYM(CApp_world_offset), &SYM(ScoreKeeper_Keeper), 0 }
};
