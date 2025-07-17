#include "subspace.h"
#include "version.h"

#include "control/controlserver.h"
#include "db/database.h"
#include "feature/featureregistry.h"
#include "gamedata/gamedata.h"
#include "gamemgr/gamemgr.h"
#include "kbmgr/kbmgr.h"
#include "lang/lang.h"
#include "run/logrelay.h"
#include "ui/subspaceui.h"

#include <cx/debug.h>
#include <cx/log.h>
#include <cx/settings.h>
#include <cx/sys.h>

DEFINE_ENTRY_POINT

VFS* filesys;
Subspace subspace = { .listenaddr = 0x7f000001 };

#define SETTINGS_VER 1

static void parseArgs(Subspace* ss, VFS* vfs)
{
    int nargs = saSize(cmdArgs);
    for (int i = 0; i < nargs; i++) {
        if (strEqi(cmdArgs.a[i], _S"-basedir") && i < nargs - 1) {
            i++;
            subspaceSetBaseDir(ss, vfs, cmdArgs.a[i]);
        }
        if (strEqi(cmdArgs.a[i], _S"-dev")) {
            ss->devmode = true;
        }
        if (strEqi(cmdArgs.a[i], _S"-dbg")) {
            ss->devmode = true;
        }
        if (strEqi(cmdArgs.a[i], _S"-listenaddr") && i < nargs - 1) {
            i++;
            ss->listenaddr = ntohl(inet_addr(strC(cmdArgs.a[i])));
        }
        if (strEqi(cmdArgs.a[i], _S"-port") && i < nargs - 1) {
            i++;
            int nport = 0;
            if (strToInt32(&nport, cmdArgs.a[i], 10, true))
                ss->port = nport;
        }
    }
}

GameInst* subspaceGame(Subspace* ss)
{
    GameInst* ret = NULL;
    withReadLock (&ss->lock) {
        ret = objAcquire(ss->game);
    }
    return ret;
}

RunInfo* subspaceRun(Subspace* ss)
{
    RunInfo* ret = NULL;
    withReadLock (&ss->lock) {
        ret = objAcquire(ss->run);
    }
    return ret;
}

void subspaceSetGame(Subspace* ss, GameInst* game)
{
    withWriteLock (&ss->lock) {
        objRelease(&ss->game);
        ss->game = objAcquire(game);
    }
    // toolbar and info panel
    ssuiUpdateMain(ss->ui, NULL);
    ssuiUpdateMain(ss->ui, _S"gameinfo");
}

bool subspaceIsGame(Subspace* ss, GameInst* game)
{
    bool ret = false;
    withReadLock (&ss->lock) {
        ret = (ss->game == game);
    }
    return ret;
}

void subspaceClearGame(Subspace* ss, GameInst* ifgame)
{
    bool cleared = false;
    withWriteLock (&ss->lock) {
        if (ss->game && (ss->game == ifgame || !ifgame)) {
            objRelease(&ss->game);
            cleared = true;
        }
    }

    if (cleared) {
        // toolbar and info panel
        ssuiUpdateMain(ss->ui, NULL);
        ssuiUpdateMain(ss->ui, _S"gameinfo");
    }
}

void subspaceSetRun(Subspace* ss, RunInfo* run)
{
    withWriteLock (&ss->lock) {
        objRelease(&ss->run);
        ss->run = objAcquire(run);
    }

    logrelayReset(ss->runlog);

    // refresh all UI components
    subspaceUpdateUI(ss);
}

bool subspaceIsRun(Subspace* ss, RunInfo* run)
{
    bool ret = false;
    withReadLock (&ss->lock) {
        ret = (ss->run == run);
    }
    return ret;
}

void subspaceClearRun(Subspace* ss, RunInfo* ifrun)
{
    bool cleared = false;
    withWriteLock (&ss->lock) {
        if (ss->run && (ss->run == ifrun || !ifrun)) {
            objRelease(&ss->run);
            cleared = true;
        }
    }

    logrelayReset(ss->runlog);

    if (cleared) {
        // refresh all UI components
        subspaceUpdateUI(ss);
    }
}

GameData* subspaceData(Subspace* ss)
{
    GameData* ret = NULL;
    withReadLock (&ss->lock) {
        ret = objAcquire(ss->data);
    }
    return ret;
}

bool subspaceLoadData(Subspace* ss, strref ftldir)
{
    bool ret        = false;
    GameData* ndata = gamedataCreate();

    ret = gamedataLoad(ndata, ss, filesys, ftldir);

    if (ret) {
        withWriteLock (&ss->lock) {
            objRelease(&ss->data);
            ss->data = ndata;
        }
    } else {
        objRelease(&ndata);
    }

    return ret;
}

void subspaceUpdateUI(Subspace* ss)
{
    ssuiUpdate(ss->ui);
}

static upgradeSettings(SSDNode* settings)
{
    int curver    = ssdVal(int32, settings, _S"configver", 0);
    string tmpstr = 0;

    if (curver < 1) {
        // update user save override
        ssdStringOut(settings, _S"ftl/saveoverride", &tmpstr);
        if (strEq(tmpstr, _S"user/"))
            ssdSet(settings, _S"ftl/saveoverride", false, stvar(string, _S"[User]"));
    }

    ssdSet(settings, _S"configver", false, stvar(int32, SETTINGS_VER));
    strDestroy(&tmpstr);
}

