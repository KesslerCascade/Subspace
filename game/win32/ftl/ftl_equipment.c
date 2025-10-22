#include "ftl/commandgui.h"
#include "ftl/equipment.h"
#include "ftl/shipmanager.h"
#include "hook/disasmtrace.h"

Symbol SYM(Equipment_Jump) = {
    SYMNAME("Equipment::Jump"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_ForceJumpComplete_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN9Equipment4JumpEv" },
             { 0 } }
};
FuncInfo FUNCINFO(Equipment_Jump) = { .nargs   = 1,
                                      .stdcall = true,
                                      .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                      .rettype = RET_VOID };

DisasmTrace Equipment_Jump_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(Equipment_Jump),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ECX }, { REG_ECX } },
                .argout = { DT_OUT_SYM1 } },   // this = this->overcapacityBox
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL },                      // virtual call
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH1 },
                .argout = { 0, DT_OUT_SYM2 } },   // this = this->overAugBox
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL },                         // virtual call
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { 0 }, { .addr = 0 } },
                .argcap = { DT_CAPTURE1 },
                .argout = { DT_OUT_SYM3 } },   // this->bOverCapacity = 0
              { DT_OP(SKIP), .imin = 0, .imax = 1 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { 0 }, { .addr = 0 } },
                .argcap = { DT_CAPTURE1 },
                .argout = { DT_OUT_SYM4 } },            // this->bOverAugCapacity = 0
              { DT_OP(FINISH) } },
    .out  = { &SYM(Equipment_overcapacityBox_offset),   // DT_OUT_SYM1
              &SYM(Equipment_overAugBox_offset),        // DT_OUT_SYM2
              &SYM(Equipment_bOverCapacity_offset),     // DT_OUT_SYM3
              &SYM(Equipment_bOverAugCapacity_offset) }
};

Symbol SYM(Equipment_overcapacityBox_offset) = {
    SYMNAME("Equipment->overcapacityBox"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Equipment_Jump_trace }, { 0 } }
};

Symbol SYM(Equipment_overAugBox_offset) = {
    SYMNAME("Equipment->overAugBox"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Equipment_Jump_trace }, { 0 } }
};

Symbol SYM(Equipment_bOverCapacity_offset) = {
    SYMNAME("Equipment->bOverCapacity"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Equipment_Jump_trace }, { 0 } }
};

Symbol SYM(Equipment_bOverAugCapacity_offset) = {
    SYMNAME("Equipment->bOverAugCapacity"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Equipment_Jump_trace }, { 0 } }
};

Symbol SYM(Equipment_AddWeapon) = {
    SYMNAME("Equipment::AddWeapon"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RunCommand_WEAPON_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name = "_ZN9Equipment9AddWeaponEPK15WeaponBlueprintbb" },
             { 0 } }
};

DisasmTrace Equipment_AddWeapon_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(Equipment_AddWeapon),
    .ops  = { { DT_OP(SKIP), .imin = 5, .imax = 15 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },           // this pointer
              { DT_OP(SKIP), .imin = 3, .imax = 9 },
             { DT_OP(LABEL), .val = 1 },              // forceCargo branch point
              { I_TEST },                              // if (!forceCargo)
              { DT_OP(SKIP), .imin = 2, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ShipManager::HasSystem
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { DT_OP(LABEL), .val = 2 },              // HasSystem branch point
              { I_TEST },                              // if (HasSystem)
              { DT_OP(SKIP), .imin = 0, .imax = 4, .flow = DT_FLOW_JMP_BOTH },
             { I_CALL,
                .argout = { DT_OUT_SYM2 },
                .argcap = { DT_CAPTURE2 } },   // CALL ShipManager::GetWeaponTotal
              { DT_OP(SKIP), .imin = 1, .imax = 5, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 0xffffffff } } },   // -1 param
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },                 // CALL ShipManager::AddWeapon

              { DT_OP(GOTO), .val = 1 },   // back to forceCargo branch point
              { I_TEST },                  // if (!forceCargo)
              { DT_OP(SKIP), .imin = 2, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { I_CALL,
                .argf   = { ARG_ADDR },
                .argcap = { DT_MATCH2 } },   // verify this is CALL ShipManager::GetWeaponTotal
              { DT_OP(SKIP), .imin = 26, .imax = 40, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { 0, ARG_PTRSIZE },
                .args   = { { 0 }, { .ptrsize = 4 } },
                .argout = { 0, DT_OUT_SYM4 } },   // this->vEquipmentBoxes
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_SAR },
             { DT_OP(SKIP), .imin = 2, .imax = 7, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV, .argf = { ARG_REG, ARG_PTRSIZE }, .args = { { REG_ECX }, { .ptrsize = 4 } } },
             { I_CALL, .argout = { DT_OUT_SYM5 } },   // CALL EquipmentBox::IsEmpty
              { I_TEST },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_HasSystem),              // DT_OUT_SYM1
              &SYM(ShipManager_GetWeaponTotal),         // DT_OUT_SYM2
              &SYM(ShipManager_AddWeapon),              // DT_OUT_SYM3
              &SYM(Equipment_vEquipmentBoxes_offset),   // DT_OUT_SYM4
              &SYM(EquipmentBox_IsEmpty) }
};

Symbol SYM(EquipmentBox_IsEmpty) = {
    SYMNAME("EquipmentBox::IsEmpty"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Equipment_AddWeapon_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12EquipmentBox7IsEmptyEv" },
             { 0 } }
};

DisasmTrace EquipmentBox_IsEmpty_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(EquipmentBox_IsEmpty),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 8 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argout = { 0, DT_OUT_SYM1 } },   // this->item
              { DT_OP(FINISH) } },
    .out  = { &SYM(EquipmentBox_item_offset) }
};

Symbol SYM(EquipmentBox_item_offset) = {
    SYMNAME("EquipmentBox->item"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &EquipmentBox_IsEmpty_trace }, { 0 } }
};

Symbol SYM(Equipment_vEquipmentBoxes_offset) = {
    SYMNAME("Equipment->vEquipmentBoxes"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Equipment_AddWeapon_trace }, { 0 } }
};