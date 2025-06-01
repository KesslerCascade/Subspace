#include "ftl/globals.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
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

// this one is kind of annoying to find, easiest way is to just check every call to
// measurePrintLines for the right parameters
DisasmTrace StarMap_RenderSectorName_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(freetype_easy_measurePrintLines),
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { DT_OP(NOOP), .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 10, .imax = 18 },
             { I_CMP,
                .argf   = { ARG_PTRSIZE, ARG_ADDR },
                .args   = { { .ptrsize = 1 }, { .addr = 0 } },
                .argcap = { DT_CAPTURE1 } },   // if (sector->desc.shortName.isLiteral == false)
              { DT_OP(SKIP), .imin = 1, .imax = 9, .flow = DT_FLOW_JMP_BOTH },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .argcap = { 0, DT_MATCH1 },
                .argout = { 0, DT_OUT_SYM2 } },   // &(sector->description).shortName
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL },
             { DT_OP(SKIP),
                .imin = 200,
                .imax = 500 },   // check the original measurePrintLines CALL
              { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 0x00002710 } } },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 0x40400000 } } },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 0x40A00000 } } },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 0x00000007 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL, .argf = { ARG_PTR }, .argsym = { &SYM(freetype_easy_measurePrintLines) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(StarMap_RenderSectorName), &SYM(Sector_description_shortName_offset) }
};

Symbol SYM(StarMap_RenderSectorName) = {
    SYMNAME("StarMap::RenderSectorName"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_RenderSectorName_trace },
             { .type = SYMBOL_FIND_EXPORT, "_ZN7StarMap16RenderSectorNameEP6Sector8GL_Color" },
             { 0 } }
};

Symbol SYM(Sector_description_shortName_offset) = {
    SYMNAME("Sector->description.shortName"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_RenderSectorName_trace }, { 0 } }
};

Symbol SYM(StarMap_NewGame) = {
    SYMNAME("StarMap::NewGame"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateNewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN7StarMap7NewGameEb" },
             { 0 } }
};
FuncInfo FUNC(StarMap_NewGame) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};

DisasmTrace StarMap_NewGame_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(StarMap_NewGame),
    .ops  = { { DT_OP(SKIP), .imin = 6, .imax = 14 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 20, .imax = 34 },
             { I_CMP, .argf = { ARG_ADDR }, .argsym = { &SYM(RNG_useSysRand) } },
             { DT_OP(SKIP), .imin = 0, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { DT_OP(LABEL), .val = 1 },   // remember current position because we need to trace
              { DT_OP(CALL) },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_IMUL,
                .argf = { 0, 0, ARG_ADDR },
                .args = { { 0 }, { 0 }, { .addr = 0x5851f42d } } },   // magic LCG
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_IMUL,
                .argf = { 0, 0, ARG_ADDR },
                .args = { { 0 }, { 0 }, { .addr = 0x4c957f2d } } },   // numbers
              { DT_OP(GOTO), .val = 1 },                              // go back into NewGame
              { I_CALL, .argout = { DT_OUT_SYM1 } },                  // CALL random32()
              { DT_OP(SKIP), .imin = 0, .imax = 6, .flow = DT_FLOW_JMP_UNCOND },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .argcap = { DT_MATCH1 },            // this [ +X ]
                .args   = { { 0 }, { REG_EAX } },   // random32 output
                .argout = { DT_OUT_SYM2 } },        // sectorMapSeed offset
              { DT_OP(SKIP), .imin = 3, .imax = 8 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH1 } },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL GenerateSectorMap
              { DT_OP(SKIP), .imin = 4, .imax = 10 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL GenerateMap
              { DT_OP(FINISH) } },
    .out  = { &SYM(random32),                          // DT_OUT_SYM1
              &SYM(StarMap_sectorMapSeed_offset),      // DT_OUT_SYM2
              &SYM(StarMap_GenerateSectorMap),         // DT_OUT_SYM3
              &SYM(StarMap_GenerateMap) }
};

Symbol SYM(StarMap_GenerateSectorMap) = {
    SYMNAME("StarMap::GenerateSectorMap"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_NewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN7StarMap17GenerateSectorMapEv" },
             { 0 } }
};
FuncInfo FUNC(StarMap_GenerateSectorMap) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

Symbol SYM(StarMap_GenerateMap) = {
    SYMNAME("StarMap::GenerateMap"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_NewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN7StarMap11GenerateMapEbb" },
             { 0 } }
};
FuncInfo FUNC(StarMap_GenerateMap) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } },
    .rettype = RET_PTR
};

Symbol SYM(StarMap_sectorMapSeed_offset) = {
    SYMNAME("StarMap->sectorMapSeed"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_NewGame_trace }, { 0 } }
};

DisasmTrace StarMap_GenerateMap_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(StarMap_GenerateMap),
    .ops  = { { DT_OP(SKIP), .imin = 8, .imax = 16 },
             { I_CMP,
                .argf = { ARG_PTRSIZE, ARG_ADDR },
                .args = { { .ptrsize = 1 }, { .addr = 0 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { DT_OP(LABEL), .val = 1 },
             { I_JZ },
             { I_MOV,
                .argf   = { ARG_IGNORE, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { 0, DT_CAPTURE1 },   // seed = this->currentSectorSeed
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(GOTO), .val = 1 },        // go back to JZ
              { DT_OP(JMP) },                   // follow JZ
              { DT_OP(SKIP), .imin = 1, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(random32) } },
             { DT_OP(SKIP), .imin = 1, .imax = 4, .flow = DT_FLOW_JMP_UNCOND },
             { I_MOV,
                .argf   = { ARG_ADDR, ARG_REG },   // this->currentSectorSeed = seed
                .argcap = { DT_MATCH1 },
                .args   = { { 0 }, { REG_EAX } } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(StarMap_currentSectorSeed_offset) }
};

Symbol SYM(StarMap_currentSectorSeed_offset) = {
    SYMNAME("StarMap->currentSectorSeed"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_GenerateMap_trace }, { 0 } }
};
