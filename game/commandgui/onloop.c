#include "ftl/functions_commandgui.h"
#include "commandgui.h"
#include "subspacegame.h"

int subspace_CommandGui_OnLoop_pre(CommandGui* self)
{
    if (gs.timeWarpActive) {
        // time warp always ends when the game is paused
        if (CommandGui_IsPaused(self))
            gs.timeWarpActive = false;
    }
    return 1;   // we should probably run CommandGui ;)
}