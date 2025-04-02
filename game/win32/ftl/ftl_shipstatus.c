#include "ftl/commandgui.h"
#include "ftl/graphics/csurface.h"
#include "ftl/shipstatus.h"
#include "hook/disasmtrace.h"

INITWRAP(ShipStatus_RenderHealth);
Symbol SYM(ShipStatus_RenderHealth) = {
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
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderStatic_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipStatus8OnRenderEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipStatus_OnRender) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                           .rettype = RET_VOID };

Symbol SYM(ShipStatus_LinkShip) = {
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

DisasmTrace ShipStatus_LinkShip_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipStatus_LinkShip),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_LEA,
                .argf = { 0, ARG_REG },
                .args = { { 0 }, { REG_ESP }, }, .argcap = { CT_CAPTURE1 } },
             { DT_OP(SKIP), .imin = 6, .imax = 12 },
             { I_SUB, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { CT_CAPTURE2, CT_MATCH1 } },   // get ShipManager pointer
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ECX } },
                .argcap = { 0, CT_MATCH2 },
                .argout = { DT_OUT_SYM1 } },   // store it, this is our offset
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipStatus_ship_offset) }
};

Symbol SYM(ShipStatus_ship_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace }, { 0 } }
};
