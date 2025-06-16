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
extern DisasmTrace ShipManager_SunDamage_trace;

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
#define ShipManager_HasEquipment(self, blueName) \
    FCALL(ftlbase, ShipManager_HasEquipment, self, blueName)

typedef int (*FUNCTYPE(ShipManager_HasAugmentation))(ShipManager* ship, basic_string* augId);
DECLFUNC(ShipManager_HasAugmentation);
#define ShipManager_HasAugmentation(self, augId) \
    FCALL(ftlbase, ShipManager_HasAugmentation, self, augId)

typedef float (*FUNCTYPE(ShipManager_GetAugmentationValue))(ShipManager* ship, basic_string* augId);
DECLFUNC(ShipManager_GetAugmentationValue);
#define ShipManager_GetAugmentationValue(self, augId) \
    FCALL(ftlbase, ShipManager_GetAugmentationValue, self, augId)

DECLSYM(ShipManager_ship_offset);
DECLSYM(ShipManager_current_target_offset);
DECLSYM(ShipManager_currentScrap_offset);
DECLSYM(ShipManager_myBlueprint_offset);
DECLSYM(ShipManager_myBlueprint_name_offset);
DECLSYM(ShipManager_myBlueprint_blueprintName_offset);
DECLSYM(ShipManager_bDestroyed_offset);

#define ShipManager_ship(ship_manager) (&MEMBER(ftlbase, ShipManager, ship_manager, Ship, ship))
#define ShipManager_current_target(ship_manager) \
    (&MEMBER(ftlbase, ShipManager, ship_manager, ShipManager, current_target))

#define ShipManager_currentScrap(ship_manager) \
    (MEMBER(ftlbase, ShipManager, ship_manager, int, currentScrap))

#define ShipManager_myBlueprint(ship_manager) \
    (&MEMBER(ftlbase, ShipManager, ship_manager, ShipBlueprint, myBlueprint))

#define ShipManager_bDestroyed(ship_manager) \
    (MEMBER(ftlbase, ShipManager, ship_manager, bool, bDestroyed))

typedef void (*FUNCTYPE(ShipManager_CheckCrystalAugment))(ShipManager* self, Pointf location);
DECLFUNC(ShipManager_CheckCrystalAugment);

typedef void (*FUNCTYPE(ShipManager_SunDamage))(ShipManager* self);
DECLFUNC(ShipManager_SunDamage);

DECLSYM(ShipManager_DamageArea);
DECLSYM(ShipManager_DamageSystem);
