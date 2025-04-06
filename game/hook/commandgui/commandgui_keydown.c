#include "ftl/commandgui.h"
#include "ftl/shipmanager.h"

#include "hook/hook.h"
#include "input/keyboard.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_CommandGui_KeyDown_pre(CommandGui* self, int key, bool shiftHeld)
{
    ShipManager* mgr = CommandGui_ship(self);
    bool ispaused    = CommandGui_IsPaused(self);
    bool isjumping   = mgr && ShipManager_GetIsJumping(mgr);

    if (keyDownInGame(self, key, shiftHeld, ispaused, isjumping))
        return 0;

    return 1;
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CommandGui_KeyDown) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CommandGui_KeyDown, subspace_CommandGui_KeyDown_pre, NULL);
}

Patch patch_CommandGui_KeyDown = { .Relevant = AlwaysRequired,
                                   .Validate = validate,
                                   .Apply    = apply };
