#include <cx/log.h>
#include "control/cmds.h"

void cmdClearToStart(ControlMsg* msg, hashtable fields)
{
    logStr(Verbose, _S"Received Clear-to-Start from main process");
    gs.clearToStart = true;
}