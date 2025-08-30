#include "ftl/misc.h"

#include "ftl/achievementtracker.h"
#include "ftl/capp.h"
#include "ftl/soundcontrol.h"
#include "ftl/worldmanager.h"
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

INITWRAP(AchievementTracker_ResetFlags);
Symbol SYM(AchievementTracker_ResetFlags) = {
    SYMNAME("AchievementTracker::ResetFlags"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateNewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN18AchievementTracker10ResetFlagsEv" },
             { 0 } }
};
DECLFUNC(AchievementTracker_ResetFlags);
FuncInfo FUNCINFO(AchievementTracker_ResetFlags) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

INITWRAP(AchievementTracker_SetAchievement);
Symbol SYM(AchievementTracker_SetAchievement) = {
    SYMNAME("AchievementTracker::SetAchievement"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_OnLoop_Mantis_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN18AchievementTracker14SetAchievementENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEbb" },
             { 0 } }
};
DECLFUNC(AchievementTracker_SetAchievement);
FuncInfo FUNCINFO(AchievementTracker_SetAchievement) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false },
                { 4, ARG_PTR, 0, true },
                { 4, ARG_INT, 0, true },
                { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};

DisasmTrace AchievementTracker_OnLoop_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(AchievementTracker_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 40, .imax = 60, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(SoundControl_Sounds) } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL SoundControl::PlaySoundMix
              { DT_OP(FINISH) } },
    .out  = { &SYM(SoundControl_PlaySoundMix) }
};
