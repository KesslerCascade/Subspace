#include "ftl/stdlib.h"

#include "ftl/blueprintmanager.h"
#include "ftl/capp.h"
#include "ftl/commandgui.h"
#include "ftl/drone.h"
#include "ftl/dronesystem.h"
#include "ftl/equipment.h"
#include "ftl/projectilefactory.h"
#include "ftl/shipmanager.h"
#include "ftl/shipobject.h"
#include "ftl/weaponsystem.h"
#include "ftl/worldmanager.h"
#include "inventory.h"

// size to 13 by default -- up to 4 weapons, 3 drones, 4 cargo, 1 overflow, 1 augment overflow
static sa_InvItem sitems;
static bool sitems_init = false;

static void invScanReset(void)
{
    if (!sitems_init) {
        saInit(&sitems, custom(opaque(InvItem), InvItem_ops), 13);
        sitems_init = true;
    }
    saClear(&sitems);
}

static void invScanAdd(InventoryType typ, const char* name, InventoryLocation loc)
{
    InvItem nitem = { .typ = typ, .loc = loc };
    strDup(&nitem.name, name);
    saPushC(&sitems, opaque, &nitem);
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

static void invScanPopulateAugmentRecurse(AugListItem* item)
{
    if (item->node.left)
        invScanPopulateAugmentRecurse((AugListItem*)item->node.left);

    if (item->key.len > 0) {
        // break out multiple of the same augmentation into seprate items
        for (int i = 0; i < item->value; i++) {
            invScanAdd(INVT_Augment, item->key.buf, INVL_Augments);
        }
    }

    if (item->node.right)
        invScanPopulateAugmentRecurse((AugListItem*)item->node.right);
}

static void invScanPopulateAugments()
{
    ShipInfo* info   = (ShipInfo*)ShipObject_shipInfoList->start;
    rb_tree* augList = info ? &info[1].augList : NULL;

    if (augList && augList->h.h.parent)
        invScanPopulateAugmentRecurse((AugListItem*)augList->h.h.parent);
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
    uintptr_t finish = min((uintptr_t)cargo->finish,
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
        invScanPopulateAugments();
        invScanPopulateCargo(gui);
        invScanPopulateOverflow(gui);
    }

    // Phase 2: process each item found
    int scount = saSize(sitems);
    for (int i = 0; i < scount; i++) {
        if (sitems.a[i].name)
            invScanProcessItem(&sitems.a[i]);
    }

    // Phase 3: anything left in the stored inventory doesn't exist anymore
    invRemoveAll();

    invClearIgnore();
}
