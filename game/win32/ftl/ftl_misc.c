#include "ftl/capp.h"
#include "ftl/misc.h"

Symbol SYM(operator_new) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace }, { 0 } }
};

Symbol SYM(operator_delete) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_audio_trace }, { 0 } }
};

Symbol SYM(ftl_log) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace }, { 0 } }
};