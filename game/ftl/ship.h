#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

// FTL functions & wrappers below

typedef int (*FUNCTYPE(Ship_DamageHull))(Ship *ship, int dmg);
DECLFUNC(Ship_DamageHull);

typedef void (*FUNCTYPE(Ship_ProjectileStrike))(Ship* self, int roomId, float damage);
DECLFUNC(Ship_ProjectileStrike);

typedef bool (*FUNCTYPE(Ship_DestroyedDone))(Ship* ship);
DECLFUNC(Ship_DestroyedDone);

DECLSYM(Ship_hullIntegrity_offset);
#define Ship_hullIntegrity(ship)    (MEMBER(ftlbase, Ship, ship, int, hullIntegrity))
#define Ship_hullIntegrityMax(ship) ((&MEMBER(ftlbase, Ship, ship, int, hullIntegrity))[1])
