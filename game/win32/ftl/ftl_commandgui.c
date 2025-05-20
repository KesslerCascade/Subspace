#include "ftl/capp.h"
#include "ftl/combatcontrol.h"
#include "ftl/commandgui.h"
#include "ftl/completeship.h"
#include "ftl/ftlbutton.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "hook/disasmtrace.h"

INITWRAP(CommandGui_KeyDown);
Symbol SYM(CommandGui_KeyDown) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnKeyDown_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui7KeyDownEib" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_KeyDown) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } }
};

INITWRAP(CommandGui_SetPaused);
Symbol SYM(CommandGui_SetPaused) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_GenInputEvents_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui9SetPausedEbb" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_SetPaused) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } }
};

INITWRAP(CommandGui_RenderStatic);
Symbol SYM(CommandGui_RenderStatic) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui12RenderStaticEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_RenderStatic) = { .nargs   = 1,
                                               .stdcall = true,
                                               .args    = { { 4, ARG_PTR, REG_ECX, false } } };

Symbol SYM(CommandGui_SpaceBar) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnKeyDown_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui8SpaceBarEv" },
             { 0 } }
};

DisasmTrace CommandGui_SpaceBar_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CommandGui_SpaceBar),
    .ops  = { { DT_OP(SKIP), .imin = 3, .imax = 7 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 },
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 2, .imax = 5 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { DT_CAPTURE2, DT_MATCH1 },
                .argout = { 0, DT_OUT_SYM2 } },
             { I_MOV, .argf = { 0, ARG_REG }, .argcap = { DT_CAPTURE3, DT_MATCH2 } },
             { I_MOV, .argf = { 0, ARG_REG }, .argcap = { DT_CAPTURE4, DT_MATCH3 } },
             { I_CMP, .argf = { ARG_REG }, .argcap = { DT_MATCH4 }, .argout = { 0, DT_OUT_SYM3 } },
             { I_JNZ },
             { I_MOVZX, .argout = { 0, DT_OUT_SYM4 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_shipComplete_offset),    // DT_OUT_SYM1
              &SYM(CompleteShip_shipManager_offset),   // DT_OUT_SYM2
              &SYM(ShipManager_GetIsJumping),
             &SYM(ShipManager_current_target_offset) }
};

Symbol SYM(CommandGui_shipComplete_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_SpaceBar_trace }, { 0 } }
};

DisasmTrace CommandGui_RenderStatic_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CommandGui_RenderStatic),
    // For now, just check every CALL. This trace is very much a brute force and a big TODO is to
    // replace it with something better.
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 500 },
             { I_LEA, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { DT_OP(LABEL), .val = 1 },
             { DT_OP(CALL) },
             { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 0, .imax = 200 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x5f6c7466 } } },   // "ftl_"
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x76697264 } } },   // "driv"
              { DT_OP(SKIP), .imin = 15, .imax = 30 },
             { I_CVTSI2SS },
             { I_MOVSS },
             { I_CALL, .argout = { DT_OUT_SYM2 } },
             { DT_OP(GOTO), .val = 1 },   // go back to RenderStatic
              { DT_OP(SKIP), .imin = 0, .imax = 500 },
             { I_MOVZX,
                .argf   = { 0, ARG_ADDR },
                .argsym = { 0, &SYM(ShipManager_current_target_offset) } },
             { I_TEST },
             { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { DT_OP(LABEL), .val = 2 },
             { DT_OP(JMP) },   // first follow part where current target is set
              { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL CombatControl::OnRenderCombat
              { DT_OP(GOTO), .val = 2 },               // go back to first part
              { DT_OP(SKIP), .imin = 0, .imax = 500 },
             { DT_OP(CALL) },
             { I_PUSH, .outip = DT_OUT_SYM4 },   // CALL ShipStatus::OnRender
              { DT_OP(SKIP), .imin = 0, .imax = 30 },
             { DT_OP(CALL) },
             { I_PUSH, .outip = DT_OUT_SYM5 },   // CALL ShipStatus::RenderHealth
              { DT_OP(SKIP), .imin = 0, .imax = 50 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x74617473 } } },   // "stat"
              { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x685f7375 } } },   // "us_h" (ull)
              { DT_OP(FINISH) } },
    .out  = { &SYM(FTLButton_OnRender),
             &SYM(freetype_easy_printCenter),
             &SYM(CombatControl_OnRenderCombat),
             &SYM(ShipStatus_OnRender),
             &SYM(ShipStatus_RenderHealth) }
};

DisasmTrace CommandGui_RunCommand_HULL_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "HULL ", // yes, with a space at the end
    .ops  = { { DT_OP(SKIP), .imin = 20, .imax = 28 },
             { I_NEG, .argcap = { DT_CAPTURE1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ESP } },
                .argcap = { 0, DT_MATCH1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // ShipManager::DamageHull
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_DamageHull) }
};

DisasmTrace CommandGui_RunCommand_DELETE_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "DELETE",
    .ops  = { { DT_OP(SKIP), .imin = 1, .imax = 7 },
             { I_JNZ },
             { DT_OP(SKIP), .imin = 2, .imax = 8 },
             { I_CALL, .argcap = { DT_CAPTURE1 } },   // CALL CombatControl::GetCurrentTarget
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_JZ },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // is called twice
              { DT_OP(FINISH) } },
    .out  = { &SYM(CombatControl_GetCurrentTarget) }
};

INITWRAP(CommandGui_IsPaused);
Symbol SYM(CommandGui_IsPaused) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui8IsPausedEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_IsPaused) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(CommandGui_IsGameOver);
Symbol SYM(CommandGui_IsGameOver) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui10IsGameOverEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_IsGameOver) = { .nargs   = 1,
                                             .stdcall = true,
                                             .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(CommandGui_Restart);
Symbol SYM(CommandGui_Restart) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui7RestartEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_Restart) = { .nargs   = 1,
                                          .stdcall = true,
                                          .args    = { { 4, ARG_PTR, REG_ECX, false } } };

DisasmTrace CommandGui_Restart_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CommandGui_Restart),
    .ops  = { { DT_OP(SKIP), .imin = 7, .imax = 17 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // remember which reg holds this
              { DT_OP(SKIP), .imin = 15, .imax = 30 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },      // somewhere it gets set back as
                .args   = { { REG_ECX } },             // ECX for a CALL, because CommandGui
                .argcap = { 0, DT_MATCH1 } },          // starts with embedded ShipManager
              { DT_OP(SKIP), .imin = 2, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ShipStatus::LinkShipManager
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipStatus_LinkShip) }
};

INITWRAP(CommandGui_OnLoop);
Symbol SYM(CommandGui_OnLoop) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_OnLoop) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                         .rettype = RET_VOID };
