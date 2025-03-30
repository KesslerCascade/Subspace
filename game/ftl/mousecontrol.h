#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct MouseControl MouseControl;

int subspace_MouseControl_OnRender_pre(MouseControl* self);

// FTL functions & wrappers below

typedef void (*FUNCTYPE(MouseControl_OnRender))(MouseControl* self);
DECLFUNC(MouseControl_OnRender);

typedef void (*FUNCTYPE(MouseControl_OnLoop))(MouseControl* self);
DECLFUNC(MouseControl_OnLoop);

extern DisasmTrace MouseControl_OnLoop_trace;