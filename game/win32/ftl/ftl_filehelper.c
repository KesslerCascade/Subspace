#include "ftl/capp.h"
#include "ftl/filehelper.h"
#include "hook/disasmtrace.h"

INITWRAP(FileHelper_getUserFolder);
Symbol SYM(FileHelper_getUserFolder) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_audio_filehelper_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10FileHelper13getUserFolderB5cxx11Ev" },
             { 0 } }
};
FuncInfo FUNCINFO(FileHelper_getUserFolder) = { .nargs     = 1,
                                                .purecdecl = true,
                                                .args      = { { 4, ARG_PTR, 0, true } } };

INITWRAP(FileHelper_getSaveFile);
Symbol SYM(FileHelper_getSaveFile) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_audio_filehelper_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10FileHelper11getSaveFileB5cxx11Ev" },
             { 0 } }
};
FuncInfo FUNCINFO(FileHelper_getSaveFile) = { .nargs     = 1,
                                              .purecdecl = true,
                                              .args      = { { 4, ARG_PTR, 0, true } } };
