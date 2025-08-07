#include "objclass.h"

_Use_decl_annotations_
ObjInst_WeakRef *_objGetWeak(ObjInst *inst)
{
    ObjInst_WeakRef *ret = atomicLoad(ptr, &inst->_weakref, Relaxed);

    for(;;) {
        if(ret) {
            atomicFetchAdd(uintptr, &ret->_ref, 1, Relaxed);
            return ret;
        }

        ObjInst_WeakRef *nref = xaAllocStruct(ObjInst_WeakRef);
        rwlockInit(&nref->_lock);
        atomicStore(uintptr, &nref->_ref, 2, Relaxed);       // object itself holds 1 of the weak refs
        nref->_inst = inst;

        if(atomicCompareExchange(ptr, strong, &inst->_weakref, (void**)&ret, nref, AcqRel, Acquire)) {
            return nref;
        } else {
            rwlockDestroy(&nref->_lock);
            xaFree(nref);
        }
    }
}

_Use_decl_annotations_
ObjInst_WeakRef *_objCloneWeak(ObjInst_WeakRef *ref)
{
    if(ref)
        atomicFetchAdd(uintptr, &ref->_ref, 1, Relaxed);

    return ref;
}

_Use_decl_annotations_
void _objDestroyWeak(ObjInst_WeakRef **refp)
{
    if(*refp && atomicFetchSub(uintptr, &(*refp)->_ref, 1, Release) == 1) {
        // Live objects that have weak references created always hold 1 reference
        // in the object itself. Therefore the only way to get here is either
        // when the object is already destroyed and the last (no longer valid) weak
        // reference to it is destroyed, or if the object is being destroyed and there
        // are no longer any weak references to it.

        // Given those preconditions, we don't need to do any locking here.

        devAssert((*refp)->_inst == NULL);      // should be cleared as part of object destruction

        rwlockDestroy(&(*refp)->_lock);

        atomicFence(Acquire);
        xaFree(*refp);
    }

    *refp = NULL;
}

_Use_decl_annotations_
ObjInst *_objAcquireFromWeak(ObjInst_WeakRef *ref)
{
    if(!ref)
        return NULL;

    ObjInst *ret = NULL;
    withReadLock(&ref->_lock)
    {
        ret = objAcquire(ref->_inst);
    }

    return ret;
}
_Use_decl_annotations_
ObjInst *_objAcquireFromWeakDyn(ObjInst_WeakRef *ref, ObjClassInfo *cls)
{
    if(!ref)
        return NULL;

    ObjInst *retbase = NULL;
    withReadLock(&ref->_lock)
    {
        retbase = objAcquire(ref->_inst);
    }

    ObjInst *ret = _objDynCast(retbase, cls);
    if(!ret) {
        objRelease(&retbase);
    }

    return ret;
}
