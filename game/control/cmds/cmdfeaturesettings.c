#include "control/cmds.h"
#include "feature/feature.h"
#include "minicrt.h"

void cmdFeatureSettings(ControlMsg* msg)
{
    SubspaceFeature* feat = NULL;

    ControlField* fname = controlMsgFindField(msg, "feature");
    if (fname && fname->h.ftype == CF_STRING)
        feat = getFeature(fname->d.cfd_str);

    if (!feat || !feat->settings || !feat->settingsspec)
        return;

    for (int i = 1; i < msg->hdr.nfields; i++) {
        ControlField* field     = msg->fields[i];
        FeatureSettingsEnt* ent = feat->settingsspec->ent;
        while (ent->name) {
            if (!strcmp(ent->name, field->h.name)) {
                void* dest = (void*)((uintptr_t)feat->settings + ent->off);

                switch (ent->type) {
                case CF_INT:
                    *(int*)dest = field->d.cfd_int;
                    break;
                case CF_BOOL:
                    *(bool*)dest = field->d.cfd_bool;
                    break;
                case CF_FLOAT32:
                    *(float*)dest = field->d.cfd_float32;
                    break;
                case CF_FLOAT64:
                    *(double*)dest = field->d.cfd_float64;
                    break;
                case CF_STRING:
                    if (*(char**)dest)
                        sfree(*(char**)dest);
                    *(char**)dest = sstrdup(field->d.cfd_str);
                    break;
                }
                break;
            }

            ent++;
        }
    }
}
