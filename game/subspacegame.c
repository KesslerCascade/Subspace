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
        controlSendStartup(&control);

        ftlbase = loadProgram(settings.gameProgram);

        PatchState ps;
        if (!patchBegin(&ps, ftlbase)) {
            // log
            return 1;
        }
    if (!patchApplySeq(&ps, OSDepPatches) || !patchApplySeq(&ps, RequiredPatches)) {
        // log
        osWriteDbg("Required patches failed.\n");
        return 1;
    }

    if (initFeature(&InfoBlock_feature, &ps)) {
        enableFeature(&InfoBlock_feature, true);
        osWriteDbg("InfoBlock: PASSED\n");
    } else {
        osWriteDbg("InfoBlock: FAILED\n");
    }
    if (initFeature(&TimeWarp_feature, &ps)) {
        enableFeature(&TimeWarp_feature, true);
        osWriteDbg("TimeWarp: PASSED\n");
    } else {
        osWriteDbg("TimeWarp: FAILED\n");
    }
    if (initFeature(&FrameAdv_feature, &ps)) {
        enableFeature(&FrameAdv_feature, true);
        osWriteDbg("FrameAdv: PASSED\n");
    } else {
        osWriteDbg("FrameAdv: FAILED\n");
    }
    if (initFeature(&NumericHull_feature, &ps)) {
        enableFeature(&NumericHull_feature, true);
        osWriteDbg("NumericHull: PASSED\n");
    } else {
        osWriteDbg("NumericHull: FAILED\n");
    }

    if (!patchEnd(&ps)) {
        // log
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
    return;
}