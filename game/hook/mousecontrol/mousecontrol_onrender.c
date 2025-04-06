#include "ftl/cfps.h"
#include "ftl/mousecontrol.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

// This is just a convenient spot to render random stuff on the screen; since the mouse cursor is
// one of the last things that's drawn
int subspace_MouseControl_OnRender_pre(MouseControl* self)
{
    return 1;
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, MouseControl_OnRender) && symResolve(base, CFPS_fps_offset);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, MouseControl_OnRender, subspace_MouseControl_OnRender_pre, NULL);
}

Patch patch_MouseControl_OnRender = { .Relevant = AlwaysRequired,
                                      .Validate = validate,
                                      .Apply    = apply };
