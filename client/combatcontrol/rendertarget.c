#include "shipmanager/shipmanager.h"
#include "combatcontrol.h"
#include "subspaceclient.h"

int subspace_CombatControl_RenderTarget_pre(CombatControl* self)
{
    ShipManager* sm = CombatControl_GetCurrentTarget(self);
    Ship* ship      = sm ? ShipManager_ship(sm) : NULL;

    if (ship) {
        tgd.overrideHullText = true;
        tgd.hull             = MEMBER(ftlbase, Ship, ship, int, hullIntegrity);
    }
    return 1;
}

int subspace_CombatControl_RenderTarget_post(int ret, CombatControl* self)
{
    tgd.overrideHullText = false;
    return ret;
}
