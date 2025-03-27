#include "hook/function.h"
#include "hook/string.h"
#include "hook/symbol.h"
#include "hook/disasmtrace.h"

static void symFindVirtual(addr_t base, Symbol *sym, SymbolFind* find)
{
    if (!(_symResolve(base, find->vtable) && _symResolve(base, find->offset)))
        return;

    SegInfo code;
    if (!getCodeSeg(base, &code))
        return;

    addr_t ret = *(addr_t*)(find->vtable->addr + find->offset->addr);
    if (ret >= code.start && ret < code.end) {
        sym->addr = ret;
        sym->resolved = true;
    }
}

static bool symFindOne(addr_t base, Symbol *sym, SymbolFind* find)
{
    addr_t addr;
    switch (find->type) {
    case SYMBOL_FIND_IMPORT:
        symFindLib(base, sym, find);
        break;
    case SYMBOL_FIND_EXPORT:
        addr = getExport(base, find->name);
        if (addr != 0) {
            sym->addr = addr;
            sym->resolved = true;
        }
        break;
    case SYMBOL_FIND_STRING:
     addr = findString(base, find->str);
     if (addr != 0) {
        sym->addr = addr;
        sym->resolved = true;
    }
    break;
    case SYMBOL_FIND_DISASM:
        disasmTrace(base, find->disasm);
        break;
    case SYMBOL_FIND_VIRTUAL:
        symFindVirtual(base, sym, find);
        break;
    }

    return sym->resolved;
}

bool _symResolve(addr_t base, Symbol* sym)
{
    // go down the list of ways to find this symbol
    for (SymbolFind* find = sym->find; !sym->resolved && find->type != 0; ++find) {
        symFindOne(base, sym, find);
    }

    return sym->resolved;
}
