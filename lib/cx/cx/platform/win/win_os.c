#include "cx/platform/os.h"
#include "cx/platform/win.h"
#include "cx/utils/compare.h"
#include "cx/utils/lazyinit.h"
#include "cx/time/time.h"

static LazyInitState coreCache;
static int ncores;
static int nlogical;

bool osIsWine()
{
    static int result = -1;

    if (result < 0) {
        HMODULE ntdllModule = GetModuleHandleW(L"ntdll.dll");
        void* wine_get_version;

        if (!ntdllModule) {
            result = 0;
            return result;
        }

        wine_get_version = GetProcAddress(ntdllModule, "wine_get_version");
        result = wine_get_version ? 1 : 0;
    }

    return result;
}

static DWORD nbits(ULONG_PTR mask)
{
    DWORD ulongptr_bits = sizeof(ULONG_PTR) * 8 - 1;
    DWORD count = 0;
    ULONG_PTR test = (ULONG_PTR)1 << ulongptr_bits;
    for (uint32 i = 0; i <= ulongptr_bits; ++i) {
        if (mask & test)
            count++;
        test >>= 1;
    }
    return count;
}

static void initCoreCache(void *dummy)
{
    ncores = nlogical = 0;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buf = NULL;
    DWORD len = 0;

    GetLogicalProcessorInformation(NULL, &len);
    if (len == 0)
        goto out;

    buf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)stackAlloc(len);
    if (GetLogicalProcessorInformation(buf, &len)) {
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION p = buf;
        DWORD off = 0;
        while (off < len) {
            if (p->Relationship == RelationProcessorCore) {
                ncores++;
                nlogical += nbits(p->ProcessorMask);
            }
            off += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            p++;
        }
    }

out:
    // fallback to prevent bad things like dividing by zero
    if (ncores == 0)
        ncores = 1;
    if (nlogical == 0)
        nlogical = 1;
}

void osYield()
{
    SwitchToThread();
}

void osSleep(int64 time)
{
    int64 msec = timeToMsec(time);

    while (msec > 0) {
        DWORD sleeptime = (DWORD)clamphigh(msec, 0xFFFFFFFF);
        Sleep(sleeptime);
        msec -= sleeptime;
    }
}

int osPhysicalCPUs()
{
    lazyInit(&coreCache, initCoreCache, NULL);
    return ncores;
}

int osLogicalCPUs()
{
    lazyInit(&coreCache, initCoreCache, NULL);
    return nlogical;
}
