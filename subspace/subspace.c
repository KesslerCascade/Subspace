#include "subspace.h"
#include "version.h"

#include "control/controlserver.h"
#include "gamemgr/gamemgr.h"
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

    // event that workers can use to notify the main thread of something
    eventInit(&subspace.notify);

    subspace.fs = vfsCreate(0);
    vfsMountVFS(subspace.fs, _S"/", filesys, subspace.basedir);

    // mount subspace:/ namespace
    if (!subspaceMount(&subspace)) {
        fatalError(
            _S
            "Required data files are missing. Please ensure the Subspace installation is complete.",
            false);
    }
    vfsSetCurDir(subspace.fs, SSNS);

    subspace.settings = setsOpen(subspace.fs, SETTINGS_FILENAME, 0);

    // Set up log file
    if (!logOpen(subspace.fs, LOG_FILENAME, &deferbuf)) {
        fatalError(
            _S"Could not open log file. Please ensure there is only one copy of Subspace running.",
            false);
    }
    logFmt(Notice, _S"Subspace ${string} starting up!", stvar(strref, (strref)subspace_version_str));
    logFmt(Info, _S"Install directory is ${string}.", stvar(string, subspace.basedir));
    if (subspace.devmode)
        logStr(Notice, _S"Developer mode engaged. Good luck and have fun!");

    // Create task queue
    int ncores = osPhysicalCPUs();
    TaskQueueConfig conf;
    bool ret = true;

    tqPresetBalanced(&conf);
    tqEnableMonitor(&conf);
    conf.pool.wInitial = ncores;
    subspace.workq     = tqCreate(_S"Main", &conf);
    if (subspace.workq)
        ret &= tqStart(subspace.workq);

    subspace.ui = ssuiCreate(&subspace);
    if (!ssuiInit(subspace.ui)) {
        fatalError(_S"Failed to initialize UI.", false);
    }

    subspace.gmgr = gmgrCreate(&subspace);

    subspace.svr = cserverCreate(&subspace);
    if (!cserverStart(subspace.svr)) {
        fatalError(_S"Failed to start control server.", false);
    }

    // load language translations
    string lang = 0;
    ssdStringOutD(subspace.settings, _S"ui/lang", &lang, _S"en-us");
    if (!langLoad(&subspace, lang)) {
        fatalError(_S"Could not load any UI language.", false);
    }
    strDestroy(&lang);

    ssuiStart(subspace.ui);

    // TEMP FOR TESTING
    string tmp = 0;
    ssdStringOut(subspace.settings, _S"ftl/exe", &tmp);
    GameInst* gitest = ginstCreate(subspace.gmgr, tmp, GI_PLAY);
    strDestroy(&tmp);
    gmgrReg(subspace.gmgr, gitest);
    ginstLaunch(gitest);

    do {
        eventWaitTimeout(&subspace.notify, timeS(10));

    } while (!subspace.exit);

    ssuiStop(subspace.ui);
    ssuiShutdown(subspace.ui);

    cserverStop(subspace.svr);

    setsClose(&subspace.settings);

    // unmount the subspace:/ namespace
    vfsSetCurDir(subspace.fs, _S"/");
    subspaceUnmount(&subspace);

    // Exiting
    tqShutdown(subspace.workq, true);
    objRelease(&subspace.workq);
    objRelease(&subspace.ui);
    objRelease(&subspace.gmgr);
    objRelease(&subspace.svr);

    logClose();

    return 0;
}
