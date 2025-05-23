#include "cmds.h"
#include "control/controlclient.h"

#define DECLCMD(cmdname) void cmd##cmdname(ControlMsg* msg)

DECLCMD(ClearToStart);
DECLCMD(EnableFeature);
DECLCMD(FeatureSettings);
DECLCMD(BindKey);

#define REGCMD(cmdname) controlClientRegister(#cmdname, cmd##cmdname)

void registerCmds()
{
    REGCMD(ClearToStart);
    REGCMD(EnableFeature);
    REGCMD(FeatureSettings);
    REGCMD(BindKey);
}
