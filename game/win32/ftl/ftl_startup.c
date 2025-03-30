#include <windows.h>

#include "ftl/capp.h"
#include "ftl/osdep.h"
#include "ftl/startup.h"
#include "hook/disasmtrace.h"

INITWRAP(sil_main);
Symbol SYM(sil_main) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WinMain_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "sil_main" },
             { 0 } }
};
FuncInfo FUNCINFO(sil_main) = { .nargs = 2, .purecdecl = true };

// trace sil_man to get the CApp vtable
DisasmTrace sil_main_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(sil_main),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 40 },
             { I_LEA, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_LEA, .argf = { ARG_REG }, .args = { { REG_EDI } } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL },   // CEvent constructor
              { I_LEA, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_MOV,
                .argf   = { ARG_REG },
                .args   = { { REG_EBP } },
                .argout = { 0, DT_OUT_SYM1 } },   // offset of vtable
              { DT_OP(FINISH) } },
    .out  = { &SYM(CApp_vtable) }
};

// CApp::OnExecute is called from sil_main
DisasmTrace sil_main_CApp_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "Starting up\n",
    .ops  = { { I_MOV },
             { I_CALL },
             { I_LEA, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // result is arg1 of the CALL
              { DT_OP(FINISH) } },
    .out  = { &SYM(CApp_OnExecute) }
};