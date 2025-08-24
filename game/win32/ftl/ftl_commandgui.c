#include "ftl/capp.h"
#include "ftl/combatcontrol.h"
#include "ftl/commandgui.h"
#include "ftl/completeship.h"
#include "ftl/filehelper.h"
#include "ftl/ftlbutton.h"
#include "ftl/gameover.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
#include "ftl/scorekeeper.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "ftl/starmap.h"
#include "ftl/tutorialmanager.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(CommandGui_KeyDown);
Symbol SYM(CommandGui_KeyDown) = {
    SYMNAME("CommandGui::KeyDown"),
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
    SYMNAME("CommandGui::SetPaused"),
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
    SYMNAME("CommandGui::RenderStatic"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui12RenderStaticEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_RenderStatic) = { .nargs   = 1,
                                               .stdcall = true,
                                               .args    = { { 4, ARG_PTR, REG_ECX, false } } };

Symbol SYM(CommandGui_SpaceBar) = {
    SYMNAME("CommandGui::SpaceBar"),
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
    SYMNAME("CommandGui->shipComplete"),
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

DisasmTrace CommandGui_RunCommand_SHIP_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "SHIP ",
    .ops  = { { DT_OP(SKIP), .imin = 1, .imax = 20, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV, .argf = { 0, ARG_ADDR }, .argstr = { 0, "ALL" } },
             { DT_OP(NOUNWIND) },
             { DT_OP(SKIP), .imin = 1, .imax = 22, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(ScoreKeeper_Keeper) } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL },                              // CALL ScoreKeeper::GetShipId
              { DT_OP(SKIP), .imin = 0, .imax = 12 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ScoreKeeper::UnlockShip
              { DT_OP(FINISH) } },
    .out  = { &SYM(ScoreKeeper_UnlockShip) }
};

INITWRAP(CommandGui_IsPaused);
Symbol SYM(CommandGui_IsPaused) = {
    SYMNAME("CommandGui::IsPaused"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui8IsPausedEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_IsPaused) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(CommandGui_IsGameOver);
Symbol SYM(CommandGui_IsGameOver) = {
    SYMNAME("CommandGui::IsGameOver"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace_s0 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui10IsGameOverEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_IsGameOver) = { .nargs   = 1,
                                             .stdcall = true,
                                             .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(CommandGui_Restart);
Symbol SYM(CommandGui_Restart) = {
    SYMNAME("CommandGui::Restart"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace_s0 },
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
    SYMNAME("CommandGui::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_OnLoop) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                         .rettype = RET_VOID };

INITWRAP(CommandGui_OnInit);
Symbol SYM(CommandGui_OnInit) = {
    SYMNAME("CommandGui::OnInit"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_OnInit_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui6OnInitEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_OnInit) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                         .rettype = RET_VOID };

INITWRAP(CommandGui_LinkMap);
Symbol SYM(CommandGui_LinkMap) = {
    SYMNAME("CommandGui::LinkMap"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_OnInit_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui7LinkMapEP7StarMap" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_LinkMap) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
    .rettype = RET_VOID
};

INITWRAP(CommandGui_GetCommand);
Symbol SYM(CommandGui_GetCommand) = {
    SYMNAME("CommandGui::GetCommand"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui10GetCommandEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_GetCommand) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_INT
};

INITWRAP(CommandGui_IsJumpComplete);
Symbol SYM(CommandGui_IsJumpComplete) = {
    SYMNAME("CommandGui::IsJumpComplete"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CheckForNewLocation_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui14IsJumpCompleteEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_IsJumpComplete) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_INT
};

INITWRAP(CommandGui_ForceJumpComplete);
Symbol SYM(CommandGui_ForceJumpComplete) = {
    SYMNAME("CommandGui::ForceJumpComplete"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CheckForNewLocation_trace_2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui17ForceJumpCompleteEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_ForceJumpComplete) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

INITWRAP(CommandGui_CanSave);
Symbol SYM(CommandGui_CanSave) = {
    SYMNAME("CommandGui::CanSave"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_PrepareAutoSave_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui7CanSaveEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_CanSave) = { .nargs   = 1,
                                          .stdcall = true,
                                          .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                          .rettype = RET_INT };

DisasmTrace CommandGui_CheckGameOver_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "gameover_fedbase",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 5, .imax = 14 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CommandGui_shipComplete_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_MOV,
                .argf   = { 0, ARG_ADDR },
                .argsym = { 0, &SYM(CompleteShip_shipManager_offset) },
                .argcap = { DT_CAPTURE2 } },           // ShipManager register
              { DT_OP(SKIP), .imin = 3, .imax = 9 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL CompleteShip::DeadCrew
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CMP,
                .argf   = { ARG_PTRSIZE, ARG_ADDR },
                .args   = { { .ptrsize = 1 }, { .addr = 0 } },
                .argout = { DT_OUT_SYM3 } },   // CMP bDestroyed, 0
              { DT_OP(SKIP), .imin = 0, .imax = 3, .flow = DT_FLOW_JMP_BOTH },
             { I_LEA,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(ShipManager_ship_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL Ship::DestroyedDone
              { DT_OP(SKIP), .imin = 2, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { DT_OP(LABEL), .val = 1 },
             { I_CALL, .argout = { DT_OUT_SYM5 } },   // CALL StarMap::CheckGameOver
              { DT_OP(SKIP), .imin = 4, .imax = 8, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ESP } },               // confirm that the previous CALL is
                .argstr = { 0, "gameover_fedbase" } },   // the correct one
              { DT_OP(GOTO), .val = 1 },                 // go back to trace other path
              { DT_OP(SKIP), .imin = 3, .imax = 7, .flow = DT_FLOW_JMP_BOTH },
             // get back on the short path
              { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(TutorialManager_Running) } },
             { DT_OP(NOUNWIND) },
             { DT_OP(SKIP), .imin = 1, .imax = 5, .flow = DT_FLOW_JMP_BOTH },
             { I_CMP,
                .argf = { ARG_PTRSIZE, ARG_ADDR },
                .args = { { .ptrsize = 1 }, { .addr = 0 } } },
             { DT_OP(SKIP), .imin = 1, .imax = 3, .flow = DT_FLOW_JMP_BOTH },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .argout = { 0, DT_OUT_SYM6 },
                .argcap = { DT_CAPTURE3, DT_MATCH1 } },   // offset of gameOverScreen
              { DT_OP(SKIP), .imin = 3, .imax = 7 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_EBP } },
                .argcap = { DT_CAPTURE4, DT_MATCH3 } },   // local variable for gameOverScreen
              { DT_OP(SKIP), .imin = 4, .imax = 12 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH3 } },          // this = gameOverScreen
              { DT_OP(SKIP), .imin = 2, .imax = 8 },
             { I_CALL, .argout = { DT_OUT_SYM7 } },   // CALL GameOver::SetVictory
              { DT_OP(SKIP), .imin = 15, .imax = 25, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH4 } },          // this = gameOverScreen (local)
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM8 } },   // CALL GameOver::OpenText
              { DT_OP(SKIP), .imin = 5, .imax = 10 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = &SYM(FileHelper_deleteAllSaveFiles) },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_CheckGameOver),           // DT_OUT_SYM1
              &SYM(CompleteShip_DeadCrew),              // DT_OUT_SYM2
              &SYM(ShipManager_bDestroyed_offset),      // DT_OUT_SYM3
              &SYM(Ship_DestroyedDone),                 // DT_OUT_SYM4
              &SYM(StarMap_CheckGameOver),              // DT_OUT_SYM5
              &SYM(CommandGui_gameOverScreen_offset),   // DT_OUT_SYM6
              &SYM(GameOver_SetVictory),                // DT_OUT_SYM7
              &SYM(GameOver_OpenText) }
};

