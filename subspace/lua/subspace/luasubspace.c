#include <cx/platform/os.h>
#include <cx/sys/entry.h>
#include <lauxlib.h>
#include <lua.h>
#include "lua/luamodules.h"
#include "lua/luathread.h"
// #include "lua/types/luajson.h"
#include "subspace.h"
#include "version.h"

#include <iup.h>
#include <iuplua.h>

static int lua_args(lua_State* L)
{
    lua_newtable(L);
    for (int i = 0; i < saSize(cmdArgs); i++) {
        lua_pushstring(L, strC(cmdArgs.a[i]));
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int lua_program(lua_State* L)
{
    lua_pushstring(L, strC(cmdProgram));
    return 1;
}

static int lua_exit(lua_State* L)
{
    LuaThread* lt = lua_touserdata(L, lua_upvalueindex(1));
    lt->ss->exit  = true;
    eventSignal(&lt->ss->notify);
    return 0;
}

static int lua_panic(lua_State* L)
{
    string str = (string)lua_tostring(L, 1);
    strPrepend(_S"Fatal script error: ", &str);
    fatalError(str, false);

    // fatalError never returns, this should be unreachable
    strDestroy(&str);
    return 0;
}

static int lua_basedir(lua_State* L)
{
    LuaThread* lt = lua_touserdata(L, lua_upvalueindex(1));
    lua_pushstring(L, strC(lt->ss->basedir));
    return 1;
}

static int lua_basedirnative(lua_State* L)
{
    LuaThread* lt  = lua_touserdata(L, lua_upvalueindex(1));
    string bnative = 0;
    pathToPlatform(&bnative, lt->ss->basedir);
    lua_pushstring(L, strC(bnative));
    strDestroy(&bnative);
    return 1;
}

static int lua_sleep(lua_State* L)
{
    lua_Integer dur = luaL_checkinteger(L, 1);
    osSleep(dur);
    return 0;
}

static int lua_sversion(lua_State* L)
{
    lua_pushstring(L, subspace_version_str);
    return 1;
}

static luaL_Reg subspace_funcs[] = {
    { "args",          lua_args          },
    { "basedir",       lua_basedir       },
    { "basedirnative", lua_basedirnative },
    { "program",       lua_program       },
    { "exit",          lua_exit          },
    { "panic",         lua_panic         },
    { "sleep",         lua_sleep         },
    { NULL,            NULL              }
};

void luaRegisterSubspace(LuaThread* lt)
{
    lua_State* L = lt->L;
    luaRegisterLib(lt, "subspace", subspace_funcs);

    lua_getglobal(L, "subspace");

    // add settings to subspace table
    // lua_pushliteral(L, "settings");
    // luaPushJson(L, lt->ss->settings, false);
    // lua_settable(L, -3);

#ifdef _PLATFORM_WIN
    lua_pushliteral(L, "iswine");
    lua_pushboolean(L, osIsWine());
    lua_settable(L, -3);
#endif

    if (lt->ss->devmode) {
        lua_pushliteral(L, "dev");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }

    if (lt->ss->luadebug) {
        lua_pushliteral(L, "debug");
        lua_pushboolean(L, true);
        lua_settable(L, -3);
    }

    // pop subspace table
    lua_pop(L, 1);
}
