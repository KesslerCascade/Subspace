#include "ftl/functions_mousecontrol.h"
#include "ftl/functions_capp.h"

INITWRAP(MouseControl_OnRender);
Symbol SYM(MouseControl_OnRender) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12MouseControl8OnRenderEv" },
             { 0 } }
};
FuncInfo FUNCINFO(MouseControl_OnRender) = { .nargs   = 1,
                                             .stdcall = true,
                                             .args    = { { 4, ARG_PTR, REG_ECX, false } } };
