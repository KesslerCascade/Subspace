#include "ftl/projectilefactory.h"
#include "ftl/weaponsystem.h"
#include "hook/disasmtrace.h"

Symbol SYM(ProjectileFactory_blueprint_offset) = {
    SYMNAME("ProjectileFactory->blueprint"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WeaponSystem_PowerWeapon_trace }, { 0 } }
};

DisasmTrace ProjectileFactory_Update_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "Invalid factory type\n",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 }, { .op = DT_FINISH }

    },
    .out  = { &SYM(ProjectileFactory_Update) }
};

Symbol SYM(ProjectileFactory_Update) = {
    SYMNAME("ProjectileFactory::Update"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ProjectileFactory_Update_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN17ProjectfileFactory6UpdateEv" },
             { 0 } }
};

FuncInfo FUNCINFO(ProjectileFactory_Update) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};