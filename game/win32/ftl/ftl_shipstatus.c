#include "ftl/commandgui.h"
#include "ftl/graphics/csurface.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "hook/disasmtrace.h"

INITWRAP(ShipStatus_RenderHealth);
Symbol SYM(ShipStatus_RenderHealth) = {
    SYMNAME("ShipStatus::RenderHealth"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderStatic_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipStatus12RenderHealthEb" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipStatus_RenderHealth) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};

INITWRAP(ShipStatus_OnRender);
Symbol SYM(ShipStatus_OnRender) = {
    SYMNAME("ShipStatus::OnRender"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderStatic_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipStatus8OnRenderEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipStatus_OnRender) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                           .rettype = RET_VOID };

Symbol SYM(ShipStatus_LinkShip) = {
    SYMNAME("ShipStatus::LinkShip"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipStatus8LinkShipEP11ShipManager" },
             { 0 } }
};

DisasmTrace ShipStatus_OnRender_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipStatus_OnRender),
    .ops  = { { DT_OP(SKIP), .imin = 16, .imax = 26 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = { &SYM(ShipStatus_RenderHealth) } },
             { DT_OP(SKIP), .imin = 2, .imax = 8 },
             { I_MOV,
                .argf = { ARG_REG, ARG_MATCH },
                .args = { { REG_ESP },
                          { REG_UNDEF, .idx = REG_UNDEF, .addr = 0 } } },   // MOV DWORD PTR [ESP], 0
              { I_CALL, .argout = { DT_OUT_SYM1 } },   // CAL ShipStatus::RenderEvadeOxygen
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipStatus_RenderEvadeOxygen) }
};

Symbol SYM(ShipStatus_RenderEvadeOxygen) = {
    SYMNAME("ShipStatus::RenderEvadeOxygen"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipStatus17RenderEvadeOxygenEb" },
             { 0 } }
};

DisasmTrace ShipStatus_RenderEvadeOxygen_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipStatus_RenderEvadeOxygen),
    // normally a skip this big would be bad, but these are literally the only 2 places in the
    // entire codebase that these two constants appear
    .ops  = { { DT_OP(SKIP), .imin = 200, .imax = 1000 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0x42c60000 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0x42a40000 } } },
             { DT_OP(SKIP), .imin = 4, .imax = 12 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0x42c60000 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0x42a40000 } } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CSurface_GL_SetColor) }
};

DisasmTrace ShipStatus_LinkShip_trace_v1 = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipStatus_LinkShip),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ESP } },
                .argcap = { DT_CAPTURE1 } },   // ShipManager pointer
              { DT_OP(SKIP), .imin = 5, .imax = 11 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE3 } },   // this
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_SUB, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { DT_CAPTURE2, DT_MATCH1 } },   // get ShipManager pointer
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH2 },
                .argout = { DT_OUT_SYM1 } },           // store it, this is our offset
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL ShipManager::GetDroneCount
              { DT_OP(SKIP), .imin = 1, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL ShipManager::GetMissileCount
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { DT_CAPTURE4, DT_MATCH2 },
                .argout = { 0, DT_OUT_SYM4 } },   // ship->currentScrap
              { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .argcap = { DT_MATCH3, DT_MATCH4 } },   // this->lastScrap = ship->currentScrap
              { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { 0, DT_MATCH2 },
                .argout = { 0, DT_OUT_SYM5 } },        // ship->myBlueprint.blueprintName
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipStatus_ship_offset),            // DT_OUT_SYM1
              &SYM(ShipManager_GetDroneCount),         // DT_OUT_SYM2
              &SYM(ShipManager_GetMissileCount),       // DT_OUT_SYM3
              &SYM(ShipManager_currentScrap_offset),   // DT_OUT_SYM4
              &SYM(ShipManager_myBlueprint_blueprintName_offset) }
};

// alternate version with compiler reordered instructions in some versions
DisasmTrace ShipStatus_LinkShip_trace_v2 = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipStatus_LinkShip),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ESP } },
                .argcap = { DT_CAPTURE1 } },   // ShipManager pointer
              { DT_OP(SKIP), .imin = 5, .imax = 11 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE3 } },   // this
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_SUB, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { DT_CAPTURE2, DT_MATCH1 } },   // get ShipManager pointer
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH2 },
                .argout = { DT_OUT_SYM1 } },           // store it, this is our offset
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL ShipManager::GetDroneCount
              { DT_OP(SKIP), .imin = 1, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL ShipManager::GetMissileCount
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { DT_CAPTURE4, DT_MATCH2 },
                .argout = { 0, DT_OUT_SYM4 } },   // ship->currentScrap
              { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { 0, DT_MATCH2 },
                .argout = { 0, DT_OUT_SYM5 } },   // ship->myBlueprint.blueprintName
              { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .argcap = { DT_MATCH3, DT_MATCH4 } },   // this->lastScrap = ship->currentScrap
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipStatus_ship_offset),             // DT_OUT_SYM1
              &SYM(ShipManager_GetDroneCount),          // DT_OUT_SYM2
              &SYM(ShipManager_GetMissileCount),        // DT_OUT_SYM3
              &SYM(ShipManager_currentScrap_offset),    // DT_OUT_SYM4
              &SYM(ShipManager_myBlueprint_blueprintName_offset) }
};

Symbol SYM(ShipStatus_ship_offset) = {
    SYMNAME("ShipStatus->ship"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v2 },
             { 0 } }
};
