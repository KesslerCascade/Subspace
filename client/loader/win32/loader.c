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