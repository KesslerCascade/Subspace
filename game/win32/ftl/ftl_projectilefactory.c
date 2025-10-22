#include "ftl/projectilefactory.h"
#include "ftl/weaponsystem.h"
#include "hook/disasmtrace.h"

Symbol SYM(ProjectileFactory_blueprint_offset) = {
    SYMNAME("ProjectileFactory->blueprint"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WeaponSystem_PowerWeapon_trace }, { 0 } }
};