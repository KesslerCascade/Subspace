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

typedef void (*FUNCTYPE(ShipManager_ModifyScrapCount))(ShipManager* ship, int amount, bool income);
DECLFUNC(ShipManager_ModifyScrapCount);
#define ShipManager_ModifyScrapCount(self, amount, income) \
    FCALL(ftlbase, ShipManager_GetMissileCount, self, amount, income)

// technically this is a ShipObject method, but it's ShipManager's base class and is always at
// offset 0
typedef int (*FUNCTYPE(ShipManager_HasEquipment))(ShipManager* ship, basic_string* blueName);
DECLFUNC(ShipManager_HasEquipment);
#define ShipManager_HasEquipment(self) FCALL(ftlbase, ShipManager_HasEquipment, self)

DECLSYM(ShipManager_ship_offset);
DECLSYM(ShipManager_current_target_offset);
DECLSYM(ShipManager_currentScrap_offset);
DECLSYM(ShipManager_myBlueprint_offset);
DECLSYM(ShipManager_myBlueprint_name_offset);
DECLSYM(ShipManager_myBlueprint_blueprintName_offset);

#define ShipManager_ship(ship_manager) (&MEMBER(ftlbase, ShipManager, ship_manager, Ship, ship))
#define ShipManager_current_target(ship_manager) \
    (&MEMBER(ftlbase, ShipManager, ship_manager, ShipManager, current_target))

#define ShipManager_myBlueprint(ship_manager) \
    (&MEMBER(ftlbase, ShipManager, ship_manager, ShipBlueprint, myBlueprint))