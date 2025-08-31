#include "ftl/capp.h"
#include "ftl/filehelper.h"
#include "ftl/misc.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_CApp_OnExecute_pre(CApp* self)
{
    // save the pointer to the CApp instance
    theApp = self;

    sscmain2();   // run secondary main function

    return 1;   // we do want to execute the original CApp::OnExecute
}

int subspace_CApp_OnExecute_post(int ret, CApp* self)
{
    if (gs.deleteSaveOnMenu) {
        // we had a save file that was supposed to be deleted when going back to the menu, but we
        // never returned to the menu, so delete it now
        gs.ignoreFileDeletion = false;
        FileHelper_deleteAllSaveFiles();
        gs.deleteSaveOnMenu = false;
    }

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        CApp_OnExecute,
                        subspace_CApp_OnExecute_pre,
                        subspace_CApp_OnExecute_post);
}

Patch patch_CApp_OnExecute = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CApp_OnExecute),
                        &SYM(version_major),
                        &SYM(version_minor),
                        &SYM(version_rev),
                        &SYM(FileHelper_deleteAllSaveFiles),
                        0 }
};
