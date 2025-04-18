#include <windows.h>
#include "subspacegame.h"

void OSShowError(const char *str)
{
    MessageBoxA(NULL, str, "Subspace", MB_ICONERROR | MB_OK);
}