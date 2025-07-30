#include "control/cmds.h"
#include "feature/featureregistry.h"
#include "feature/savemanager/savemanager.h"
#include "run/runinfo.h"

void cmdAutoSave(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    RunInfo* run = ginstRun(inst);

    strref fname    = cfieldString(fields, _S"filename");
    SaveManager* sm = fregGet(SaveManager, inst->ss->freg);
    if (!fname || !sm)
        return;

    string fn = 0;
    pathFromPlatform(&fn, fname);

    // can't do anything without a run in progress
    if (!run || !featureIsEnabled(sm) || !runinfoIsRecording(run)) {
        vfsDelete(inst->ss->rootfs, fn);
        goto out;
    }

    savemanagerAutoSave(sm, run, fn);

out:
    strDestroy(&fn);
    objRelease(&run);
}
