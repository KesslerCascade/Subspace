#include "ftl/ftlbutton.h"
#include "ftl/commandgui.h"
#include "hook/disasmtrace.h"

Symbol SYM(FTLButton_OnRender) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RenderStatic_trace }, { 0 } }
};
