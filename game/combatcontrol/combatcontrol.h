#pragma once
#include "ftl/ftl.h"
#include "ftl/functions_combatcontrol.h"

typedef struct CombatControl CombatControl;

int subspace_CombatControl_RenderTarget_pre(CombatControl* self);
void subspace_CombatControl_RenderTarget_post(CombatControl* self);
