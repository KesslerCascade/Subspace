#include "hook/string.h"
#include "hook/module.h"

addr_t findString(addr_t base, strref str)
{
    ModuleInfo* mi = moduleInfo(base);

    sa_uintptr* al = addrListFindByStr(mi->stringhash, str);
    if (al && saSize(*al) > 0)
        return al->a[0];
    return 0;
}

AddrList* findAllStrings(addr_t base, strref str)
{
    ModuleInfo* mi = moduleInfo(base);

    return addrListFindByStr(mi->stringhash, str);
}
