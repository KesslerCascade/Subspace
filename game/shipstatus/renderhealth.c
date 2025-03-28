#include "shipmanager/shipmanager.h"
#include "shipstatus.h"
#include "subspacegame.h"

int subspace_ShipStatus_RenderHealth_pre(ShipStatus* self, bool renderText)
{
    ShipManager* sm = ShipStatus_ship(self);
    Ship* ship      = sm ? ShipManager_ship(sm) : NULL;

    if (ship) {
        tgd.overrideHullText = true;
        tgd.hull             = MEMBER(ftlbase, Ship, ship, int, hullIntegrity);
    }
    return 1;
}

int subspace_ShipStatus_RenderHealth_post(int ret, ShipStatus* self, bool renderText)
{
    tgd.overrideHullText = false;
    return ret;
}
