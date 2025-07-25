#pragma once
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/ship.h"
#include "ftl/shipmanager.h"
#include "ftl/shipsystem.h"
#include "ftl/worldmanager.h"
#include "feature.h"
#include "subspacegame.h"

typedef struct DamageSource {
    const char* prev;
    bool set;
} DamageSource;

// most should be static; special handling needed for this one
extern DamageSource systemExplodedSrc;

void damageSourceSet(DamageSource* ds, const char* src);
void damageSourceFinish(DamageSource* ds);
void runTrackerHandleUpdate(bool r);
bool runTrackerRenderWarning(void);
