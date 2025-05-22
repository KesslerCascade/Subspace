#include "ftl/commandgui.h"
#include "ftl/completeship.h"
#include "ftl/misc.h"
#include "ftl/shipmanager.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

Symbol SYM(CompleteShip_shipManager_offset) = {
    SYMNAME("CompleteShip->shipManager"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_SpaceBar_trace }, { 0 } }
};

Symbol SYM(CompleteShip_OnInit) = {
    SYMNAME("CompleteShip::OnInit"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateShip_trace }, { 0 } }
};

DisasmTrace CompleteShip_OnInit_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CompleteShip_OnInit),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 22 },
             { I_CALL, .argsym = { &SYM(operator_new) } },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_EAX } },
                .argcap = { DT_CAPTURE1 } },   // ShipManager*
              { DT_OP(SKIP), .imin = 5, .imax = 14 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ShipManager::OnInit
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_OnInit) }
};
