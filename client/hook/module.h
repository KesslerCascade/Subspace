#include "loader/loader.h"
#include "minicrt.h"

typedef struct ModuleInfo {
    // Hash table of exported function names to function addresses
    hashtbl exporthash;

    // Hash table of function addresses to address list of all places that function is the target of
    // a relative CALL instruction. Note this points to the function address, not the start of the
    // CALL instruction.
    hashtbl relcallhash;

    // Hash table of string values to addresses in the string table
    hashtbl stringhash;

    // Hash table of addresses that have been found to be valid strings; used to know which pointers
    // can be safely followed during code analysis.
    hashtbl stringlochash;

    // Hash table of string values to address list of all instructions in the code section that
    // reference the string
    hashtbl stringrefhash;

    // Hash table of all relocations for fast lookup during code analysis. Also records the original
    // value of the pointer, though it's not currently used for anything.
    hashtbl relochash;

    // Hash table of pointer targets to address list of all pointers with that value, including
    // pointers in static data.
    hashtbl ptrhash;

    // Hash table of pointer targets to address list of all *instructions* in the code section that
    // reference that destination
    hashtbl ptrrefhash;

    // Hash table of function addresses to address list of all instructions in the code section that
    // call it.
    hashtbl funccallhash;
    bool init;
} ModuleInfo;

ModuleInfo* moduleInfo(addr_t base);

bool analyzeModule(addr_t base, ModuleInfo* mi);
