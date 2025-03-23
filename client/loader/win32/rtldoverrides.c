#include <windows.h>

#include "win32/kernel32_overrides.h"
#include "rtld.h"
#include "minicrt.h"

enum {
    OVERRIDE_NONE = 0,
    OVERRIDE_KERNEL32 = 1,
};

int overrideModule(const char* name)
{
    if (!stricmp(name, "Kernel32.dll"))
        return OVERRIDE_KERNEL32;

    return 0;
}

#define OVR(x)             \
    if (!strcmp(name, #x)) \
    return (DWORD)x
#define SCOVR(x)           \
    if (!strcmp(name, #x)) \
    return (DWORD)sc_##x

static DWORD overrideKernel32(const char* name)
{
    SCOVR(GetModuleFileNameA);
    SCOVR(GetModuleFileNameW);
    return 0;
}

DWORD overrideProcAddress(int module, const char* name)
{
    switch (module & OVERRIDE_ALL_MASK) {
        case OVERRIDE_KERNEL32:
            return overrideKernel32(name);
    }
    return 0;
}

DWORD overrideProcAddressOrdinal(int module, DWORD ordinal)
{
    /*    switch (module & OVERRIDE_ALL_MASK) {
        case OVERRIDE_WINSOCK2:
            return overrideWS2Ordinal(ordinal);
        } */
    return 0;
}
