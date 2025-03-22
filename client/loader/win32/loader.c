#include "rtld.h"

addr_t loadProgram(const char* filename)
{
    addr_t ret = loadExe(filename);
    if (!ret)
        return 0;

    importResources(ret);
    importTLSInitializers(ret);
    return ret;
}

entrypoint getProgramEntry(addr_t baseaddr)
{
    return (entrypoint)crva(nthdr(ftlbase)->OptionalHeader.AddressOfEntryPoint);
}

static bool getSegHelper(addr_t baseaddr, SegInfo* si, const char* name)
{
    IMAGE_SECTION_HEADER* hdr = getSection(baseaddr, name);
    if (!hdr)
        return false;
    si->start = hdr->VirtualAddress;
    si->size  = hdr->SizeOfRawData;
    si->end   = si->start + si->size;
    return true;
}

bool getCodeSeg(addr_t baseaddr, SegInfo* si)
{
    return getSegHelper(baseaddr, si, ".text");
}

bool getDataSeg(addr_t baseaddr, SegInfo* si)
{
    return getSegHelper(baseaddr, si, ".data");
}

bool getRDataSeg(addr_t baseaddr, SegInfo* si)
{
    return getSegHelper(baseaddr, si, ".rdata");
}

uintptr_t makeSegWritable(SegInfo* si)
{
    DWORD oldProtect = 0;

    if (!VirtualProtect((LPVOID)si->start, si->size, PAGE_READWRITE, &oldProtect))
        return 0;
    return oldProtect;
}

bool restoreSegAccess(SegInfo* si, uintptr_t origaccess)
{
    DWORD dummy;
    return VirtualProtect((LPVOID)si->start, si->size, PAGE_READWRITE, &dummy);
}
