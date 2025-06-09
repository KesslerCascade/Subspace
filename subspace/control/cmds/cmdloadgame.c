#include "control/cmds.h"
#include "feature/featureregistry.h"
#include "run/runinfo.h"

void cmdLoadGame(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    strref shiptype = cfieldString(fields, _S"ship");
    strref shipname = cfieldString(fields, _S"name");
    int seed        = cfieldValD(int32, fields, _S"seed", 0);
    int difficulty  = cfieldValD(int32, fields, _S"difficulty", 0);
    int beacons  = cfieldValD(int32, fields, _S"beacons", 0);

    RunInfo* nrun = runinfoCreate(inst->ss);
    ginstSetRun(inst, nrun);
    runinfoLoadGame(nrun, seed, shiptype, shipname, difficulty, beacons);
    objRelease(&nrun);
}
