#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CEvent CEvent;
typedef struct InputEvent InputEvent;

extern DisasmTrace CEvent_OnEvent_InputEvent_trace;

typedef int (*FUNCTYPE(CEvent_OnEvent_InputEvent))(CEvent* self, InputEvent* event);
DECLFUNC(CEvent_OnEvent_InputEvent);

DECLSYM(CEvent_callback);   // small callback function for input_set_event_callback
