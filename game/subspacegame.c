#include "netsocket.h"

#include "subspacegame.h"
// NOTE: the entry point (i.e. "main" equivalent) is not located here, but is instead in entry.c in
// one of the playform-specific subdirectories

#include "control/cmds.h"
#include "control/controlclient.h"
#include "control/controlconnect.h"
#include "feature/feature.h"
#include "ftl/ftl.h"
#include "hook/module.h"
#include "loader/loader.h"
#include "log/log.h"
#include "patch/patch.h"
#include "control.h"
#include "osdep.h"
#include "version.h"

#include "minicrt.h"

SubspaceGameSettings settings = {
    .addr = 0x7f000001,   // 127.0.0.1
};
GameGlobalState gs;

static void parseArgs(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (!stricmp(argv[i], "-addr") && i + 1 < argc) {
            settings.addr = ntohl(inet_addr(argv[++i]));
        }
        if (!stricmp(argv[i], "-port") && i + 1 < argc) {
            settings.port = atoi(argv[++i]);
        }
        if (!stricmp(argv[i], "-cookie") && i + 2 < argc) {
            settings.cookie = strtol(argv[++i], NULL, 16) << 16;
            settings.cookie |= strtol(argv[++i], NULL, 16);
        }
    }
}

entrypoint ftlentry;
int sscmain(int argc, char* argv[])
{
    parseArgs(argc, argv);
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

    osSetCurrentDir(settings.gameDir);
    log_fmt(LOG_Info, "Loading executable:  %s", settings.gamePath);
    ftlbase = loadProgram(settings.gamePath);

    if (!ftlbase) {
        log_str(LOG_Error, "Failed to load game executable!");
        controlSendLaunchFail(&control, LAUNCH_FAIL_NOEXE);
        return 1;
    }

    registerAllFeatures();

    PatchState ps;
    if (!patchBegin(&ps, ftlbase)) {
        log_str(LOG_Error, "Patching failed to initialize");
        controlSendLaunchFail(&control, LAUNCH_FAIL_OTHER);
        return 1;
    }

    patchValidateSeq(&ps, OSDepPatches);
    validateAllFeatures(&ps);

    if (!patchApplySeq(&ps, OSDepPatches) || !patchFeature(&Base_feature, &ps)) {
        log_str(LOG_Error, "Required patches failed");
        controlSendLaunchFail(&control, LAUNCH_FAIL_REQPATCH);
        return 1;
    }
    patchAllFeatures(&ps);

    if (!patchEnd(&ps)) {
        log_str(LOG_Error, "Patching failed to complete");
        controlSendLaunchFail(&control, LAUNCH_FAIL_OTHER);
        return 1;
    }

    switch (settings.mode) {
    case LAUNCH_PLAY:
        ftlentry = getProgramEntry(ftlbase);
        ftlentry();
        break;
    case LAUNCH_VALIDATE:
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
    controlClientStart();
    registerCmds();
    log_client();
    log_str(LOG_Info, "Communication thread started");
    sendAllFeatureState();

    ControlMsg* msg = controlNewMsg("GameReady", 1);
    controlMsgBool(msg, 0, "start", 1);
    controlClientQueue(msg);

    // loop until we get the all-clear
    while (!gs.clearToStart) {
        controlClientProcess();
        osSleep(1);

        // ensure we don't get stuck here if the connection closes
        if (!control.sock || control.closed) {
            osExit(1);
        }
    }

    // start a background thead to clean up free analysis data
    osStartThread(cleanupthread, NULL);

    return;
}
