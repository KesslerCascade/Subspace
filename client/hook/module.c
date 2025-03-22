#include "module.h"

// bookkeeping for per-module info

static lazy_init modulehash_is_init;
static lock_t modulehash_lock;
static hashtbl modulehash;

typedef struct ModuleInfo {
    hashtbl exporthash;
    hashtbl relcallhash;
    hashtbl stringhash;
    bool exporthash_init;
    bool relcallhash_init;
    bool stringhash_init;
} ModuleInfo;

static void modulehash_init(void* dummy)
{
    hashtbl_init(&modulehash, 8, 0);
    lock_init(&modulehash_lock);
}

#define MODINFO_IMPL(funcname, hashname)                        \
    hashtbl* funcname(addr_t base, modinfo_initfunc_t initfunc) \
    {                                                           \
        lazyinit(&modulehash_is_init, modulehash_init, NULL);   \
        lock_acq(&modulehash_lock);                             \
                                                                \
        ModuleInfo* mi = hashtbl_get(&modulehash, base);        \
        if (!mi) {                                              \
            mi = scalloc(1, sizeof(ModuleInfo));                \
            hashtbl_set(&modulehash, base, mi);                 \
        }                                                       \
                                                                \
        if (!mi->hashname##_init) {                             \
            initfunc(base, &mi->hashname);                      \
            mi->hashname##_init = true;                         \
        }                                                       \
                                                                \
        lock_rel(&modulehash_lock);                             \
                                                                \
        return &mi->hashname;                                   \
    }

MODINFO_IMPL(moduleExportHash, exporthash)
MODINFO_IMPL(moduleRelcallHash, relcallhash)
MODINFO_IMPL(moduleStringHash, stringhash)
