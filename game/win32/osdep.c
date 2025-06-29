#include <windows.h>

#include "int64.h"
#include "minicrt.h"
#include "osdep.h"

static int64_t curFrameTime;

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

int64_t osFrameTime()
{
    return curFrameTime;
}

void osNextFrame()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    uint64_u nft = { .p.low = ft.dwLowDateTime, .p.high = ft.dwHighDateTime };

    // convert filetime to CX time

    // convert from 100-ns intervals to microseconds
    nft = _uint64_div(nft, 10, NULL);
    // FILETIME epoch is midnight on Jan 1, 1601
    // Which is a julian date of 2305813.50000
    // That's 199222286400 in seconds, or in microseconds...

    nft = _uint64_add(nft, (uint64_u) { .v = 199222286400000000LL });   // adjust epoch

    curFrameTime = (int64_t)nft.v;   // dividing by 10 earlier ensures this will never be negative
}