#include "winmain.h"

#include "ftl/functions_osdep.h"

// Wrap WinMain to call it with the correct module handle
int subspace_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MessageBoxA(NULL,
                "Hey it's not the real WinMain",
                "WinMain (or is it???)",
                MB_ICONQUESTION | MB_OKCANCEL);

    HINSTANCE SubspaceModule = GetModuleHandleA(NULL);
    // Call FTL's WinMain but using our module handle instead of the base address of the loaded
    // executable. We copied resources and TLS data already so it will be able to find them in the
    // Subspace Client module.
    FTL_WinMain(SubspaceModule, NULL, lpCmdLine, nCmdShow);
    return 0;
}
