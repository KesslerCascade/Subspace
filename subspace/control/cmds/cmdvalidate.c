#include "control/cmds.h"
#include "ui/subspaceui.h"

void cmdValidate(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    bool result;
    if (!cfieldVal(bool, fields, _S"result", &result))
        return;

    withWriteLock (&inst->lock) {
        if (result) {
            stvar val;
            if (htFind(fields, strref, _S"ver", stvar, &val)) {
                if (stvarIs(&val, sarray) && saSize(val.data.st_sarray) == 3) {
                    int* ver     = (int*)val.data.st_sarray.a;
                    inst->ver[0] = ver[0];
                    inst->ver[1] = ver[1];
                    inst->ver[2] = ver[2];
                }
                stDestroy(stvar, &val);
            }

            if (htFind(fields, strref, _S"features", stvar, &val)) {
                if (stvarIs(&val, sarray)) {
                    string* feats = (string*)val.data.st_sarray.a;
                    for (int i = 0; i < saSize(val.data.st_sarray); i++) {
                        ClientFeature* nf = clientfeatureCreate(feats[i]);
                        nf->available     = true;
                        htInsertC(&inst->features, strref, nf->name, object, &nf);
                    }
                }
                stDestroy(stvar, &val);
            }

            ginstSetStateLocked(inst, GI_Validated);
        } else {
            cfieldVal(int32, fields, _S"reason", &inst->failReason);
            ginstSetStateLocked(inst, GI_Failed);
        }
    }

    ssuiUpdateSettings(inst->ss->ui, _S"setup");
}
