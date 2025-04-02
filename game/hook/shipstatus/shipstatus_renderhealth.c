#include "feature/numerichull.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "subspacegame.h"

int subspace_ShipStatus_RenderHealth_pre(ShipStatus* self, bool renderText)
{
    if (NumericHull_feature.enabled) {
        ShipManager* sm = ShipStatus_ship(self);
        if (sm)
            numericHullBeforeRenderHealth(sm, renderText);
    }
    return 1;
}

void subspace_ShipStatus_RenderHealth_post(ShipStatus* self, bool renderText)
{
    if (NumericHull_feature.enabled)
        numericHullAfterRenderHealth(renderText);
}
