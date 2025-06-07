#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct StarMap StarMap;
typedef struct Sector Sector;
typedef struct Location Location;

extern DisasmTrace StarMap_OnRender_sector_title_trace;
extern DisasmTrace StarMap_NewGame_trace;

// FTL functions & wrappers below

typedef void (*FUNCTYPE(StarMap_StartSecretSector))(StarMap* self);
DECLFUNC(StarMap_StartSecretSector);

typedef void (*FUNCTYPE(StarMap_RenderSectorName))(StarMap* self, Sector* sector, GL_Color color);
DECLFUNC(StarMap_RenderSectorName);

typedef Location* (*FUNCTYPE(StarMap_NewGame))(StarMap* self, bool tutorial);
DECLFUNC(StarMap_NewGame);

typedef void (*FUNCTYPE(StarMap_GenerateSectorMap))(StarMap* self);
DECLFUNC(StarMap_GenerateSectorMap);

typedef Location* (*FUNCTYPE(StarMap_GenerateMap))(StarMap* self, bool bTutorial, bool useOldSeed);
DECLFUNC(StarMap_GenerateMap);

DECLSYM(StarMap_bSecretSector_offset);
DECLSYM(StarMap_currentSector_offset);
DECLSYM(StarMap_secretSector_offset);
DECLSYM(StarMap_sectorMapSeed_offset);
DECLSYM(StarMap_currentSectorSeed_offset);
DECLSYM(StarMap_worldLevel_offset);

#define StarMap_worldLevel(starmap)    (*(&MEMBER(ftlbase, StarMap, starmap, int, worldLevel)))
#define Starmap_bSecretSector(starmap) (*(&MEMBER(ftlbase, StarMap, starmap, bool, bSecretSector)))
#define StarMap_currentSector(starmap) *(&MEMBER(ftlbase, StarMap, starmap, Sector*, currentSector))
#define StarMap_sectorMapSeed(starmap) *(&MEMBER(ftlbase, StarMap, starmap, int, sectorMapSeed))
#define StarMap_currentSectorSeed(starmap) \
    *(&MEMBER(ftlbase, StarMap, starmap, int, currentSectorSeed))

DECLSYM(Sector_description_type_offset);
#define Sector_description_type(sector) \
    (&MEMBER(ftlbase, Sector, sector, basic_string, description_type))

DECLSYM(Sector_description_shortName_offset);
#define Sector_description_shortName(sector) \
    (&MEMBER(ftlbase, Sector, sector, TextString, description_shortName))