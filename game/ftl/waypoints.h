#pragma once
#include "hook/symbol.h"

// Commonly used nexus points for finding other functions and offsets

// the switch() statement for dispatching GetCommand outputs
DECLSYM(wp_CApp_OnLoop_GetCommand_switch);

// The part of OnLoop that handles dispatching the main menu options
DECLSYM(wp_CApp_OnLoop_MenuMenu_handler);