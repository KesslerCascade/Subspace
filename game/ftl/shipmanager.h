#pragma once
#include "ftl/completeship.h"
#include "ftl/ftl.h"
#include "ftl/ship.h"
#include "ftl/struct.h"
#include "hook/function.h"

// FTL functions & wrappers below

extern DisasmTrace ShipManager_OnLoop_trace;
extern DisasmTrace ShipManager_DamageHull_trace;
extern DisasmTrace ShipManager_SunDamage_trace;
extern DisasmTrace ShipManager_PowerWeapon_trace;
extern DisasmTrace ShipManager_GetWeaponTotal_trace;
extern DisasmTrace ShipManager_AddDrone_trace;
extern DisasmTrace ShipManager_GetDroneTotal_trace;
extern DisasmTrace ShipManager_ModifyDroneCount_trace;
extern DisasmTrace ShipManager_PowerDrone_trace;
extern DisasmTrace ShipManager_DePowerDrone_trace;

typedef bool (*FUNCTYPE(ShipManager_OnInit))(ShipManager* ship, ShipBlueprint* bluePrint,
                                             int level);
DECLFUNC(ShipManager_OnInit);

typedef void (*FUNCTYPE(ShipManager_OnLoop))(ShipManager* self);
DECLFUNC(ShipManager_OnLoop);

typedef int (*FUNCTYPE(ShipManager_DamageHull))(ShipManager* ship, int dmg, bool force);
DECLFUNC(ShipManager_DamageHull);
#define ShipManager_DamageHull(self, dmg, force) \
    FCALL(ftlbase, ShipManager_DamageHull, self, dmg, force)

typedef bool (*FUNCTYPE(ShipManager_GetIsJumping))(ShipManager* ship);
DECLFUNC(ShipManager_GetIsJumping);
#define ShipManager_GetIsJumping(self) FCALL(ftlbase, ShipManager_GetIsJumping, self)

typedef void (*FUNCTYPE(ShipManager_Wait))(ShipManager* ship);
DECLFUNC(ShipManager_Wait);

typedef void (*FUNCTYPE(ShipManager_JumpLeave))(ShipManager* ship);
DECLFUNC(ShipManager_JumpLeave);

typedef void (*FUNCTYPE(ShipManager_JumpArrive))(ShipManager* ship);
DECLFUNC(ShipManager_JumpArrive);

typedef int (*FUNCTYPE(ShipManager_GetDroneCount))(ShipManager* ship);
DECLFUNC(ShipManager_GetDroneCount);
#define ShipManager_GetDroneCount(self) FCALL(ftlbase, ShipManager_GetDroneCount, self)

typedef void (*FUNCTYPE(ShipManager_ModifyDroneCount))(ShipManager* ship, int amount);
DECLFUNC(ShipManager_ModifyDroneCount);
#define ShipManager_ModifyDroneCount(self, amount) \
    FCALL(ftlbase, ShipManager_ModifyDroneCount, self, amount)

typedef int (*FUNCTYPE(ShipManager_GetMissileCount))(ShipManager* ship);
DECLFUNC(ShipManager_GetMissileCount);
#define ShipManager_GetMissileCount(self) FCALL(ftlbase, ShipManager_GetMissileCount, self)

typedef void (*FUNCTYPE(ShipManager_ModifyScrapCount))(ShipManager* ship, int amount, bool income);
DECLFUNC(ShipManager_ModifyScrapCount);
#define ShipManager_ModifyScrapCount(self, amount, income) \
    FCALL(ftlbase, ShipManager_GetMissileCount, self, amount, income)

typedef bool (*FUNCTYPE(ShipManager_HasSystem))(ShipManager* ship, int systemId);
DECLFUNC(ShipManager_HasSystem);
#define ShipManager_HasSystem(self, systemId) FCALL(ftlbase, ShipManager_HasSystem, self, systemId)

DECLSYM(ShipManager_ship_offset);
DECLSYM(ShipManager_current_target_offset);
DECLSYM(ShipManager_currentScrap_offset);
DECLSYM(ShipManager_myBlueprint_offset);
DECLSYM(ShipManager_myBlueprint_name_offset);
DECLSYM(ShipManager_myBlueprint_blueprintName_offset);
DECLSYM(ShipManager_bDestroyed_offset);
DECLSYM(ShipManager_fuel_count_offset);

#define ShipManager_ship(ship_manager) (&MEMBER(ftlbase, ShipManager, ship_manager, Ship, ship))
#define ShipManager_current_target(ship_manager) \
    (&MEMBER(ftlbase, ShipManager, ship_manager, ShipManager, current_target))

