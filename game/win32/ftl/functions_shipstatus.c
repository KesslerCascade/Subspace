#include "ftl/functions_shipstatus.h"
#include "ftl/functions_commandgui.h"
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
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } }
};

Symbol SYM(ShipStatus_OnRender) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderStatic_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipStatus8OnRenderEv" },
             { 0 } }
};

Symbol SYM(ShipStatus_LinkShip) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipStatus8LinkShipEP11ShipManager" },
             { 0 } }
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
