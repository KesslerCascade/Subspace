// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "luaexec.h"
// clang-format on
// ==================== Auto-generated section ends ======================

#include <cx/log.h>

#include <lauxlib.h>
#include <lua.h>

#include "subspace.h"

static bool _luaExec_compile(lua_State* L, strref scriptdata, strref srcname)
{
    bool ret = false;

    if (strEmpty(scriptdata))
        goto out;

    if (luaL_loadbuffer(L, strC(scriptdata), strLen(scriptdata), strC(srcname)) != LUA_OK) {
        logFmt(Error,
               _S"luaexec: Parse error: ${string}",
               stvar(string, (string)lua_tostring(L, -1)));
        goto out;
    }

    ret = true;

out:
    return ret;
}

static bool _luaExec_exec(lua_State* L)
{
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        logFmt(Error,
               _S"luaexec: Execution error: ${string}",
               stvar(string, (string)lua_tostring(L, -1)));
        lua_pop(L, 1);   // remove error
        return false;
    }

    return true;
}

_objfactory_guaranteed LuaExecString*
LuaExecString_create(LuaThread* lt, _In_opt_ strref scriptdata)
{
    LuaExecString* self;
    self = objInstCreate(LuaExecString);

    self->lt = lt;
    strDup(&self->scriptdata, scriptdata);

    self->name = _S"LuaExecString";
    objInstInit(self);
    return self;
}

bool LuaExecString_load(lua_State* L, _In_opt_ strref scriptdata)
{
    return _luaExec_compile(L, scriptdata, _S"LuaExecString");
}

bool LuaExecString_exec(lua_State* L, _In_opt_ strref scriptdata)
{
    if (!_luaExec_compile(L, scriptdata, _S"LuaExecString"))
        return false;
    return _luaExec_exec(L);
}

uint32 LuaExecString_run(_In_ LuaExecString* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                         _Inout_ TaskControl* tcon)
{
    if (!self->lt->L)
        return TASK_Result_Failure;

    return LuaExecString_exec(self->lt->L, self->scriptdata) ?
        TASK_Result_Success :
        TASK_Result_Failure;
}

void LuaExecString_destroy(_In_ LuaExecString* self)
{
    // Autogen begins -----
    strDestroy(&self->scriptdata);
    // Autogen ends -------
}

_objfactory_guaranteed LuaExecFile* LuaExecFile_create(LuaThread* lt, _In_opt_ strref filename)
{
    LuaExecFile* self;
    self = objInstCreate(LuaExecFile);

    self->lt = lt;
    strDup(&self->filename, filename);

    self->name = _S"LuaExecFile";
    objInstInit(self);
    return self;
}

bool LuaExecFile_load(lua_State* L, VFS* vfs, _In_opt_ strref filename)
{
    bool ret        = false;
    string contents = 0;
    VFSFile* file   = 0;
    FSStat stat;

    // make sure file exists and isn't insanely huge
    if (vfsStat(vfs, filename, &stat) != FS_File || stat.size > 256ULL * 1024 * 1024) {
        logFmt(Error, _S"luaexecfile: ${string} not found", stvar(strref, filename));
        goto out;
    }

    file = vfsOpen(vfs, filename, FS_Read);
    if (!file) {
        logFmt(Error, _S"luaexecfile: Error opening ${string}", stvar(strref, filename));
        goto out;
    }

    // lua files are small enough to just suck into memory for processing
    char* buf = strBuffer(&contents, (uint32)stat.size);
    size_t didread;
    if (!vfsRead(file, buf, (size_t)stat.size, &didread) || stat.size != didread)
        goto out;

    ret = _luaExec_compile(L, contents, filename);

out:
    strDestroy(&contents);
    if (file)
        vfsClose(file);
    return ret;

}

bool LuaExecFile_exec(lua_State* L, VFS* vfs, _In_opt_ strref filename)
{
    if (!LuaExecFile_load(L, vfs, filename))
        return false;
    return _luaExec_exec(L);
}

uint32 LuaExecFile_run(_In_ LuaExecFile* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                       _Inout_ TaskControl* tcon)
{
    if (!self->lt->L)
        return TASK_Result_Failure;

    return LuaExecFile_exec(self->lt->L, self->lt->ss->fs, self->filename) ?
        TASK_Result_Success :
        TASK_Result_Failure;
}

void LuaExecFile_destroy(_In_ LuaExecFile* self)
{
    // Autogen begins -----
    strDestroy(&self->filename);
    // Autogen ends -------
}

// Autogen begins -----
#include "luaexec.auto.inc"
// Autogen ends -------
