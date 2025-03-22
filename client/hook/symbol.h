#pragma once

#include "codegen/codegen.h"
#include "loader/loader.h"

enum SymbolFindEnum {
    SYMBOL_FIND_LIST_END      = 0,   // implicit end of the list
    SYNBOL_FIND_FIXED_ADDRESS = 1,   // never use this
    SYMBOL_FIND_EXPORT,              // look in executable's export table
    SYMBOL_FIND_IMPORT,              // imported from a DLL
    SYMBOL_FIND_STRING,              // find in string table
};

typedef struct SymbolFind {
    int type;           // from SymbolFindEnum
    const char* name;   // name as it appears in import/export table

    // For SYMBOL_FIND_IMPORT, the name of the library it's located in
    // For SYMBOL_FIND_STRING, the literal string to search for
    const char* str;
} SymbolFind;

typedef struct Symbol {
    addr_t addr;         // address of this symbol if we've found it
    SymbolFind find[];   // 0-terminated array of ways to find this symbol in preference order
} Symbol;

// symbol info macro
#define SYM(name)     _sym_##name
#define DECLSYM(name) extern Symbol SYM(name)

addr_t getExport(addr_t base, const char* name);
bool _symResolve(addr_t base, Symbol* sym);
#define symResolve(base, name) _symResolve(base, &SYM(name));

#define symAddr(base, name) \
    (SYM(name).addr ? SYM(name).addr : (_symResolve(base, &SYM(name)), SYM(name).addr))
#define symPtr(type, base, name) ((type*)symAddr(base, name))
