#include "ftl/cfps.h"
#include "ftl/ftl.h"
#include "ftl/mousecontrol.h"
#include "hook/hook.h"
#include "patch/patch.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, MouseControl_OnRender) && symResolve(base, CFPS_fps_offset);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, MouseControl_OnRender, subspace_MouseControl_OnRender_pre, NULL);
}

Patch patch_MouseControl_OnRender = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
