#include <windows.h>
#include "hook/module.h"

#include "hook/hook.h"
#include "loader/rtld.h"
#include "disasm.h"

static bool scanExports(addr_t base, hashtbl* tbl)
{
    IMAGE_DATA_DIRECTORY* data   = &datadir(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
    IMAGE_EXPORT_DIRECTORY* etbl = datadirptr(base, data, IMAGE_EXPORT_DIRECTORY);
    addr_t* nametbl              = prva(base, etbl->AddressOfNames);
    WORD* ordinaltbl             = prva(base, etbl->AddressOfNameOrdinals);
    addr_t* addrtbl              = prva(base, etbl->AddressOfFunctions);

    for (int i = 0; i < etbl->NumberOfNames; i++) {
        DWORD ordinal = ordinaltbl[i];
        if (ordinal < etbl->NumberOfFunctions) {
            hashtbl_addint(tbl, (const char*)prva(base, nametbl[i]), rva(base, addrtbl[ordinal]));
        }
    }

    return true;
}

static void addString(hashtbl* tbl, const char* str, addr_t loc)
{
    AddrList* fcl = hashtbl_get(tbl, str);
    if (!fcl) {
        fcl = addrListCreate();
        hashtbl_set(tbl, str, fcl);
    }
    addrListAdd(fcl, loc);
}

static bool scanStrings(addr_t base, ModuleInfo* mi)
{
    SegInfo rdata;
    if (!getRDataSeg(base, &rdata))
        return false;

    char* s = (char*)rdata.start;
    char* p = s;
    char* e = (char*)rdata.end;
    while (p < e) {
        char c = *p;
        p++;
        if (c == '\0') {
            if (p - s >= 5) {
                hashtbl_set(&mi->stringlochash, s, s);
                addString(&mi->stringhash, (const char*)s, (addr_t)s);
            }
            s = p;
        } else if (!((c >= 32 && c <= 126) || c == '\n')) {
            // non-ASCII character, start over
            s = p;
        }
    }

    return true;
}

static void addPtr(hashtbl* tbl, addr_t ptr, addr_t loc)
{
    AddrList* fcl = hashtbl_get(tbl, ptr);
    if (!fcl) {
        fcl = addrListCreate();
        hashtbl_set(tbl, ptr, fcl);
    }
    addrListAdd(fcl, loc);
}

static bool scanRelocs(addr_t base, ModuleInfo* mi)
{
    IMAGE_BASE_RELOCATION* reloc = datadirptr(base,
                                              &datadir(base, IMAGE_DIRECTORY_ENTRY_BASERELOC),
                                              IMAGE_BASE_RELOCATION);
    IMAGE_BASE_RELOCATION* end   = datadirend(base,
                                            &datadir(base, IMAGE_DIRECTORY_ENTRY_BASERELOC),
                                            IMAGE_BASE_RELOCATION);
    DWORD i;

    while (reloc < end && reloc->SizeOfBlock) {
        for (i = sizeof(IMAGE_BASE_RELOCATION); i < reloc->SizeOfBlock; i += sizeof(WORD)) {
            WORD offset = *(WORD*)((char*)reloc + i);
            WORD rtype  = offset >> 12;
            offset &= (1 << 12) - 1;
            if (rtype == IMAGE_REL_BASED_HIGHLOW) {
                long* addr = dwprva(base, reloc->VirtualAddress + offset);
                hashtbl_setint(&mi->relochash, addr, *addr);
                addPtr(&mi->ptrhash, *addr, (addr_t)addr);
            }
        }
        reloc = (IMAGE_BASE_RELOCATION*)((char*)reloc + reloc->SizeOfBlock);
    }

    return true;
}

// basic code analysis via disassembly
static bool scanCode(addr_t base, ModuleInfo* mi)
{
    SegInfo code;
    SegInfo rdata;
    addr_t p;
    t_disasm disasm;

    if (!getCodeSeg(base, &code))
        return false;
    if (!getRDataSeg(base, &rdata))
        return false;

    p = code.start;
    while (p < code.end) {
        ulong ilen = Disasm((char*)p, MIN(MAXCMDSIZE, code.end - p), addr(p), &disasm, DISASM_FILE);

        // if this is a relative call, record it
        if (*(uint8_t*)p == 0xe8 ||   // E8 = relative CALL NEAR
            *(uint8_t*)p == 0xe9      // E9 = relative JMP NEAR (for trampolines)
        ) {
            int32_t reldest = (*(int32_t*)(p + 1));
            addr_t dest     = (p + 5) + reldest;

            // Only process addresses within the code segment.
            // We don't care about library calls anyway, and this helps filter out false positives.
            if (dest >= code.start && dest <= code.end) {
                addPtr(&mi->relcallhash, dest, p + 1);
                if (disasm.inst == I_CALL)   // for CALL only, record it as a function call
                    addPtr(&mi->funccallhash, dest, p);
            }
        } else if (disasm.inst == I_CALL) {
            // get absolute calls to addresses
            t_arg* arg = &disasm.arg[0];
            if (arg->base == REG_UNDEF && arg->idx == REG_UNDEF && arg->addr >= code.start &&
                arg->addr <= code.end) {
                addPtr(&mi->funccallhash, disasm.arg[0].addr, p);
            }
        } else {
            // check args for pointers
            for (int i = 0; i < 3; i++) {
                t_arg* arg = &disasm.arg[i];
                if (arg->base == REG_UNDEF && arg->idx == REG_UNDEF && arg->addr > 0) {
                    if (arg->addr >= rdata.start && arg->addr < rdata.end &&
                        hashtbl_find(&mi->stringlochash, arg->addr) != HT_NOT_FOUND) {
                        // reference to something that's in the string table
                        addPtr(&mi->stringrefhash, arg->addr, p);
                    } else if (hashtbl_find(&mi->ptrhash, arg->addr) != HT_NOT_FOUND) {
                        // reference to some other relocatable pointer
                        addPtr(&mi->ptrrefhash, arg->addr, p);
                    }
                }
            }
        }

        p += ilen;
    }

    return true;
}

bool analyzeModule(addr_t base, ModuleInfo* mi)
{
    if (!scanExports(base, &mi->exporthash))
        return false;
    if (!scanStrings(base, mi))
        return false;
    if (!scanRelocs(base, mi))
        return false;
    if (!scanCode(base, mi))
        return false;

    return true;
}
