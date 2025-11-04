#include "control/cmds.h"
#include "feature/feature.h"

void cmdEnableFeature(ControlMsg* msg, hashtable fields)
{
    SubspaceFeature* feat = NULL;

    strref fname = cfieldString(fields, _S "feature");
    if (!strEmpty(fname))
        feat = getFeature(strC(fname));

    if (!feat)
        return;

    bool enabled;
    if (cfieldVal(bool, fields, _S "enabled", &enabled))
        enableFeature(feat, enabled);
}