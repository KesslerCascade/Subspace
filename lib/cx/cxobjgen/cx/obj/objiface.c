#include "obj_private.h"
#include "cx/debug/assert.h"
#include "cx/utils/compare.h"

typedef void(*genfunc)();

// dummy structure to get correct offset in case of insane compiler struct padding
typedef struct iface_hdr {
    ObjIface iface;
    genfunc first;
} iface_hdr;
#define IFACE_FIRST_FPTR offsetof(iface_hdr, first)

static int ifNumFuncs(_In_ ObjIface *iface)
{
    // size of anything extra after interface header
    size_t sze = iface->_size - IFACE_FIRST_FPTR;
    if (sze % sizeof(genfunc) != 0) {
        // not an even multiple of the function pointer size? somebody screwed up!
        devFatalError("Invalid interface structure size!");
        return 0;
    }

    return (int)(sze / sizeof(genfunc));
}

// merge interface implementations from src to dest, ignoring any functions that are
// already populated in dest
static void mergeIface(_Inout_ ObjIface *dest, _In_ ObjIface *src)
{
    genfunc *intbl, *outtbl;
    int nfsrc, nfdst;
    int i;

    nfsrc = ifNumFuncs(src);
    nfdst = ifNumFuncs(dest);
    intbl = (genfunc*)(((uintptr_t)src) + IFACE_FIRST_FPTR);
    outtbl = (genfunc*)(((uintptr_t)dest) + IFACE_FIRST_FPTR);
    for (i = min(nfsrc, nfdst) - 1; i >= 0; --i) {
        if (!outtbl[i])
            outtbl[i] = intbl[i];
    }
}

// ---------- Interface Hydration ----------
// Use a single interface implementation to populate the method pointer table for
// the interface *and all of its parent interfaces*

// impls is a handle to an array of pointers to interfaces that have been hydrated
// so far. The impltbl hashtable maps the interface template pointer to the
// implementation.
void _objHydrateIface(_In_ ObjIface *ifimpl, _Inout_ sa_ObjIface *impls, _Inout_ hashtable *impltbl)
{
    ObjIface *destif = NULL, *tmp = NULL;

    devAssert(ifimpl->_implements);
    if (!htFind(*impltbl, ptr, ifimpl->_implements, ptr, &destif)) {
        // we didn't find an existing table entry, so add one
        destif = xaAlloc(ifimpl->_size, XA_Zero);
        destif->_implements = ifimpl->_implements;
        destif->_size = ifimpl->_size;

        // It's unlikely, but possible, that a parent interface of ours is already
        // present in the implementation table. An example case where this can happen:
        // IfA -> IfB
        // ClsA -> ClsB
        // ClsA implements IfB
        // ClsB implements IfA

        // Since interface hydration starts at the leaves of the tree (ClsB) and
        // proceeds upward, work around this edge case during hydration of ClsA's
        // interfaces into ClsB's table by checking if a parent of the interface already
        // exists, and copying its function pointers if so. This will result in both IfA
        // and IfB containing pointers to ClsB's implementation.

        for (ObjIface *pif = ifimpl->_implements->_parent; pif; pif = pif->_parent) {
            if (htFind(*impltbl, ptr, pif, ptr, &tmp))
                mergeIface(destif, tmp);
        }

        saPush(impls, ptr, destif);
    }

    // merge in the interface we are implementing
    mergeIface(destif, ifimpl);

    // register interface tables for this interface and all its parents,
    // or merge them with tables registered by child classes
    for (ObjIface *pif = destif->_implements; pif; pif = pif->_parent) {
        if (htFind(*impltbl, ptr, pif, ptr, &tmp))
            mergeIface(tmp, destif);
        else
            htInsert(impltbl, ptr, pif, ptr, destif);
    }
}

bool _objCheckIface(_In_ ObjIface *iface)
{
    genfunc *tbl;
    int nf;
    int i;

    nf = ifNumFuncs(iface);
    tbl = (genfunc*)(((uintptr_t)iface) + IFACE_FIRST_FPTR);
    for (i = nf - 1; i >= 0; --i) {
        if (!tbl[i])
            return false;
    }
    return true;
}
