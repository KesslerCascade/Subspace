#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "subspacegame.h"

int subspace_ShipStatus_RenderHealth_pre(ShipStatus* self, bool renderText)
{
    ShipManager* sm = ShipStatus_ship(self);
    Ship* ship      = sm ? ShipManager_ship(sm) : NULL;

    if (ship) {
        gs.overrideHullText = true;
        gs.hull             = MEMBER(ftlbase, Ship, ship, int, hullIntegrity);
    }
    return 1;
}

int subspace_ShipStatus_RenderHealth_post(int ret, ShipStatus* self, bool renderText)
{
    gs.overrideHullText = false;
    return ret;
}
