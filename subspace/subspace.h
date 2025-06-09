#pragma once

#include <cx/fs.h>
#include <cx/log.h>
#include <cx/ssdtree.h>
#include <cx/taskqueue.h>

#define SETTINGS_FILENAME    _S "/subspace.json"
#define DAT_FILENAME         _S "/subspace.dat"
#define DBDIR_FILENAME       _S "/db"
#define DB_FILENAME          _S "subspace.db"
#define DEVMODE_FILENAME     _S "/is-dev-tree"
#define DATADIR_FILENAME     _S "/data"
#define GAMEEXE_FILENAME     _S "/subspacegame.exe"
#define GAMEEXE_DEV_FILENAME _S "/build/game-win32/Debug/subspacegame.exe"
#define LOG_FILENAME         _S "/log/subspace.log"
#define LANGDIR_FILENAME     _S "subspace:/lang"
#define LANGLIST_FILENAME    _S "subspace:/lang/languages.json"
#define SAVEDIR_FILENAME     _S "/saves"

#define LUA_MAIN_FILENAME _S "subspace.lua"

#define SSNS  _S "subspace:/"
#define FTLNS _S "ftl:/"

extern VFS* filesys;
extern LogCategory* gamecat;

typedef struct ControlServer ControlServer;
typedef struct GameMgr GameMgr;
typedef struct KBMgr KBMgr;
typedef struct FeatureRegistry FeatureRegistry;
typedef struct SubspaceUI SubspaceUI;
typedef struct LanguageDB LanguageDB;
typedef struct GameInst GameInst;
typedef struct RunInfo RunInfo;
typedef struct Database Database;

typedef struct Subspace {
    string basedir;
    VFS* fs;
    string gamepath;

    SSDNode* settings;
    Event notify;   // notification event for the main thread

    RWLock lock;    // for volatile data (game, run)
    GameInst* game;   // active game instance
    RunInfo* run;     // run that is focused in the UI

    TaskQueue* workq;
    ControlServer* svr;
    SubspaceUI* ui;
    GameMgr* gmgr;
    KBMgr* kbmgr;
    FeatureRegistry* freg;
    LanguageDB* lang;
    Database* db;
    string langid;

    // global settings
    uint32_t listenaddr;
    int port;
    bool devmode;
    bool exit;   // application is exiting
    bool reloadui;
} Subspace;

bool subspaceCheckBaseDir(VFS* vfs, strref path);
bool subspaceFindBaseDir(Subspace* ss, VFS* vfs);
void subspaceSetBaseDir(Subspace* ss, VFS* vfs, strref path);
bool subspaceMount(Subspace* ss);
void subspaceUnmount(Subspace* ss);
void subspaceUpdateUI(Subspace* ss);       // use sparingly, updates entire UI
void spointFormat(string* out, int64 spoint);
GameInst* subspaceGame(Subspace* ss);      // acquires gameinst, must release!
void subspaceSetGame(Subspace* ss, GameInst* game);
bool subspaceIsGame(Subspace* ss, GameInst* game);
void subspaceClearGame(Subspace* ss, GameInst* ifgame);
RunInfo* subspaceRun(Subspace* ss);   // acquires runinfo, must release!
void subspaceSetRun(Subspace* ss, RunInfo* run);
bool subspaceIsRun(Subspace* ss, RunInfo* run);
void subspaceClearRun(Subspace* ss, RunInfo* ifrun);
bool logOpen(VFS* vfs, string filename, LogDest** defer);
bool logClose(void);
void fatalError(strref msg, bool osdeperr);
int64 toLocalTime(int64 time);