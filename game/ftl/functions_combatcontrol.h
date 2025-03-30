#pragma once
#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CombatControl CombatControl;
typedef struct ShipManager ShipManager;

typedef void (*FUNCTYPE(CombatControl_OnRenderCombat))(CombatControl* self);
DECLFUNC(CombatControl_OnRenderCombat);

typedef void (*FUNCTYPE(CombatControl_RenderTarget))(CombatControl* self);
DECLFUNC(CombatControl_RenderTarget);

typedef ShipManager* (*FUNCTYPE(CombatControl_GetCurrentTarget))(CombatControl *self);
DECLFUNC(CombatControl_GetCurrentTarget);
#define CombatControl_GetCurrentTarget(self) FCALL(ftlbase, CombatControl_GetCurrentTarget, self)