#include "feature/timewarp.h"
#include "ftl/cfps.h"
#include "subspacegame.h"

void subspace_CFPS_OnLoop_post(CFPS* self)
{
    if (gs.timeWarpActive) {
        CFPS_SpeedFactor(self) = timeWarpAdjustSpeedFactor(CFPS_SpeedFactor(self));
    }
}
