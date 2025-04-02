#include "feature/timewarp.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "subspacegame.h"

void subspace_ShipStatus_OnRender_post(ShipStatus* self, bool renderText)
{
    if (TimeWarp_feature.enabled)
        timeWarpRender();
}
