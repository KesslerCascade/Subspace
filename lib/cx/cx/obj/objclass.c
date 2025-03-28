#include "obj_private.h"
#include "cx/debug/assert.h"
#include "cx/thread/mutex.h"
#include "cx/utils/lazyinit.h"
#include "objstdif.h"

// simple mutex to prevent multiple threads from trying to init a class implemenation at
// the same time
static Mutex classMutex;
static LazyInitState classMutexState;

static void initClassMutex(void *unused)
{
    mutexInit(&classMutex);
}

static void hydrateIfaces(ObjClassInfo *cls, sa_ObjIface *impls, hashtable *impltbl)
{
    // Start with current class before moving on to parents.
    // The assumption is that classes deeper in the hierarchy will (usually)
    // implement interfaces that are the same or lower than their parents.
    for (ObjIface **iflistp = cls->ifimpl; *iflistp; ++iflistp) {
        _objHydrateIface(*iflistp, impls, impltbl);
    }

    // Hydrate skips functions that are already in the implementation table, so by
    // traversing children first we override parents. We recurse to here to populate
    // the child class's table with any parent class implementations that are not
    // overriden.
    if (cls->parent)
        hydrateIfaces(cls->parent, impls, impltbl);
}

bool _objCheckIfaces(sa_ObjIface impls)
{
    foreach(sarray, i, ObjIface*, impl, impls) {
        if (!_objCheckIface(impl))
            return false;
    }
    return true;
}

static void classInitImpl(ObjClassInfo *cls, bool locked)
{
    lazyInit(&classMutexState, initClassMutex, NULL);

    if (!locked) {
        mutexAcquire(&classMutex);

        // check and make sure another thread didn't win the race
        if (cls->_impl.a) {
            mutexRelease(&classMutex);
            return;
        }
    }

    // Runtime class information is populated only once per class during the lifetime
    // of the program. It's okay for this to be moderately expensive in terms of
    // computation, as well as to allocate and never free memory.

    sa_ObjIface impl;
    saInit(&impl, ptr, 4, SA_Grow(Minimal));
    htInit(&cls->_tmpl, ptr, ptr, 8, HT_Grow(At50) | HT_Compact);

    // Fully hydrated interface implementation tables include methods that are
    // implemented by the parent class, but not any children. Fill them in by recursing
    // the hierarchy.
    hydrateIfaces(cls, &impl, &cls->_tmpl);

    // Sanity check
    if (!cls->_abstract)
        devAssertMsg(_objCheckIfaces(impl), "Interface not fully implemented");

    // Perform some slight of hand and swap the classif from the template to the hydrated
    // method table.
    if (cls->classif)
        relAssert(htFind(cls->_tmpl, ptr, cls->classif, ptr, &cls->classif));

    // Go ahead and init parent class, in case child class needs to call parent
    // class functions by interface.
    if (cls->parent && !cls->parent->_impl.a)
        classInitImpl(cls->parent, true);

    // If this class implements Sortable or Hashable (even through a parent), cache the
    // function pointer to avoid having to check them again.
    Sortable *sortableIf;
    if (htFind(cls->_tmpl, ptr, &Sortable_tmpl, ptr, &sortableIf))
        cls->_cmp = sortableIf->cmp;

    Hashable *hashableIf;
    if (htFind(cls->_tmpl, ptr, &Hashable_tmpl, ptr, &hashableIf))
        cls->_hash = hashableIf->hash;

    // This must be done immediately before unlocking, since other threads check it
    // without holding the lock to determine if the class needs to be initialized.
    cls->_impl = impl;
    if (!locked)
        mutexRelease(&classMutex);
}

_Ret_notnull_ ObjInst *_objInstCreate(_In_ ObjClassInfo *cls)
{
    ObjInst *ret;

    devAssertMsg(!cls->_abstract, "Tried to create an instance of an abstract class");

    ret = xaAlloc(cls->instsize, XA_Zero);
    ret->_clsinfo = cls;
    atomicStore(uintptr, &ret->_ref, 1, Relaxed);

    // Initialize interface implementation tables the first time the class is
    // instantiated
    if (!cls->_impl.a)
        classInitImpl(cls, false);

    ret->_classif = cls->classif;

    // Note we do NOT call init here. Init functions do not take parameters, so this
    // allows the factory function to set up some per-instance data before calling
    // objInstInit.

    return ret;
}

bool _objInstInit(_Inout_ ObjInst *inst, _In_ ObjClassInfo *cls)
{
    bool ret = true;

    // init parent classes first
    if (cls->parent)
        ret &= _objInstInit(inst, cls->parent);

    if (cls->init)
        ret &= cls->init(inst);

    return ret;
}

_Ret_maybenull_ ObjIface *_objClassIf(_In_ ObjClassInfo *cls, _In_ ObjIface *iftmpl)
{
    ObjIface *ret = NULL;
    htFind(cls->_tmpl, ptr, iftmpl, ptr, &ret);
    return ret;
}

_Ret_maybenull_ ObjIface *_objInstIf(_In_opt_ ObjInst *inst, _In_ ObjIface *iftmpl)
{
    if (!inst)
        return NULL;

    return _objClassIf(inst->_clsinfo, iftmpl);
}

_Ret_maybenull_ ObjInst *_objDynCast(_In_opt_ ObjInst *inst, _In_ ObjClassInfo *cls)
{
    if (inst) {
        ObjClassInfo *test = inst->_clsinfo;
        while (test) {
            if (test == cls)
                return inst;
            test = test->parent;
        }
    }

    return NULL;
}

static void instDtor(_In_ ObjInst *inst, _In_ ObjClassInfo *cls)
{
    // call destructors on child classes first
    if (cls->destroy)
        cls->destroy(inst);

    if (cls->parent)
        instDtor(inst, cls->parent);
}

void _objDestroy(_Pre_notnull_ _Post_invalid_ ObjInst *inst)
{
    devAssert(atomicLoad(uintptr, &inst->_ref, Acquire) == 0);
    instDtor(inst, inst->_clsinfo);
    xaFree(inst);
}

_Use_decl_annotations_
void _objRelease(ObjInst **instp)
{
    if(*instp) {
        // Relaxed because it's not valid for another thread to CREATE a weak reference
        // while we hold the only remaining ref.
        ObjInst_WeakRef *weakref = atomicLoad(ptr, &(*instp)->_weakref, Relaxed);

        if(weakref)
        {
            // We normally assume that if this is the last reference it's impossible
            // for another thread to increment the count concurrently (because they
            // can't have a reference). However weak references can be converted at
            // any time from any thread and violate that assumption, making a lock
            // needed here if they are in use for this object.
            rwlockAcquireWrite(&weakref->_lock);
        }

        if(atomicFetchSub(uintptr, &(*instp)->_ref, 1, Release) == 1) {
            if(weakref) {
                weakref->_inst = NULL;          // object is about to be destroyed, invalidate weak refs
                rwlockReleaseWrite(&weakref->_lock);
                objDestroyWeak(&weakref);       // remove the extra weak ref the object itself holds
            }

            atomicFence(Acquire);
            _objDestroy(*instp);
        } else if(weakref) {
            rwlockReleaseWrite(&weakref->_lock);
        }
    }

    *instp = NULL;
}
