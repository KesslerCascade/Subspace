#include "ftl/combatcontrol.h"
#include "ftl/commandgui.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
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
FuncInfo FUNCINFO(CombatControl_RenderTarget) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

INITWRAP(CombatControl_GetCurrentTarget);
Symbol SYM(CombatControl_GetCurrentTarget) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RunCommand_DELETE_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN13CombatControl16GetCurrentTargetEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CombatControl_GetCurrentTarget) = { .nargs   = 1,
                                                      .stdcall = true,
                                                      .args = { { 4, ARG_PTR, REG_ECX, false } } };

DisasmTrace CombatControl_RenderTarget_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CombatControl_RenderTarget),
    .ops  = { { DT_OP(SKIP), .imin = 250, .imax = 1500 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x69676e65 } } },   // engi
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x5f73656e } } },   // nes_
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x6b636168 } } },   // hack
              { DT_OP(SKIP), .imin = 10, .imax = 40 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = { &SYM(operator_delete) } },
             { DT_OP(SKIP), .imin = 2, .imax = 7 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = { &SYM(operator_delete) } },
             { DT_OP(SKIP), .imin = 0, .imax = 10, .flow = DT_FLOW_JMP_UNCOND },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 7 } } },   // selecting font 7
              { DT_OP(SKIP), .imin = 4, .imax = 15 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },        // CALL easy_printRightAlign
              { DT_OP(FINISH) } },
    .out  = { &SYM(freetype_easy_printRightAlign) }
};
