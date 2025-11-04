#include "control/cmds.h"

void cmdClearToStart(ControlMsg* msg, hashtable fields)
{
    gs.clearToStart = true;
}