#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef bool (*FUNCTYPE(ShipSystem_GetExploded))(ShipSystem* self);
DECLFUNC(ShipSystem_GetExploded);