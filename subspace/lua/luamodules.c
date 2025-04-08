#include "luamodules.h"
#include <lauxlib.h>
#include <lua.h>
#include "luathread.h"

void luaRegisterLib(LuaThread* lt, const char* name, luaL_Reg* funcs)
{
    // all libraries we register have access to the LuaThread through a lightweight userdata pointer
    // in their first upvalue
    lua_newtable(lt->L);
    lua_pushlightuserdata(lt->L, lt);
    luaL_setfuncs(lt->L, funcs, 1);
    lua_setglobal(lt->L, name);
    lua_pop(lt->L, 1);
}
