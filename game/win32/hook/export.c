#include "hook/module.h"

#include <windows.h>

addr_t getExport(addr_t base, const char* name)
{
    ModuleInfo* mi = moduleInfo(base);
    addr_t ret     = 0;
    htFind(mi->exporthash, strref, (strref)name, uintptr, &ret);
    return ret;
}
