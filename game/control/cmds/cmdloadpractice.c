#include "control/cmds.h"
#include "feature/practicemode.h"

void cmdLoadPractice(ControlMsg* msg, hashtable fields)
{
    strref savepath = cfieldString(fields, _S"savepath");
    if (!strEmpty(savepath)) {
        practiceSetSave(savepath);
        gs.practiceLoadSave = true;
    }
}
