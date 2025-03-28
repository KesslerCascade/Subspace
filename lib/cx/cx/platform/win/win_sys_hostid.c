#include "cx/sys/hostid_private.h"
#include "cx/string.h"
#include "cx/utils/scratch.h"
#include "cx/platform/win.h"

#include <mbedtls/entropy.h>

typedef int (WINAPI *GetSystemFirmwareTable_t)(DWORD FirmwareTableProviderSignature, DWORD FirmwareTableID, PVOID pFirmwareTableBuffer, DWORD BufferSize);

typedef struct DMIHeader {
    BYTE type;
    BYTE length;
    WORD handle;
} DMIHeader;

typedef struct RawSMBIOSData {
    BYTE    Used20CallingMethod;
    BYTE    SMBIOSMajorVersion;
    BYTE    SMBIOSMinorVersion;
    BYTE    DmiRevision;
    DWORD   Length;
    BYTE    SMBIOSTableData[];
} RawSMBIOSData;

// UUIDs known to not be unique
static uint8 uuid_blacklist[][16] = {
    // 00000000-0000-0000-0000-000000000000 - Empty or missing UUID
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    // FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF - Sometimes used as error code
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    // 03000200-0400-0500-0006-000700080009 - Gigabyte motherboard
    { 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09 },
    // 4C4C4544-0000-2010-8020-80C04F202020 - Dell generic (missing service tag)
    { 0x44, 0x45, 0x4c, 0x4c, 0x00, 0x00, 0x10, 0x20, 0x80, 0x20, 0x80, 0xc0, 0x4f, 0x20, 0x20, 0x20 },
};

static int32 hostIdTrySMBIOS(mbedtls_md_context_t *shactx)
{
    int32 ret = 0;
    char *buf = 0;

    GetSystemFirmwareTable_t pGetSystemFirmwareTable;
    HANDLE hDll = LoadLibrary(TEXT("kernel32.dll"));
    if (!hDll)
        goto out;
    pGetSystemFirmwareTable = (GetSystemFirmwareTable_t)GetProcAddress(hDll, "GetSystemFirmwareTable");
    if (!pGetSystemFirmwareTable)
        goto out;

    int bufsize = pGetSystemFirmwareTable('RSMB', 0, 0, 0);
    if (!bufsize)
        goto out;

    buf = xaAlloc(bufsize);
    if (!pGetSystemFirmwareTable('RSMB', 0, buf, bufsize))
        goto out;

    RawSMBIOSData *smb = (RawSMBIOSData *)buf;
    BYTE *p = smb->SMBIOSTableData;

    bufsize -= 8;       // reuse bufsize as data length remaining
    if (smb->Length != bufsize)
    {
        // invalid smbios data
        goto out;
    }

    for (int i = 0; i < (int)smb->Length; i++) {
        DMIHeader *h = h = (DMIHeader*)p;
        if (h->length > bufsize) {
            // tried to run off the end of the buffer
            goto out;
        }

        if (h->type == 1 && h->length >= 0x19) {
            BYTE *uuid = (p + 8);

            // check the uuid against the blacklist
            for (int j = 0; j < sizeof(uuid_blacklist) / 16; j++) {
                if (memcmp(uuid, uuid_blacklist[j], 16) == 0)
                    goto out;
            }

            // okay, we have a valid UUID!
            // theoretically some byte swapping should be done to get it into the
            // proper order per smbios 2.6 spec, but since all we need is a fingerprint
            // to hash, it doesn't matter. A given system will always be consistent
            // about the ordering it uses so just hash it as-is.

            mbedtls_md_update(shactx, uuid, 16);
            ret = HID_SourceBIOSUUID;
            goto out;
        }

        // move to next header, skip over NULLs, being careful about overflow
        p += h->length;
        bufsize -= h->length;
        while (bufsize > 0 && (*(WORD *)p) != 0) { p++; bufsize--; }
        p += 2;
        bufsize -= 2;
    }

out:
    if (hDll)
        FreeLibrary(hDll);
    xaFree(buf);
    return ret;
}

static int32 hostIdTryRegistry(HKEY hive, int32 retval, mbedtls_md_context_t *shactx)
{
    int32 ret = 0;
    HKEY key;

    if (RegCreateKeyExW(hive, L"SOFTWARE\\CX", 0, NULL, 0,
                        STANDARD_RIGHTS_REQUIRED | KEY_QUERY_VALUE, NULL, &key, NULL) == ERROR_SUCCESS) {
        DWORD sz = 0;
        if (RegQueryValueExW(key, L"HostId", NULL, NULL, NULL, &sz) == ERROR_SUCCESS) {
            if (sz == 64) {
                uint8 *buf = scratchGet(sz);
                RegQueryValueExW(key, L"HostId", NULL, NULL, (uint8*)buf, &sz);
                mbedtls_md_update(shactx, buf, sz);
                ret = retval;
            }
        }

        // couldn't get good data from registry, try to create it
        if (!ret) {
            RegCloseKey(key);
            if (RegCreateKeyExW(hive, L"SOFTWARE\\CX", 0, NULL, 0,
                                STANDARD_RIGHTS_REQUIRED | KEY_SET_VALUE, NULL, &key, NULL) == ERROR_SUCCESS) {
                mbedtls_entropy_context entropy;
                uint8 randbuf[64];

                mbedtls_entropy_init(&entropy);
                mbedtls_entropy_func(&entropy, (unsigned char*)randbuf, sizeof(randbuf));
                mbedtls_entropy_free(&entropy);

                if (RegSetValueExW(key, L"HostId", 0, REG_BINARY, randbuf, sizeof(randbuf)) == ERROR_SUCCESS) {
                    mbedtls_md_update(shactx, randbuf, sizeof(randbuf));
                    ret = retval;
                }
            }
        }
        RegCloseKey(key);
    }

    return ret;
}

int32 hostIdPlatformInit(mbedtls_md_context_t *shactx)
{
    int32 ret = 0;
    HKEY key;

    // prefer SMBIOS UUID if possible
    ret = hostIdTrySMBIOS(shactx);

    // next best is machine-wide registry entry
    if (!ret)
        ret = hostIdTryRegistry(HKEY_LOCAL_MACHINE, HID_SourceMachineRegistry, shactx);

    // but if running as non-admin it's not likely it can be created, so save it per-user instead
    if (!ret)
        ret = hostIdTryRegistry(HKEY_CURRENT_USER, HID_SourceUserRegistry, shactx);

    // mostly unique but may not be reliable on wine
    if (!ret &&
        RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
        DWORD sz = 0;
        if (RegQueryValueExW(key, L"MachineGuid", NULL, NULL, NULL, &sz) == ERROR_SUCCESS) {
            wchar_t *buf = scratchGet(sz);
            RegQueryValueExW(key, L"MachineGuid", NULL, NULL, (uint8*)buf, &sz);
            string guid = 0;
            strFromUTF16(&guid, buf, cstrLenw(buf));
            mbedtls_md_update(shactx, strC(guid), strLen(guid));
            strDestroy(&guid);
            ret = HID_SourceCrypto;
        }
        RegCloseKey(key);
    }

    return ret;
}

int32 hostIdPlatformInitFallback(mbedtls_md_context_t *shactx)
{
    // ultimate panic fallback
    char buf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD bufsz = sizeof(buf);
    GetComputerNameA(buf, &bufsz);
    mbedtls_md_update(shactx, buf, cstrLen(buf));
    return HID_SourceComputerName;
}
