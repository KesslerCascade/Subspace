#include <cx/log.h>
#include <lauxlib.h>
#include <lua.h>
#include "lua/luamodules.h"
#include "lua/luathread.h"
#include "lua/task/luacall.h"
#include "lua/task/luaexec.h"
#include "subspace.h"

static int lua_loadfile(lua_State* L)
{
    LuaThread* lt        = lua_touserdata(L, lua_upvalueindex(1));
    const char* filename = lua_tostring(L, 1);

    if (!filename)
        return 0;

    if (!LuaExecFile_load(L, lt->ss->fs, (string)filename)) {
        logFmt(Error,
               _S"luaload: Failed to load lua file included by script: ${filename}",
               stvar(string, (string)filename));
        return 0;
    }

    // check if we were passed an environment block
    if (lua_istable(L, 2)) {
        lua_pushvalue(L, 2);
        lua_setupvalue(L, -2, 1);
    }

    return 1;
}

static int lua_execfile(lua_State* L)
{
    if (lua_loadfile(L) == 0)
        return 0;

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        logFmt(Error,
               _S"luaexec: Execution error: ${string}",
               stvar(string, (string)lua_tostring(L, -1)));
        lua_pop(L, 1);   // remove error
        return false;
    }

    return 0;
}

static int lua_loadstr(lua_State* L)
{
    const char* scriptdata = lua_tostring(L, 1);

    if (!scriptdata)
        return 0;

    if (!LuaExecString_load(L, (string)scriptdata)) {
        logStr(Error, _S"luaload: Failed to load lua string included by script");
        return 0;
    }

    // check if we were passed an environment block
    if (lua_istable(L, 2)) {
        lua_pushvalue(L, 2);
        lua_setupvalue(L, -2, 1);
    }

    return 1;
}

static int lua_execstr(lua_State* L)
{
    if (lua_loadstr(L) == 0)
        return 0;

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        logFmt(Error,
               _S"luaexec: Execution error: ${string}",
               stvar(string, (string)lua_tostring(L, -1)));
        lua_pop(L, 1);   // remove error
    }

    return 0;
}

static int lua_runlater(lua_State* L)
{
    LuaThread* lt = lua_touserdata(L, lua_upvalueindex(1));

    if (lua_isfunction(L, 1)) {
        lua_pushvalue(L, 1);
        LuaFuncRef* func          = luafuncrefCreate(lt);
        LuaCallbackDispatch* task = luacallbackdispatchCreate(lt, func, 0, 0, NULL);
        tqRun(lt->uiq, &task);
        objRelease(&func);
    }

    return 0;
}

static luaL_Reg lua_funcs[] = {
    { "loadfile", lua_loadfile },
    { "execfile", lua_execfile },
    { "loadstr",  lua_loadstr  },
    { "execstr",  lua_execstr  },
    { "runlater", lua_runlater },
    { NULL,       NULL         }
};

void luaRegisterLua(LuaThread* lt)
{
    luaRegisterLib(lt, "lua", lua_funcs);
}
