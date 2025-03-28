#pragma once

#include <cx/closure/closure.h>

// commonly used callback functions that do useful things, to be used as closures

// Signals the event passed as ptr in the first cvar when called
bool ccbSignalEvent(stvlist* cvars, stvlist* args);

// Signals and releases the shared event passed as ptr in the first cvar when called
bool ccbSignalSharedEvent(stvlist* cvars, stvlist* args);

// Given a weak reference to a complex task, advances it
bool ccbAdvanceTask(stvlist* cvars, stvlist* args);
