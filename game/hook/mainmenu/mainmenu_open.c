#include "control/controlclient.h"

#include "ftl/filehelper.h"
#include "ftl/mainmenu.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "proto.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_MenuMenu_Open_pre(MainMenu* self)
{
    ControlMsg* msg = controlMsgCreate(_S"GameState");
    cfieldSet(msg, _S"state", int32, GAME_MENU);
    controlClientQueue(msg);

    // if we were practicing, going to the main menu cancels it
    gs.practiceMode = false;

    if (gs.deleteSaveOnMenu) {
        gs.ignoreFileDeletion = false;
        FileHelper_deleteAllSaveFiles();
        gs.deleteSaveOnMenu = false;
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, MainMenu_Open, subspace_MenuMenu_Open_pre, NULL);
}

Patch patch_MainMenu_Open = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(MainMenu_Open), &SYM(FileHelper_deleteAllSaveFiles), 0 }
};
