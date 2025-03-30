#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct MouseControl MouseControl;

int subspace_MouseControl_OnRender_pre(MouseControl* self);

// FTL functions & wrappers below

typedef void (*FUNCTYPE(MouseControl_OnRender))(MouseControl* self);
DECLFUNC(MouseControl_OnRender);
