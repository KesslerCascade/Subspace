#include "ftl/stdlib.h"

#include "control/controlclient.h"
#include "feature/feature.h"
#include "ftl/cfps.h"
#include "ftl/filehelper.h"
#include "ftl/misc.h"
#include "ftl/worldmanager.h"
#include "patch/patchlist.h"
#include "osdep.h"
#include "savemanager.h"

// only lower 32 bits of state are used, but on 64-bit platforms we want uintptr_t to keep
// structures aligned
#define LCG_MAX (0x7ffffffe)
static unsigned int lcgstate;
static bool lcgstate_init;
static int lcg_random()
{
    if (!lcgstate_init) {
        lcgstate      = rand();
        lcgstate_init = true;
    }
    return ((lcgstate = lcgstate * 1103515245 + 12345) % ((unsigned int)LCG_MAX + 1));
}

void saveManagerAutoSave(WorldManager* world)
{
    if (!controlClientConnected() || !world)
        return;

    gs.autoSaveInProgress = true;

    basic_string sfile;
    basic_string_reset(&sfile);
    FileHelper_getSaveFile(&sfile);

    if (gs.saveFileOverride)
        free(gs.saveFileOverride);   // shouldn't be possible? but don't leak anyway

    char* tempfn = malloc(1024);
    if (!osAbsolutePathUTF8(sfile.buf, tempfn, 1024)) {
        free(tempfn);
        basic_string_destroy(&sfile);
        return;
    }
    size_t len = strlen(tempfn);

    gs.saveFileOverride = malloc(len + 19);
    memcpy(gs.saveFileOverride, tempfn, len);
    xsnprintf(gs.saveFileOverride + len, 19, ".subspace-%08d", lcg_random() % 100000000);
    basic_string_destroy(&sfile);
    free(tempfn);

    WorldManager_SaveGame(world);

    // notify main process to pick up the save file
    ControlMsg* msg = controlNewMsg("AutoSave", 1);
    controlMsgStr(msg, 0, "filename", gs.saveFileOverride);
    msg->priority = 100;   // AutoSave should happen last after everything else this frame
    controlClientQueue(msg);

    free(gs.saveFileOverride);
    gs.saveFileOverride   = NULL;
    gs.autoSaveInProgress = false;
}

// ---- Patching ----------------

Patch* SaveManager_patches[] = { &patch_FileHelper_getSaveFile,
                                 &patch_WorldManager_CreateLocation,
                                 0 };

SubspaceFeature SaveManager_feature = { .name            = "SaveManager",
                                        .requiredPatches = SaveManager_patches,
                                        .requiredSymbols = { 0 } };
