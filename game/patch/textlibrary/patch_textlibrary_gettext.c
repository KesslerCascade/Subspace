#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ftl/textlibrary.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, TextLibrary_GetText_2arg) || symResolve(base, TextLibrary_GetText_3arg);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    // select which version of GetText to use
    if (symResolve(base, TextLibrary_GetText_2arg))
        FUNCP_SELECT(TextLibrary_GetText, TextLibrary_GetText_2arg);
    else
        FUNCP_SELECT(TextLibrary_GetText, TextLibrary_GetText_3arg);
    return replaceFunctionP(base, TextLibrary_GetText, subspace_TextLibrary_GetText);
}

Patch patch_TextLibrary_GetText = { .Relevant = AlwaysRequired,
                                    .Validate = validate,
                                    .Apply    = apply };
