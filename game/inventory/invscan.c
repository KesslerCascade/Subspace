#include "ftl/stdlib.h"

#include "ftl/blueprintmanager.h"
#include "ftl/capp.h"
#include "ftl/commandgui.h"
#include "ftl/drone.h"
#include "ftl/dronesystem.h"
#include "ftl/equipment.h"
#include "ftl/projectilefactory.h"
#include "ftl/shipmanager.h"
#include "ftl/weaponsystem.h"
#include "ftl/worldmanager.h"
#include "invscan.h"

// up to 4 weapons, 3 drones, 4 cargo, 1 overflow, 1 augment overflow
#define SCAN_SLOTS 13
static InvItem sitems[SCAN_SLOTS];
static int scount;

static void invScanReset(void)
{
    for (int i = 0; i < scount; i++) {
        if (sitems[i].name)
            free((void*)sitems[i].name);
        sitems[i].name = NULL;
        sitems[i].typ  = 0;
        sitems[i].loc  = 0;
    }
    scount = 0;
}

static void invScanAdd(InventoryType typ, const char* name, InventoryLocation loc)
{
    if (scount >= SCAN_SLOTS)
        return;

    sitems[scount].name = strdup(name);
    sitems[scount].typ  = typ;
    sitems[scount].loc  = loc;
    scount++;
}

static void invScanPopulateWeapons(ShipManager* sm)
{
    WeaponSystem* ws = ShipManager_weaponSystem(sm);
    vector* weapons  = ws ? WeaponSystem_weapons(ws) : NULL;
    if (!weapons)
        return;

    for (ProjectileFactory** pf = (ProjectileFactory**)weapons->start;
         pf < (ProjectileFactory**)weapons->finish;
         pf++) {
        WeaponBlueprint* bp = *pf ? ProjectileFactory_blueprint(*pf) : NULL;
        basic_string* wname = bp ? Blueprint_name(bp) : NULL;
        if (wname)
            invScanAdd(INVT_Weapon, wname->buf, INVL_Weapons);
    }
}

static void invScanPopulateDrones(ShipManager* sm) {
    DroneSystem* ds = ShipManager_droneSystem(sm);
    vector* drones  = ds ? DroneSystem_drones(ds) : NULL;
    if (!drones)
        return;

    for (Drone** d = (Drone**)drones->start; d < (Drone**)drones->finish; d++) {
        DroneBlueprint* bp  = *d ? Drone_blueprint(*d) : NULL;
        basic_string* dname = bp ? Blueprint_name(bp) : NULL;
        if (dname)
            invScanAdd(INVT_Drone, dname->buf, INVL_Drones);
    }
}

static void invScanPopulateEquipBox(EquipmentBox* eb, InventoryLocation loc)
{
    EquipmentBoxItem* item = eb ? EquipmentBox_item(eb) : NULL;
    if (item) {
        if (item->pWeapon) {
            WeaponBlueprint* bp = ProjectileFactory_blueprint(item->pWeapon);
            basic_string* wname = bp ? Blueprint_name(bp) : NULL;
            if (wname)
                invScanAdd(INVT_Weapon, wname->buf, loc);
        } else if (item->pDrone) {
            DroneBlueprint* bp  = Drone_blueprint(item->pDrone);
            basic_string* dname = bp ? Blueprint_name(bp) : NULL;
            if (dname)
                invScanAdd(INVT_Drone, dname->buf, loc);
        } else if (item->augment) {
            basic_string* aname = Blueprint_name(item->augment);
            if (aname)
                invScanAdd(INVT_Augment, aname->buf, loc);
        }
    }
}

static void invScanPopulateCargo(CommandGui* gui)
{
    Equipment* eq = CommandGui_equipScreen(gui);
    vector* cargo = eq ? Equipment_vEquipmentBoxes(eq) : NULL;
    if (!cargo)
        return;

    int cargoid      = Equipment_cargoId(eq);
    uintptr_t start  = (uintptr_t)cargo->start + Equipment_cargoId(eq) * sizeof(void*);
    uintptr_t finish = MIN((uintptr_t)cargo->finish,
                           (uintptr_t)start + 4 * sizeof(void*));   // max 4 cargo
    for (EquipmentBox** eb = (EquipmentBox**)start; eb < (EquipmentBox**)finish; eb++) {
        invScanPopulateEquipBox(*eb, INVL_Cargo);
    }
}

static void invScanPopulateOverflow(CommandGui* gui)
{
    Equipment* eq               = CommandGui_equipScreen(gui);
    EquipmentBox* overflowBox   = eq ? Equipment_overcapacityBox(eq) : NULL;
    AugmentEquipBox* overaugBox = eq ? Equipment_overAugBox(eq) : NULL;

    invScanPopulateEquipBox(overflowBox, INVL_Overflow);
    invScanPopulateEquipBox((EquipmentBox*)overaugBox, INVL_Overflow);
}

void invScanProcessItem(InvItem* item)
{
    // check if it's where it was last
    InventoryLocation loc = invFindAndIgnore(item->typ, item->name, item->loc);
    if (loc == item->loc) {
        // still there
        return;
    } else {
        // try to move it from somewhere else
        if (invMove(item->typ, item->name, INVL_Any, item->loc)) {
            loc = invFindAndIgnore(item->typ, item->name, item->loc);   // mark it as ignored
            return;
        } else {
            // not found, must be new
            invAdd(item->typ, item->name, item->loc);
            loc = invFindAndIgnore(item->typ, item->name, item->loc);   // mark it as ignored
        }
    }
}

void invScan(void)
{
    invScanReset();

    // Phase 1: populate scan list from actual ship inventory
    CommandGui* gui     = theApp ? CApp_gui(theApp) : NULL;
    WorldManager* world = theApp ? CApp_world(theApp) : NULL;
    CompleteShip* cship = world ? WorldManager_playerShip(world) : NULL;
    ShipManager* sm     = cship ? CompleteShip_shipManager(cship) : NULL;
    if (sm) {
        invScanPopulateWeapons(sm);
        invScanPopulateDrones(sm);
        invScanPopulateCargo(gui);
    }

    // Phase 2: process each item found
    for (int i = 0; i < scount; i++) {
        if (sitems[i].name)
            invScanProcessItem(&sitems[i]);
    }

    // Phase 3: anything left in the stored inventory doesn't exist anymore
    invRemoveAll();

    invClearIgnore();
}
