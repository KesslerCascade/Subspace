#pragma once

#include <cx/cx.h>
#include <cx/platform/win.h>

extern int entryCmdShow;

#if defined(SUBSYSTEM_WINDOWS)
// Windows subsystem

#if defined(_UNICODE)
#define DEFINE_ENTRY_POINT                                                                               \
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrev, _In_ PWSTR cmdline, _In_ int show) { \
    entryCmdShow = show;                                                                                 \
    _entryParseArgsU16(__argc, __wargv);                                                                 \
    return entryPoint();                                                                                 \
}
#else
#define DEFINE_ENTRY_POINT                                                                               \
int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrev, _In_ PSTR cmdline, _In_ int show) {  \
    entryCmdShow = show;                                                                                 \
    _entryParseArgs(__argc, __argv);                                                                     \
    return entryPoint();                                                                                 \
}
#endif

#else
// Console subsystem

#if defined(_UNICODE)
#define DEFINE_ENTRY_POINT              \
int wmain(int argc, wchar_t *wargv[]) { \
    _entryParseArgsU16(argc, wargv);    \
    return entryPoint();                \
}
#else
#define DEFINE_ENTRY_POINT              \
int main(int argc, char *argv[]) {      \
    _entryParseArgs(argc, argv);        \
    return entryPoint();                \
}
#endif

#endif
