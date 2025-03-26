#include <windows.h>

#include "hook/function.h"
#include "hook/string.h"
#include "hook/symbol.h"
#include "loader/rtld.h"

#include "disasmfind.h"

static addr_t findVirtual(addr_t base, SymbolFind* find)
{
    addr_t vtable = _symResolve(base, find->vtable);
    if (!vtable)
        return 0;

    SegInfo code;
    if (!getCodeSeg(base, &code))
        return 0;

    // TODO: Currently there is no way to distinguish between an offset that failed to be located,
    // and an offset of 0, which is valid in a vtable. Need to rework the Symbol structure to
    // explicitly track resolution state rather than overloading the address.
    addr_t ret = *(addr_t*)(vtable + _symResolve(base, find->offset));
    if (ret >= code.start && ret < code.end)
        return ret;

    return 0;
}

static addr_t symFindOne(addr_t base, SymbolFind* find)
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
    case SYMBOL_FIND_VIRTUAL:
        return findVirtual(base, find);
    }

    return 0;
}

addr_t _symResolve(addr_t base, Symbol* sym)
{
    if (sym->addr)
        return sym->addr;

    // go down the list of ways to find this symbol
    for (SymbolFind* find = sym->find; find->type != 0; ++find) {
        sym->addr = symFindOne(base, find);
        if (sym->addr)
            return sym->addr;
    }

    return 0;
}
