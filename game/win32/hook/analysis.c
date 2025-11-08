#include <cx/log.h>

#include "hook/module.h"

#include "hook/hook.h"
#include "loader/rtld.h"
#include "disasm.h"

#include <windows.h>

typedef struct ImportTrackerEnt {
    const char* lib;
    const char* funcname;
} ImportTrackerEnt;

static bool scanExports(addr_t base, hashtable* tbl)
{
    IMAGE_DATA_DIRECTORY* data   = &datadir(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
    IMAGE_EXPORT_DIRECTORY* etbl = datadirptr(base, data, IMAGE_EXPORT_DIRECTORY);
    addr_t* nametbl              = prva(base, etbl->AddressOfNames);
    WORD* ordinaltbl             = prva(base, etbl->AddressOfNameOrdinals);
    addr_t* addrtbl              = prva(base, etbl->AddressOfFunctions);

    for (int i = 0; i < etbl->NumberOfNames; i++) {
        DWORD ordinal = ordinaltbl[i];
        if (ordinal < etbl->NumberOfFunctions) {
            htInsert(tbl,
                     strref,
                     (strref)prva(base, nametbl[i]),
                     uintptr,
                     rva(base, addrtbl[ordinal]));
        }
    }

    return true;
}

static void addImportTracker(hashtable* tbl, const char* lib, const char* funcname, addr_t loc)
{
    ImportTrackerEnt* ent = xaAlloc(sizeof(ImportTrackerEnt));
    ent->lib              = lib;
    ent->funcname         = funcname;

    htInsert(tbl, uintptr, loc, ptr, ent);
}

static bool scanImports(addr_t base, hashtable* tbl)
{
    IMAGE_DATA_DIRECTORY* data = &nthdr(base)
                                      ->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    IMAGE_IMPORT_DESCRIPTOR* imp = datadirptr(base, data, IMAGE_IMPORT_DESCRIPTOR);
    IMAGE_IMPORT_DESCRIPTOR* end = datadirend(base, data, IMAGE_IMPORT_DESCRIPTOR);

    logBatchBegin();
    logFmt(Diag,
           _S"Scanning import table (${0uint(8,hex)}-${0uint(8,hex)})",
           stvar(uintptr, (uintptr)imp - base),
           stvar(uintptr, (uintptr)end - base));

    while (imp < end && imp->Name) {
        DWORD* lookup       = dwprva(base,
                               imp->OriginalFirstThunk ? imp->OriginalFirstThunk : imp->FirstThunk);
        DWORD* addr         = dwprva(base, imp->FirstThunk);
        const char* libname = chprva(base, imp->Name);

        while (*lookup) {
            DWORD iname = *lookup;
            if (!iname)
                break;

            if (!IMAGE_SNAP_BY_ORDINAL(iname) && *addr) {
                // subtract 1 from addr to position at the jump instruction
                addImportTracker(tbl, libname, chprva(base, iname + sizeof(WORD)), (addr_t)addr);
            }

            ++lookup;
            ++addr;
        }

        ++imp;
    }

    logFmt(Diag, _S"Found ${int} imports", stvar(int32, htSize(*tbl)));
    logBatchEnd();

    return 1;
}

static bool scanStrings(addr_t base, ModuleInfo* mi)
{
    SegInfo rdata;
    sa_uintptr relocs;
    if (!getRDataSeg(base, &rdata))
        return false;

    logBatchBegin();
    logFmt(Diag,
           _S"Scanning rdata segment (${0uint(8,hex)}-${0uint(8,hex)})",
           stvar(uintptr, rdata.start - base),
           stvar(uintptr, rdata.end - base));

    // build a sorted list of relocation addresses
    saInit(&relocs, uintptr, 1024, SA_Sorted);
    foreach (hashtable, hti, mi->relochash) {
        saPush(&relocs, uintptr, htiKey(uintptr, hti));
    }

    int nptrs      = 0;
    int32 curreloc = 0;
    int32 nrelocs  = saSize(relocs);
    char* s        = (char*)rdata.start;
    char* p        = s;
    char* e        = (char*)rdata.end;
    while (p < e) {
        if (curreloc < nrelocs) {
            // fast forward the relocation array to the current position
            while (relocs.a[curreloc] < (uintptr)p) curreloc++;
            if ((uintptr)p == relocs.a[curreloc]) {
                // this is actually a pointer, not a string
                // skip over it
                p += 4;
                s = p;
                ++nptrs;
                continue;
            }
        }

        char c = *p;
        p++;
        if (c == '\0') {
            if (p - s >= 4) {
                htInsert(&mi->stringlochash, uintptr, (uintptr)s, ptr, s);
                addrListAddByStr(&mi->stringhash, (strref)s, (addr_t)s);
            }
            s = p;
        } else if (!((c >= 32 && c <= 126) || c == '\n')) {
            // non-ASCII character, start over
            s = p;
        }
    }

    logFmt(Diag,
           _S"Found ${int} strings in ${int} locations (skipped over ${int} pointers)",
           stvar(int32, htSize(mi->stringhash)),
           stvar(int32, htSize(mi->stringlochash)),
           stvar(int32, nptrs));
    logBatchEnd();

    saDestroy(&relocs);

    return true;
}

static bool scanRelocs(addr_t base, ModuleInfo* mi, hashtable* functrackers)
{
    IMAGE_BASE_RELOCATION* reloc = datadirptr(base,
                                              &datadir(base, IMAGE_DIRECTORY_ENTRY_BASERELOC),
                                              IMAGE_BASE_RELOCATION);
    IMAGE_BASE_RELOCATION* end   = datadirend(base,
                                            &datadir(base, IMAGE_DIRECTORY_ENTRY_BASERELOC),
                                            IMAGE_BASE_RELOCATION);
    SegInfo code;
    DWORD i;

    if (!getCodeSeg(base, &code))
        return false;

    logBatchBegin();
    logFmt(Diag,
           _S"Scanning relocation table (${0uint(8,hex)}-${0uint(8,hex)})",
           stvar(uintptr, (uintptr)reloc - base),
           stvar(uintptr, (uintptr)end - base));

    while (reloc < end && reloc->SizeOfBlock) {
        for (i = sizeof(IMAGE_BASE_RELOCATION); i < reloc->SizeOfBlock; i += sizeof(WORD)) {
            WORD offset = *(WORD*)((char*)reloc + i);
            WORD rtype  = offset >> 12;
            offset &= (1 << 12) - 1;
            if (rtype == IMAGE_REL_BASED_HIGHLOW) {
                long* addr = dwprva(base, reloc->VirtualAddress + offset);
                htInsert(&mi->relochash, uintptr, (uintptr)addr, uintptr, *addr);
                addrListAddByPtr(&mi->ptrhash, *addr, (addr_t)addr);

                // if the pointer points to something in the code segment, it's probably a function
                if (*addr >= code.size && *addr <= code.end) {
                    htInsert(functrackers, uintptr, *addr, int32, 1);
                }
            }
        }
        reloc = (IMAGE_BASE_RELOCATION*)((char*)reloc + reloc->SizeOfBlock);
    }

    logFmt(Diag, _S"Found ${int} relocations", stvar(int32, htSize(mi->relochash)));
    logBatchEnd();

    return true;
}

static void addImport(hashtable* tbl, const char* lib, const char* funcname, addr_t loc)
{
    htelem elem = htFind(*tbl, strref, (strref)lib, none, NULL);
    if (!elem) {
        hashtable nhash;
        htInit(&nhash, strref, uintptr, 16);
        elem = htInsert(tbl, strref, (strref)lib, hashtable, nhash);
        htDestroy(&nhash);
    }

    hashtable* fhash = hteValPtr(*tbl, hashtable, elem);
    htInsert(fhash, strref, (strref)funcname, uintptr, loc);
}

// basic code analysis via disassembly
static bool scanCode(addr_t base, ModuleInfo* mi, hashtable* importtrackers,
                     hashtable* functrackers)
{
    SegInfo code;
    SegInfo rdata;
    addr_t p;
    t_disasm disasm;

    if (!getCodeSeg(base, &code))
        return false;
    if (!getRDataSeg(base, &rdata))
        return false;

    logBatchBegin();
    logFmt(Diag,
           _S"Scanning code segment (${0uint(8,hex)}-${0uint(8,hex)})",
           stvar(uintptr, (uintptr)code.start - base),
           stvar(uintptr, (uintptr)code.end - base));

    p = code.start;
    while (p < code.end) {
        bool added = false;
        ulong ilen = Disasm((char*)p, min(MAXCMDSIZE, code.end - p), addr(p), &disasm, DISASM_FILE);

        // if this is a relative call, record it
        if (*(uint8_t*)p == 0xe8 ||   // E8 = relative CALL NEAR
            *(uint8_t*)p == 0xe9      // E9 = relative JMP NEAR (for trampolines)
        ) {
            int32_t reldest = (*(int32_t*)(p + 1));
            addr_t dest     = (p + 5) + reldest;

            // Only process addresses within the code segment.
            // We don't care about library calls anyway, and this helps filter out false
            // positives.
            if (dest >= code.start && dest <= code.end) {
                addrListAddByPtr(&mi->relcallhash, dest, p + 1);
                if (disasm.inst == I_CALL) {   // for CALL only, record it as a function call
                    addrListAddByPtr(&mi->funccallhash, dest, p);
                    htInsert(functrackers, uintptr, dest, int32, 2);
                }
                added = true;
            }
        }

        if (!added && disasm.inst == I_CALL) {
            // get absolute calls to addresses
            t_arg* arg = &disasm.arg[0];
            if (arg->base == REG_UNDEF && arg->idx == REG_UNDEF && arg->addr >= code.start &&
                arg->addr <= code.end) {
                addrListAddByPtr(&mi->funccallhash, disasm.arg[0].addr, p);
                htInsert(functrackers, uintptr, disasm.arg[0].addr, int32, 2);
                added = true;
            }
        }

        ImportTrackerEnt* trk;
        if (!added && disasm.inst == I_JMP &&
            htFind(*importtrackers, uintptr, disasm.arg[0].addr, ptr, &trk)) {
            // this is a jump into the thunk table; record its location so imported symbols can
            // be used as waypoints in the code
            addImport(&mi->importhash, trk->lib, trk->funcname, p);
            added = true;
        }

        if (!added) {
            // check args for pointers
            for (int i = 0; i < 3; i++) {
                t_arg* arg = &disasm.arg[i];
                if (arg->base == REG_UNDEF && arg->idx == REG_UNDEF && arg->addr > 0) {
                    if (arg->addr >= rdata.start && arg->addr < rdata.end &&
                        htFind(mi->stringlochash, uintptr, arg->addr, none, NULL)) {
                        // reference to something that's in the string table
                        addrListAddByPtr(&mi->stringrefhash, arg->addr, p);
                    } else if (htFind(mi->ptrhash, uintptr, arg->addr, none, NULL)) {
                        // reference to some other relocatable pointer
                        addrListAddByPtr(&mi->ptrrefhash, arg->addr, p);
                    }
                }
            }
        }

        p += ilen;
    }

    logFmt(Diag,
           _S"Found ${int} relative call/jump destinations",
           stvar(int32, htSize(mi->relcallhash)));
    logFmt(Diag, _S"Found ${int} call destinations", stvar(int32, htSize(mi->funccallhash)));
    logFmt(Diag, _S"Found ${int} imported symbol references", stvar(int32, htSize(mi->importhash)));
    logFmt(Diag, _S"Found ${int} string references", stvar(int32, htSize(mi->stringrefhash)));
    logFmt(Diag, _S"Found ${int} pointer references", stvar(int32, htSize(mi->ptrrefhash)));
    logBatchEnd();

    return true;
}

static void checkFunctions(addr_t base, hashtable* trackers, AddrList* funcs)
{
    SegInfo code;
    t_disasm disasm;

    if (!getCodeSeg(base, &code))
        return;

    logBatchBegin();
    logFmt(Diag, _S"Checking ${int} potential functions", stvar(int32, htSize(*trackers)));

    foreach (hashtable, hti, *trackers) {
        int32 typ;
        addr_t addr;

        typ  = htiVal(int32, hti);
        addr = htiKey(uintptr, hti);

        if (typ == 1) {
            // 1 is from a pointer found in data, check if it's likely a function (could also be a
            // jumptable target)

            // disassemble the first instruction
            ulong ilen = Disasm((char*)addr,
                                min(MAXCMDSIZE, code.end - addr),
                                addr,
                                &disasm,
                                DISASM_FILE);

            // if it's a PUSH instruction it's very likely a function preamble
            if (disasm.error == 0 && disasm.inst == I_PUSH)
                saPush(funcs, uintptr, addr);
        } else if (typ == 2) {
            // 2 is a CALL target and is guaranteed to be a function
            saPush(funcs, uintptr, addr);
        }
    }

    logFmt(Diag, _S"Identified ${int} functions", stvar(int32, saSize(*funcs)));
    logBatchEnd();
}

bool analyzeModule(addr_t base, ModuleInfo* mi)
{
    bool ret = false;
    hashtable importtrackers;
    htInit(&importtrackers, uintptr, ptr, 64);
    hashtable functrackers;
    htInit(&functrackers, uintptr, int32, 64);

    logBatchBegin();
    logStr(Info, _S"Starting code analysis");

    if (!scanImports(base, &importtrackers))
        goto out;
    if (!scanExports(base, &mi->exporthash))
        goto out;
    if (!scanRelocs(base, mi, &functrackers))
        goto out;
    if (!scanStrings(base, mi))
        goto out;
    if (!scanCode(base, mi, &importtrackers, &functrackers))
        goto out;

    checkFunctions(base, &functrackers, &mi->funclist);

    ret = true;

out:
    if (ret)
        logStr(Info, _S"Code analysis complete");
    else
        logStr(Error, _S"Code analysis failed");
    logBatchEnd();

    htDestroy(&functrackers);
    foreach (hashtable, hti, importtrackers) {
        ImportTrackerEnt* trk = htiVal(ptr, hti);
        xaFree(trk);
    }
    htDestroy(&importtrackers);

    return ret;
}
