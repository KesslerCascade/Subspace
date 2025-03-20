#include <windows.h>
#include "rtld.h"

// we don't actually have any overrides at the moment in this project, but go ahead and leave this
// in case it's needed later

enum {
    OVERRIDE_NONE = 0,
    // OVERRIDE_FIRSTMODULE = 1,
};

int overrideModule(const char* name)
{
    return 0;
}

DWORD overrideProcAddress(int module, const char* name)
{
    switch (module & OVERRIDE_ALL_MASK) {
        /*
        case OVERRIDE_KERNEL32:
            return overrideKernel32(name);
        case OVERRIDE_ADVAPI32:
            return overrideAdvapi32(name);
        case OVERRIDE_WINSOCK2:
            return overrideWS2Name(name); */
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
