#include "control/cmds.h"

void cmdGameReady(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    ginstOnGameReady(inst, client);
}
