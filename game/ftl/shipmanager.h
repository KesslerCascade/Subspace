#pragma once
#include "ftl/completeship.h"
#include "ftl/ftl.h"
#include "ftl/ship.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ShipManager ShipManager;
typedef struct ShipBlueprint ShipBlueprint;

// FTL functions & wrappers below

extern DisasmTrace ShipManager_DamageHull_trace;

typedef bool (*FUNCTYPE(ShipManager_OnInit))(ShipManager* ship, ShipBlueprint* bluePrint,
                                             int level);
DECLFUNC(ShipManager_OnInit);

typedef int (*FUNCTYPE(ShipManager_DamageHull))(ShipManager* ship, int dmg, bool force);
DECLFUNC(ShipManager_DamageHull);
#define ShipManager_DamageHull(self, dmg, force) \
    FCALL(ftlbase, ShipManager_DamageHull, self, dmg, force)

typedef bool (*FUNCTYPE(ShipManager_GetIsJumping))(ShipManager* ship);
DECLFUNC(ShipManager_GetIsJumping);
#define ShipManager_GetIsJumping(self) FCALL(ftlbase, ShipManager_GetIsJumping, self)

typedef int (*FUNCTYPE(ShipManager_GetDroneCount))(ShipManager* ship);
DECLFUNC(ShipManager_GetDroneCount);
#define ShipManager_GetDroneCount(self) FCALL(ftlbase, ShipManager_GetDroneCount, self)

typedef int (*FUNCTYPE(ShipManager_GetMissileCount))(ShipManager* ship);
DECLFUNC(ShipManager_GetMissileCount);
#define ShipManager_GetMissileCount(self) FCALL(ftlbase, ShipManager_GetMissileCount, self)

DECLSYM(ShipManager_ship_offset);
DECLSYM(ShipManager_current_target_offset);
DECLSYM(ShipManager_currentScrap_offset);
DECLSYM(ShipManager_myBlueprint_offset);
DECLSYM(ShipManager_myBlueprint_name_offset);
DECLSYM(ShipManager_myBlueprint_blueprintName_offset);

#define ShipManager_ship(ship_manager) (&MEMBER(ftlbase, ShipManager, ship_manager, Ship, ship))
#define ShipManager_current_target(ship_manager) \
    (&MEMBER(ftlbase, ShipManager, ship_manager, ShipManager, current_target))
