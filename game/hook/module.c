#include "module.h"

// bookkeeping for per-module info

static lazy_init modulehash_is_init;
static lock_t modulehash_lock;
static hashtbl modulehash;

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

void addrListDestroy(AddrList* l)
{
    sfree(l->addrs);
    sfree(l);
}

static void modulehash_init(void* dummy)
{
    hashtbl_init(&modulehash, 8, 0);
    lock_init(&modulehash_lock);
}

ModuleInfo* moduleInfo(addr_t base)
{
    lazyinit(&modulehash_is_init, modulehash_init, NULL);
    lock_acq(&modulehash_lock);

    ModuleInfo* mi = hashtbl_get(&modulehash, base);
    if (!mi) {
        mi = scalloc(1, sizeof(ModuleInfo));
        hashtbl_set(&modulehash, base, mi);
    }

    if (!mi->init) {
        hashtbl_init(&mi->exporthash, 256, HT_STRING_KEYS);
        hashtbl_init(&mi->importhash, 8, HT_STRING_KEYS | HT_CASE_INSENSITIVE);
        hashtbl_init(&mi->stringhash, 256, HT_STRING_KEYS);
        hashtbl_init(&mi->stringrefhash, 256, HT_STRING_KEYS);
        hashtbl_init(&mi->stringlochash, 256, 0);
        hashtbl_init(&mi->relochash, 256, 0);
        hashtbl_init(&mi->ptrhash, 256, 0);
        hashtbl_init(&mi->ptrrefhash, 256, 0);
        hashtbl_init(&mi->relcallhash, 256, 0);
        hashtbl_init(&mi->funccallhash, 256, 0);
        mi->funclist = addrListCreate();
        analyzeModule(base, mi);
        addrListSort(mi->funclist);
        mi->init = true;
    }

    lock_rel(&modulehash_lock);
    return mi;
}
