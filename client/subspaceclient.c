#include "subspaceclient.h"
// NOTE: the entry point (i.e. "main" equivalent) is not located here, but is instead in entry.c in one of the playform-specific subdirectories

#include "ftl/ftl.h"
#include "loader/loader.h"
#include "patch/patch.h"
#include "patch/seq/seq_capp.h"
#include "patch/seq/seq_commandgui.h"
#include "patch/seq/seq_mousecontrol.h"
#include "patch/seq/seq_osdep.h"

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
    if (!patchApplySeq(&ps, OSDepPatches) || !patchApplySeq(&ps, CAppPatches) ||
        !patchApplySeq(&ps, CommandGuiPatches) || !patchApplySeq(&ps, MouseControlPatches)) {
        // log
        return 1;
    }
    if (!patchEnd(&ps)) {
        // log
        return 1;
    }

    ftlentry = getProgramEntry(ftlbase);
    ftlentry();
    return 0;
}
