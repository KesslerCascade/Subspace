#include "ftl/capp.h"
#include "ftl/misc.h"
#include "ftl/scorekeeper.h"
#include "ftl/tutorialmanager.h"
#include "hook/disasmtrace.h"

Symbol SYM(TutorialManager_Tutorial) = {
    SYMNAME("TutorialManager::Tutorial"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ScoreKeeper_Save_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN15TutorialManager8TutorialE" },
             { 0 } }
};

// WorldManager is constructed in CApp:OnExecute
INITWRAP(TutorialManager_Running);
Symbol SYM(TutorialManager_Running) = {
    SYMNAME("TutorialManager::Running"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ScoreKeeper_Save_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN15TutorialManager7RunningEv" },
             { 0 } }
};
FuncInfo FUNCINFO(TutorialManager_Running) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(TutorialManager_Stop);
Symbol SYM(TutorialManager_Stop) = {
    SYMNAME("TutorialManager::Stop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace_s0 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN15TutorialManager4StopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(TutorialManager_Stop) = { .nargs   = 1,
                                            .stdcall = true,
                                            .args    = { { 4, ARG_PTR, REG_ECX, false } } };
