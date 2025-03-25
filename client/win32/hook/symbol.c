#include <windows.h>

#include "hook/function.h"
#include "hook/string.h"
#include "hook/symbol.h"
#include "loader/rtld.h"

#include "disasmfind.h"

static addr_t symFindOne(addr_t base, Symbol* sym, SymbolFind* find)
{
    switch (find->type) {
    case SYMBOL_FIND_IMPORT: {
        HMODULE lib = getLib(find->str);
        if (lib)
            return (addr_t)GetProcAddress(lib, find->name);
        return 0;
    }
    case SYMBOL_FIND_EXPORT:
        return getExport(base, find->name);
    case SYMBOL_FIND_STRING:
        return findString(base, find->str);
    case SYMBOL_FIND_DISASM:
        return findByDisasm(base, find->disasm);
    }

    return 0;
}

addr_t _symResolve(addr_t base, Symbol* sym)
{
    if (sym->addr)
        return sym->addr;

    // go down the list of ways to find this symbol
    for (SymbolFind* find = sym->find; find->type != 0; ++find) {
        sym->addr = symFindOne(base, sym, find);
        if (sym->addr)
            return sym->addr;
    }

    return 0;
}
