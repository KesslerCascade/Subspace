#include "cmds.h"

#define DECLCMD(cmdname) \
    void cmd ## cmdname(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)

DECLCMD(FeatureState);
DECLCMD(GameReady);
DECLCMD(GameStart);
DECLCMD(GameState);
DECLCMD(LaunchFail);
DECLCMD(LoadGame);
DECLCMD(Loading);
DECLCMD(Log);
DECLCMD(NewGame);
DECLCMD(RunLog);
DECLCMD(Sector);
DECLCMD(Stats);
DECLCMD(Validate);
DECLCMD(Beacon);
DECLCMD(AutoSave);
DECLCMD(Screenshot);

#define REGCMD(cmdname) \
cserverRegisterHandler(csvr, _S #cmdname, cmd ## cmdname, false, (TaskResource*)NULL)
#define REGCMDREQ(cmdname, req) \
cserverRegisterHandler(csvr, _S #cmdname, cmd ## cmdname, false, req)
// versions that require a GameInst
#define REGCMDI(cmdname) \
cserverRegisterHandler(csvr, _S #cmdname, cmd ## cmdname, true, (TaskResource*)NULL)
#define REGCMDREQI(cmdname, req) \
cserverRegisterHandler(csvr, _S #cmdname, cmd ## cmdname, true, req)

void registerCmds(ControlServer* csvr)
{
    REGCMDREQI(FeatureState, csvr->preGameReady);
    REGCMD(GameStart);
    REGCMDREQ(GameReady, csvr->preGameReady);
    REGCMDI(Validate);
    REGCMDI(GameState);
    REGCMDI(LaunchFail);
    REGCMDI(Loading);
    REGCMDREQ(Log, csvr->logs);
    REGCMDREQI(NewGame, csvr->gameEvents);
    REGCMDREQI(LoadGame, csvr->gameEvents);
    REGCMDREQI(Sector, csvr->gameEvents);
    REGCMDREQI(Stats, csvr->gameEvents);
    REGCMDREQI(RunLog, csvr->gameEvents);
    REGCMDREQI(Beacon, csvr->gameEvents);
    REGCMDREQI(AutoSave, csvr->gameEvents);
    REGCMD(Screenshot);
}