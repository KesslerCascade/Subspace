#include <stdio.h>
#include <windows.h>
#include "subspaceclient.h"

#include "minicrt.h"

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

typedef int(WINAPI* WinMain_t)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                               int nShowCmd);

int __stdcall entry()
{
    scsettings.gameDir     = smalloc(MAX_PATH);
    scsettings.gameProgram = smalloc(MAX_PATH);
    scsettings.gamePath    = smalloc(MAX_PATH);

#if 0
    strcpy(scsettings.gameDir, "M:\\Dev\\FTLCopy");
    strcpy(scsettings.gameProgram, "FTLGame.exe");
    strcpy(scsettings.gamePath, "M:\\Dev\\FTLCopy\\FTLGame.exe");
#elif 0
    strcpy(scsettings.gameDir, "M:\\games\\Steam\\steamapps\\common\\FTL Faster Than Light");
    strcpy(scsettings.gameProgram, "FTLGame.exe");
    strcpy(scsettings.gamePath,
           "M:\\games\\Steam\\steamapps\\common\\FTL Faster Than Light\\FTLGame.exe");
#elif 1
    strcpy(scsettings.gameDir, "M:\\Games\\Sync\\FTL");
    strcpy(scsettings.gameProgram, "FTLGame.exe");
    strcpy(scsettings.gamePath, "M:\\Games\\Sync\\FTL\\FTLGame.exe");
#endif

    SetCurrentDirectoryA(scsettings.gameDir);
    SetEnvironmentVariableA("USERPROFILE", scsettings.gameDir);

    // if (AllocConsole()) {
    //  FILE* temp;
    //  freopen_s(&temp, "CONIN$", "r", stdin);
    //  freopen_s(&temp, "CONOUT$", "w", stdout);
    //  freopen_s(&temp, "CONOUT$", "w", stderr);
    //}

    //    setvbuf(stdout, NULL, _IONBF, 0);
    // setvbuf(stderr, NULL, _IONBF, 0);

    int ret = sscmain();

    // probably will never reach this, but just in case
    ExitProcess(ret);
}
