#pragma once

#include <cx/obj/objiface.h>
#include <cx/container/hashtable.h>
#include <cx/thread/atomic.h>
#include <cx/utils/macros/unused.h>
#include <cx/thread/rwlock.h>

// A class stores arbitrary per-instance data and may implement one or more interfaces.
// The class info structure contains runtime meta-information about the class.

CX_C_BEGIN

typedef struct ObjClassInfo ObjClassInfo;
typedef struct ObjInst ObjInst;
typedef struct ObjClassInfo {
    // ----- Statically defined in class info instance by class implementer -----
    size_t instsize;            // size of a class instance
    ObjClassInfo *parent;       // parent class
    ObjIface *classif;          // class interface
    ObjIface **ifimpl;          // static list of interface implementations, terminated by NULL pointer

    // init is required to be called at the end of the factory function, just before
    // returning the object to the caller, but after any members are initialized by
    // the factory. If init returns false, construction must aborted and the factory
    // should return NULL.
    // This is useful to have a final check or common object initialization that
    // applies regardless of which factory is used.
    bool(*init)(void *_self);

    // destroy is called just before the object's memory is deallocated
    void(*destroy)(void *_self);

    // this is an abstract class and cannot be instanced
    bool _abstract;

    // ----- Runtime use only, do not set manually! -----
    // internal cache of certain functions so the interface doesn't have to be looked up
    intptr (*_cmp)(void *self, void *other, uint32 flags);
    uint32 (*_hash)(void *self, uint32 flags);
    sa_ObjIface _impl;          // storage for hydrated implementations
    hashtable _tmpl;            // mapping of interface templates to implementations
} ObjClassInfo;

#define objClassInfoName(cname) cname##_clsinfo

// Generic class instance and implicit base for all dynamic objects
typedef struct ObjInst {
    union {
        ObjIface *_classif;         // basically vtable, this is called _ in class instances for less typing
        void *_is_ObjInst;
    };
    ObjClassInfo *_clsinfo;
    atomic(uintptr) _ref;        // reference count
    atomic(ptr) _weakref;       // associated weak reference object

    // user data members here
} ObjInst;

typedef struct ObjInst_WeakRef
{
    union
    {
        ObjInst *_inst;
        void *_is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ObjInst_WeakRef;

#define Weak(clsname) clsname##_WeakRef

#define ObjInst(inst) ((ObjInst*)(unused_noeval(&((inst)->_is_ObjInst)), (inst)))
#define ObjInst_WeakRef(ref) ((ObjInst_WeakRef*)(unused_noeval(&((ref)->_is_ObjInst_WeakRef)), (ref)))
#define objInstBase(inst) ObjInst(inst)
#define objWeakRefBase(ref) ObjInst_WeakRef(ref)
#define objClsInfo(inst) (inst->_clsinfo)

// DO NOT CALL DIRECTLY! Use objRelease instead
void _objDestroy(_Pre_notnull_ _Post_invalid_ ObjInst *inst);

_meta_inline void _objAcquire(_In_opt_ ObjInst *inst)
{
    if (inst)
        atomicFetchAdd(uintptr, &inst->_ref, 1, Relaxed);
}
#define objAcquire(inst) (_objAcquire(objInstBase(inst)), (inst))

_At_(*instp, _Pre_maybenull_ _Post_null_)
void _objRelease(_Inout_ ObjInst **instp);
#define objRelease(pinst) (unused_noeval(&((*(pinst))->_is_ObjInst)), _objRelease((ObjInst**)(pinst)))

// Functions to get a populated interface from a class or instance
_Ret_maybenull_ ObjIface *_objClassIf(_In_ ObjClassInfo *cls, _In_ ObjIface *iftmpl);
#define objClassIf(clsname, ifname) ((ifname*)_objClassIf(&objClassInfoName(clsname), objIfBase(&objIfTmplName(ifname))))
_Ret_maybenull_ ObjIface *_objInstIf(_In_opt_ ObjInst *inst, _In_ ObjIface *iftmpl);
#define objInstIf(inst, ifname) ((ifname*)_objInstIf(objInstBase(inst), objIfBase(&objIfTmplName(ifname))))

_Ret_valid_ ObjInst_WeakRef *_objGetWeak(_In_ ObjInst *inst);
#define objGetWeak(clsname, inst) ((Weak(clsname)*)_objGetWeak((ObjInst*)clsname(inst)))

_Ret_maybenull_ ObjInst_WeakRef *_objCloneWeak(_In_opt_ ObjInst_WeakRef *ref);
#define objCloneWeak(ref) (_objCloneWeak(objWeakRefBase(ref)))

void _objDestroyWeak(_Inout_ ObjInst_WeakRef **refp);
#define objDestroyWeak(pref) (unused_noeval(&((*(pref))->_is_ObjInst_WeakRef)), _objDestroyWeak((ObjInst_WeakRef**)(pref)))

_Ret_maybenull_ ObjInst *_objAcquireFromWeak(_In_opt_ ObjInst_WeakRef *ref);
#define objAcquireFromWeak(clsname, ref) ((clsname*)_objAcquireFromWeak((ObjInst_WeakRef*)Weak(clsname)(ref)))

// Dynamic object casting within the hierarchy
// Returns instance pointer if it's safe to cast inst to the given class, NULL otherwise
_Ret_maybenull_ ObjInst *_objDynCast(_In_opt_ ObjInst *inst, _In_ ObjClassInfo *cls);
#define objDynCast(clsname, inst) ((clsname*)_objDynCast(objInstBase(inst), &objClassInfoName(clsname)))

_Ret_maybenull_ ObjInst *_objAcquireFromWeakDyn(_In_opt_ ObjInst_WeakRef *ref, _In_ ObjClassInfo *cls);
#define objAcquireFromWeakDyn(clsname, ref) ((clsname*)_objAcquireFromWeakDyn(objWeakRefBase(ref), &objClassInfoName(clsname)))

CX_C_END
