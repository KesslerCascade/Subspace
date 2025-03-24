#include "hook/symbol.h"
#include "hook/function.h"
#include "hook/string.h"

#include "disasmfind.h"

static bool symFindOne(addr_t base, Symbol* sym, SymbolFind* find)
{
    switch (find->type) {
    case SYMBOL_FIND_EXPORT:
        sym->addr = getExport(base, find->name);
        return (sym->addr != 0);
    case SYMBOL_FIND_STRING:
        sym->addr = findString(base, find->str);
        return (sym->addr != 0);
    case SYMBOL_FIND_DISASM:
        sym->addr = findByDisasm(base, find->disasm);
        return (sym->addr != 0);
    }

    return false;
}

bool _symResolve(addr_t base, Symbol* sym)
{
    if (sym->addr)
        return true;

    // go down the list of ways to find this symbol
    for (SymbolFind* find = sym->find; find->type != 0; ++find) {
        if (symFindOne(base, sym, find))
            return true;
    }
    return false;
}
