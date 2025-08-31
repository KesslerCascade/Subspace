#include "control/cmds.h"
#include "feature/featureregistry.h"
#include "feature/screenshot/screenshot.h"
#include "run/runinfo.h"

void cmdScreenshot(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    RunInfo* run   = ginstRun(inst);
    Screenshot* ss = fregGet(Screenshot, inst->ss->freg);
    strref fname   = cfieldString(fields, _S"filename");
    bool automatic = false;

    if (!fname || !ss || !cfieldVal(bool, fields, _S"auto", &automatic))
        goto out;

    pathFromPlatform(&fname, fname);
    screenshotProcess(ss, fname, run, msg->hdr.timestamp, automatic, fields);

out:
    objRelease(&run);
}
