#include <windows.h>
#include "disasm.h"
#include "minicrt.h"

#include "hook/hook.h"
#include "hook/module.h"
#include "hook/string.h"
#include "loader/win32/rtld.h"

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

static bool addFunction(hashtbl* tbl, addr_t func, addr_t loc)
{
    AddrList* fcl = hashtbl_get(tbl, func);
    if (!fcl) {
        fcl = addrListCreate();
        hashtbl_set(tbl, func, fcl);
    }
    addrListAdd(fcl, loc);

    return true;
}

// Do a simple code scan to find relative function calls, and toss them into some
// hash buckets.
static void relcallhash_init(addr_t base, hashtbl* tbl)
{
    SegInfo code;
    addr_t p;
    t_disasm disasm;

    if (!getCodeSeg(base, &code))
        return;

    hashtbl_init(tbl, 128, 0);

    p = code.start;
    while (p < code.end) {
        if (*(uint8_t*)p == 0xe8 ||   // E8 = relative CALL NEAR
            *(uint8_t*)p == 0xe9      // E9 = relative JMP NEAR
        ) {
            int32_t reldest = (*(int32_t*)(p + 1));
            addr_t dest     = (p + 5) + reldest;

            // Only process addresses within the code segment.
            // We don't care about library calls anyway, and this helps filter out false positives.
            if (dest >= code.start && dest <= code.end) {
                if (!addFunction(tbl, dest, p + 1))
                    return;
            }
        }
        p += Disasm((char*)p, min(MAXCMDSIZE, code.end - p), addr(p), &disasm, DISASM_SIZE);
    }
}

// Tricksy trick: The relocation table contains every address in the program that needs to
// be adjusted should the base address change. Which conveniently means that it hands us
// a list of every pointer in both code and static data, as well as absolute jumps.
// So do a simple search and replace. This won't get relative calls, unfortunately, but
// does take care of the otherwise hard to detect function pointers, so we can brute force
// the relative calls.
static int replacePointers(addr_t base, addr_t oldfunc, addr_t newfunc)
{
    IMAGE_BASE_RELOCATION* reloc = datadirptr(base,
                                              &datadir(base, IMAGE_DIRECTORY_ENTRY_BASERELOC),
                                              IMAGE_BASE_RELOCATION);
    IMAGE_BASE_RELOCATION* end   = datadirend(base,
                                            &datadir(base, IMAGE_DIRECTORY_ENTRY_BASERELOC),
                                            IMAGE_BASE_RELOCATION);
    DWORD i;
    int count = 0;

    while (reloc < end && reloc->SizeOfBlock) {
        for (i = sizeof(IMAGE_BASE_RELOCATION); i < reloc->SizeOfBlock; i += sizeof(WORD)) {
            WORD offset = *(WORD*)((char*)reloc + i);
            WORD rtype  = offset >> 12;
            offset &= (1 << 12) - 1;
            if (rtype == IMAGE_REL_BASED_HIGHLOW) {
                long* addr    = dwprva(base, reloc->VirtualAddress + offset);
                DWORD relbase = addr(addr) + 4;
                if (*addr == oldfunc) {
                    *addr = newfunc;
                    ++count;
                }
            }
        }
        reloc = (IMAGE_BASE_RELOCATION*)((char*)reloc + reloc->SizeOfBlock);
    }
    return count;
}

// Scan the cached table of relative call destinations and find any that need to
// be replaced.
static void replaceRelcalls(addr_t base, addr_t oldfunc, addr_t newfunc)
{
    hashtbl* relcallhash = moduleRelcallHash(base, relcallhash_init);
    AddrList* fcl        = hashtbl_get(relcallhash, oldfunc);
    if (fcl) {
        for (uint32_t i = 0; i < fcl->num; i++) {
            addr_t p      = fcl->addrs[i];
            *(addr_t*)(p) = newfunc - (p + 4);
        }
    }
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

#ifdef DEBUG
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

    replacePointers(base, addr, addr(hookfunc));
    replaceRelcalls(base, addr, addr(hookfunc));
    return true;
}

#ifdef DEBUG
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

    replacePointers(base, addr, addr(replfunc));
    replaceRelcalls(base, addr, addr(replfunc));
    return true;
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
