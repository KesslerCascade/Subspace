#pragma once
#include "subspacegame.h"
#include "ftl/capp.h"
#include "ftl/commandgui.h"

// keystrokes received from anywhere in the application
bool keyDownGlobal(CApp *app, int key);

// keystrokes that are received only in the main game interface
bool keyDownInGame(CommandGui *gui, int key, bool shiftHeld, bool isPaused, bool isJumping);
