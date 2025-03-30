#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct CFPS CFPS;

void subspace_CFPS_OnLoop_post(CFPS* self);

// FTL functions & wrappers below

DECLSYM(CFPS_FPSControl);
#define FPSControl ((CFPS*)symAddr(ftlbase, CFPS_FPSControl))

typedef double (*FUNCTYPE(CFPS_GetTime))(CFPS* self);
DECLFUNC(CFPS_GetTime);
#define CFPS_GetTime(self) FCALL(ftlbase, CFPS_GetTime, self)

typedef void (*FUNCTYPE(CFPS_OnLoop))(CFPS* self);
DECLFUNC(CFPS_OnLoop);

typedef float (*FUNCTYPE(CFPS_GetSpeedFactor))(CFPS* self);
DECLFUNC(CFPS_GetSpeedFactor);

DECLSYM(CFPS_SpeedFactor_offset);
DECLSYM(CFPS_fps_offset);

#define CFPS_SpeedFactor(cfps) MEMBER(ftlbase, CFPS, cfps, float, SpeedFactor)
#define CFPS_fps(cfps)         MEMBER(ftlbase, CFPS, cfps, int, fps)
