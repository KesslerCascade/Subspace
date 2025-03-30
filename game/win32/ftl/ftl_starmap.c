#include "ftl/globals.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
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
