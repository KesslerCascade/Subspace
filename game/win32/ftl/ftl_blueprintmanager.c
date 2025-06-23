#include "ftl/blueprintmanager.h"
#include "ftl/misc.h"
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

/* Not consistent enough across versions

DisasmTrace BlueprintManager_TestBlueprints_Disasm = {
    .c    = DTRACE_STRREFS,
    .cstr = "PLAYER_SHIP",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { DT_OP(SKIP), .imin = 22, .imax = 34 },
             { I_MOV,
                .argf = { ARG_PTRSIZE, ARG_ADDR },
                .args = { { .ptrsize = 4 }, { .addr = 1 } } },
             { DT_OP(SKIP), .imin = 2, .imax = 8 },
             { I_CALL },   // CALL CompleteShip::CompleteShip
              { DT_OP(SKIP), .imin = 1, .imax = 6 },
             { I_MOV,
                .argf   = { ARG_REG },
                .args   = { { REG_ECX } },
                .argout = { 0, DT_OUT_SYM1 } },   // this = BlueprintManager_Blueprints
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 0xffffffff } } },
             { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL BlueprintManager::GetShipBlueprint
              { DT_OP(FINISH) } },
    .out  = { &SYM(BlueprintManager_Blueprints),       // DT_OUT_SYM1
              &SYM(BlueprintManager_GetShipBlueprint) }
}; */

/* Symbol SYM(BlueprintManager_Blueprints) = {
    SYMNAME("BlueprintManager::Blueprints"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &BlueprintManager_TestBlueprints_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN16BlueprintManager10BlueprintsE" },
             { 0 } }
}; */

DisasmTrace BlueprintManager_GetShipBlueprint_Disasm = {
    .c    = DTRACE_STRREFS,
    .cstr = "DEFAULT",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { I_LEA },
             { I_AND },
             { DT_OP(SKIP), .imin = 70, .imax = 150 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(random32) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(BlueprintManager_GetShipBlueprint) }
};

Symbol SYM(BlueprintManager_GetShipBlueprint) = {
    SYMNAME("BlueprintManager::GetShipBlueprint"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &BlueprintManager_GetShipBlueprint_Disasm },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN16BlueprintManager16GetShipBlueprintERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEi" },
             { 0 } }
};
FuncInfo FUNCINFO(BlueprintManager_GetShipBlueprint) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true }, { 4, ARG_INT, 0, true } },
    .rettype = RET_PTR
};
