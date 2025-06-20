#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CompleteShip CompleteShip;
typedef struct ShipManager ShipManager;
typedef struct ShipBlueprint ShipBlueprint;

// FTL functions and wrappers below

extern DisasmTrace CompleteShip_OnInit_trace;

typedef void (*FUNCTYPE(CompleteShip_OnInit))(CompleteShip* ship, ShipBlueprint* bluePrint,
                                              int level);
DECLFUNC(CompleteShip_OnInit);

DECLSYM(CompleteShip_shipManager_offset);
#define CompleteShip_shipManager(ship) \
    *(&MEMBER(ftlbase, CompleteShip, ship, ShipManager*, shipManager))

typedef bool (*FUNCTYPE(CompleteShip_DeadCrew))(CompleteShip* self);
DECLFUNC(CompleteShip_DeadCrew);
#define CompleteShip_DeadCrew(ship) FCALL(ftlbase, CompleteShip_DeadCrew, ship)