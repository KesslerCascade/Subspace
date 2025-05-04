#include "control/cmds.h"

void cmdLog(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    int32 level   = cfieldValD(int32, fields, _S"level", LOG_Count);
    string logmsg = 0;
    strDup(&logmsg, cfieldString(fields, _S"msg"));

    uint32 pid;
    if (inst && inst->process && procGetID(&inst->process, &pid)) {
        string pidstr = 0;
        strFromUInt32(&pidstr, pid, 10);
        strNConcat(&logmsg, _S"[", pidstr, _S"] ", logmsg);
        strDestroy(&pidstr);
    } else {
        strPrepend(_S"[Game] ", &logmsg);
    }

    if (level < LOG_Count && !strEmpty(logmsg))
        _logStr(level, gamecat, logmsg);

    strDestroy(&logmsg);
}
