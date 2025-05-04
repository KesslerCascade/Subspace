#include "control/cmds.h"

void cmdFeatureState(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    strref featname = cfieldString(fields, _S"feature");
    if (strEmpty(featname))
        return;

    withWriteLock (&inst->lock) {
        ClientFeature* feat = NULL;
        if (!htFind(inst->features, strref, featname, object, &feat)) {
            feat = clientfeatureCreate(featname);
            htInsert(&inst->features, string, feat->name, object, feat);
        }

        cfieldVal(bool, fields, _S"available", &feat->available);
        cfieldVal(bool, fields, _S"enabled", &feat->enabled);

        objRelease(&feat);
    }
}
