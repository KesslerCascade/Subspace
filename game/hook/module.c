#include "module.h"

#include <cx/string.h>
#include <cx/thread.h>
#include <cx/utils/lazyinit.h>
#include "log/log.h"

// bookkeeping for per-module info

static LazyInitState modulehash_is_init;
static Mutex modulehash_lock;
static hashtable modulehash;

static bool no_more_analysis;

AddrList* addrListFindByPtr(hashtable ht, addr_t addr)
{
    htelem elem = htFind(ht, uintptr, addr, none, NULL);
    if (elem)
        return (AddrList*)hteValPtr(ht, sarray, elem);
    return NULL;
}

AddrList* addrListFindByStr(hashtable ht, strref str)
{
    htelem elem = htFind(ht, strref, str, none, NULL);
    if (elem)
        return (AddrList*)hteValPtr(ht, sarray, elem);
    return NULL;
}

void addrListAddByPtr(hashtable* ht, addr_t addr, addr_t val)
{
    htelem elem = htFind(*ht, uintptr, addr, none, NULL);
    if (!elem) {
        sa_uintptr nsa;
        saInit(&nsa, uintptr, 8);
        elem = htInsert(ht, uintptr, addr, sarray, nsa);
        saDestroy(&nsa);
    }

    sa_uintptr* l = (sa_uintptr*)hteValPtr(*ht, sarray, elem);
    saPush(l, uintptr, val);
}

void addrListAddByStr(hashtable* ht, strref str, addr_t val)
{
    htelem elem = htFind(*ht, strref, str, none, NULL);
    if (!elem) {
        sa_uintptr nsa;
        saInit(&nsa, uintptr, 8);
        elem = htInsert(ht, strref, str, sarray, nsa);
        saDestroy(&nsa);
    }

    sa_uintptr* l = (sa_uintptr*)hteValPtr(*ht, sarray, elem);
    saPush(l, uintptr, val);
}

static void modulehash_init(void* dummy)
{
    htInit(&modulehash, uintptr, object, 16);
    mutexInit(&modulehash_lock);
}

ModuleInfo* moduleInfo(addr_t base)
{
    lazyInit(&modulehash_is_init, modulehash_init, NULL);
    ModuleInfo* mi = NULL;

    withMutex (&modulehash_lock) {
        if (!htFind(modulehash, uintptr, base, object, &mi, HT_Borrow)) {
            ModuleInfo* ninfo = moduleinfoCreate();
            htInsert(&modulehash, uintptr, base, object, ninfo);
            mi = ninfo;   // borrow the ref owned by the hashtable
            objRelease(&ninfo);
        }

        if (!mi->analyzed && !no_more_analysis) {
            analyzeModule(base, mi);
            mi->analyzed = true;
        }

#ifdef _DEBUG
        if (!mi->analyzed && no_more_analysis) {
            log_fmt(LOG_Debug, "Module at %p already analyzed", (void*)base);
        }
#endif
    }
    return mi;
}

void cleanupAnalysis()
{
    withMutex (&modulehash_lock) {
        htClear(&modulehash);
        no_more_analysis = true;
    }
}
