#include <cx/platform/os.h>
#include <cx/string.h>
#include <cx/sys.h>
#include <cx/time.h>

#include "netsocket.h"

#include "subspacegame.h"

#include "control/cmds.h"
#include "control/controlclient.h"
#include "control/controlconnect.h"
#include "feature/feature.h"
#include "ftl/ftl.h"
#include "hook/module.h"
#include "loader/loader.h"
#include "log/gamelog.h"
#include "patch/patch.h"
#include "control.h"
#include "osdep.h"
#include "version.h"

DEFINE_ENTRY_POINT

SubspaceGameSettings settings = {
    .addr = 0x7f000001,   // 127.0.0.1
};
GameGlobalState gs;
GameGlobalContext gc;

static void parseArgs(void)
{
    int argc = saSize(cmdArgs);
    for (int i = 0; i < argc; i++) {
        if (strEq(cmdArgs.a[i], _S"-addr") && i + 1 < argc) {
            settings.addr = ntohl(inet_addr(strC(cmdArgs.a[++i])));
        }
        if (strEq(cmdArgs.a[i], _S"-port") && i + 1 < argc) {
            strToInt32(&settings.port, cmdArgs.a[++i], 10, true);
        }
        if (strEq(cmdArgs.a[i], _S"-cookie") && i + 2 < argc) {
            int32 temp = 0;
            strToInt32(&temp, cmdArgs.a[++i], 16, true);
            settings.cookie = (uint32_t)(temp << 16);
            strToInt32(&temp, cmdArgs.a[++i], 16, true);
            settings.cookie |= (uint32_t)temp;
        }
    }
}

entrypoint ftlentry;
int entryPoint()
{
    parseArgs();
    netInit();

    if (settings.port == 0 || settings.cookie == 0) {
        osShowError("This executible is not intended to be run directly. "
                    "Please launch the main Subspace program to run the game.");
        return 1;
    }

    // First order of business: open the control connection
    socket_t sock;
    if (!controlConnect(&sock))
        return 1;
    controlInit(&control, sock);

    int lcmd = controlStartupHandshake(&control);
    if (lcmd != RLC_Launch)
        return (lcmd != RLC_Exit);

    gameLogRegister();
    logFmt(Info,
           _S"Subspace Game Loader ${string} starting up!",
           stvar(strref, (strref)subspace_version_str));

    osSetCurrentDir(settings.gameDir);
    logFmt(Info, _S"Loading executable:  ${string}", stvar(strref, (strref)settings.gamePath));
    ftlbase = loadProgram(settings.gamePath);
    gameLogSend();

    if (!ftlbase) {
        logStr(Error, _S"Failed to load game executable!");
        gameLogSend();
        controlSendLaunchFail(&control, LAUNCH_FAIL_NOEXE);
        return 1;
    }

    registerAllFeatures();

    PatchState ps;
    if (!patchBegin(&ps, ftlbase)) {
        logStr(Error, _S"Patching failed to initialize");
        gameLogSend();
        controlSendLaunchFail(&control, LAUNCH_FAIL_OTHER);
        return 1;
    }

    patchValidateSeq(&ps, OSDepPatches);
    gameLogSend();
    validateAllFeatures(&ps);
    gameLogSend();

    logBatchBegin();
    logStr(Info, _S"Applying required patches");
    if (!patchApplySeq(&ps, OSDepPatches) || !patchFeature(&Base_feature, &ps)) {
        logStr(Error, _S"Required patches failed");
        logBatchEnd();
        gameLogSend();
        controlSendLaunchFail(&control, LAUNCH_FAIL_REQPATCH);
        return 1;
    }
    logBatchEnd();
    gameLogSend();

    patchAllFeatures(&ps);

    if (!patchEnd(&ps)) {
        logStr(Error, _S"Patching failed to complete");
        gameLogSend();
        controlSendLaunchFail(&control, LAUNCH_FAIL_OTHER);
        return 1;
    }

    switch (settings.mode) {
    case LAUNCH_PLAY:
        logStr(Verbose, _S"Calling main FTL entry point");
        ftlentry = getProgramEntry(ftlbase);
        ftlentry();
        break;
    case LAUNCH_VALIDATE:
        logStr(Verbose, _S"Sending validation results and exiting");
        gameLogSend();
        controlSendValidate(&control, true, 0);
        controlDisconnect(&sock);
        break;
    }
    return 0;
}

int cleanupthread(void* unused)
{
    // do this in a background thread, because salloc is kind of slow when the freelist gets large.
    // this ends up consolidating most of the fragmented freelists.
    cleanupAnalysis();
    return 0;
}

void sscmain2(void)
{
    gameLogSend();
    controlClientStart();
    registerCmds();
    gameLogSwitchToClientQueue();
    logStr(Info, _S"Communication thread started");
    sendAllFeatureState();

    ControlMsg* msg = controlMsgCreate(_S"GameReady");
    cfieldSet(msg, _S"start", bool, true);
    controlClientQueue(msg);

    // loop until we get the all-clear
    while (!gs.clearToStart) {
        controlClientProcessInbound();
        controlClientProcessOutbound();
        osSleep(timeFromMsec(1));

        // ensure we don't get stuck here if the connection closes
        if (!control.sock || control.closed) {
            osExit(1);
        }
    }

    // start a background thead to clean up free analysis data
    osStartThread(cleanupthread, NULL);

    return;
}
