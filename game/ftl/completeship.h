#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CompleteShip CompleteShip;
typedef struct ShipBlueprint ShipBlueprint;

// FTL functions and wrappers below

extern DisasmTrace CompleteShip_OnInit_trace;

typedef void (*FUNCTYPE(CompleteShip_OnInit))(CompleteShip* ship, ShipBlueprint* bluePrint,
                                              int level);
DECLFUNC(CompleteShip_OnInit);

DECLSYM(CompleteShip_shipManager_offset);
