#include <windows.h>

DWORD __stdcall sc_GetModuleFileNameA(HANDLE hModule, LPSTR lpFilename, DWORD nSize);

DWORD __stdcall sc_GetModuleFileNameW(HANDLE hModule, LPWSTR lpFilename, DWORD nSize);
