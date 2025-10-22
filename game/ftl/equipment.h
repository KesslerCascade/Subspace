#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

extern DisasmTrace Equipment_AddWeapon_trace;

typedef void (*FUNCTYPE(Equipment_Jump))(Equipment* self);
DECLFUNC(Equipment_Jump);

typedef void (*FUNCTYPE(Equipment_AddWeapon))(Equipment* self, WeaponBlueprint* weaponBlueprint,
                                              bool free, bool forceCargo);
DECLFUNC(Equipment_AddWeapon);

DECLSYM(Equipment_overcapacityBox_offset);
DECLSYM(Equipment_overAugBox_offset);
DECLSYM(Equipment_bOverCapacity_offset);
DECLSYM(Equipment_bOverAugCapacity_offset);

#define Equipment_overcapacityBox(equip) \
    MEMBER(ftlbase, Equipment, equip, EquipmentBox*, overcapacityBox)
#define Equipment_overAugBox(equip)       MEMBER(ftlbase, Equipment, equip, AugmentEquipBox*, overAugBox)
#define Equipment_bOverCapacity(equip)    MEMBER(ftlbase, Equipment, equip, bool, bOverCapacity)
#define Equipment_bOverAugCapacity(equip) MEMBER(ftlbase, Equipment, equip, bool, bOverAugCapacity)

DECLSYM(Equipment_vEquipmentBoxes_offset);
#define Equipment_vEquipmentBoxes(equip) MEMBER(ftlbase, Equipment, equip, vector*, vEquipmentBoxes)

DECLSYM(EquipmentBox_item_offset);
#define EquipmentBox_item(box) MEMBER(ftlbase, EquipmentBox, box, EquipmentBoxItem*, item)

typedef bool (*FUNCTYPE(EquipmentBox_IsEmpty))(EquipmentBox* self);
DECLFUNC(EquipmentBox_IsEmpty);