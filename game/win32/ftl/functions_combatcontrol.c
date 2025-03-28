#include "ftl/functions_combatcontrol.h"
#include "ftl/functions_commandgui.h"
#include "hook/disasmtrace.h"

INITWRAP(CombatControl_OnRenderCombat);
Symbol SYM(CombatControl_OnRenderCombat) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderStatic_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN13CombatControl14OnRenderCombatEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CombatControl_OnRenderCombat) = { .nargs   = 1,
                                                    .stdcall = true,
                                                    .args    = { { 4, ARG_PTR, REG_ECX, false } } };

DisasmTrace CombatControl_OnRenderCombat_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CombatControl_OnRenderCombat),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 18 },
             { I_TEST },
             { I_JZ },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CombatControl_RenderTarget) }
};

INITWRAP(CombatControl_RenderTarget);
Symbol SYM(CombatControl_RenderTarget) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CombatControl_OnRenderCombat_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN13CombatControl12RenderTargetEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CombatControl_RenderTarget) = { .nargs   = 1,
                                                  .stdcall = true,
                                                  .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(CombatControl_GetCurrentTarget);
Symbol SYM(CombatControl_GetCurrentTarget) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RunCommand_DELETE_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN13CombatControl16GetCurrentTargetEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CombatControl_GetCurrentTarget) = { .nargs   = 1,
                                                      .stdcall = true,
                                                      .args = { { 4, ARG_PTR, REG_ECX, false } } };
