#include "control/cmds.h"
#include "feature/featureregistry.h"
#include "run/runinfo.h"

void cmdNewGame(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    strref shiptype = cfieldString(fields, _S"ship");
    strref shipname = cfieldString(fields, _S"name");
    int seed        = cfieldValD(int32, fields, _S"seed", 0);
    int difficulty  = cfieldValD(int32, fields, _S"difficulty", 0);

    RunInfo* nrun = runinfoCreate(inst->ss);
    ginstSetRun(inst, nrun);
    runinfoNewGame(nrun, seed, shiptype, shipname, difficulty);
    objRelease(&nrun);
}
