#include "ftl/filehelper.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

basic_string* subspace_FileHelper_getSaveFile(basic_string* out)
{
    if (gs.saveFileOverride) {
        basic_string_set(out, gs.saveFileOverride);
        return out;
    }

    return FileHelper_getSaveFile(out);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return replaceFunction(base, FileHelper_getSaveFile, subspace_FileHelper_getSaveFile);
}

Patch patch_FileHelper_getSaveFile = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(FileHelper_getSaveFile), 0 }
};
