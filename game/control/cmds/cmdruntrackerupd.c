#include "control/cmds.h"
#include "feature/runtracker.h"

void cmdRunTrackerUpd(ControlMsg* msg, hashtable fields)
{
    bool recording = false;
    if (cfieldVal(bool, fields, _S"recording", &recording))
        runTrackerHandleUpdate(recording);
}