#define ShipManager_currentScrap(ship_manager) \
    (MEMBER(ftlbase, ShipManager, ship_manager, int, currentScrap))

#define ShipManager_myBlueprint(ship_manager) \
    (&MEMBER(ftlbase, ShipManager, ship_manager, ShipBlueprint, myBlueprint))

#define ShipManager_bDestroyed(ship_manager) \
    (MEMBER(ftlbase, ShipManager, ship_manager, bool, bDestroyed))

#define ShipManager_fuel_count(ship_manager) \
    (MEMBER(ftlbase, ShipManager, ship_manager, int, fuel_count))

typedef void (*FUNCTYPE(ShipManager_CheckCrystalAugment))(ShipManager* self, Pointf location);
DECLFUNC(ShipManager_CheckCrystalAugment);

typedef void (*FUNCTYPE(ShipManager_SunDamage))(ShipManager* self);
DECLFUNC(ShipManager_SunDamage);

typedef bool (*FUNCTYPE(ShipManager_DamageArea))(ShipManager* self, Pointf location, Damage damage,
                                                 bool forceHit);
DECLFUNC(ShipManager_DamageArea);

typedef bool (*FUNCTYPE(ShipManager_DamageBeam))(ShipManager* self, Pointf current, Pointf last,
                                                 Damage damage);
DECLFUNC(ShipManager_DamageBeam);

DECLSYM(ShipManager_DamageSystem);

typedef int (*FUNCTYPE(ShipManager_GetWeaponTotal))(ShipManager* self);
DECLFUNC(ShipManager_GetWeaponTotal);
#define ShipManager_GetWeaponTotal(self) FCALL(ftlbase, ShipManager_GetWeaponTotal, self)

typedef int (*FUNCTYPE(ShipManager_AddWeapon))(ShipManager* self, WeaponBlueprint* weapon,
                                               int slot);
DECLFUNC(ShipManager_AddWeapon);
#define ShipManager_AddWeapon(self, weapon, slot) \
    FCALL(ftlbase, ShipManager_AddWeapon, self, weapon, slot)

typedef bool (*FUNCTYPE(ShipManager_DePowerWeapon))(ShipManager* self, ProjectileFactory* weapon,
                                                    bool userDriven);
DECLFUNC(ShipManager_DePowerWeapon);

typedef bool (*FUNCTYPE(ShipManager_PowerWeapon))(ShipManager* self, ProjectileFactory* weapon,
                                                  bool userDriven, bool force);
DECLFUNC(ShipManager_PowerWeapon);

DECLSYM(ShipManager_weaponSystem_offset);
#define ShipManager_weaponSystem(self) \
    (MEMBER(ftlbase, ShipManager, self, WeaponSystem*, weaponSystem))

typedef int (*FUNCTYPE(ShipManager_GetDroneTotal))(ShipManager* self);
DECLFUNC(ShipManager_GetDroneTotal);
#define ShipManager_GetDroneTotal(self) FCALL(ftlbase, ShipManager_GetDroneTotal, self)

typedef Drone* (*FUNCTYPE(ShipManager_AddDrone))(ShipManager* self, DroneBlueprint* drone,
                                                 int slot);
DECLFUNC(ShipManager_AddDrone);
#define ShipManager_AddDrone(self, drone, slot) \
    FCALL(ftlbase, ShipManager_AddDrone, self, drone, slot)

DECLSYM(ShipManager_droneSystem_offset);
#define ShipManager_droneSystem(self) \
    (MEMBER(ftlbase, ShipManager, self, DroneSystem*, droneSystem))

typedef void (*FUNCTYPE(ShipManager_AddItem))(ShipManager* self, ItemBlueprint* item);
DECLFUNC(ShipManager_AddItem);

typedef bool (*FUNCTYPE(ShipManager_DePowerDrone))(ShipManager* self, Drone* drone,
                                                   bool userDriven);
DECLFUNC(ShipManager_DePowerDrone);

typedef bool (*FUNCTYPE(ShipManager_PowerDrone))(ShipManager* self, Drone* drone, int roomId,
                                                 bool userDriven, bool force);
DECLFUNC(ShipManager_PowerDrone);

DECLSYM(ShipManager_droneSystem_offset);
#define ShipManager_droneSystem(self) \
    (MEMBER(ftlbase, ShipManager, self, DroneSystem*, droneSystem))

DECLSYM(ShipManager_hackingSystem_offset);
#define ShipManager_hackingSystem(self) \
    (MEMBER(ftlbase, ShipManager, self, HackingSystem*, hackingSystem))
