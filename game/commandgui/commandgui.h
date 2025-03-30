#pragma once

#include "ftl/ftl.h"
#include "ftl/functions_commandgui.h"
#include "ftl/struct.h"

typedef struct CommandGui CommandGui;

int subspace_CommandGui_KeyDown_pre(CommandGui* self, int key, bool shiftHeld);
int subspace_CommandGui_OnLoop_pre(CommandGui* self);
