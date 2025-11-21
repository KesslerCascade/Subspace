#pragma once
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/ship.h"
#include "ftl/shipmanager.h"
#include "ftl/shipsystem.h"
#include "ftl/worldmanager.h"
#include "feature.h"
#include "subspacegame.h"

typedef struct EventSource {
    string prev;
    bool set;
} EventSource;

// most should be static; special handling needed for these two
extern EventSource systemExplodedSrc;
extern EventSource hackingDroneSrc;

void _eventSourceSet(EventSource* es, string* cur, strref src);
void _eventSourceFinish(EventSource* es, string* cur);
#define eventSourceSet(type, es, src) _eventSourceSet(es, &gc.cur##type##Source, src)
#define eventSourceFinish(type, es)   _eventSourceFinish(es, &gc.cur##type##Source)

void runTrackerHandleUpdate(bool r);
bool runTrackerRenderWarning(void);
