#pragma once

#include <cx/fs.h>
#include <cx/log.h>
#include <cx/ssdtree.h>
#include <cx/taskqueue.h>

#define SETTINGS_FILENAME    _S "/subspace.json"
#define DAT_FILENAME         _S "/subspace.dat"
#define DB_FILENAME          _S "/db/subspace.db"
#define DEVMODE_FILENAME     _S "/is-dev-tree"
#define DATADIR_FILENAME     _S "/data"
#define GAMEEXE_FILENAME     _S "/subspacegame.exe"
#define GAMEEXE_DEV_FILENAME _S "/build/game-win32/game/Debug/subspacegame.exe"
#define LOG_FILENAME         _S "/log/subspace.log"
#define LANGDIR_FILENAME     _S "subspace:/lang"
#define LANGLIST_FILENAME    _S "subspace:/lang/languages.json"

#define LUA_MAIN_FILENAME _S "subspace.lua"

#define SSNS  _S "subspace:/"
#define FTLNS _S "ftl:/"

extern VFS* filesys;
extern LogCategory* gamecat;

typedef struct ControlServer ControlServer;
typedef struct GameMgr GameMgr;
typedef struct FeatureRegistry FeatureRegistry;
typedef struct SubspaceUI SubspaceUI;
typedef struct LanguageDB LanguageDB;

typedef struct Subspace {
    string basedir;
    VFS* fs;
    string gamepath;

    SSDNode* settings;
    Event notify;   // notification event for the main thread

    TaskQueue* workq;
    ControlServer* svr;
    SubspaceUI* ui;
    GameMgr* gmgr;
    FeatureRegistry* freg;
    LanguageDB* lang;

    // global settings
    uint32_t listenaddr;
    int port;
    bool devmode;
    bool exit;   // application is exiting
} Subspace;

bool subspaceCheckBaseDir(VFS* vfs, strref path);
bool subspaceFindBaseDir(Subspace* ss, VFS* vfs);
void subspaceSetBaseDir(Subspace* ss, VFS* vfs, strref path);
bool subspaceMount(Subspace* ss);
void subspaceUnmount(Subspace* ss);
bool logOpen(VFS* vfs, string filename, LogDest** defer);
bool logClose(void);
void fatalError(strref msg, bool osdeperr);
