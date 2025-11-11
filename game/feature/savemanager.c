#include "ftl/stdlib.h"

#include "control/controlclient.h"
#include "feature/feature.h"
#include "ftl/cfps.h"
#include "ftl/filehelper.h"
#include "ftl/misc.h"
#include "ftl/worldmanager.h"
#include "patch/patchlist.h"
#include "util/lcg.h"
#include "osdep.h"
#include "savemanager.h"

void saveManagerAutoSave(WorldManager* world)
{
    if (!controlClientConnected() || !world)
        return;

    gs.autoSaveInProgress = true;

    basic_string sfile;
    basic_string_reset(&sfile);
    FileHelper_getSaveFile(&sfile);

    if (gs.saveFileOverride)
        xa_free(gs.saveFileOverride);   // shouldn't be possible? but don't leak anyway

    char* tempfn = xa_malloc(1024);
    if (!osAbsolutePathUTF8(sfile.buf, tempfn, 1024)) {
        xa_free(tempfn);
        basic_string_destroy(&sfile);
        return;
    }
    size_t len = strlen(tempfn);

    gs.saveFileOverride = xa_malloc(len + 19);
    memcpy(gs.saveFileOverride, tempfn, len);
    snprintf(gs.saveFileOverride + len, 19, ".subspace-%08d", lcg_random() % 100000000);
    basic_string_destroy(&sfile);
    xa_free(tempfn);

    WorldManager_SaveGame(world);

    // notify main process to pick up the save file
    ControlMsg* msg = controlMsgCreate(_S"AutoSave");
    cfieldSet(msg, _S"filename", strref, (strref)gs.saveFileOverride);
    msg->priority = 100;   // AutoSave should happen last after everything else this frame
    controlClientQueue(msg);

    xa_free(gs.saveFileOverride);
    gs.saveFileOverride   = NULL;
    gs.autoSaveInProgress = false;
}

// ---- Patching ----------------

Patch* SaveManager_patches[] = { &patch_FileHelper_getSaveFile,
                                 &patch_WorldManager_CreateLocation,
                                 0 };

SubspaceFeature SaveManager_feature = { .name            = _S"SaveManager",
                                        .requiredPatches = SaveManager_patches,
                                        .requiredSymbols = { 0 } };
