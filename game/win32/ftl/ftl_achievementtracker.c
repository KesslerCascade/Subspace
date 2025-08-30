#include "ftl/misc.h"

#include "ftl/achievementtracker.h"
#include "ftl/animationtracker.h"
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
    .ops  = { { DT_OP(SKIP), .imin = 5, .imax = 15 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argout = { 0, DT_OUT_SYM1 },
                .argcap = { DT_CAPTURE2 } },   // this->achievementPopup
              { DT_OP(LABEL),
                .val = 1 },   // need to come back here later as we're tracing 2 different branches
              { DT_OP(SKIP), .imin = 3, .imax = 10, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .args   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH2 } },
             { I_CALL, .argout = { DT_OUT_SYM2 } },
             { DT_OP(GOTO), .val = 1 },
             { DT_OP(SKIP), .imin = 29, .imax = 49, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(SoundControl_Sounds) } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },              // CALL SoundControl::PlaySoundMix
              { DT_OP(FINISH) } },
    .out  = { &SYM(AchievementTracker_achievementPopup_offset),   // DT_OUT_SYM1
              &SYM(AnimationTracker_Update),                      // DT_OUT_SYM2
              &SYM(SoundControl_PlaySoundMix) }
};

// alternate version for our old friends 1.6.8 - 1.6.12
DisasmTrace AchievementTracker_OnLoop_trace_2 = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(AchievementTracker_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 5, .imax = 15 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { DT_OP(LABEL),
                .val = 1 },   // need to come back here later as we're tracing 2 different branches
              { I_SUB },
             { DT_OP(SKIP), .imin = 3, .imax = 9, .flow = DT_FLOW_JMP_BOTH },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .argout = { 0, DT_OUT_SYM1 },
                .argcap = { DT_CAPTURE2, DT_MATCH1 } },   // this->achievementPopup
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .args   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH2 } },
             { I_CALL, .argout = { DT_OUT_SYM2 } },
             { DT_OP(GOTO), .val = 1 },
             { DT_OP(SKIP), .imin = 19, .imax = 39, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(SoundControl_Sounds) } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },              // CALL SoundControl::PlaySoundMix
              { DT_OP(FINISH) } },
    .out  = { &SYM(AchievementTracker_achievementPopup_offset),   // DT_OUT_SYM1
              &SYM(AnimationTracker_Update),                      // DT_OUT_SYM2
              &SYM(SoundControl_PlaySoundMix) }
};

Symbol SYM(AchievementTracker_achievementPopup_offset) = {
    SYMNAME("AchievementTracker->achievementPopup"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &AchievementTracker_OnLoop_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &AchievementTracker_OnLoop_trace_2 },
             { 0 } }
};
