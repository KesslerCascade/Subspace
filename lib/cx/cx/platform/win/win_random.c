#include "cx/platform/os.h"

#ifndef CX_XP_COMPAT

#pragma comment(lib, "bcrypt.lib")

#include <intsafe.h>
#include <windows.h>
#include <bcrypt.h>

bool osGenRandom(uint8* buffer, uint32 size)
{
    return BCRYPT_SUCCESS(BCryptGenRandom(NULL, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG));
}

#else

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0400
#endif
#include <windows.h>

#include <wincrypt.h>

bool osGenRandom(uint8* buffer, uint32 size)
{
    HCRYPTPROV provider;

    if (CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE)
        return false;

    if (CryptGenRandom(provider, (DWORD)size, buffer) == FALSE) {
        CryptReleaseContext(provider, 0);
        return false;
    }

    CryptReleaseContext(provider, 0);
    return true;
}

#endif
