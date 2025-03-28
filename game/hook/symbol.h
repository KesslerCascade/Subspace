#pragma once

#include "codegen/codegen.h"
#include "loader/loader.h"

enum SymbolFindEnum {
    SYMBOL_FIND_LIST_END      = 0,   // implicit end of the list
    SYNBOL_FIND_FIXED_ADDRESS = 1,   // never use this
    SYMBOL_FIND_EXPORT,              // look in executable's export table
    SYMBOL_FIND_IMPORT,              // imported from a DLL
    SYMBOL_FIND_STRING,              // find in string table
    SYMBOL_FIND_DISASM,              // use a dissaembly trace to find the symbol
    SYMBOL_FIND_VIRTUAL,   // for C++ virtual functions, provide the vtable and offset symbols
};

typedef struct Symbol Symbol;
typedef struct DisasmTrace DisasmTrace;
typedef struct SymbolFind {
    int type;           // from SymbolFindEnum
    const char* name;   // name as it appears in import/export table

    // For SYMBOL_FIND_IMPORT, the name of the library it's located in
    // For SYMBOL_FIND_STRING, the literal string to search for
    const char* str;

    Symbol* vtable;   // vtable for SYMBOL_FIND_VIRTUAL
    Symbol* offset;   // offset for SYMBOL_FIND_VIRTUAL

    // for SYMBOL_FIND_DISASM, the trace to run. It shoould be set to output to this symbol
    DisasmTrace* disasm;
} SymbolFind;

typedef struct Symbol {
    addr_t addr;         // address of this symbol if we've found it
    bool resolved;       // has this symbol been successfully resolved
    SymbolFind find[];   // 0-terminated array of ways to find this symbol in preference order
} Symbol;

// symbol info macro
#define SYM(name)      _sym_##name
#define SYMP(name)     _sym_p_##name
#define DECLSYM(name)  extern Symbol SYM(name)
#define DECLSYMP(name) extern Symbol* SYMP(name)

#define SYMP_SELECT(pname, name)  \
    do {                          \
        SYMP(pname) = &SYM(name); \
    } while (0)

addr_t getExport(addr_t base, const char* name);
bool _symResolve(addr_t base, Symbol* sym);
#define symResolve(base, name)  _symResolve(base, &SYM(name))
#define symResolveP(base, name) _symResolve(base, SYMP(name))

#define _symAddr(base, sym) \
    ((sym)->resolved ? (sym)->addr : (_symResolve(base, sym) ? (sym)->addr : 0))
#define symAddr(base, name)       _symAddr(base, &SYM(name))
#define symAddrP(base, name)      _symAddr(base, SYMP(name))
#define symPtr(type, base, name)  ((type*)symAddr(base, name))
#define symPtrP(type, base, name) ((type*)symAddrP(base, name))

// internal use only, os-specific
void symFindLib(addr_t base, Symbol* sym, SymbolFind* find);
