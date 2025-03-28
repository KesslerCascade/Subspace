#include "ftl/functions_textlibrary.h"
#include "ftl/functions_capp.h"
#include "hook/disasmtrace.h"

INITWRAP(TextLibrary_GetText);
Symbol* SYMP(TextLibrary_GetText);
Symbol SYM(TextLibrary_GetText_2arg) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v1_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN11TextLibrary7GetTextERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
Symbol SYM(TextLibrary_GetText_3arg) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v2_trace },
             { 0 } }
};

FuncInfo* FUNCINFOP(TextLibrary_GetText);
FuncInfo FUNCINFO(TextLibrary_GetText_2arg) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, 0, true },
                { 4, ARG_PTR, 0, true },
                { 4, ARG_PTR, REG_ECX, false },
                { 4, ARG_DUMMY, 0, false } }
};
FuncInfo FUNCINFO(TextLibrary_GetText_3arg) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, 0, true },
                { 4, ARG_PTR, 0, true },
                { 4, ARG_PTR, REG_ECX, false },
                { 4, ARG_PTR, 0, true } }
};
