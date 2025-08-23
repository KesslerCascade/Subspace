#include "ftl/powermanager.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(PowerManager_RestartAll);
Symbol SYM(PowerManager_RestartAll) = {
    SYMNAME("PowerManager::RestartAll"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN12PowerManager10RestartAllEv" },
             { 0 } }
};
FuncInfo FUNCINFO(PowerManager_RestartAll) = {
    .nargs   = 0,
    .stdcall = true,
    .rettype = RET_VOID
};