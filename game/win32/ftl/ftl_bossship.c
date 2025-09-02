#include "ftl/bossship.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(BossShip_StartStage);
Symbol SYM(BossShip_StartStage) = {
    SYMNAME("BossShip::StartStage"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateShip_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8BossShip10StartStageEv" },
             { 0 } }
};
DECLFUNC(BossShip_StartStage);
FuncInfo FUNCINFO(BossShip_StartStage) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                           .rettype = RET_VOID };

Symbol SYM(BossShip_currentStage_offset) = {
    SYMNAME("BossShip->currentStage"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_OnLoop_trace }, { 0 } }
};
