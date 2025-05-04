#include <windows.h>

#include "minicrt.h"
#include "osdep.h"

void osWriteDbg(const char* str)
{
    HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written  = 0;
    WriteConsoleA(hstdout, (void*)str, strlen(str), &written, NULL);
}

void osSetCurrentDir(const char* dir)
{
    int dlen      = MultiByteToWideChar(CP_UTF8, 0, dir, -1, NULL, 0);
    wchar_t* dirw = smalloc(dlen * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, dir, -1, dirw, dlen);

    SetCurrentDirectoryW(dirw);
}

void osSleep(int msec)
{
    SleepEx(msec, true);
}

void osExit(int retcode)
{
    ExitProcess(retcode);
}
