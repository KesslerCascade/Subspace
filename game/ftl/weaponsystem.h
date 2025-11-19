#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

extern DisasmTrace WeaponSystem_PowerWeapon_trace;
extern DisasmTrace WeaponSystem_OnLoop_trace;

typedef void (*FUNCTYPE(WeaponSystem_OnLoop))(WeaponSystem* self);
DECLFUNC(WeaponSystem_OnLoop);

typedef bool (*FUNCTYPE(WeaponSystem_PowerWeapon))(WeaponSystem* self, ProjectileFactory* weapon,
                                                   bool userDriven, bool force);
DECLFUNC(WeaponSystem_PowerWeapon);

DECLSYM(WeaponSystem_weapons_offset);
// vector<ProjectileFactory*>
#define WeaponSystem_weapons(ws) (&MEMBER(ftlbase, WeaponSystem, ws, vector, weapons))