// alternate ordering that some older version use
DisasmTrace CommandGui_CheckGameOver_trace_2 = {
    .c    = DTRACE_STRREFS,
    .cstr = "gameover_fedbase",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 5, .imax = 14 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CommandGui_shipComplete_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_MOV,
                .argf   = { 0, ARG_ADDR },
                .argsym = { 0, &SYM(CompleteShip_shipManager_offset) },
                .argcap = { DT_CAPTURE2 } },           // ShipManager register
              { DT_OP(SKIP), .imin = 3, .imax = 9 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL CompleteShip::DeadCrew
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CMP,
                .argf   = { ARG_PTRSIZE, ARG_ADDR },
                .args   = { { .ptrsize = 1 }, { .addr = 0 } },
                .argout = { DT_OUT_SYM3 } },   // CMP bDestroyed, 0
              { DT_OP(SKIP), .imin = 0, .imax = 3, .flow = DT_FLOW_JMP_BOTH },
             { I_LEA,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(ShipManager_ship_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL Ship::DestroyedDone
              { DT_OP(SKIP), .imin = 2, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { DT_OP(LABEL), .val = 1 },
             { I_CALL, .argout = { DT_OUT_SYM5 } },   // CALL StarMap::CheckGameOver
              { DT_OP(SKIP), .imin = 4, .imax = 8, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ESP } },               // confirm that the previous CALL is
                .argstr = { 0, "gameover_fedbase" } },   // the correct one
              { DT_OP(GOTO), .val = 1 },                 // go back to trace other path
              { DT_OP(SKIP), .imin = 3, .imax = 7, .flow = DT_FLOW_JMP_BOTH },
             // get back on the short path
              { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(TutorialManager_Running) } },
             { DT_OP(NOUNWIND) },
             { DT_OP(SKIP), .imin = 1, .imax = 5, .flow = DT_FLOW_JMP_BOTH },
             { I_CMP,
                .argf = { ARG_PTRSIZE, ARG_ADDR },
                .args = { { .ptrsize = 1 }, { .addr = 0 } } },
             { DT_OP(SKIP), .imin = 1, .imax = 15, .flow = DT_FLOW_JMP_BOTH },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .argout = { 0, DT_OUT_SYM6 },
                .argcap = { DT_CAPTURE3, DT_MATCH1 } },   // offset of gameOverScreen
              { DT_OP(SKIP), .imin = 1, .imax = 8 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_EBP } },
                .argcap = { DT_CAPTURE4, DT_MATCH3 } },   // local variable for gameOverScreen
              { DT_OP(SKIP), .imin = 0, .imax = 8 },
             { I_CALL, .argout = { DT_OUT_SYM7 } },      // CALL GameOver::SetVictory
              { DT_OP(SKIP), .imin = 15, .imax = 25, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH4 } },          // this = gameOverScreen (local)
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM8 } },   // CALL GameOver::OpenText
              { DT_OP(SKIP), .imin = 5, .imax = 10 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = &SYM(FileHelper_deleteAllSaveFiles) },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_CheckGameOver),           // DT_OUT_SYM1
              &SYM(CompleteShip_DeadCrew),              // DT_OUT_SYM2
              &SYM(ShipManager_bDestroyed_offset),      // DT_OUT_SYM3
              &SYM(Ship_DestroyedDone),                 // DT_OUT_SYM4
              &SYM(StarMap_CheckGameOver),              // DT_OUT_SYM5
              &SYM(CommandGui_gameOverScreen_offset),   // DT_OUT_SYM6
              &SYM(GameOver_SetVictory),                // DT_OUT_SYM7
              &SYM(GameOver_OpenText) }
};

