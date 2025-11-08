#include "control/cmds.h"
#include "feature/feature.h"

void cmdFeatureSettings(ControlMsg* msg, hashtable fields)
{
    SubspaceFeature* feat = NULL;

    strref fname = cfieldString(fields, _S "feature");
    if (!strEmpty(fname))
        feat = getFeature(strC(fname));

    if (!feat || !feat->settings || !feat->settingsspec)
        return;

    FeatureSettingsEnt* ent = feat->settingsspec->ent;
    while (ent->name) {
        void* dest = (void*)((uintptr_t)feat->settings + ent->off);

        switch (ent->type) {
        case CF_INT:
            cfieldVal(int32, fields, (strref)ent->name, (int*)dest);
            break;
        case CF_BOOL:
            cfieldVal(bool, fields, (strref)ent->name, (bool*)dest);
            break;
        case CF_FLOAT32:
            cfieldVal(float32, fields, (strref)ent->name, (float*)dest);
            break;
        case CF_FLOAT64:
            cfieldVal(float64, fields, (strref)ent->name, (double*)dest);
            break;
        case CF_STRING:
            if (*(char**)dest)
                xaFree(*(char**)dest);
            strref s      = cfieldString(fields, (strref)ent->name);
            *(char**)dest = s ? xa_strdup(strC(s)) : NULL;
            break;
        }

        ent++;
    }
}
