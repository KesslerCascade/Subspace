#include "feature/timewarp.h"
#include "ftl/commandgui.h"
#include "subspacegame.h"

int subspace_CommandGui_OnLoop_pre(CommandGui* self)
{
    if (settings.timeWarp->enabled) {
        timeWarpBeginFrame(self);
    }
    return 1;   // we should probably run CommandGui ;)
}