// can't use export for this one because the exported symbol is just a wrapper function
Symbol SYM(CommandGui_CheckGameOver) = {
    SYMNAME("CommandGui::CheckGameOver"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_CheckGameOver) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

Symbol SYM(CommandGui_gameOverScreen_offset) = {
    SYMNAME("CommandGui->gameOverScreen"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { 0 } }
};

DisasmTrace CommandGui_LinkMap_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CommandGui_LinkMap),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 16 },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } }, .argout = { DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_starMap_offset) }
};

Symbol SYM(CommandGui_starMap_offset) = {
    SYMNAME("CommandGui->starMap"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_LinkMap_trace }, { 0 } }
};

DisasmTrace CommandGui_OnLoop_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CommandGui_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 7, .imax = 13 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 200, .imax = 300 },
             { I_CMP,
                .argf = { ARG_PTRSIZE, ARG_ADDR },
                .args = { { .ptrsize = 1 }, { .addr = 0 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 3, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CommandGui_starMap_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(StarMap_StartSecretSector) } },
             { DT_OP(NOUNWIND) },
             { DT_OP(SKIP), .imin = 3, .imax = 10, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CommandGui_starMap_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL StarMap::GetNewLocation
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .argcap = { DT_MATCH1 },
                .args   = { { 0 }, { REG_EAX } },
                .argout = { DT_OUT_SYM2 } },   // this->newLocation
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CommandGui_starMap_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL StarMap::GetWaitLocation
              { DT_OP(SKIP), .imin = 0, .imax = 8 },
             { I_MOV,
                .argf   = { 0, ARG_ADDR },
                .argsym = { 0, &SYM(CommandGui_shipComplete_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL ShipManager::Wait
              { DT_OP(FINISH) } },
    .out  = { &SYM(StarMap_GetNewLocation),            // DT_OUT_SYM1
              &SYM(CommandGui_newLocation_offset),     // DT_OUT_SYM2
              &SYM(StarMap_GetWaitLocation),           // DT_OUT_SYM3
              &SYM(ShipManager_Wait) }
};

Symbol SYM(CommandGui_newLocation_offset) = {
    SYMNAME("CommandGui->newLocation"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_OnLoop_trace }, { 0 } }
};

DisasmTrace CommandGui_OnLoop_GameOverLoop_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CommandGui_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 7, .imax = 13 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 1000 },
             { I_LEA,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CommandGui_gameOverScreen_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL GameOver::OnLoop
              { DT_OP(FINISH) } },
    .out  = { &SYM(GameOver_OnLoop) }
};

INITWRAP(CommandGui_OnCleanup);
Symbol SYM(CommandGui_OnCleanup) = {
    SYMNAME("CommandGui::OnCleanup"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace_s5 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui9OnCleanupEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_OnCleanup) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

DisasmTrace CommandGui_RenderPause_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "paused_text",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_CMP, .outip = DT_OUT_SYM1 }, { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_RenderPause) }
};

Symbol SYM(CommandGui_RenderPause) = {
    SYMNAME("CommandGui::RenderPause"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderPause_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui11RenderPauseEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_RenderPause) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};