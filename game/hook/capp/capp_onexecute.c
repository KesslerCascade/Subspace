#include "ftl/capp.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_CApp_OnExecute_pre(CApp* self)
{
    // save the pointer to the CApp instance
    theApp = self;

    sscmain2();   // run secondary main function

    return 1;   // we do want to execute the original CApp::OnExecute
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CApp_OnExecute) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnExecute, subspace_CApp_OnExecute_pre, NULL);
}

Patch patch_CApp_OnExecute = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
