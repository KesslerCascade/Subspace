#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct StarMap StarMap;
typedef struct Sector Sector;

extern DisasmTrace StarMap_OnRender_sector_title_trace;

// FTL functions & wrappers below

typedef void (*FUNCTYPE(StarMap_StartSecretSector))(StarMap* self);
DECLFUNC(StarMap_StartSecretSector);

DECLSYM(StarMap_bSecretSector_offset);
DECLSYM(StarMap_currentSector_offset);
DECLSYM(StarMap_secretSector_offset);

#define StarMap_currentSector(starmap) *(&MEMBER(ftlbase, StarMap, starmap, Sector*, currentSector))

DECLSYM(Sector_description_type_offset);
#define Sector_description_type(sector) \
    (&MEMBER(ftlbase, Sector, sector, basic_string, description_type))
