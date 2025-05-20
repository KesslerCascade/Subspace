#include "ftl/ship.h"
#include "ftl/shipmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(Ship_DamageHull);
Symbol SYM(Ship_DamageHull) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_DamageHull_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4Ship10DamageHullEi" },
             { 0 } }
};
FuncInfo FUNCINFO(Ship_DamageHull) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } }
};

DisasmTrace Ship_DamageHull_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(Ship_DamageHull),
    .ops  = { { DT_OP(SKIP), .imin = 2, .imax = 8 },
             { I_MOV,                      // capture1 = register math is being done on
                .argf   = { 0, ARG_REG },   // capture2 = candidate offset for hullIntegrity
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1, DT_CAPTURE2 } },
             { DT_OP(SKIP), .imin = 2, .imax = 10 },
             { I_SUB,
                .argf   = { ARG_REG },
                .argcap = { DT_MATCH1 } },   // looking for a sub on math reg
              { DT_OP(SKIP), .imin = 2, .imax = 10 },
             { I_MOV,                       // and storing back in the same offset
                .argf   = { ARG_ADDR, ARG_REG },
                .argcap = { DT_MATCH2, DT_MATCH1 },
                .argout = { DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(Ship_hullIntegrity_offset) }
};

Symbol SYM(Ship_hullIntegrity_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Ship_DamageHull_trace }, { 0 } }
};
