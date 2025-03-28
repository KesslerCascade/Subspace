#include "cchain.h"
#include "closure_private.h"
#include <cx/thread/aspin.h>

// This module used to use a global lock to handle thread safety. However this became quite
// inefficient once there was a mix of renewable and one-shot chains.

// Now it uses sentinel values with a strict compare-and-exchange regime to implement simple
// spinlocks using the pointers themselves. This is slightly worse for the case with many
// concurrent calls to the same closure chain as they are now serialized, but better for
// the general use case as the locking is specific to a chain rather than global.

// Sentinel value for a chain that's been invalidated for future use.
#define CCNODE_INVALID ((CChainNode*)-1)
// Sentinel value for a chain that is currently busy
#define CCNODE_BUSY ((CChainNode*)-2)

static CChainNode* ccnodeFree(_In_ CChainNode* node)
{
    for (int i = node ? node->nvars - 1 : -1; i >= 0; --i) {
        stvarDestroy(&node->cvars[i]);
    }

    CChainNode *prev = node->prev;
    xaFree(node);
    return prev;
}

// Get the head node of a chain and mark it busy (or invalid)
_Success_(return)
static bool cchainAcquire(_Inout_ cchain* chain, _Out_ CChainNode** onode, bool invalidate)
{
    AdaptiveSpinState astate = { 0 };
    CChainNode* out          = atomicLoad(ptr, (atomic(ptr)*)chain, Relaxed);
    CChainNode* nstate       = invalidate ? CCNODE_INVALID : CCNODE_BUSY;

    for (;;) {
        // another thread beat us to it and invalidated the chain
        if (out == CCNODE_INVALID)
            return false;

        // chain is busy, spinloop
        if (out == CCNODE_BUSY) {
            aspinHandleContention(NULL, &astate);   // abuse contention code for backoff
            out = atomicLoad(ptr, (atomic(ptr)*)chain, Relaxed);
        } else {
            // try to swap it for desired
            if (atomicCompareExchange(ptr, weak, (atomic(ptr)*)chain, (void**)&out, nstate, Acquire, Relaxed))
                break;
            aspinHandleContention(NULL, &astate);
        }
    }
    aspinEndContention(&astate);

    *onode = out;
    return true;
}

// Release a busy chain
static void cchainRelease(_Inout_ cchain* chain, _In_opt_ CChainNode *node)
{
    AdaptiveSpinState astate = { 0 };
    // Write the original value back. This SHOULD be uncontended, but still have to CAS it anyway.

    CChainNode* cur = CCNODE_BUSY;      // only valid starting point for this function
    while (!atomicCompareExchange(ptr, weak, (atomic(ptr)*)chain, (void**)&cur, node, Release, Relaxed)) {
        if (!devVerifyMsg(cur == CCNODE_BUSY, "Incorrect use of cchainRelease"))
            return;
        aspinHandleContention(NULL, &astate);
    }
    aspinEndContention(&astate);
}

_Use_decl_annotations_
bool _cchainAttach(cchain *chain, closureFunc func, intptr token, int n, stvar cvars[])
{
    CChainNode *nchain = xaAlloc(sizeof(CChainNode) + sizeof(stvar) * (n + (token ? 1 : 0)));
    nchain->func = func;
    nchain->nvars = n + (token ? 1 : 0);
    nchain->token = token;

    for(int i = 0; i < n; i++) {
        stvarCopy(&nchain->cvars[i], cvars[i]);
    }
    if(token)
        nchain->cvars[n] = stvar(intptr, token);

    CChainNode* cur;
    if (cchainAcquire(chain, &cur, false)) {
        nchain->prev = cur;
        cchainRelease(chain, nchain);
        return true;
    } else {
        ccnodeFree(nchain);
        return false;
    }
}

_Use_decl_annotations_
bool cchainDetach(cchain* chain, closureFunc func, intptr token)
{
    CChainNode* top;

    if (cchainAcquire(chain, &top, false)) {
        CChainNode** curptr = &top;
        CChainNode* cur     = top;
        bool ret            = false;

        // walk up the list to find the callback to remove
        while (cur) {
            if (cur->func == func && cur->token == token)
                break;
            curptr = &cur->prev;
            cur    = cur->prev;
        }

        if (cur) {
            // found one? remove it from the chain
            *curptr = cur->prev;   // this will modify either top or the parent prev pointer
            ccnodeFree(cur);
            ret = true;
        }

        cchainRelease(chain, top);   // may write modified top
        return ret;
    }

    return false;
}

