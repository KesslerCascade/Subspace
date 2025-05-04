#include "control/cmds.h"
#include "feature/feature.h"

void cmdEnableFeature(ControlMsg* msg)
{
    SubspaceFeature* feat = NULL;

    ControlField* fname = controlMsgFindField(msg, "feature");
    if (fname && fname->h.ftype == CF_STRING)
        feat = getFeature(fname->d.cfd_str);

    if (!feat)
        return;

    ControlField* fenabled = controlMsgFindField(msg, "enabled");
    if (fenabled && fenabled->h.ftype == CF_INT) {
        enableFeature(feat, fenabled->d.cfd_int != 0);
    }
}