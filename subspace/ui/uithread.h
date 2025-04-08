#pragma once

#include <cx/taskqueue.h>
#include "lua/luathread.h"

typedef struct Subspace Subspace;

typedef struct UIThread {
    TaskQueue* uiq;
    TaskQueue* uiworkers;
    LuaThread lua;
    Subspace* ss;
    bool started;
} UIThread;

bool uiLoadLua(UIThread* uithr, strref filename);
bool uiInit(UIThread* uithr, Subspace* ss);
bool uiShutdown(UIThread* uithr);
void uiStart(UIThread* uithr);
bool uiStop(UIThread* uithr);