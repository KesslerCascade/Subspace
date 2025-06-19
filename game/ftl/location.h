#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct Location Location;
typedef struct LocationEvent LocationEvent;

// FTL functions & wrappers below

DECLSYM(Location_event_offset);
#define Location_event(loc) \
    (MEMBER(ftlbase, Location, loc, LocationEvent*, event))

DECLSYM(LocationEvent_eventName_offset);
#define LocationEvent_eventName(ev) \
    (&MEMBER(ftlbase, LocationEvent, ev, basic_string, eventName))
