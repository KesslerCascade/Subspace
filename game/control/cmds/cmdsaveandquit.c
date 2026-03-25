#include "control/cmds.h"

void cmdSaveAndQuit(ControlMsg* msg, hashtable fields)
{
    gs.saveAndQuit = true;
}