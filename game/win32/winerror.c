#include "subspacegame.h"

#include <windows.h>

void osShowError(const char* str)
{
    MessageBoxA(NULL, str, "Subspace", MB_ICONERROR | MB_OK);
}