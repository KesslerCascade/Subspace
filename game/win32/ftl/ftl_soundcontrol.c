#include "ftl/gameover.h"
#include "ftl/soundcontrol.h"
#include "hook/disasmtrace.h"

Symbol SYM(SoundControl_Sounds) = {
    SYMNAME("SoundControl::Sounds"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &GameOver_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12SoundControl6SoundsE" },
             { 0 } }
};

INITWRAP(SoundControl_StartPlaylist);
Symbol SYM(SoundControl_StartPlaylist) = {
    SYMNAME("SoundControl::StartPlaylist"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &GameOver_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN12SoundControl13StartPlaylistERKSt6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS6_EE" },
             { 0 } }
};
FuncInfo FUNCINFO(SoundControl_StartPlaylist) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
    .rettype = RET_VOID
};
