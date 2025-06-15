#include "ftl/commandgui.h"
#include "ftl/gameover.h"
#include "hook/disasmtrace.h"

Symbol SYM(GameOver_SetVictory) = {
    SYMNAME("GameOver::SetVictory"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8GameOver10SetVictoryEbNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt6vectorIS5_SaIS5_EE" },
             { 0 } }
};
FuncInfo FUNCINFO(GameOver_SetVictory) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false },
                { 4, ARG_INT, 0, true },
                { 4, ARG_PTR, 0, true },
                { 4, ARG_PTR, 0, true } },
    .rettype = RET_VOID
};

Symbol SYM(GameOver_OpenText) = {
    SYMNAME("GameOver::OpenText"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8GameOver8OpenTextENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(GameOver_OpenText) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
    .rettype = RET_VOID
};
