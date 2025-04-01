#pragma once

#include <windows.h>
#include "ftl/ftl.h"
#include "hook/function.h"

typedef int (*FUNCTYPE(WinMain))(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                                 int nShowCmd);
DECLFUNC(WinMain);
#define FTL_WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd) \
    FCALL(ftlbase, WinMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd)

extern DisasmTrace WinMain_trace;
extern DisasmTrace WinMain_fallback_trace;