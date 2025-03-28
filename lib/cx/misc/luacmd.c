#include <cx/sys.h>
#include <cx/format.h>
#include <cx/fs.h>
#include <cx/string.h>
#include <cx/container.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <stdio.h>

VFS *filesys;

DEFINE_ENTRY_POINT

void _errmsg(string fmt, int n, stvar *args)
{
    string tmp = 0;
    _strFormat(&tmp, fmt, n, args);
    fputs(strC(tmp), stderr);
    fputs("\n", stderr);
    strDestroy(&tmp);
}
#define errmsg(fmt, ...) _errmsg(fmt, count_macro_args(__VA_ARGS__), (stvar[]){ __VA_ARGS__ })

static bool loadFile(string path, char **buf, size_t *sz)
{
    FSStat stat;
    bool ret = true;

    if (vfsStat(filesys, path, &stat) != FS_File) {
        errmsg(_S"Could not open ${string}", stvar(string, path));
        return false;
    }

    VFSFile *file = vfsOpen(filesys, path, FS_Read);
    if (!file) {
        errmsg(_S"Could not open ${string}", stvar(string, path));
        return false;
    }

    size_t didread;
    *sz = (size_t)stat.size;
    *buf = xaAlloc(*sz);
    if (!vfsRead(file, *buf, *sz, &didread)) {
        errmsg(_S"Failed to read from ${string}", stvar(string, path));
        ret = false;
    } else if (didread < *sz) {
        errmsg(_S"WARNING: Short read from ${string}. Script may not execute successfully.", stvar(string, path));
    }

    return ret;
}

int entryPoint()
{
    string path = 0;
    int ret = 0;

    filesys = vfsCreateFromFS();
    if (!filesys) {
        fputs("Error opening filesystem\n", stderr);
        return -1;
    }

    if (saSize(cmdArgs) < 1) {
        fputs("Usage: luacmd filename\n", stderr);
        return 1;
    }

    pathFromPlatform(&path, cmdArgs.a[0]);
    char *buf;
    size_t sz;

    if (loadFile(path, &buf, &sz)) {
        lua_State *L = luaL_newstate();

        // this program is intended for use as a build tool, so it has full access
        // to all lua libraries
        luaL_openlibs_unsafe(L);

        // set up the arg table
        lua_createtable(L, saSize(cmdArgs) - 1, 1);
        for (int i = 0; i < saSize(cmdArgs); i++) {
            lua_pushstring(L, strC(cmdArgs.a[i]));
            lua_rawseti(L, -2, i);
        }
        lua_setglobal(L, "arg");

        if (luaL_loadbuffer(L, buf, sz, strC(path)) != LUA_OK) {
            errmsg(_S"Parse error: ${string}",
                   stvar(string, (string)lua_tostring(L, -1)));
            ret = 2;
        } else if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            errmsg(_S"Execution error: ${string}",
                   stvar(string, (string)lua_tostring(L, -1)));
            ret = 3;
        }
        lua_close(L);
        xaFree(buf);
    } else
        ret = 1;

    strDestroy(&path);
    return ret;
}
