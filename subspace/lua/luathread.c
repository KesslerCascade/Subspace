#include "luathread.h"
#include <cx/log.h>
#include <cx/string.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include "subspace.h"
#include "luamodules.h"

static int luaPanic(lua_State* L)
{
    logStr(Fatal, _S"An unexpected script error occured. Unable to continue.");
    return 0;
}

bool luaInit(LuaThread* lthr, Subspace* ss, TaskQueue* uiq, TaskQueue* uiworkers)
{
    devAssert(!lthr->L);
    lthr->ss       = ss;
    lthr->uiq = objAcquire(uiq);
    lthr->uiworkers  = objAcquire(uiworkers);

    lthr->L = luaL_newstate();
    lua_atpanic(lthr->L, luaPanic);

    // add some of the standard Lua libraries
    luaL_requiref(lthr->L, "_G", luaopen_base, 1);
    luaL_requiref(lthr->L, LUA_MATHLIBNAME, luaopen_math, 1);
    luaL_requiref(lthr->L, LUA_OSLIBNAME, luaopen_os_safe, 1);
    luaL_requiref(lthr->L, LUA_STRLIBNAME, luaopen_string, 1);
    luaL_requiref(lthr->L, LUA_TABLIBNAME, luaopen_table, 1);
    luaL_requiref(lthr->L, LUA_UTF8LIBNAME, luaopen_utf8, 1);

    if (ss->luadebug) {
        // debug module needs io, coroutines, and debug
        luaL_requiref(lthr->L, LUA_IOLIBNAME, luaopen_io_unsafe, 1);
        luaL_requiref(lthr->L, LUA_COLIBNAME, luaopen_coroutine, 1);
        luaL_requiref(lthr->L, LUA_DBLIBNAME, luaopen_debug_unsafe, 1);

        lua_newtable(lthr->L);
        lua_newtable(lthr->L);
        lua_setfield(lthr->L, -2, "loaded");
        lua_setglobal(lthr->L, "package");

        // vscode extension passes the path to lldebugger.lua in LUA_PATH
        string dbgpath = 0;
        char* buf      = strBuffer(&dbgpath, 1024);
        size_t ret;
        getenv_s(&ret, buf, 1024, "LUA_PATH");
        strSetLen(&dbgpath, (uint32)ret);

        // chop off leading semicolons
        int32 idx = strFindR(dbgpath, strEnd, _S";");
        if (idx)
            strSubStrI(&dbgpath, idx + 1, strEnd);
        pathParent(&dbgpath, dbgpath);
        pathJoin(&dbgpath, dbgpath, _S"lldebugger.lua");
        pathNormalize(&dbgpath);

        logFmt(Verbose,
               _S"luathread: Attempting to load lua debugger from ${string}",
               stvar(string, dbgpath));
        if (LuaExecFile_load(lthr->L, filesys, dbgpath, false) &&
            lua_pcall(lthr->L, 0, 1, 0) == LUA_OK &&
            (lua_setglobal(lthr->L, "lldebugger"),
             LuaExecString_exec(lthr->L, _S"lldebugger.start()", false))) {
            logStr(Info, _S"luathread: Successfully loaded lua debugger");
        } else {
            logStr(Error, _S"luathread: Failed to load lua debugger");
        }
        strDestroy(&dbgpath);
    }

    // register various library modules
    luaRegisterLog(lthr);
    luaRegisterLua(lthr);
    luaRegisterSubspace(lthr);

    return true;
}

void luaShutdown(LuaThread* lthr)
{
    lua_close(lthr->L);
    lthr->L = NULL;
    objRelease(&lthr->uiq);
    objRelease(&lthr->uiworkers);
}

bool _luaAddTask(LuaThread* lthr, LuaTask* task)
{
    if (lthr->uiq)
        return tqAdd(lthr->uiq, (BasicTask*)task);
    return false;
}