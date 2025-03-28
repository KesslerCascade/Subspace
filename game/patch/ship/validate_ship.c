#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ship/ship.h"
#include "shipmanager/shipmanager.h"

// Not actually a patch, just a validator to insert into patch sequences and make sure the Ship
// structure is known

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, ShipManager_ship_offset) && symResolve(base, Ship_hullIntegrity_offset);
}

Patch validate_Ship = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = NoPatch };
