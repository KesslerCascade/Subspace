#include "ftl/shipgraph.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(ShipGraph_Restart);
Symbol SYM(ShipGraph_Restart) = {
    SYMNAME("ShipGraph::Restart"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN9ShipGraph7RestartEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipGraph_Restart) = {
    .nargs   = 0,
    .stdcall = true,
    .rettype = RET_VOID
};