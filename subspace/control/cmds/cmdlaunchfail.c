#include "control/cmds.h"

void cmdLaunchFail(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    withWriteLock (&inst->lock) {
        cfieldVal(int32, fields, _S"reason", &inst->failReason);
    }
    ginstSetState(inst, GI_Failed);
}
