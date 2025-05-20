#include "ftl/capp.h"
#include "ftl/filehelper.h"
#include "hook/disasmtrace.h"

INITWRAP(FileHelper_getUserFolder);
Symbol SYM(FileHelper_getUserFolder) = {
    SYMNAME("FileHelper::getUserFolder"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_audio_filehelper_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10FileHelper13getUserFolderB5cxx11Ev" },
             { 0 } }
};
FuncInfo FUNCINFO(FileHelper_getUserFolder) = { .nargs     = 1,
                                                .purecdecl = true,
                                                .args      = { { 4, ARG_PTR, 0, true } } };

INITWRAP(FileHelper_getSaveFile);
Symbol SYM(FileHelper_getSaveFile) = {
    SYMNAME("FileHelper::getSaveFile"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_audio_filehelper_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10FileHelper11getSaveFileB5cxx11Ev" },
             { 0 } }
};
FuncInfo FUNCINFO(FileHelper_getSaveFile) = { .nargs     = 1,
                                              .purecdecl = true,
                                              .args      = { { 4, ARG_PTR, 0, true } } };

DisasmTrace FileHelper_closeBinaryFile_trace = {
    .c    = DTRACE_STRREFS,
    .mod  = DTRACE_MOD_FUNCSTART,
    .cstr = "Failed to save savefile\n",
    .ops  = { { DT_OP(NOOP), .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_PUSH, .argf = { ARG_REG }, .args = { { REG_EBP } } },
             { DT_OP(SKIP), .imin = 10, .imax = 18, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV, .argf = { 0, ARG_ADDR }, .argstr = { 0, "continue.sav" } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(FileHelper_closeBinaryFile) }
};

INITWRAP(FileHelper_closeBinaryFile);
Symbol SYM(FileHelper_closeBinaryFile) = {
    SYMNAME("FileHelper::closeBinaryFile"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &FileHelper_closeBinaryFile_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10FileHelper15closeBinaryFileEi" },
             { 0 } }
};
FuncInfo FUNCINFO(FileHelper_closeBinaryFile) = { .nargs     = 1,
                                                  .purecdecl = true,
                                                  .args      = { { 4, ARG_INT, 0, true } } };
