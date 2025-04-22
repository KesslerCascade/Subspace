#include "control/controlclient.h"
#include "ftl/capp.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_CApp_OnLoop_pre(CApp* self)
{
    controlClientProcess();
    return 1;   // we do want to execute the original CApp::OnLoop
}

void subspace_CApp_OnLoop_post(CApp* self) {}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CApp_OnLoop) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnLoop, subspace_CApp_OnLoop_pre, subspace_CApp_OnLoop_post);
}

Patch patch_CApp_OnLoop = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
