#include "ftl/globals.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
#include "ftl/starmap.h"
#include "hook/disasmtrace.h"

DisasmTrace StarMap_OnRender_sector_title_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "sector_title",
    .ops  = { { DT_OP(SKIP), .imin = 14, .imax = 25 },
             { I_MOVSS },
             { I_PXOR },
             { I_CVTSI2SS },
             { I_MOVSS },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 2, .imax = 6 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = { &SYM(operator_delete) } },
             { DT_OP(SKIP), .imin = 4, .imax = 8 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = { &SYM(operator_delete) } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV, .argf = { 0, ARG_MATCH }, .argstr = { 0, "sector_legend_civilian" } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(freetype_easy_print) }
};

DisasmTrace StarMap_StartSecretSector_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "ACH_ROCK_CRYSTAL",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { DT_OP(NOOP), .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 8, .imax = 18 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 },
                .argout = { 0, DT_OUT_SYM2 } },   // get secretSector
              { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX }, { .addr = 1 } },
                .argout = { DT_OUT_SYM3 } },   // set bSecretSector
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH1 },
                .argout = { DT_OUT_SYM4 } },   // set currentSector to secretSector
              { DT_OP(FINISH) } },
    .out  = { &SYM(StarMap_StartSecretSector),
             &SYM(StarMap_secretSector_offset),
             &SYM(StarMap_bSecretSector_offset),
             &SYM(StarMap_currentSector_offset) }
};

Symbol SYM(StarMap_StartSecretSector) = {
    SYMNAME("StarMap::StartSecretSector"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_StartSecretSector_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN7StarMap17StartSecretSectorEv" },
             { 0 } }
};
FuncInfo FUNC(StarMap_StartSecretSector) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

Symbol SYM(StarMap_currentSector_offset) = {
    SYMNAME("StarMap->currentSector"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_StartSecretSector_trace }, { 0 } }
};

Symbol SYM(StarMap_bSecretSector_offset) = {
    SYMNAME("StarMap->bSecretSector"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_StartSecretSector_trace }, { 0 } }
};
Symbol SYM(StarMap_secretSector_offset) = {
    SYMNAME("StarMap->secretSector"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_StartSecretSector_trace }, { 0 } }
};

// wow, how incredibly convenient that this is hardcoded
DisasmTrace Disasm_GenerateMap_Rock_Home_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "ROCK_HOME",
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_LEA, .argf = { ARG_REG }, .args = { { REG_ECX } }, .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(Sector_description_type_offset) }
};

Symbol SYM(Sector_description_type_offset) = {
    SYMNAME("Sector->description.type"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Disasm_GenerateMap_Rock_Home_trace }, { 0 } }
};
