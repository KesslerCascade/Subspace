#include "ftl/shipobject.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(ShipObject_HasEquipment);
Symbol SYM(ShipObject_HasEquipment) = {
    SYMNAME("ShipObject::HasEquipment"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CheckForNewLocation_trace_2 },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN10ShipObject12HasEquipmentENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipObject_HasEquipment) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } }
};

DisasmTrace OuterHull_OnLoop_SLUG_GEL_Trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "SLUG_GEL",
    .ops  = { { DT_OP(SKIP), .imin = 2, .imax = 6 },
             { I_CALL },   // string constructor
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_LEA, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_SUB, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ShipObject::HasAugmentation
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipObject_HasAugmentation) }
};

Symbol SYM(ShipObject_HasAugmentation) = {
    SYMNAME("ShipObject::HasAugmentation"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &OuterHull_OnLoop_SLUG_GEL_Trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN10ShipObject15HasAugmentationENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipObject_HasAugmentation) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
    .rettype = RET_INT
};

DisasmTrace ShipObject_HasAugmentation_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipObject_HasAugmentation),
    .ops  = { { DT_OP(SKIP), .imin = 14, .imax = 22 },
             { I_IMUL,
                .argf   = { 0, 0, ARG_ADDR },
                .args   = { { 0 }, { 0 }, { .addr = 0x34 } },
                .argcap = { DT_CAPTURE1 } },
             { I_ADD,
                .argf   = { ARG_MATCH },
                .argcap = { DT_MATCH1 },
                .argout = { 0, DT_OUT_SYM1 } },   // ShipObject::shipInfoList
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipObject_shipInfoList) }
};

Symbol SYM(ShipObject_shipInfoList) = {
    SYMNAME("ShipObject::shipInfoList"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipObject_HasAugmentation_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipObject12shipInfoListE" },
             { 0 } }
};
