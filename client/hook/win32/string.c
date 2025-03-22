#include <windows.h>

#include "hook/module.h"
#include "hook/string.h"
#include "loader/win32/rtld.h"

static bool addString(hashtbl* tbl, const char* str, addr_t loc)
{
    AddrList* fcl = hashtbl_get(tbl, str);
    if (!fcl) {
        fcl = addrListCreate();
        hashtbl_set(tbl, str, fcl);
    }
    addrListAdd(fcl, loc);

    return true;
}

static void stringhash_init(addr_t base, hashtbl* tbl)
{
    SegInfo rdata;
    if (!getRDataSeg(base, &rdata))
        return;

    hashtbl_init(tbl, 8192, HT_STRING_KEYS);

    char* s = (char*)rdata.start;
    char* p = s;
    char* e = (char*)rdata.end;
    while (p < e) {
        char c = *p;
        p++;
        if (c == '\0') {
            if (p - s >= 5) {
                addString(tbl, (const char*)s, (addr_t)s);
            }
            s = p;
        } else if (!((c >= 32 && c <= 126) || c == '\n')) {
            // non-ASCII character, start over
            s = p;
        }
    }
}

addr_t findString(addr_t base, const char* str)
{
    hashtbl* stringhash = moduleStringHash(base, stringhash_init);

    AddrList* l = hashtbl_get(stringhash, str);
    if (l)
        return l->addrs[0];
    return 0;
}

AddrList* findAllStrings(addr_t base, const char* str)
{
    hashtbl* stringhash = moduleStringHash(base, stringhash_init);

    return hashtbl_get(stringhash, str);
}
