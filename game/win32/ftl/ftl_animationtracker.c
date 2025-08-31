#include "ftl/achievementtracker.h"
#include "ftl/animationtracker.h"
#include "hook/disasmtrace.h"

INITWRAP(AnimationTracker_Update);
Symbol SYM(AnimationTracker_Update) = {
    SYMNAME("AnimationTracker::Update"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &AchievementTracker_OnLoop_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &AchievementTracker_OnLoop_trace_2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN16AnimationTracker6UpdateEv" },
             { 0 } }
};
DECLFUNC(AnimationTracker_Update);
FuncInfo FUNCINFO(AnimationTracker_Update) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

DisasmTrace AnimationTracker_Update_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(AnimationTracker_Update),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 10 },
             { I_CMP,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX }, { .addr = 0 } },
                .argout = { DT_OUT_SYM1 } },   // if (this->running == false)
              { DT_OP(FINISH) } },
    .out  = { &SYM(AnimationTracker_running_offset) }
};

Symbol SYM(AnimationTracker_running_offset) = {
    SYMNAME("AnimationTracker->running"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &AnimationTracker_Update_trace }, { 0 } }
};

void AnimationTracker_done_calc(addr_t base, Symbol* sym, SymbolFind* find)
{
    // it's always 2 after AnimationTracker->running
    if (!symResolve(base, AnimationTracker_running_offset))
        return;

    sym->addr     = SYM(AnimationTracker_running_offset).addr + 2;
    sym->resolved = true;
}

Symbol SYM(AnimationTracker_done_offset) = {
    SYMNAME("AnimationTracker->done"),
    .find = { { .type = SYMBOL_FIND_CUSTOM, .func = AnimationTracker_done_calc }, { 0 } }
};
