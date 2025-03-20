#include <stdio.h>
#include <windows.h>
#include "loader/rtld.h"

// IMPORTANT: This program is bare and jumps directly into its entry point. It does
// not have the C runtime library available at startup. It does not perform CRT0
// initialization. It does not pass go.
//
// All of the runtime initialization tasks are delegated to the loaded process (the FTL
// executable). Once the hooks are in place and its execution has begun, a *limited*
// subset of MSVCRT becomes avalable through function pointers looked up at runtime.
// This prevents Bad Things from happening by not having two independent C libraries
// stepping on each other's toes.
//
// Until that initialization happens, use ONLY low-level Win32 API calls.

addr_t ftlbase = 0;

typedef (*entrypoint)();
entrypoint ftlentry;

typedef int(WINAPI* WinMain_t)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                               int nShowCmd);

int __stdcall entry()
{
    SetCurrentDirectoryA("M:\\games\\sync\\FTL");
    SetEnvironmentVariableA("USERPROFILE", "M:\\games\\sync\\FTL");

    if (AllocConsole()) {
        // FILE* temp;
        // freopen_s(&temp, "CONIN$", "r", stdin);
        // freopen_s(&temp, "CONOUT$", "w", stdout);
        // freopen_s(&temp, "CONOUT$", "w", stderr);
    }

    //    setvbuf(stdout, NULL, _IONBF, 0);
    // setvbuf(stderr, NULL, _IONBF, 0);

    ftlbase = loadExe("FTLGame.exe");
    importResources(ftlbase);
    importTLSInitializers(ftlbase);

    ftlentry = (entrypoint)crva(nthdr(ftlbase)->OptionalHeader.AddressOfEntryPoint);
    ftlentry();

    // probably will never reach this, but just in case
    ExitProcess(0);
}
