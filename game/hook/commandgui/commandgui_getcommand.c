#include "ftl/commandgui.h"

#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_CommandGui_GetCommand_post(int ret, CommandGui* self)
{
    if (gs.saveAndQuit)
        return 8;

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CommandGui_GetCommand, NULL, subspace_CommandGui_GetCommand_post);
}

Patch patch_CommandGui_GetCommand = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CommandGui_GetCommand), 0 }
};
