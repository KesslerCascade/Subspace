#include "cmds.h"

void cmdGameState(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    int32 state;
    if (cfieldVal(int32, fields,_S"state", &state))
        ginstSetState(inst, state);
}
