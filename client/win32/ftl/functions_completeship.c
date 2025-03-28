#include "ftl/functions_commandgui.h"
#include "ftl/functions_completeship.h"
#include "hook/disasmtrace.h"

Symbol SYM(CompleteShip_shipManager_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_SpaceBar_trace }, { 0 } }
};
