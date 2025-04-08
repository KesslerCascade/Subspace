#pragma once

typedef struct LuaThread LuaThread;
typedef struct luaL_Reg luaL_Reg;

void luaRegisterLib(LuaThread* lt, const char* name, luaL_Reg* funcs);

void luaRegisterLog(LuaThread* lt);
void luaRegisterLua(LuaThread* lt);
void luaRegisterSubspace(LuaThread* lt);
