#include "subspacegame.h"
// NOTE: the entry point (i.e. "main" equivalent) is not located here, but is instead in entry.c in
// one of the playform-specific subdirectories

#include "net.h"

#include "control/controlclient.h"
#include "control/controlconnect.h"
#include "feature/frameadv.h"
#include "feature/infoblock.h"
#include "feature/numerichull.h"
#include "feature/timewarp.h"
#include "ftl/ftl.h"
#include "loader/loader.h"
#include "log/log.h"
#include "patch/patch.h"
#include "control.h"
#include "osdep.h"
#include "version.h"

SubspaceGameSettings settings = {
    .addr     = 0x7f000001,   // 127.0.0.1
    .frameAdv = &FrameAdv_feature,
    //.numericHull = &NumericHull_feature,
    .timeWarp = &TimeWarp_feature,
};
GameState gs;

static void parseArgs(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (!stricmp(argv[i], "-addr") && i + 1 < argc) {
            settings.addr = ntohl(inet_addr(argv[++i]));
        }
        if (!stricmp(argv[i], "-port") && i + 1 < argc) {
            settings.port = atoi(argv[++i]);
        }
    }
}

entrypoint ftlentry;
int sscmain(int argc, char* argv[])
{
    parseArgs(argc, argv);
    netInit();

    if (settings.port == 0) {
        osShowError("This executible is not intended to be run directly. "
                    "Please launch the main Subspace program to run the game.");
        return 1;
    }

    // First order of business: open the control connection
    socket_t sock;
    if (!controlConnect(&sock))
        return 1;
    controlInit(&control, sock);
    controlSendGameStart(&control);

    int lcmd = controlStartupHandshake(&control);
    if (lcmd != RLC_Launch)
        return (lcmd != RLC_Exit);

    osSetCurrentDir(settings.gameDir);
    log_fmt(LOG_Info, "Loading executable:  %s", settings.gamePath);
    ftlbase = loadProgram(settings.gameProgram);

    if (!ftlbase) {
        log_str(LOG_Error, "Failed to load game executable!");
        return 1;
    }

    PatchState ps;
    if (!patchBegin(&ps, ftlbase)) {
        log_str(LOG_Error, "Patching failed to initialize");
        return 1;
    }
    if (!patchApplySeq(&ps, OSDepPatches) || !patchApplySeq(&ps, RequiredPatches)) {
        log_str(LOG_Error, "Required patches failed");
        return 1;
    }

    if (initFeature(&InfoBlock_feature, &ps)) {
        enableFeature(&InfoBlock_feature, true);
        log_str(LOG_Verbose, "InfoBlock: PASSED");
    } else {
        log_str(LOG_Warn, "InfoBlock: FAILED");
    }
    if (initFeature(&TimeWarp_feature, &ps)) {
        enableFeature(&TimeWarp_feature, true);
        log_str(LOG_Verbose, "TimeWarp: PASSED");
    } else {
        log_str(LOG_Warn, "TimeWarp: FAILED");
    }
    if (initFeature(&FrameAdv_feature, &ps)) {
        enableFeature(&FrameAdv_feature, true);
        log_str(LOG_Verbose, "FrameAdv: PASSED");
    } else {
        log_str(LOG_Warn, "FrameAdv: FAILED");
    }
    if (initFeature(&NumericHull_feature, &ps)) {
        enableFeature(&NumericHull_feature, true);
        log_str(LOG_Verbose, "NumericHull: PASSED");
    } else {
        log_str(LOG_Warn, "NumericHull: FAILED");
    }

    if (!patchEnd(&ps)) {
        log_str(LOG_Error, "Patching failed to complete");
        return 1;
    }

    if (!settings.testMode) {
        ftlentry = getProgramEntry(ftlbase);
        ftlentry();
    }
    return 0;
}

void sscmain2(void)
{
    controlClientStart();
    log_client();
    log_str(LOG_Info, "Game communication thread started");
    return;
}