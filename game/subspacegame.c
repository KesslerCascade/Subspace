#include "subspacegame.h"
// NOTE: the entry point (i.e. "main" equivalent) is not located here, but is instead in entry.c in
// one of the playform-specific subdirectories

#include "feature/frameadv.h"
#include "feature/infoblock.h"
#include "feature/numerichull.h"
#include "feature/timewarp.h"
#include "ftl/ftl.h"
#include "loader/loader.h"
#include "patch/patch.h"

SubspaceGameSettings settings = {
    .frameAdv = &FrameAdv_feature,
    //.numericHull = &NumericHull_feature,
    .timeWarp = &TimeWarp_feature,
};
GameState gs;

entrypoint ftlentry;
int sscmain(void)
{
    ftlbase = loadProgram(settings.gameProgram);

    PatchState ps;
    if (!patchBegin(&ps, ftlbase)) {
        // log
        return 1;
    }
    if (!patchApplySeq(&ps, OSDepPatches) || !patchApplySeq(&ps, RequiredPatches)) {
        // log
        WriteDbg("Required patches failed.\n");
        return 1;
    }

    if (initFeature(&InfoBlock_feature, &ps)) {
        enableFeature(&InfoBlock_feature, true);
        WriteDbg("InfoBlock: PASSED\n");
    } else {
        WriteDbg("InfoBlock: FAILED\n");
    }
    if (initFeature(&TimeWarp_feature, &ps)) {
        enableFeature(&TimeWarp_feature, true);
        WriteDbg("TimeWarp: PASSED\n");
    } else {
        WriteDbg("TimeWarp: FAILED\n");
    }
    if (initFeature(&FrameAdv_feature, &ps)) {
        enableFeature(&FrameAdv_feature, true);
        WriteDbg("FrameAdv: PASSED\n");
    } else {
        WriteDbg("FrameAdv: FAILED\n");
    }
    if (initFeature(&NumericHull_feature, &ps)) {
        enableFeature(&NumericHull_feature, true);
        WriteDbg("NumericHull: PASSED\n");
    } else {
        WriteDbg("NumericHull: FAILED\n");
    }

    if (!patchEnd(&ps)) {
        // log
        return 1;
    }

    if (!settings.testMode) {
        ftlentry = getProgramEntry(ftlbase);
        ftlentry();
    }
    return 0;
}
