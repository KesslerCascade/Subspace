#include "kernel32_overrides.h"

#include "minicrt.h"
#include "subspaceclient.h"

// override GetModuleFileName to return the path to the FTL executable, not to the Subspace client

DWORD __stdcall sc_GetModuleFileNameA(HANDLE hModule, LPSTR lpFilename, DWORD nSize)
{
    DWORD len = strlen(scsettings.gamePath);

    if (len > nSize - 1) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        len = nSize - 1;
    }
    memcpy(lpFilename, scsettings.gamePath, len);
    lpFilename[len + 1] = '\0';
    return len;
}

DWORD __stdcall sc_GetModuleFileNameW(HANDLE hModule, LPWSTR lpFilename, DWORD nSize)
{
    DWORD len = strlen(scsettings.gamePath);

    if (len > nSize - 1) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        len = nSize - 1;
    }
    MultiByteToWideChar(CP_UTF8, 0, scsettings.gamePath, len, lpFilename, nSize);
    lpFilename[len + 1] = '\0';
    return len;
}
