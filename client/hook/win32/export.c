#include <windows.h>

#include "hook/module.h"
#include "hook/symbol.h"
#include "loader/win32/rtld.h"

static void exporthash_init(addr_t base, hashtbl* tbl)
{
    IMAGE_DATA_DIRECTORY* data   = &datadir(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
    IMAGE_EXPORT_DIRECTORY* etbl = datadirptr(base, data, IMAGE_EXPORT_DIRECTORY);
    addr_t* nametbl              = prva(base, etbl->AddressOfNames);
    WORD* ordinaltbl             = prva(base, etbl->AddressOfNameOrdinals);
    addr_t* addrtbl              = prva(base, etbl->AddressOfFunctions);

    hashtbl_init(tbl, MIN(etbl->NumberOfNames * 2, 64), HT_STRING_KEYS);

    for (int i = 0; i < etbl->NumberOfNames; i++) {
        DWORD ordinal = ordinaltbl[i];
        if (ordinal < etbl->NumberOfFunctions) {
            hashtbl_addint(tbl, (const char*)prva(base, nametbl[i]), rva(base, addrtbl[ordinal]));
        }
    }
}

addr_t getExport(addr_t base, const char* name)
{
    hashtbl* exporthash = moduleExportHash(base, exporthash_init);
    return (addr_t)hashtbl_get(exporthash, name);
}
