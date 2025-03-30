#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct CFPS CFPS;

void subspace_CFPS_OnLoop_post(CFPS* self);

// FTL functions & wrappers below

DECLSYM(CFPS_FPSControl);

typedef double (*FUNCTYPE(CFPS_GetTime))(CFPS* self);
DECLFUNC(CFPS_GetTime);

typedef void (*FUNCTYPE(CFPS_OnLoop))(CFPS* self);
DECLFUNC(CFPS_GetTime);

typedef float (*FUNCTYPE(CFPS_GetSpeedFactor))(CFPS* self);
DECLFUNC(CFPS_GetSpeedFactor);

DECLSYM(CFPS_SpeedFactor_offset);
