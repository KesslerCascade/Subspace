#include <windows.h>

#include "hook/module.h"
#include "hook/string.h"

addr_t findString(addr_t base, const char* str)
{
    ModuleInfo* mi = moduleInfo(base);

    AddrList* l = hashtbl_get(&mi->stringhash, str);
    if (l)
        return l->addrs[0];
    return 0;
}

AddrList* findAllStrings(addr_t base, const char* str)
{
    ModuleInfo* mi = moduleInfo(base);

    return hashtbl_get(&mi->stringhash, str);
}
