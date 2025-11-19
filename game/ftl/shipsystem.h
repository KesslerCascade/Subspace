#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef void (*FUNCTYPE(ShipSystem_OnLoop))(ShipSystem* self);
DECLFUNC(ShipSystem_OnLoop);

typedef bool (*FUNCTYPE(ShipSystem_GetExploded))(ShipSystem* self);
DECLFUNC(ShipSystem_GetExploded);