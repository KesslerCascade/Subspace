#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef int (*FUNCTYPE(HackingSystem_GetSpendDrone))(HackingSystem* self);
DECLFUNC(HackingSystem_GetSpendDrone);