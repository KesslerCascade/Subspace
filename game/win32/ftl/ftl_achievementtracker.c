#include "ftl/misc.h"

#include "ftl/capp.h"
#include "ftl/achievementtracker.h"
#include "hook/disasmtrace.h"

Symbol SYM(AchievementTracker_Tracker) = {
    SYMNAME("AchievementTracker::Tracker") ,
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN18AchievementTracker7TrackerE" },
             { 0 } }
};


INITWRAP(AchievementTracker_OnLoop);
Symbol SYM(AchievementTracker_OnLoop) = {
    SYMNAME("AchievementTracker::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN18AchievementTracker6OnLoopEv" },
             { 0 } }
};
DECLFUNC(AchievementTracker_OnLoop);
FuncInfo FUNCINFO(AchievementTracker_OnLoop) = { .nargs   = 1,
                                   .stdcall = true,
                                   .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                   .rettype = RET_VOID };
