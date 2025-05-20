#include "ftl/ftlbutton.h"
#include "ftl/commandgui.h"
#include "hook/disasmtrace.h"

INITWRAP(FTLButton_OnRender);
Symbol SYM(FTLButton_OnRender) = {
    SYMNAME("FTLButton::OnRender"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderStatic_trace }, { 0 } }
};
FuncInfo FUNCINFO(FTLButton_OnRender) = { .nargs   = 1,
                                          .stdcall = true,
                                          .args    = { { 4, ARG_PTR, REG_ECX, false } } };
