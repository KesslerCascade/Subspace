#include "ftl/commandgui.h"
#include "ftl/completeship.h"
#include "ftl/misc.h"
#include "ftl/shipmanager.h"
#include "ftl/shipobject.h"
#include "ftl/soundcontrol.h"
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

INITWRAP(CompleteShip_DeadCrew);
Symbol SYM(CompleteShip_DeadCrew) = {
    SYMNAME("CompleteShip::DeadCrew"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { 0 } }
};
FuncInfo FUNCINFO(CompleteShip_DeadCrew) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_INT
};

DisasmTrace CompleteShip_InitiateTeleport_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(ShipObject_HasEquipment),
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { DT_OP(NOOP), .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 15, .imax = 25, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 },
                .argout = { 0, DT_OUT_SYM2 } },   // this->enemyShip
              { I_TEST, .args = { ARG_MATCH, ARG_MATCH }, .argcap = { DT_MATCH1, DT_MATCH1 } },
             { DT_OP(SKIP), .imin = 20, .imax = 50, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x656c6574 } } },   // tele
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x74726f70 } } },   // port
              { DT_OP(SKIP), .imin = 0, .imax = 10 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(SoundControl_PlaySoundMix) } },
             { DT_OP(SKIP), .imin = 5, .imax = 20, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x454c4554 } } },   // TELE
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x54524f50 } } },   // PORT
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x4145485f } } },   // _HEA
              { DT_OP(SKIP), .imin = 0, .imax = 10 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(ShipObject_HasEquipment) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CompleteShip_InitiateTeleport),   // DT_OUT_SYM1
              &SYM(CompleteShip_enemyShip_offset) }
};

INITWRAP(CompleteShip_InitiateTeleport);
Symbol SYM(CompleteShip_InitiateTeleport) = {
    SYMNAME("CompleteShip::InitiateTeleport"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CompleteShip_InitiateTeleport_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12CompleteShip16InitiateTeleportEii" },
             { 0 } }
};
FuncInfo FUNCINFO(CompleteShip_InitiateTeleport) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};

Symbol SYM(CompleteShip_enemyShip_offset) = {
    SYMNAME("CompleteShip->enemyOffset"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CompleteShip_InitiateTeleport_trace },
             { 0 } }
};
