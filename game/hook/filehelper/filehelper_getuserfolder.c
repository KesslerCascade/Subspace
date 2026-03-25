#include "ftl/filehelper.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

basic_string* subspace_FileHelper_getUserFolder(basic_string* out)
{
    if (!strEmpty(settings.saveOverride)) {
        basic_string_set_str(out, settings.saveOverride);
        return out;
    }

    return FileHelper_getUserFolder(out);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return replaceFunction(base, FileHelper_getUserFolder, subspace_FileHelper_getUserFolder);
}

Patch patch_FileHelper_getUserFolder = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(FileHelper_getUserFolder), 0 }
};
