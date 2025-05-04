#include "control/cmds.h"

void cmdClearToStart(ControlMsg* msg)
{
    gs.clearToStart = true;
}