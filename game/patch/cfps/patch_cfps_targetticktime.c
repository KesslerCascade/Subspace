#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ftl/globals.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, TargetFrameTimeMS);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    // Relocate the global variable out of rdata into writable memory
    // Copy value first
    g_TargetFrameTimeMS = *(double*)symAddr(base, TargetFrameTimeMS);
    return replaceVariable(base, symAddr(base, TargetFrameTimeMS), &g_TargetFrameTimeMS);
}

Patch patch_CFPS_TargetTickTime = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
