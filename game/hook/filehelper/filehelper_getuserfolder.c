#include "ftl/filehelper.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

basic_string* subspace_FileHelper_getUserFolder(basic_string* out)
{
    if (settings.saveOverride) {
        basic_string_set(out, settings.saveOverride);
        return out;
    }

    return FileHelper_getUserFolder(out);
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, FileHelper_getUserFolder);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return replaceFunction(base, FileHelper_getUserFolder, subspace_FileHelper_getUserFolder);
}

Patch patch_FileHelper_getUserFolder = { .Relevant = AlwaysRequired,
                                         .Validate = validate,
                                         .Apply    = apply };
