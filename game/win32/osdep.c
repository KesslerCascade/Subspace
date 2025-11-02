#include "osdep.h"
#include <cx/xalloc.h>
#include "int64.h"

#include <windows.h>

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
    wchar_t* dirw = xaAlloc(dlen * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, dir, -1, dirw, dlen);

    SetCurrentDirectoryW(dirw);
    xaFree(dirw);
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

bool osAbsolutePathUTF8(const char* fname, char* buf, size_t bufsz)
{
    bool ret       = false;
    wchar_t* cbuf  = xaAlloc(bufsz * 2);
    wchar_t* cbuf2 = xaAlloc(bufsz * 2);

    if (MultiByteToWideChar(CP_UTF8, 0, fname, -1, cbuf, bufsz) == 0)
        goto out;

    if (GetFullPathNameW(cbuf, bufsz, cbuf2, NULL) >= bufsz)
        goto out;

    if (WideCharToMultiByte(CP_UTF8, 0, cbuf2, -1, buf, bufsz, NULL, NULL) == 0)
        goto out;

    ret = true;

out:
    xaFree(cbuf);
    xaFree(cbuf2);
    return ret;
}

bool osWriteFile(const char* fn, uint8_t* buf, size_t sz)
{
    bool ret     = false;
    int fnlen    = MultiByteToWideChar(CP_UTF8, 0, fn, -1, NULL, 0);
    wchar_t* fnw = xaAlloc(fnlen * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, fn, -1, fnw, fnlen);

    HANDLE f = CreateFileW(fnw,
                           GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
    if (!f)
        goto out;

    DWORD didwrite;
    WriteFile(f, buf, sz, &didwrite, NULL);
    ret = (didwrite == sz);

out:
    if (f != INVALID_HANDLE_VALUE)
        CloseHandle(f);
    xaFree(fnw);
    return ret;
}
