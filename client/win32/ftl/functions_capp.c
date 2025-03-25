#include "ftl/functions_capp.h"
#include "ftl/functions_cfps.h"
#include "ftl/functions_misc.h"
#include "ftl/functions_worldmanager.h"
#include "hook/disasmfind.h"

// CApp::OnExecute is called from sil_main
INITWRAP(CApp_OnExecute);
DisasmFind CApp_OnExecute_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Starting up\n",
    .ops        = { { .inst = I_MOV },
                   { .inst = I_CALL },
                   { .inst = I_LEA, .argfilter = { ARG_REG }, .args = { { .base = REG_ECX } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },   // result is arg1 of the CALL
                    { DISASM_FINISH } }
};
Symbol SYM(CApp_OnExecute) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp9OnExecuteEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnExecute) = { .nargs = 1, .args = { { 4, ARG_PTR, REG_ECX, false } } };

// Find the offset of "WorldManager *world" within CApp, it's stored right after being
// constructed
DisasmFind CApp_world_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Generating world...\n",
    .ops        = { { .type = DISASM_SKIP, .imin = 1, .imax = 8 },
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
                      .argcap = { 0, CT_MATCH0 },   // the register captured above is stored into the CApp
                                                    // instance
                      .mark   = MARK_ARG1 },   // result is arg1 of the MOV (actually the disp value)
                    { DISASM_FINISH } }
};
Symbol SYM(CApp_world_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_world_Disasm }, { 0 } }
};

// Find the offset of "CommandGui *gui" within CApp
DisasmFind CApp_gui_Disasm = {
    .candidates = DISASM_SEARCH_CALL,
    .sym        = &SYM(WorldManager_OnInit),
    .ops        = { { .inst = I_CALL },
                   { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { { .base = REG_ECX } } },
                   { .inst = I_CALL },
                   { .inst      = I_MOV,
                      .argfilter = { ARG_IGNORE, ARG_REG },
                      .args      = { { 0 }, { .base = REG_EAX } },
                      .mark      = MARK_ARG1 },   // Result is the offset from CApp base
                    { DISASM_FINISH } }
};
Symbol SYM(CApp_gui_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_gui_Disasm }, { 0 } }
};

DisasmFind CApp_OnLoop_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Running Game!\n",
    .ops        = { { DISASM_SKIP, .imin = 14, .imax = 20 },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = &SYM(CFPS_GetTime) },
                   { .inst = I_CMP },
                   { .inst = I_FSTP },
                   { DISASM_JMP },                          // follow the JE
                    { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { { .base = REG_ECX } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },   // call to OnLoop
                    { DISASM_FINISH } }
};
Symbol SYM(CApp_OnLoop) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnLoop) = { .nargs = 1, .args = { { 4, ARG_PTR, REG_ECX, false } } };
