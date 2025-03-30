#include "ftl/capp.h"
#include "ftl/cevent.h"
#include "ftl/cfps.h"
#include "ftl/commandgui.h"
#include "ftl/globals.h"
#include "ftl/misc.h"
#include "ftl/mousecontrol.h"
#include "ftl/startup.h"
#include "ftl/textlibrary.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(CApp_OnExecute);
Symbol SYM(CApp_OnExecute) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &sil_main_CApp_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp9OnExecuteEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnExecute) = { .nargs   = 1,
                                      .stdcall = true,
                                      .args    = { { 4, ARG_PTR, REG_ECX, false } } };

DisasmTrace CApp_OnExecute_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CApp_OnExecute),
    .ops  = { { DT_OP(SKIP), .imin = 12, .imax = 22 },
             { I_SUB, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL CApp::OnInit
              { I_TEST, .argf = { ARG_REG, ARG_REG }, .args = { { REG_AL }, { REG_AL } } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CApp_OnInit) }
};

INITWRAP(CApp_OnInit);
Symbol SYM(CApp_OnInit) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp6OnInitEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnInit) = { .nargs   = 1,
                                   .stdcall = true,
                                   .args    = { { 4, ARG_PTR, REG_ECX, false } } };

// The "Generating world..." section of CApp::OnExecute has a LOT of references to find
DisasmTrace CApp_OnExecute_worldgen_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "Generating world...\n",
    .ops  = { { I_MOV },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ftl_log
              { DT_OP(SKIP), .imin = 1, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL operator new
              { I_MOV,                                 // after calling new, the register that the
                .argf   = { ARG_IGNORE, ARG_REG },     // WorldManager instance is copied from
                .args   = { { 0 }, { REG_EAX } },      // to varies between game versions,
                .argcap = { CT_CAPTURE1 } },           // so capture it to check later.
              { I_MOV,   // The result from ::new always goes into EAX because it's
                .argf = { ARG_REG, ARG_REG },   // the return value, and must always be moved
                .args = { { REG_ECX },          //  into ECX prior to world manager constructor,
                          { REG_EAX } } },      //  as it uses thiscall convention.
              { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL WorldManager::WorldManager()
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_IGNORE, ARG_MATCH },
                .argcap = { CT_CAPTURE2, CT_MATCH1 } },   // Capture the WorldManager offset in CApp
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_IGNORE, ARG_MATCH },
                .argstr = { 0, "Loading achievements...\n" } },
             { DT_OP(SKIP), .imin = 7, .imax = 40 },   // steam version does a bunch more in between
              { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, CT_MATCH2 } },          // Match 1 is the WorldManager pointer
              { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL WorldManager::OnInit
              { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, CT_MATCH2 } },        // Match 1 is the WorldManager pointer
              { I_CALL },                            // CALL WorldManager::CreateCommandGui
              { I_MOV,                               // MOV [*+off], EAX
                .argf   = { ARG_IGNORE, ARG_REG },   // this is the offset of capp->gui
                .args   = { { 0 }, { REG_EAX } },
                .argout = { DT_OUT_SYM5 } },
             { DT_OP(SKIP), .imin = 3, .imax = 7 },
             { I_MOV, .argf = { ARG_IGNORE, ARG_MATCH }, .argstr = { 0, "Running Game!\n" } },
             { I_CALL },   // CALL ftl_log
              { I_MOV,
                .argf   = { ARG_REG },
                .args   = { { REG_ESP } },
                .argout = { 0, DT_OUT_SYM6 } },   // CEvent callback
              { DT_OP(SKIP), .imin = 4, .imax = 13 },
             { I_CALL,
                .argf   = { ARG_MATCH },
                .argsym = &SYM(operator_delete) },   // cross-reference with operator delete
              { DT_OP(SKIP), .imin = 2, .imax = 6 },
             { I_CALL,
                .argf = { ARG_REG },
                .args = { { REG_EAX } } },             // CALL CApp:OnLanguageChangeEv (virtual)
              { DT_OP(SKIP), .imin = 0, .imax = 1 },   // Steam version calls the store wrapper here
              { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } }, .argout = { 0, DT_OUT_SYM7 } },
             { I_CALL, .argout = { DT_OUT_SYM8 } },
             { I_CMP },
             { I_FSTP },
             { DT_OP(LABEL), .val = 1 },   // save this address so we can trace both paths
              { DT_OP(JMP) },               // follow the JE
              { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL, .argout = { DT_OUT_SYM9 } },   // call to CApp::OnLoop
              { I_CMP },
             { I_JZ },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL, .argout = { DT_OUT_SYM10 } },   // call to CApp::OnRender
              { DT_OP(GOTO), .val = 1 },                // go back to before the JMP
              { I_JZ },
             { I_CMP, .argf = { ARG_MATCH }, .argsym = &SYM(opt_framelimit) },
             { DT_OP(SKIP), .imin = 2, .imax = 7 },
             { I_CALL },                               // CALL input_update
              { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL, .argout = { DT_OUT_SYM11 } },   // CALL CApp::GenInputEvents
              { DT_OP(FINISH) } },
    .out  = { &SYM(ftl_log),                            // DT_OUT_SYM1
              &SYM(operator_new),                       // DT_OUT_SYM2
              &SYM(WorldManager_ctor),                  // DT_OUT_SYM3
              &SYM(WorldManager_OnInit),                // DT_OUT_SYM4
              &SYM(CApp_gui_offset),                    // DT_OUT_SYM4
              &SYM(CEvent_callback),                    // DT_OUT_SYM6
              &SYM(CFPS_FPSControl),                    // DT_OUT_SYM7
              &SYM(CFPS_GetTime),                       // DT_OUT_SYM8
              &SYM(CApp_OnLoop),                        // DT_OUT_SYM9
              &SYM(CApp_OnRender),                      // DT_OUT_SYM10
              &SYM(CApp_GenInputEvents) }
};

