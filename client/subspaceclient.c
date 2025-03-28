#include "subspaceclient.h"
// NOTE: the entry point (i.e. "main" equivalent) is not located here, but is instead in entry.c in one of the playform-specific subdirectories

#include "ftl/ftl.h"
#include "loader/loader.h"
#include "patch/patch.h"
#include "patch/seq/seq_numerichull.h"
#include "patch/seq/seq_osdep.h"
#include "patch/seq/seq_required.h"

SubspaceClientSettings scsettings;

entrypoint ftlentry;
int sscmain(void)
{
    ftlbase = loadProgram(scsettings.gameProgram);

    PatchState ps;
    if (!patchBegin(&ps, ftlbase)) {
        // log
        return 1;
    }
    if (!patchApplySeq(&ps, OSDepPatches) || !patchApplySeq(&ps, RequiredPatches)) {
        // log
        return 1;
    }

    patchApplySeq(&ps, NumericHullPatches);

    if (!patchEnd(&ps)) {
        // log
        return 1;
    }

    ftlentry = getProgramEntry(ftlbase);
    ftlentry();
    return 0;
}
