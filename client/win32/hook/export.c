#include <windows.h>

#include "hook/module.h"

addr_t getExport(addr_t base, const char* name)
{
    ModuleInfo* mi = moduleInfo(base);
    return (addr_t)hashtbl_get(&mi->exporthash, name);
}
