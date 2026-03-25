#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef void (*FUNCTYPE(WeaponStoreBox_Purchase))(WeaponStoreBox* self);
DECLFUNC(WeaponStoreBox_Purchase);