#include "ftl/blueprintmanager.h"
#include "ftl/shipmanager.h"
#include "ftl/struct.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

static void ShipBlueprint_name_offset_find(addr_t addr, Symbol* sym, SymbolFind* find)
{
    // This is really annoying to find "raw", because the blueprint structure is usually embedded in
    // something else.

    // Instead, calculate the offset based on easier to find offsets in ShipManager.

    if (symResolve(addr, ShipManager_myBlueprint_offset) &&
        symResolve(addr, ShipManager_myBlueprint_name_offset)) {
        sym->addr = SYM(ShipManager_myBlueprint_name_offset).addr -
            SYM(ShipManager_myBlueprint_offset).addr;
        sym->resolved = true;
    }
}

Symbol SYM(ShipBlueprint_name_offset) = {
    SYMNAME("ShipBlueprint->name"),
    .find = { { .type = SYMBOL_FIND_CUSTOM, .func = ShipBlueprint_name_offset_find }, { 0 } }
};

static void ShipBlueprint_blueprintName_offset_find(addr_t addr, Symbol* sym, SymbolFind* find)
{
    // see comments in ShipBlueprint_name_offset_find
    if (symResolve(addr, ShipManager_myBlueprint_offset) &&
        symResolve(addr, ShipManager_myBlueprint_blueprintName_offset)) {
        sym->addr = SYM(ShipManager_myBlueprint_blueprintName_offset).addr -
            SYM(ShipManager_myBlueprint_offset).addr;
        sym->resolved = true;
    }
}

Symbol SYM(ShipBlueprint_blueprintName_offset) = {
    SYMNAME("ShipBlueprint->blueprintName"),
    .find = { { .type = SYMBOL_FIND_CUSTOM, .func = ShipBlueprint_blueprintName_offset_find },
             { 0 } }
};