static void subspaceStartup(LogDest** pdeferredlogs)
{
    // Basic prerequisites
    rwlockInit(&subspace.lock);

    // 01 -------- event that workers can use to notify the main thread of something
    eventInit(&subspace.notify);

    // 02 -------- Filesystem setup
    subspace.fs = vfsCreate(0);
    fsSetCurDir(subspace.basedir);   // for pathMakeAbsolute
    vfsMountVFS(subspace.fs, _S"/", filesys, subspace.basedir);

    // 03 -------- mount subspace:/ namespace
    if (!subspaceMount(&subspace)) {
        fatalError(
            _S
            "Required data files are missing. Please ensure the Subspace installation is complete.",
            false);
    }
    vfsSetCurDir(subspace.fs, SSNS);

    // 04 -------- Load Settings
    subspace.settings = setsOpen(subspace.fs, SETTINGS_FILENAME, 0);
    upgradeSettings(subspace.settings);

    // 05 -------- Log file setup
    if (!logOpen(subspace.fs, LOG_FILENAME, pdeferredlogs)) {
        fatalError(
            _S"Could not open log file. Please ensure there is only one copy of Subspace running.",
            false);
    }
    logFmt(Notice, _S"Subspace ${string} starting up!", stvar(strref, (strref)subspace_version_str));
    logFmt(Info, _S"Install directory is ${string}.", stvar(string, subspace.basedir));
    if (subspace.devmode)
        logStr(Notice, _S"Developer mode engaged. Good luck and have fun!");

    // 06 -------- Keybind manager
    subspace.kbmgr = kbmgrCreate(&subspace);

    // 07 -------- Feature registry
    subspace.freg = fregCreate(&subspace);

    // 08 -------- GameMgr
    subspace.gmgr = gmgrCreate(&subspace);

    // 09 -------- Task queue setup
    int ncores   = osPhysicalCPUs();
    int nthreads = osLogicalCPUs();
    TaskQueueConfig conf;
    bool ret = true;

    tqPresetBalanced(&conf);
    tqEnableMonitor(&conf);
    conf.pool.wInitial = ncores;
    conf.pool.wIdle    = ncores;
    conf.pool.wBusy    = nthreads;
    conf.pool.wMax     = nthreads * 2;
    subspace.workq     = tqCreate(_S"Main", &conf);
    if (subspace.workq)
        ret &= tqStart(subspace.workq);

    // 10 -------- Database
    subspace.db = dbCreate(&subspace);
    if (!dbOpen(subspace.db) || !dbCheck(subspace.db)) {
        fatalError(_S"Failed to open database.", false);
    }

    // 11 -------- Log relay
    subspace.runlog = logrelayCreate(&subspace);

    // 12 -------- UI setup
    subspace.ui = ssuiCreate(&subspace);
    if (!ssuiInitialize(subspace.ui)) {
        fatalError(_S"Failed to initialize UI.", false);
    }

    // 13 -------- Control Server setup
    subspace.svr = cserverCreate(&subspace);
    if (!cserverStart(subspace.svr)) {
        fatalError(_S"Failed to start control server.", false);
    }

    // 14 -------- Game data
    string ftldir = 0;
    ssdStringOut(subspace.settings, _S"ftl/exe", &ftldir);
    if (!strEmpty(ftldir)) {
        pathParent(&ftldir, ftldir);
        subspaceLoadData(&subspace, ftldir);
    }
}

static void subspaceShutdown()
{
    // 15 -------- Running state
    withWriteLock (&subspace.lock) {
        objRelease(&subspace.game);
        objRelease(&subspace.run);
    }

    // 14 -------- Game data
    objRelease(&subspace.data);

    // 13 -------- Control Server shutdown
    cserverStop(subspace.svr);

    // 12 -------- UI teardown
    ssuiShutdown(subspace.ui);

    // 11 -------- Log relay
    objRelease(&subspace.runlog);

    // 10 -------- Database shutdown
    dbClose(subspace.db);
    objRelease(&subspace.db);

    // 09 -------- Task queue shutdown
    tqShutdown(subspace.workq, true);

    // 08 -------- Game manager
    objRelease(&subspace.gmgr);

    // 07 -------- Feature registry
    objRelease(&subspace.freg);

    // 06 -------- Keybind manager
    objRelease(&subspace.kbmgr);

    // 05 -------- Log file
    logClose();

    // 04 -------- Load Settings
    setsClose(&subspace.settings);

    // 03 -------- unmount the subspace:/ namespace
    vfsSetCurDir(subspace.fs, _S"/");
    subspaceUnmount(&subspace);

    // 02 -------- Filesystem setup
    vfsDestroy(&subspace.fs);

    // 01 -------- event that workers can use to notify the main thread of something
    eventDestroy(&subspace.notify);

    // Release some objects that were kept avialable during shutdown
    objRelease(&subspace.svr);
    objRelease(&subspace.workq);

    // Final cleanup
    rwlockDestroy(&subspace.lock);
}

int entryPoint()
{
    // do memory logging for crash dumps
    dbgLogEnable(LOG_Info);

    // create defer buffer to capture any early log messages
    LogDeferData* dd  = logDeferCreate();
    LogDest* deferbuf = logRegisterDest(LOG_Debug, NULL, logDeferDest, dd);

    filesys = vfsCreateFromFS();
    if (!filesys)
        fatalError(_S"Could not open filesystem", false);

    if (!netInit())
        fatalError(_S"Could not initialize networking", true);

    parseArgs(&subspace, filesys);

    // try to find basedir
    if (strEmpty(subspace.basedir)) {
        if (!subspaceFindBaseDir(&subspace, filesys)) {
            fatalError(
                _S
                "Could not find Subspace installation folder. Please ensure that the required files are present.",
                false);
        }
    }

    subspaceStartup(&deferbuf);

    do {
        subspace.reloadui = false;
        ssuiStart(subspace.ui);
        do {
            eventWaitTimeout(&subspace.notify, timeS(10));
        } while (!(subspace.exit || subspace.reloadui));
        ssuiStop(subspace.ui);
    } while (subspace.reloadui);

    subspaceShutdown();

    return 0;
}
