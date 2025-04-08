#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>

int wmain(int argc, wchar_t *argv[])
{
    // Only needed on Win32 due to windows subsystem not having a console for the lua debugger.

    wchar_t *exename = L"subspace.exe";
    wchar_t *p1 = L" -dbg";
    LPWSTR cmdline = GetCommandLineW();

    wchar_t *p = wcschr(cmdline, L' ');
    size_t argslen = 0;
    if (p)
        argslen = wcslen(p);

    size_t wbuflen = wcslen(argv[0]) + wcslen(p1) + argslen + 2;
    wchar_t *wbuf = calloc(wbuflen, sizeof(wchar_t));
    if (!wbuf)
        return 1;

    wcscpy_s(wbuf, wbuflen, argv[0]);
    wcscat_s(wbuf, wbuflen, p1);
    if (argslen > 0)
        wcscat_s(wbuf, wbuflen, p);

    wchar_t exebuf[4096];
    GetModuleFileNameW(NULL, exebuf, 4096);
    exebuf[4095] = 0;
    p = wcsrchr(exebuf, '\\');
    if (p)
        p[1] = '\0';
    wcscat_s(exebuf, 4096, exename);

    STARTUPINFOW startup = { 0 };
    PROCESS_INFORMATION pinfo = { 0 };
    startup.cb = sizeof(startup);
    if (!CreateProcessW(exebuf, wbuf, NULL, NULL,
                        TRUE, 0, NULL, NULL, &startup, &pinfo)) {
        fputs("Could not run subspace.exe", stderr);
        return 1;
    }

    WaitForSingleObject(pinfo.hProcess, INFINITE);

    DWORD ret = 0;
    GetExitCodeProcess(pinfo.hProcess, &ret);

    CloseHandle(pinfo.hProcess);
    CloseHandle(pinfo.hThread);

    return ret;
}
