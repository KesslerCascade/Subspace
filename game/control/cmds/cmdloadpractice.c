#include "control/cmds.h"
#include "feature/practicemode.h"

void cmdLoadPractice(ControlMsg* msg)
{
    ControlField* fsavepath = controlMsgFindField(msg, "savepath");
    if (fsavepath && fsavepath->h.ftype == CF_STRING) {
        practiceSetSave(fsavepath->d.cfd_str);
        gs.practiceLoadSave = true;
    }
}
