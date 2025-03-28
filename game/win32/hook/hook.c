#include <windows.h>
#include "disasm.h"
#include "minicrt.h"

#include "hook/hook.h"
#include "hook/module.h"
#include "hook/string.h"
#include "loader/rtld.h"

AddrList* addrListCreate(void)
{
    AddrList* l = smalloc(sizeof(AddrList));
    l->num      = 0;
    l->addrs    = smalloc(16 * sizeof(addr_t));
    return l;
}

void addrListAdd(AddrList* l, addr_t addr)
{
    l->num++;
    l->addrs             = srealloc(l->addrs, (((l->num + 15) >> 4) << 4) * sizeof(addr_t));
    l->addrs[l->num - 1] = addr;
}

static int replacePointers(addr_t base, addr_t oldfunc, addr_t newfunc)
{
    // Tricksy trick: The relocation table contains every address in the program that needs to
    // be adjusted should the base address change. Which conveniently means that it hands us
    // a list of every pointer in both code and static data, as well as absolute jumps, indirect
    // jumps through a pointer, and things like vtable entries. So do a simple search and replace.
    // This won't get relative calls, unfortunately, but does take care of the otherwise hard to
    // detect function pointers. We handle relative calls though a separate table based on
    // disassembling the code.

    ModuleInfo* mi = moduleInfo(base);
    AddrList* fcl  = hashtbl_get(&mi->ptrhash, oldfunc);
    int count      = 0;
    if (fcl) {
        for (uint32_t i = 0; i < fcl->num; i++) {
            addr_t p      = fcl->addrs[i];
            *(addr_t*)(p) = newfunc;
            ++count;
        }
    }

    return count;
}

// Scan the cached table of relative call destinations and find any that need to
// be replaced.
static int replaceRelcalls(addr_t base, addr_t oldfunc, addr_t newfunc)
{
    ModuleInfo* mi = moduleInfo(base);
    AddrList* fcl  = hashtbl_get(&mi->relcallhash, oldfunc);
    int count      = 0;
    if (fcl) {
        for (uint32_t i = 0; i < fcl->num; i++) {
            addr_t p      = fcl->addrs[i];
            *(addr_t*)(p) = newfunc - (p + 4);
            ++count;
        }
    }

    return count;
}

/*
#ifdef DEBUG
static void emitWarnings(const FuncInfo* fi, const char* funcname)
{
    if (fi->wasinlined)
        LogDWarn(L"Inlined function %S had a hook added! Probably will not work.", funcname);
    if (fi->duplicates)
        LogDWarn(L"Function %S has duplicates and had a hook added! Probably will not work.",
                 funcname);
}
#else
#define emitWarnings(x, y)
#endif
*/

#ifdef _DEBUG
bool _hookFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* pre, void* post,
                   const char* funcname)
#else
bool _hookFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* pre, void* post)
#endif
{
    void* hookfunc;

    hookfunc = hookCreate(addr, fi, pre, post);
    if (!hookfunc)
        return false;

    // EmitWarnings(fi, funcname);

    int count = 0;
    count += replacePointers(base, addr, addr(hookfunc));
    count += replaceRelcalls(base, addr, addr(hookfunc));
    return (count > 0);
}

#ifdef _DEBUG
bool _replaceFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* replacement,
                      const char* funcname)
#else
bool _replaceFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* replacement)
#endif
{
    void* replfunc;

    replfunc = replacementCreate(fi, replacement);
    if (!replfunc)
        return false;

    // EmitWarnings(fi, funcname);

    int count = 0;
    count += replacePointers(base, addr, addr(replfunc));
    count += replaceRelcalls(base, addr, addr(replfunc));
    return (count > 0);
}

// Replace references to the string 'from' with references to the string 'to'.
// 'to' must be a literal or otherwise permanent, as this function replaces all
// pointers to the string.
int replaceString(addr_t base, const char* from, const char* to)
{
    AddrList* al = findAllStrings(base, from);

    int count = 0;
    for (uint32_t i = 0; i < al->num; i++) {
        count += replacePointers(base, al->addrs[i], addr(to));
    }

    return count;
}

int replaceVariable(addr_t base, addr_t from, const void* to)
{
    return replacePointers(base, from, addr(to));
}
