#include "cmds.h"

void cmdLoading(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    withWriteLock (&inst->lock) {
        cfieldVal(float32, fields, _S"pct", &inst->loadPct);
        ginstSetStateLocked(inst, GI_Loading);
    }
}
