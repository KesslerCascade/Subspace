#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct EventSystem EventSystem;

// FTL functions & wrappers below

DECLSYM(EventSystem_EventManager);
#define EventManager ((EventSystem*)symAddr(ftlbase, EventSystem_EventManager))

typedef int (*FUNCTYPE(EventSystem_PollEvent))(EventSystem* self, int eventid);
DECLFUNC(EventSystem_PollEvent);
#define EventSystem_PollEvent(self) FCALL(ftlbase, EventSystem_PollEvent, self)
