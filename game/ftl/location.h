#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

// FTL functions & wrappers below

DECLSYM(Location_event_offset);
#define Location_event(loc) (MEMBER(ftlbase, Location, loc, LocationEvent*, event))

DECLSYM(Location_visited_offset);
#define Location_visited(loc) (MEMBER(ftlbase, Location, loc, int, visited))

DECLSYM(LocationEvent_eventName_offset);
#define LocationEvent_eventName(ev) (&MEMBER(ftlbase, LocationEvent, ev, basic_string, eventName))
