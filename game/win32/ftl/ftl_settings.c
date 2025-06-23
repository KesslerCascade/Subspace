#include "ftl/settings.h"
#include "ftl/scorekeeper.h"
#include "hook/disasmtrace.h"

Symbol SYM(Settings_GetDlcEnabled) = {
    SYMNAME("Settings::GetDlcEnabled"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ScoreKeeper_Save_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8Settings13GetDlcEnabledEv" },
             { 0 } }
};
FuncInfo FUNCINFO(Settings_GetDlcEnabled) = {
    .nargs   = 0,
    .purecdecl = true,
    .rettype = RET_INT
};
