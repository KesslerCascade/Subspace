#include <windows.h>

#include "codegen_private.h"

// Reserve 4MB of address space for codegen, increase if necessary
#define PAGESIZE 4096
#define MAXPAGES 1024
#define ALIGN    16

static addr_t cgBase   = 0;
static addr_t nextAddr = 0;    // address to begin next section
static addr_t reserved = 0;    // from nextAddr to reserved is committed

static DWORD inProgress = 0;   // set to thread ID of owner
static bool batch       = false;

CRITICAL_SECTION cgCrit;

// Macros to make the code more readable
#define PAGE(x)       ((x) / PAGESIZE)
#define NEXTPAGE(x)   PAGE(x + PAGESIZE - 1)
#define PAGEADDR(x)   ((x) * PAGESIZE)
#define PAGEPTR(x)    (cgBase + PAGEADDR(x))
#define ALIGNPDOWN(x) PAGEADDR(PAGE(x))
#define ALIGNPUP(x)   PAGEADDR(NEXTPAGE(x))

#define ALIGNED(x) (((x + ALIGN - 1) / ALIGN) * ALIGN)

static lazy_init codegen_is_init;

static void cgInit(void* dummy)
{
    if (cgBase)
        return;

    InitializeCriticalSection(&cgCrit);

    // reserve a contiguous memory region
    cgBase = addr(VirtualAlloc(0, PAGESIZE * MAXPAGES, MEM_RESERVE, PAGE_READWRITE));
    if (!(cgBase > 0))
        return;
    reserved = nextAddr = cgBase;
}

unsigned char* cgReserve(int maxsize)
{
    lazyinit(&codegen_is_init, cgInit, NULL);

    EnterCriticalSection(&cgCrit);
    if (inProgress)   // we already own the critical section and
        goto fail;    // someone entered it recursively, no cookie!

    inProgress = GetCurrentThreadId();

    maxsize = ALIGNED(maxsize);

    if (!batch && reserved > nextAddr) {
        DWORD dummy;
        // we have existing space, make it writable
        // (RWX in case code is running in another thread)
        if (!VirtualProtect(ptr(ALIGNPDOWN(nextAddr)),
                            reserved - ALIGNPDOWN(nextAddr),
                            PAGE_EXECUTE_READWRITE,
                            &dummy))
            goto fail;
    }

    if (nextAddr + maxsize > reserved) {
        addr_t end;
        // don't have enough reserved space left over from last time
        end = ALIGNPUP(nextAddr + maxsize);
        if (end > cgBase + PAGESIZE * MAXPAGES)
            goto fail;   // sorry, not enough room
        // commit some of the previously reserved address space
        if (addr(VirtualAlloc(ptr(reserved), end - reserved, MEM_COMMIT, PAGE_READWRITE)) !=
            reserved)
            goto fail;
        reserved = end;
    }

    // we have enough reserved space, hand it over!
    return ptr(nextAddr);

fail:
    LeaveCriticalSection(&cgCrit);
    return 0;
}

bool cgComplete(int actualsize)
{
    lazyinit(&codegen_is_init, cgInit, NULL);

    bool ret = true;
    DWORD dummy;

    if (inProgress != GetCurrentThreadId())
        return false;

    if (!batch) {
        // mark pages as executable
        if (!VirtualProtect(ptr(ALIGNPDOWN(nextAddr)),
                            reserved - ALIGNPDOWN(nextAddr),
                            PAGE_EXECUTE_READ,
                            &dummy))
            ret = false;
    }

    nextAddr = ALIGNED(nextAddr + actualsize);
    if (nextAddr > reserved) {
        nextAddr = reserved;
        ret      = false;   // congrats, you probably corrupted memory
    }

    inProgress = 0;
    LeaveCriticalSection(&cgCrit);
    return ret;
}

bool cgStartBatch()
{
    lazyinit(&codegen_is_init, cgInit, NULL);

    EnterCriticalSection(&cgCrit);
    if (inProgress || batch)
        goto fail;

    if (reserved > cgBase) {
        DWORD dummy;
        // make the whole thing writable
        // (RWX in case code is running in another thread)
        if (!VirtualProtect(ptr(cgBase), reserved - cgBase, PAGE_EXECUTE_READWRITE, &dummy))
            goto fail;
    }

    batch = true;

fail:
    LeaveCriticalSection(&cgCrit);
    return batch;
}

bool cgEndBatch()
{
    lazyinit(&codegen_is_init, cgInit, NULL);

    EnterCriticalSection(&cgCrit);
    if (inProgress || !batch)
        goto fail;

    batch = false;

    if (reserved > cgBase) {
        DWORD dummy;
        // protect all the memories
        if (!VirtualProtect(ptr(cgBase), reserved - cgBase, PAGE_EXECUTE_READ, &dummy))
            goto fail;
    }

fail:
    LeaveCriticalSection(&cgCrit);
    return !batch;
}
