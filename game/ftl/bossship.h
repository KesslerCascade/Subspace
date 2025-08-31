#pragma once

#include "ftl/ftl.h"
#include "ftl/completeship.h"
#include "hook/function.h"

typedef struct BossShip BossShip;

// FTL functions and wrappers below

typedef void (*FUNCTYPE(BossShip_StartStage))(BossShip* self);
DECLFUNC(BossShip_StartStage);

DECLSYM(BossShip_currentStage_offset);
#define BossShip_currentStage(bship) MEMBER(ftlbase, BossShip, bship, int, currentStage)
