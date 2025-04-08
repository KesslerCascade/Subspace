#include <lauxlib.h>
#include <lua.h>
#include "lua/luamodules.h"
#include "lua/luathread.h"

#include <cx/log.h>

static int lua_log(lua_State* L)
{
    int level       = luaL_checkinteger(L, 1);
    const char* str = lua_tostring(L, 2);
    if (str && cstrLen(str) > 0) {
        _logFmt(level, NULL, _S"${string}", 1, (stvar[]) { stvar(string, (string)str) });
    }
    return 0;
}

static luaL_Reg log_funcs[] = {
    { "log", lua_log },
    { NULL,  NULL    }
};

void luaRegisterLog(LuaThread* lt)
{
    lua_State* L = lt->L;
    luaRegisterLib(lt, "log", log_funcs);

    // add log level constants
    lua_getglobal(L, "log");

    lua_pushliteral(L, "debug");
    lua_pushinteger(L, LOG_Debug);
    lua_settable(L, -3);

    lua_pushliteral(L, "diag");
    lua_pushinteger(L, LOG_Diag);
    lua_settable(L, -3);

    lua_pushliteral(L, "verbose");
    lua_pushinteger(L, LOG_Verbose);
    lua_settable(L, -3);

    lua_pushliteral(L, "info");
    lua_pushinteger(L, LOG_Info);
    lua_settable(L, -3);

    lua_pushliteral(L, "warn");
    lua_pushinteger(L, LOG_Warn);
    lua_settable(L, -3);

    lua_pushliteral(L, "error");
    lua_pushinteger(L, LOG_Error);
    lua_settable(L, -3);

    lua_pushliteral(L, "fatal");
    lua_pushinteger(L, LOG_Fatal);
    lua_settable(L, -3);

    // pop log table
    lua_pop(L, 1);
}
