#include "ftl/misc.h"

#include "ftl/capp.h"
#include "ftl/cfps.h"

// WorldManager construction is a convenient place to find the address of 'operator new' because
// it's called immediately after a specific log message.
Symbol SYM(CFPS_FPSControl) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS10FPSControlE" },
             { 0 } }
};

Symbol SYM(CFPS_GetTime) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS7GetTimeEv" },
             { 0 } }
};
