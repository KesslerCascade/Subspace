#include "subspacegame.h"
// NOTE: the entry point (i.e. "main" equivalent) is not located here, but is instead in entry.c in one of the playform-specific subdirectories

#include "feature/frameadv.h"
#include "feature/numerichull.h"
#include "feature/timewarp.h"
#include "ftl/ftl.h"
#include "loader/loader.h"
#include "patch/patch.h"
#include "patch/seq/seq_osdep.h"
#include "patch/seq/seq_required.h"

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
        return 1;
    }

    if (initFeature(&FrameAdv_feature, &ps))
        enableFeature(&FrameAdv_feature, true);

    if (!patchEnd(&ps)) {
        // log
        return 1;
    }

    ftlentry = getProgramEntry(ftlbase);
    ftlentry();
    return 0;
}
