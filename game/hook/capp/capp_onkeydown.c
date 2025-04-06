#include "ftl/capp.h"
#include "hook/hook.h"
#include "input/keyboard.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_CApp_OnKeyDown_pre(CApp* self, int key)
{
    if (keyDownGlobal(self, key))
        return 0;
    return 1;   // we do want to execute the original CApp::OnKeyDown
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CApp_OnKeyDown) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnKeyDown, subspace_CApp_OnKeyDown_pre, NULL);
}

Patch patch_CApp_OnKeyDown = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
