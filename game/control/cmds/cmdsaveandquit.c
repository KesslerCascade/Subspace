#include "control/cmds.h"

void cmdSaveAndQuit(ControlMsg* msg)
{
    gs.saveAndQuit = true;
}