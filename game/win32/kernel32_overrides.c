#include <cx/cx.h>

#include "kernel32_overrides.h"

#include "subspacegame.h"

// override GetModuleFileName to return the path to the FTL executable, not to the Subspace client

DWORD __stdcall sc_GetModuleFileNameA(HANDLE hModule, LPSTR lpFilename, DWORD nSize)
{
    DWORD len = strLen(settings.gamePath);

    if (len > nSize - 1) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        len = nSize - 1;
    }
    strCopyOut(settings.gamePath, 0, lpFilename, len);
    return len;
}

DWORD __stdcall sc_GetModuleFileNameW(HANDLE hModule, LPWSTR lpFilename, DWORD nSize)
{
    DWORD len = strLen(settings.gamePath);

    if (len > nSize - 1) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        len = nSize - 1;
    }
    strToUTF16(settings.gamePath, lpFilename, len);
    return len;
}
