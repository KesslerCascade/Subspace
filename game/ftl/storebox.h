#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

extern DisasmTrace StoreBox_Purchase_trace;

// FTL functions & wrappers below

typedef void (*FUNCTYPE(StoreBox_Purchase))(StoreBox* self);
DECLFUNC(StoreBox_Purchase);

typedef void (*FUNCTYPE(DroneStoreBox_Purchase))(DroneStoreBox* self);
DECLFUNC(DroneStoreBox_Purchase);

typedef void (*FUNCTYPE(ItemStoreBox_Purchase))(ItemStoreBox* self);
DECLFUNC(ItemStoreBox_Purchase);

typedef void (*FUNCTYPE(RepairStoreBox_Purchase))(RepairStoreBox* self);
DECLFUNC(RepairStoreBox_Purchase);

typedef void (*FUNCTYPE(SystemStoreBox_Confirm))(SystemStoreBox* self, bool val);
DECLFUNC(SystemStoreBox_Confirm);

typedef void (*FUNCTYPE(SystemStoreBox_Purchase))(SystemStoreBox* self);
DECLFUNC(SystemStoreBox_Purchase);

typedef void (*FUNCTYPE(WeaponStoreBox_Purchase))(WeaponStoreBox* self);
DECLFUNC(WeaponStoreBox_Purchase);
