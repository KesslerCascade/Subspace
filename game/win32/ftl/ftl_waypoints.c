#include "ftl/waypoints.h"
#include "ftl/capp.h"
#include "hook/disasmtrace.h"

Symbol SYM(wp_CApp_OnLoop_GetCommand_switch) = { SYMNAME("wp:CApp_OnLoop_GetCommand_switch"),
                                                 .find = { 
                                                    { .type = SYMBOL_FIND_DISASM, .disasm  = &CApp_OnLoop_trace },
                                                    { 0 } } };

Symbol SYM(wp_CApp_OnLoop_MenuMenu_handler) = {
    SYMNAME("wp:CApp_OnLoop_MenuMenu_handler"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace }, { 0 } }
};
