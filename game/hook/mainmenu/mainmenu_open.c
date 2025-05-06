#include "control/controlclient.h"

#include "ftl/mainmenu.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "proto.h"

// ---- Hooks ----------------

int subspace_MenuMenu_Open_pre(MainMenu* self)
{
    ControlMsg* msg = controlNewMsg("GameState", 1);
    controlMsgInt(msg, 0, "state", GAME_MENU);
    controlClientQueue(msg);

    return 1;
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, MainMenu_Open);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, MainMenu_Open, subspace_MenuMenu_Open_pre, NULL);
}

Patch patch_MainMenu_Open = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
