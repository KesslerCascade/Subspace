#include "feature/numerichull.h"
#include "ftl/textlibrary.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

basic_string* subspace_TextLibrary_GetText(TextLibrary* self, basic_string* text, basic_string* out, basic_string* lang) {
    if (NumericHull_feature.enabled && numericHullCheckText(self, text, out))
        return out;   // out was overridden

    return TextLibrary_GetText(self, text, out, lang);
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, TextLibrary_GetText_3arg) || symResolve(base, TextLibrary_GetText_2arg);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    // select which version of GetText to use
    if (symResolve(base, TextLibrary_GetText_3arg))
        FUNCP_SELECT(TextLibrary_GetText, TextLibrary_GetText_3arg);
    else
        FUNCP_SELECT(TextLibrary_GetText, TextLibrary_GetText_2arg);
    return replaceFunctionP(base, TextLibrary_GetText, subspace_TextLibrary_GetText);
}

Patch patch_TextLibrary_GetText = { .Relevant = AlwaysRequired,
                                    .Validate = validate,
                                    .Apply    = apply };
