#pragma once
#include "codegen/codegen.h"
#include "hook/symbol.h"
#include "loader/loader.h"

typedef struct AddrList {
    uint32_t num;
    addr_t* addrs;
} AddrList;

AddrList* addrListCreate(void);
void addrListAdd(AddrList* l, addr_t addr);

// These should only be used when patching is in progress!

int replaceString(addr_t base, const char* from, const char* to);
int replaceVariable(addr_t base, addr_t from, const void* to);

// pre is called before, if it returns 0, the original function is not called.
// post is called after, with an extra first argument that is the return of the original.
// Whatever post returns is substituted.
#ifdef _DEBUG
#define hookFunction(base, func, pre, post) \
    _hookFunction(base, symAddr(base, func), &FUNCINFO(func), pre, post, #func)
#define hookFunctionP(base, func, pre, post) \
    _hookFunction(base, symAddrP(base, func), FUNCINFOP(func), pre, post, #func)
bool _hookFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* pre, void* post,
                   const char* funcname);
#else
#define hookFunction(base, func, pre, post) \
    _hookFunction(base, symAddr(base, func), &FUNCINFO(func), pre, post)
#define hookFunctionP(base, func, pre, post) \
    _hookFunction(base, symAddrP(base, func), FUNCINFOP(func), pre, post)
bool _hookFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* pre, void* post);
#endif

// Just a simple (hah!) replacement
#ifdef _DEBUG
#define replaceFunction(base, func, replacement) \
    _replaceFunction(base, symAddr(base, func), &FUNCINFO(func), replacement, #func)
#define replaceFunctionP(base, func, replacement) \
    _replaceFunction(base, symAddrP(base, func), FUNCINFOP(func), replacement, #func)
bool _replaceFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* replacement,
                      const char* funcname);
#else
#define replaceFunction(base, func, replacement) \
    _replaceFunction(base, symAddr(base, func), &FUNCINFO(func), replacement)
#define replaceFunctionP(base, func, replacement) \
    _replaceFunction(base, symAddrP(base, func), FUNCINFOP(func), replacement)
bool _replaceFunction(addr_t base, addr_t addr, const FuncInfo* fi, void* replacement);
#endif
