#include "ftl/cfps.h"
#include "timewarp/timewarp.h"
#include "subspacegame.h"

void subspace_CFPS_OnLoop_post(CFPS* self)
{
    if (gs.timeWarpActive) {
        // SpeedFactor is capped to 0.5 to prevent things like projectiles warping past shields
        float speedFactor      = MIN(CFPS_SpeedFactor(self) * gs.warpFactor, 0.5);
        CFPS_SpeedFactor(self) = speedFactor;
        timeWarpFrameCalcActual(speedFactor);
    }
}
