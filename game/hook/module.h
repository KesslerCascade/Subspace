#pragma once

#include "loader/loader.h"
#include "minicrt.h"

typedef struct AddrList {
    uint32_t num;
    addr_t* addrs;
} AddrList;

AddrList* addrListCreate(void);
void addrListAdd(AddrList* l, addr_t addr);
void addrListSort(AddrList* l);
void addrListDestroy(AddrList* l);

typedef struct ModuleInfo {
    // Hash table of exported function names to function addresses
    // VAL: int
    hashtbl exporthash;

    // Hash table of library names to hash table of imported function names to import function jump
    // table entries
    // VAL: hashtbl[int]
    hashtbl importhash;

    // Hash table of function addresses to address list of all places that function is the target of
    // a relative CALL instruction. Note this points to the function address, not the start of the
    // CALL instruction.
    // VAL: AddrList*
    hashtbl relcallhash;

    // Hash table of string values to addresses in the string table
    // VAL: AddrList*
    hashtbl stringhash;

    // Hash table of addresses that have been found to be valid strings; used to know which pointers
    // can be safely followed during code analysis.
    // VAL: char* (loc in module)
    hashtbl stringlochash;

    // Hash table of string values to address list of all instructions in the code section that
    // reference the string
    // VAL: AddrList*
    hashtbl stringrefhash;

    // Hash table of all relocations for fast lookup during code analysis. Also records the original
    // value of the pointer, though it's not currently used for anything.
    // VAL: addr_t
    hashtbl relochash;

    // Hash table of pointer targets to address list of all pointers with that value, including
    // pointers in static data.
    // VAL: AddrList*
    hashtbl ptrhash;

    // Hash table of pointer targets to address list of all *instructions* in the code section that
    // reference that destination
    // VAL: AddrList*
    hashtbl ptrrefhash;

    // Hash table of function addresses to address list of all instructions in the code section that
    // call it.
    // VAL: AddrList*
    hashtbl funccallhash;

    // Sorted list of addresses of known/suspected functions. This includes functions that are the
    // target of relative CALls, as well as addresses in the code section that are referenced by
    // pointers in the data section, which are likely to be function pointers of vtables.
    AddrList* funclist;

    bool init;
} ModuleInfo;

ModuleInfo* moduleInfo(addr_t base);

bool analyzeModule(addr_t base, ModuleInfo* mi);

// free analysis info for all modules and block future use
void cleanupAnalysis();