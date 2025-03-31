#include "feature/numerichull.h"
#include "ftl/combatcontrol.h"
#include "ftl/shipmanager.h"
#include "subspacegame.h"

int subspace_CombatControl_RenderTarget_pre(CombatControl* self)
{
    if (NumericHull_feature.enabled) {
        ShipManager* sm = CombatControl_GetCurrentTarget(self);
        if (sm)
            numericHullBeforeRenderTarget(sm);
    }

    return 1;
}

void subspace_CombatControl_RenderTarget_post(CombatControl* self)
{
    if (NumericHull_feature.enabled)
        numericHullAfterRenderTarget();
}
