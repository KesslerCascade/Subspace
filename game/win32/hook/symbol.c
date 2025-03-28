#include <windows.h>

#include "hook/symbol.h"
#include "loader/rtld.h"

void symFindLib(addr_t base, Symbol* sym, SymbolFind* find)
{
    HMODULE lib = getLib(find->str);
    if (lib) {
        addr_t addr = (addr_t)GetProcAddress(lib, find->name);
        if (addr) {
            sym->addr     = addr;
            sym->resolved = true;
        }
    }
}
