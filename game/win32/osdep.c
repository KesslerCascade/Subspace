#include <windows.h>

#include "minicrt.h"
#include "osdep.h"

static uint64_t curFrameTime;

#ifdef _DEBUG
void osWriteDbg(const char* str)
{
    HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written  = 0;
    WriteConsoleA(hstdout, (void*)str, strlen(str), &written, NULL);
}
#endif

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

void osFrameTime(int64_t* time)
{
    memcpy(time, &curFrameTime, sizeof(int64_t));
}

void osNextFrame()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    uint32_t* parts = (uint32_t*)&curFrameTime;
    parts[0]        = ft.dwLowDateTime;
    parts[1]        = ft.dwHighDateTime;
}