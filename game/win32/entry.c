#include <stdio.h>
#include <windows.h>
#include "subspacegame.h"

#include "minicrt.h"

HANDLE dbgconsole;

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

static char* nextp(char* p)
{
    char* p1 = strchr(p, ' ');
    char* p2 = strchr(p, '"');
    if (!p1)
        return p2;
    if (!p2)
        return p1;

    if (p1 < p2)
        return p1;
    return p2;
}

int __stdcall entry()
{
    settings.gameDir     = smalloc(MAX_PATH);
    settings.gameProgram = smalloc(MAX_PATH);
    settings.gamePath    = smalloc(MAX_PATH);

    const char* wincmdline = GetCommandLineA();
    char* cmdline          = sstrdup(wincmdline);
    bool q                 = false;
    char* p                = cmdline;

    int nargs = 1;

    if (!*p)
        nargs = 0;

    // scan to count args
    while (p && *p) {
        p = nextp(p);
        if (p) {
            if (*p == '"')
                q = !q;
            if (*p == ' ' && !q) {
                if (*(p + 1) != ' ' && *(p + 1) != '\0')
                    ++nargs;
            }
            ++p;
        }
    }

    char** argv = scalloc(nargs, sizeof(char*));
    if (nargs > 0)
        argv[0] = cmdline;

    // tokenize
    int carg = 1;
    p        = cmdline;
    while (p && *p) {
        p = nextp(p);
        if (p) {
            if (*p == '"') {
                if (!q && argv[carg - 1] == p)
                    argv[carg - 1] = p + 1;
                q  = !q;
                *p = '\0';
            }
            if (*p == ' ' && !q) {
                *p = '\0';
                if (*(p + 1) != ' ' && *(p + 1) != '\0')
                    argv[carg++] = p + 1;
            }
            ++p;
        }
    }

#if 1
    strcpy(settings.gameDir, "M:\\Dev\\FTL1.6.3");
    strcpy(settings.gameProgram, "FTLGame.exe");
    strcpy(settings.gamePath, "M:\\Dev\\FTL1.6.3\\FTLGame.exe");
#elif 0
    strcpy(settings.gameDir, "M:\\Dev\\FTLGoG1.6.13b");
    strcpy(settings.gameProgram, "FTLGame.exe");
    strcpy(settings.gamePath, "M:\\Dev\\FTLGoG1.6.13b\\FTLGame.exe");
#elif 0
    strcpy(settings.gameDir, "M:\\Dev\\FTLSteam1.6.8");
    strcpy(settings.gameProgram, "FTLGame.exe");
    strcpy(settings.gamePath, "M:\\Dev\\FTLSteam1.6.8\\FTLGame.exe");
#elif 0
    strcpy(settings.gameDir, "M:\\games\\Steam\\steamapps\\common\\FTL Faster Than Light");
    strcpy(settings.gameProgram, "FTLGame.exe");
    strcpy(settings.gamePath,
           "M:\\games\\Steam\\steamapps\\common\\FTL Faster Than Light\\FTLGame.exe");
#elif 1
    strcpy(settings.gameDir, "M:\\Games\\FTL");
    strcpy(settings.gameProgram, "FTLGame.exe");
    strcpy(settings.gamePath, "M:\\Games\\FTL\\FTLGame.exe");
#endif

    SetCurrentDirectoryA(settings.gameDir);
    SetEnvironmentVariableA("USERPROFILE", settings.gameDir);

#ifdef _DEBUG
    // AllocConsole();
#endif
    // if (AllocConsole()) {
    //  FILE* temp;
    //  freopen_s(&temp, "CONIN$", "r", stdin);
    //  freopen_s(&temp, "CONOUT$", "w", stdout);
    //  freopen_s(&temp, "CONOUT$", "w", stderr);
    //}

    //    setvbuf(stdout, NULL, _IONBF, 0);
    // setvbuf(stderr, NULL, _IONBF, 0);

    int ret = sscmain(nargs, argv);

    // if (settings.testMode) {
    // Sleep(3000);
    //}

    // probably will never reach this, but just in case
    ExitProcess(ret);
}

void osWriteDbg(const char* str)
{
    HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written  = 0;
    WriteConsoleA(hstdout, (void*)str, strlen(str), &written, NULL);
}
