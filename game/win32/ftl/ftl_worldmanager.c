#include "ftl/capp.h"
#include "ftl/misc.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

// WorldManager is constructed in CApp:OnExecute
INITWRAP(WorldManager_ctor);
Symbol SYM(WorldManager_ctor) = {
    SYMNAME("WorldManager::ctor"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManagerC1Ev" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_ctor) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } } };

Symbol SYM(WorldManager_OnInit) = {
    SYMNAME("WorldManager::OnInit"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManager6OnInitEv" },
             { 0 } }
};
