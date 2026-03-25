#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef void (*FUNCTYPE(ShipSystem_OnLoop))(ShipSystem* self);
DECLFUNC(ShipSystem_OnLoop);

typedef bool (*FUNCTYPE(ShipSystem_GetExploded))(ShipSystem* self);
DECLFUNC(ShipSystem_GetExploded);

// this comes from Repairable::ShipObject and is always at offset 12 in all versions.
// NOTE: 0 is player ship, 1 is enemy ship
#define ShipSystem_iShipId(sys) (MEMBER_FIXED(ftlbase, ShipSystem, sys, int, 12))