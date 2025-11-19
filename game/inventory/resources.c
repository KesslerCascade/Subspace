#include "resources.h"
#include "control/runlog.h"
#include "ftl/capp.h"
#include "ftl/shipmanager.h"
#include "ftl/worldmanager.h"
#include "subspacegame.h"

static int fuel     = 0;
static int missiles = 0;
static int drones   = 0;

void resourceReset(void)
{
    fuel     = 0;
    missiles = 0;
    drones   = 0;
}

void resourceScan(void)
{
    WorldManager* world      = CApp_world(theApp);
    CompleteShip* playerShip = world ? WorldManager_playerShip(world) : NULL;
    ShipManager* sm          = playerShip ? CompleteShip_shipManager(playerShip) : NULL;

    if (sm) {
        int nfuel     = ShipManager_fuel_count(sm);
        int nmissiles = ShipManager_GetMissileCount(sm);
        int ndrones   = ShipManager_GetDroneCount(sm);

        if (nfuel != fuel && !gc.loadingGame) {
            runLogSend(&Log_Resource,
                       gc.curResourceSource ? gc.curResourceSource : _S"Other",
                       _S"Fuel",
                       nfuel - fuel,
                       nfuel);
        }

        if (nmissiles != missiles && !gc.loadingGame) {
            runLogSend(&Log_Resource,
                       gc.curResourceSource ? gc.curResourceSource : _S"Other",
                       _S"Missiles",
                       nmissiles - missiles,
                       nmissiles);
        }

        if (ndrones != drones && !gc.loadingGame) {
            runLogSend(&Log_Resource,
                       gc.curResourceSource ? gc.curResourceSource : _S"Other",
                       _S"Drones",
                       ndrones - drones,
                       ndrones);
        }

        fuel     = nfuel;
        missiles = nmissiles;
        drones   = ndrones;
    }
}
