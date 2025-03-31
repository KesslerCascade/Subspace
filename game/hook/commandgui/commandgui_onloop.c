#include "feature/frameadv.h"
#include "feature/timewarp.h"
#include "ftl/commandgui.h"
#include "subspacegame.h"

int subspace_CommandGui_OnLoop_pre(CommandGui* self)
{
    if (TimeWarp_feature.enabled) {
        timeWarpBeginFrame(self);
    }
    return 1;   // we should probably run CommandGui ;)
}

void subspace_CommandGui_OnLoop_post(CommandGui* self)
{
    if (FrameAdv_feature.enabled)
        frameAdvEndFrame(self);
}
