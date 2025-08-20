#include "ftl/filehelper.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_FileHelper_deleteAllSaveFiles_pre()
{
    if (gs.ignoreFileDeletion || gs.practiceMode)
        return 0;

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, FileHelper_deleteAllSaveFiles, subspace_FileHelper_deleteAllSaveFiles_pre, NULL);
}

Patch patch_FileHelper_deleteAllSaveFiles = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(FileHelper_deleteAllSaveFiles), 0 }
};