DisasmTrace CApp_OnExecute_audio_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "Starting audio library...\n",
    .ops  = { { DT_OP(SKIP), .imin = 12, .imax = 18 },
             { I_JZ },
             { I_MOV, .argf = { ARG_REG }, .args = { REG_ESP } },
             { DT_OP(SKIP), .imin = 0, .imax = 1 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(operator_delete) }
};

Symbol SYM(CApp_world_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace }, { 0 } }
};

Symbol SYM(CApp_gui_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace }, { 0 } }
};

Symbol SYM(CApp_OnLoop) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnLoop) = { .nargs   = 1,
                                   .stdcall = true,
                                   .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                   .rettype = RET_VOID };

Symbol SYM(CApp_vtable) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &sil_main_trace }, { 0 } }
};

Symbol SYM(CApp_vtable_OnKeyDown_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CEvent_OnEvent_InputEvent_trace }, { 0 } }
};

Symbol(SYM(CApp_OnKeyDown)) = {
    .find = { { .type   = SYMBOL_FIND_VIRTUAL,
                .vtable = &SYM(CApp_vtable),
                .offset = &SYM(CApp_vtable_OnKeyDown_offset) },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp9OnKeyDownEi" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnKeyDown) = {
    .nargs   = 2,
    .stdcall = true, // thiscall is essentially stdcall with ECX
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } }
};

Symbol(SYM(CApp_GenInputEvents)) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp14GenInputEventsEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_GenInputEvents) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } } };

Symbol SYM(CApp_OnRender) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp8OnRenderEv" },
             { 0 } }
};

DisasmTrace CApp_OnKeyDown_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CApp_OnKeyDown),
    .ops  = { { DT_OP(SKIP), .imin = 35, .imax = 45 },
             { I_CALL, .argcap = { CT_CAPTURE1 } },
             { DT_OP(SKIP), .imin = 3, .imax = 10 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = &SYM(operator_delete) },
             { DT_OP(SKIP),
                .imin = 10,
                .imax = 50 },   // steam version reorders this with the MainMenu::OnKeyDown call
              { I_CALL, .argf = { ARG_MATCH }, .argcap = { CT_MATCH1 } },
             { DT_OP(SKIP), .imin = 3, .imax = 10 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = &SYM(operator_delete) },
             { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { DT_OP(LABEL), .val = 1 },
             { DT_OP(JMP) },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL CommandGui::SpaceBar
              { DT_OP(GOTO), .val = 1 },
             { I_JZ },                                // the JE that we followed earlier
              { I_MOVZX },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL CommandGui::KeyDown
              { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_KeyDown), &SYM(CommandGui_SpaceBar) }
};

