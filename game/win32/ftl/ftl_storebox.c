#include "ftl/achievementtracker.h"
#include "ftl/shipmanager.h"
#include "ftl/soundcontrol.h"
#include "ftl/storebox.h"
#include "hook/disasmtrace.h"

DisasmTrace StoreBox_Purchase_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(ShipManager_ModifyScrapCount),
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 0, .imax = 20 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x7562 } } },   // 62 75 = 'bu' from 'buy'
              { DT_OP(SKIP), .imin = 0, .imax = 24 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(SoundControl_PlaySoundMix) } },
             { DT_OP(SKIP), .imin = 8, .imax = 19 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x726f7473 } } },   // 'stor' from 'store_purchase'
              { DT_OP(SKIP), .imin = 2, .imax = 10 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },           // CALL AchievementTracker::SetFlag
              { DT_OP(SKIP), .imin = 6, .imax = 18 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(ShipManager_ModifyScrapCount) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(StoreBox_Purchase),   // DT_OUT_SYM1
              &SYM(AchievementTracker_SetFlag) }
};

Symbol SYM(StoreBox_Purchase) = {
    SYMNAME("StoreBox::Purchase"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StoreBox_Purchase_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8StoreBox8PurchaseEv" },
             { 0 } }
};

FuncInfo FUNCINFO(StoreBox_Purchase) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                         .rettype = RET_VOID };

DisasmTrace ItemStoreBox_Purchase_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(ShipManager_AddItem),
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 18, .imax = 28 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(SoundControl_PlaySoundMix) } },
             { DT_OP(SKIP), .imin = 8, .imax = 18 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x726f7473 } } },   // 'stor' from 'store_purchase'
              { DT_OP(SKIP), .imin = 2, .imax = 10 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },           // CALL AchievementTracker::SetFlag
              { DT_OP(SKIP), .imin = 6, .imax = 18 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(ShipManager_ModifyScrapCount) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ItemStoreBox_Purchase) }
};

// alternate version for builds (1.6.8, 1.6.9) that don't inline the StoreBox::Purchase call
DisasmTrace ItemStoreBox_Purchase_trace_2 = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(ShipManager_AddItem),
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 7, .imax = 15 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(StoreBox_Purchase) } },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(ShipManager_AddItem) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ItemStoreBox_Purchase) }
};

Symbol SYM(ItemStoreBox_Purchase) = {
    SYMNAME("ItemStoreBox::Purchase"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ItemStoreBox_Purchase_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &ItemStoreBox_Purchase_trace_2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12ItemStoreBox8PurchaseEv" },
             { 0 } }
};

FuncInfo FUNCINFO(ItemStoreBox_Purchase) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};
