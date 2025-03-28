#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "shipstatus/shipstatus.h"

// Not actually a patch, just a validator to insert into patch sequences and make sure the Ship
// structure is known

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, ShipStatus_ship_offset);
}

Patch validate_ShipStatus = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = NoPatch };
