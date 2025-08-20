#include "ftl/filehelper.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_FileHelper_deleteFile_pre(basic_string* filename)
{
    if (gs.ignoreFileDeletion || gs.practiceMode)
        return 0;

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, FileHelper_deleteFile, subspace_FileHelper_deleteFile_pre, NULL);
}

Patch patch_FileHelper_deleteFile = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(FileHelper_deleteFile), 0 }
};
