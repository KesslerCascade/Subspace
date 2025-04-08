#pragma once

#include <cx/taskqueue.h>
#include "lua/task/luatask.h"

typedef struct Subspace Subspace;
typedef struct lua_State lua_State;
typedef struct LuaThread {
    TaskQueue* uiq;
    TaskQueue* uiworkers;
    lua_State* L;
    Subspace* ss;
} LuaThread;

bool luaInit(LuaThread* lthr, Subspace* ss, TaskQueue* uiq, TaskQueue* uiworkers);
void luaShutdown(LuaThread* lthr);

bool _luaAddTask(LuaThread* lthr, LuaTask* task);
#define luaAddTask(lthr, task) _luaAddTask(lthr, LuaTask(task))
#define luaRun(lthr, task)            \
    _luaAddTask(lthr, LuaTask(task)); \
    objRelease(&task)