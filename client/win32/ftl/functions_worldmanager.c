#include "ftl/functions_worldmanager.h"
#include "ftl/functions_capp.h"
#include "ftl/functions_misc.h"
#include "hook/disasmfind.h"

// WorldManager is constructed in CApp:OnExecute
INITWRAP(WorldManager_ctor);
DisasmFind WorldManager_ctor_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Generating world...\n",
    .ops        = { { DISASM_SKIP, .imin = 1, .imax = 8 },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = { &SYM(operator_new) } },
                   { DISASM_SKIP, .imin = 0, .imax = 8 },
                   { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { { .base = REG_ECX } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },   // result is arg1 of the CALL
                    { DISASM_FINISH } }
};
Symbol SYM(WorldManager_ctor) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_ctor_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManagerC1Ev" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_ctor) = { .nargs = 1, .args = { { 4, ARG_PTR, REG_ECX, false } } };

DisasmFind WorldManager_OnInit_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Generating world...\n",
    .ops        = { { DISASM_SKIP, .imin = 1, .imax = 8 },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = { &SYM(operator_new) } },
                   // after calling new, the register that the WorldManager instance is copied from EAX to
                    // varies between game versions, so capture it to check later
                    { .inst      = I_MOV,
                      .argfilter = { ARG_IGNORE, ARG_REG },
                      .args      = { { 0 }, { .base = REG_EAX } },
                      .argcap    = { CT_CAPTURE0 } },
                   { .inst      = I_MOV,
                      .argfilter = { ARG_REG, ARG_REG },
                      .args      = { { .base = REG_ECX }, { .base = REG_EAX } } },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = { &SYM(WorldManager_ctor) } },
                   { DISASM_SKIP, .imin = 0, .imax = 4 },
                   { .inst      = I_MOV,
                      .argfilter = { ARG_IGNORE, ARG_MATCH },
                      .argcap = { CT_CAPTURE1, CT_MATCH0 } },   // Capture the WorldManager offset in CApp
                    { .type = DISASM_SKIP, .imin = 0, .imax = 4 },
                   { .inst      = I_MOV,
                      .argfilter = { ARG_IGNORE, ARG_MATCH },
                      .argstr    = { 0, "Loading achievements...\n" } },
                   { DISASM_SKIP, .imin = 7, .imax = 40 },   // steam version does a bunch more in between
                    { .inst      = I_MOV,
                      .argfilter = { ARG_REG, ARG_MATCH },
                      .args      = { { .base = REG_ECX } },
                      .argcap    = { 0, CT_MATCH1 } },   // Match 1 is the WorldManager pointer
                    { .inst = I_CALL,
                      .mark = { MARK_ARG1 } },        // this is the call to WorldManager::OnInit
                    { .inst      = I_MOV,
                      .argfilter = { ARG_REG, ARG_MATCH },
                      .args      = { { .base = REG_ECX } },
                      .argcap    = { 0, CT_MATCH1 } },   // Match 1 is the WorldManager pointer
                    { DISASM_FINISH } }
};
Symbol SYM(WorldManager_OnInit) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_OnInit_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManager6OnInitEv" },
             { 0 } }
};
