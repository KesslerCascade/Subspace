#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct Ship Ship;

// FTL functions & wrappers below

typedef int (*FUNCTYPE(Ship_DamageHull))(Ship *ship, int dmg);
DECLFUNC(Ship_DamageHull);

DECLSYM(Ship_hullIntegrity_offset);