DisasmTrace CApp_GenInputEvents_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CApp_GenInputEvents),
    .ops  = { { DT_OP(SKIP), .imin = 20, .imax = 25 },
             { I_CMP },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_EAX } }, .argcap = { 0, CT_CAPTURE1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 10 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_EAX } },
                .argcap = { 0, CT_MATCH1 } },
             { DT_OP(SKIP), .imin = 3, .imax = 10 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .disp = 8 } } },
             { I_MOV, .argf = { ARG_ADDR }, .args = { { .disp = 4 } } },
             { I_MOV, .argf = { ARG_ADDR }, .args = { { .disp = -9 } } },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_SetPaused) }
};

DisasmTrace CApp_OnRender_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CApp_OnRender),
    // TODO: Make this better and more comprehensive
    .ops  = { { DT_OP(SKIP), .imin = 45, .imax = 60 },
             { I_CMP },
             { I_JZ },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CMP },
             { DT_OP(LABEL), .val = 1 },   // save location to come back here later
              { DT_OP(JMP) },               // follow JE
              { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(GOTO), .val = 1 },   // Go back to the JE
              { DT_OP(SKIP), .imin = 7, .imax = 20 },
             { I_CALL },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL, .argout = { DT_OUT_SYM2 } },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL },
             { I_CMP },
             { I_JNZ },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_RenderStatic), &SYM(MouseControl_OnRender) }
};

// Trace for version of the code with 2-argument TextLibrary::GetText
DisasmTrace CApp_OnInit_v1_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CApp_OnInit),
    .ops  = { { DT_OP(SKIP), .imin = 30, .imax = 40 },
             { I_MOV, .argf = { 0, ARG_MATCH }, .argstr = { 0, "vsync" } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL },   // CALL graphics_set_display_attr
              { I_LEA },
             { I_LEA },
             { I_MOV,
                .argf   = { ARG_REG },
                .args   = { { REG_ESP } },
                .argout = { DT_OUT_SYM2, DT_OUT_SYM1 } },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_CALL, .argout = { DT_OUT_SYM3 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(Globals_Library),
             &SYM(Globals_Library_currentLanguage),   // gets assigned a 0 just so it's resolved
              &SYM(TextLibrary_GetText_2arg) }
};

// Trace for version of the code with 3-argument TextLibrary::GetText
DisasmTrace CApp_OnInit_v2_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CApp_OnInit),
    .ops  = { { DT_OP(SKIP), .imin = 30, .imax = 40 },
             { I_MOV, .argf = { 0, ARG_MATCH }, .argstr = { 0, "vsync" } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL },   // CALL graphics_set_display_attr
              { I_LEA },
             { I_LEA },
             { I_MOV,
                .argf   = { ARG_REG },
                .args   = { { REG_ESP } },
                .argout = { 0, DT_OUT_SYM2 } },   // current language (arg3, out of order)
              { I_MOV, .argf = { ARG_REG }, .args = { { REG_ESP } }, .argout = { 0, DT_OUT_SYM1 } },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_MOV },
             { I_CALL, .argout = { DT_OUT_SYM3 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(Globals_Library),
             &SYM(Globals_Library_currentLanguage),
             &SYM(TextLibrary_GetText_3arg) }
};

DisasmTrace CApp_OnLoop_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CApp_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 8, .imax = 16 },
             { I_MOV, .argf = { 0, ARG_MATCH }, .argsym = { 0, &SYM(CFPS_FPSControl) } },
             { DT_OP(SKIP), .imin = 8, .imax = 18 },
             { I_CMP },
             { DT_OP(JMP) },   // follow the JNE
              { I_MOV },
             { I_CALL,
                .argcap = { CT_CAPTURE1 },
                .argout = { DT_OUT_SYM1 } },   // CALL CommandGui::IsPaused
              { DT_OP(SKIP), .imin = 7, .imax = 16 },
             { I_CALL,
                .argf   = { ARG_MATCH },
                .argcap = { CT_MATCH1 } },   // CALL CommandGui::IsPaused
              { DT_OP(SKIP), .imin = 7, .imax = 16 },
             { I_CMP },
             { I_JA },
             { DT_OP(JMPTBL), .val = 0 },   // switch(), case 0
              { DT_OP(SKIP), .imin = 6, .imax = 14 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CApp_gui_offset) } },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CommandGui_IsGameOver
              { DT_OP(SKIP), .imin = 2, .imax = 7 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CApp_gui_offset) } },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CommandGui_Restart
              { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_IsPaused), &SYM(CommandGui_IsGameOver), &SYM(CommandGui_Restart) }
};
