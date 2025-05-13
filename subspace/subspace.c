#include "subspace.h"
#include "version.h"

#include "control/controlserver.h"
#include "feature/featureregistry.h"
#include "gamemgr/gamemgr.h"
#include "kbmgr/kbmgr.h"
#include "lang/lang.h"
#include "ui/subspaceui.h"

#include <cx/debug.h>
#include <cx/log.h>
#include <cx/settings.h>
#include <cx/sys.h>

DEFINE_ENTRY_POINT

VFS* filesys;
Subspace subspace = { .listenaddr = 0x7f000001 };

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

GameInst* subspaceCurInst(Subspace* ss)
{
    return objAcquireFromWeak(GameInst, ss->curinst);
}

void subspaceUpdateUI(Subspace* ss)
{
    ssuiUpdate(ss->ui);
}

static void subspaceStartup(LogDest** pdeferredlogs)
{
    // 01 -------- event that workers can use to notify the main thread of something
    eventInit(&subspace.notify);

    // 02 -------- Filesystem setup
    subspace.fs = vfsCreate(0);
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

    // 10 -------- UI setup
    subspace.ui = ssuiCreate(&subspace);
    if (!ssuiInit(subspace.ui)) {
        fatalError(_S"Failed to initialize UI.", false);
    }

    // 11 -------- Control Server setup
    subspace.svr = cserverCreate(&subspace);
    if (!cserverStart(subspace.svr)) {
        fatalError(_S"Failed to start control server.", false);
    }
}

static void subspaceShutdown()
{
    // 11 -------- Control Server shutdown
    cserverStop(subspace.svr);

    // 10 -------- UI teardown
    ssuiShutdown(subspace.ui);

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
