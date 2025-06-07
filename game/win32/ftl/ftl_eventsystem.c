#include "ftl/eventsystem.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

Symbol SYM(EventSystem_EventManager) = {
    SYMNAME("EventSystem::EventManager"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_PauseLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11EventSystem12EventManagerE" },
             { 0 } }
};

INITWRAP(EventSystem_PollEvent);
Symbol SYM(EventSystem_PollEvent) = {
    SYMNAME("EventSystem::PollEvent"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_PauseLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11EventSystem9PollEventEi" },
             { 0 } }
};
FuncInfo FUNCINFO(EventSystem_PollEvent) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } },
    .rettype = RET_INT
};