_Use_decl_annotations_
bool cchainClone(cchain* destchain, cchain* srcchain)
{
    CChainNode *src, *dest;

    if (cchainAcquire(destchain, &dest, false)) {
        CChainNode* origdest = dest;
        if (cchainAcquire(srcchain, &src, false)) {
            CChainNode** dptr = &dest;
            CChainNode* snode = src;

            while (snode) {
                CChainNode* nnode = xaAlloc(sizeof(CChainNode) + snode->nvars * sizeof(stvar));
                nnode->func       = snode->func;
                nnode->token      = snode->token;
                nnode->nvars      = snode->nvars;

                for (int i = 0; i < snode->nvars; i++) {
                    stvarCopy(&nnode->cvars[i], snode->cvars[i]);
                }

                *dptr = nnode;
                dptr  = &nnode->prev;
                snode = snode->prev;
            }

            *dptr = origdest;   // insert cloned nodes before the original dest
            
            cchainRelease(srcchain, src);
        }
        
        cchainRelease(destchain, dest);
        return true;
    } else {
        atomicStore(ptr, (atomic(ptr)*)destchain, NULL, Relaxed);
        return false;
    }
}

_Use_decl_annotations_
bool _cchainCall(cchain *chain, int n, stvar args[])
{
    cchain tempchain = { 0 };
    CChainNode *cur;
    bool ret = true;

    stvlist stv_cvars;
    stvlist stv_args;

    // Because the callback can take an indeterminate amount of time to complete, for the non
    // oneshot call, clone the chain so that it can be called without holding it in a busy
    // state for as long.
    if (cchainClone(&tempchain, chain)) {
        cur = atomicLoad(ptr, (atomic(ptr)*)&tempchain, Relaxed);
        while (cur) {
            stvlInit(&stv_cvars, cur->nvars, cur->cvars);
            stvlInit(&stv_args, n, args);
            ret &= cur->func(&stv_cvars, &stv_args);

            cur = ccnodeFree(cur);
        }

        return ret;
    } else {
        return false;
    }
}

_Use_decl_annotations_
bool _cchainCallOnce(cchain* chain, int n, stvar args[])
{
    CChainNode *cur;
    bool ret = true;

    stvlist stv_cvars;
    stvlist stv_args;

    // Atomically invalidate the chain while getting the value, then we can be assured it's owned
    // exclusively and destroy it as we go.

    if (cchainAcquire(chain, &cur, true)) {
        while (cur) {
            stvlInit(&stv_cvars, cur->nvars, cur->cvars);
            stvlInit(&stv_args, n, args);
            ret &= cur->func(&stv_cvars, &stv_args);

            cur = ccnodeFree(cur);
        }

        // no need to call cchainRelease because it's invalid now
        return ret;
    } else {
        return false;
    }
}

_Use_decl_annotations_
bool cchainTransfer(cchain* dest, cchain* src)
{
    CChainNode *srcnode, *destnode;
    if (cchainAcquire(dest, &destnode, false)) {
        if (cchainAcquire(src, &srcnode, true)) {
            // Add whatever was in the original dest chain onto the transferred one, so that the
            // transferred chain gets effectively inserted.
            CChainNode** srcptr = &srcnode;
            while (*srcptr) {
                srcptr = &(*srcptr)->prev;
            }
            *srcptr = destnode;

            cchainRelease(dest, srcnode);
            return true;
        } else {
            cchainRelease(dest, destnode);
            return false;
        }
    } else {
        return false;
    }
}

_Use_decl_annotations_
bool cchainReset(cchain* chain)
{
    CChainNode* cur;

    cur = atomicLoad(ptr, (atomic(ptr)*)chain, Relaxed);

    do {
        if (cur != CCNODE_INVALID)
            return false;
    } while (!atomicCompareExchange(ptr, weak, (atomic(ptr)*)chain, (void**)&cur, NULL, Relaxed, Relaxed));

    return true;
}


_Use_decl_annotations_
bool cchainClear(cchain *chain)
{
    CChainNode *cur;

    if (cchainAcquire(chain, &cur, false)) {
        cchainRelease(chain, NULL);
        while(cur) {
            cur = ccnodeFree(cur);
        }
        return true;
    } else {
        return false;
    }
}

_Use_decl_annotations_
void cchainDestroy(cchain *chain)
{
    CChainNode *cur;

    if (cchainAcquire(chain, &cur, true)) {
        while(cur) {
            cur = ccnodeFree(cur);
        }
    }
}
