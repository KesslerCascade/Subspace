#pragma once

#include <cx/fs.h>
#include <cx/taskqueue.h>
#include <cx/ssdtree.h>

#include "ui/uithread.h"

#define SETTINGS_FILENAME _S"/subspace.json"
#define DAT_FILENAME _S"/subspace.dat"
#define DB_FILENAME _S"/db/subspace.db"
#define DEVMODE_FILENAME _S"/is-dev-tree"
#define DATADIR_FILENAME _S"/data"
#define GAMEEXE_FILENAME _S"/subspacegame.exe"
#define GAMEEXE_DEV_FILENAME _S"/build/game-win32/game/Debug/subspacegame.exe"
#define LOG_FILENAME _S"/log/subspace.log"

#define LUA_MAIN_FILENAME _S"subspace.lua"

#define SSNS _S"subspace:/"
#define FTLNS _S"ftl:/"

extern VFS* filesys;

typedef struct Subspace {
    string basedir;
    VFS* fs;

    SSDNode* settings;
    Event notify; // notification event for the main thread

    TaskQueue* workq;
    UIThread ui;

    bool devmode;
    bool luadebug;
    bool exit;  // application is exiting
} Subspace;

bool subspaceCheckBaseDir(VFS* vfs, strref path);
bool subspaceFindBaseDir(string* out, VFS* vfs);
bool subspaceMount(Subspace* ss);
void subspaceUnmount(Subspace* ss);
bool logOpen(VFS* vfs, string filename, LogDest** defer);
bool logClose(void);
void fatalError(strref msg, bool osdeperr);