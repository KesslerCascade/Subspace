#include "ftl/commandgui.h"
#include "ftl/completeship.h"
#include "hook/disasmtrace.h"

Symbol SYM(CompleteShip_shipManager_offset) = {
    SYMNAME("CompleteShip->shipManager"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_SpaceBar_trace }, { 0 } }
};
