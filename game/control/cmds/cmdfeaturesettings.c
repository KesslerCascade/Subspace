#include "control/cmds.h"
#include "feature/feature.h"

void cmdFeatureSettings(ControlMsg* msg, hashtable fields)
{
    SubspaceFeature* feat = NULL;

    strref fname = cfieldString(fields, _S "feature");
    if (!strEmpty(fname))
        feat = getFeature(fname);

    if (!feat || !feat->settings || !feat->settingsspec)
        return;

    FeatureSettingsEnt* ent = feat->settingsspec->ent;
    while (ent->name) {
        void* dest = (void*)((uintptr_t)feat->settings + ent->off);

        switch (ent->type) {
        case CF_INT:
            cfieldVal(int32, fields, ent->name, (int*)dest);
            break;
        case CF_BOOL:
            cfieldVal(bool, fields, ent->name, (bool*)dest);
            break;
        case CF_FLOAT32:
            cfieldVal(float32, fields, ent->name, (float*)dest);
            break;
        case CF_FLOAT64:
            cfieldVal(float64, fields, ent->name, (double*)dest);
            break;
        case CF_STRING:
            strDup((string*)dest, cfieldString(fields, ent->name));
            break;
        }

        ent++;
    }
}
