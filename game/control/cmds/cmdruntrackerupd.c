#include "control/cmds.h"
#include "feature/runtracker.h"

void cmdRunTrackerUpd(ControlMsg* msg)
{
    ControlField* frecording = controlMsgFindField(msg, "recording");
    if (frecording && frecording->h.ftype == CF_BOOL)
        runTrackerHandleUpdate(frecording->d.cfd_bool);
}