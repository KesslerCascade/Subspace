#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/ssdtree/ssdtreeobj.h>
#include <cx/stype/stvar.h>

typedef struct SSDIterator SSDIterator;
typedef struct SSDIterator_WeakRef SSDIterator_WeakRef;
typedef struct SSDNode SSDNode;
typedef struct SSDNode_WeakRef SSDNode_WeakRef;
saDeclarePtr(SSDIterator);
saDeclarePtr(SSDIterator_WeakRef);
saDeclarePtr(SSDNode);
saDeclarePtr(SSDNode_WeakRef);

enum SSD_INDEX_MARKER {
    SSD_ByName = -1             // Pass as index to address a child by name
};

#define ssditeratorObj(self, clsname) objDynCast(clsname, ssditeratorObjInst(self))
#define ssdnodeIterLocked(self) ssdnode_iterLocked(self, _ssdCurrentLockState)

typedef struct SSDIteratorIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*valid)(_In_ void* self);
    bool (*next)(_In_ void* self);
    bool (*get)(_In_ void* self, stvar* out);
    stvar* (*ptr)(_In_ void* self);
    strref (*name)(_In_ void* self);
    int32 (*idx)(_In_ void* self);
    bool (*iterOut)(_In_ void* self, _When_(return == true, _Out_) int32* idx, _When_(return == true, _Out_) strref* name, _When_(return == true, _Out_) stvar** val);
} SSDIteratorIf;
extern SSDIteratorIf SSDIteratorIf_tmpl;

typedef struct SSDNodeIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // Gets a value. Caller owns the value and must destroy it with stDestroy!
    bool (*get)(_In_ void* self, int32 idx, _In_opt_ strref name, _When_(return == true, _Out_) stvar* out, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Gets a pointer to a value. This points to the internal storage within the node
    // so it is only guaranteed to be valid while the read lock is held.
    _Ret_opt_valid_ stvar* (*ptr)(_In_ void* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Sets the given value
    bool (*set)(_In_ void* self, int32 idx, _In_opt_ strref name, stvar val, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Same as setValue but consumes the value
    // (consumes even on failure)
    bool (*setC)(_In_ void* self, int32 idx, _In_opt_ strref name, _Inout_ stvar* val, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Removes a value
    bool (*remove)(_In_ void* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState);
    // How many values / objects does this node contain?
    int32 (*count)(_In_ void* self, _Inout_ SSDLockState* _ssdCurrentLockState);
    // IMPORTANT NOTE: The generic object iterator interface cannot take any parameters;
    // thus it always acquires a transient read lock and holds it until the iterator is
    // destroyed. The caller MUST NOT already have an SSDLock held.
    // If you want to use iterators inside a larger locked transaction or modify the tree,
    // use iterLocked() instead.
    _Ret_valid_ SSDIterator* (*iter)(_In_ void* self);
    SSDIterator* (*_iterLocked)(_In_ void* self, _Inout_ SSDLockState* _ssdCurrentLockState);
} SSDNodeIf;
extern SSDNodeIf SSDNodeIf_tmpl;

typedef struct SSDIterator_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*isHashtable)(_In_ void* self);
    bool (*isArray)(_In_ void* self);
    bool (*valid)(_In_ void* self);
    bool (*next)(_In_ void* self);
    bool (*get)(_In_ void* self, stvar* out);
    stvar* (*ptr)(_In_ void* self);
    strref (*name)(_In_ void* self);
    int32 (*idx)(_In_ void* self);
    bool (*iterOut)(_In_ void* self, _When_(return == true, _Out_) int32* idx, _When_(return == true, _Out_) strref* name, _When_(return == true, _Out_) stvar** val);
} SSDIterator_ClassIf;
extern SSDIterator_ClassIf SSDIterator_ClassIf_tmpl;

typedef struct SSDNode_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // This node is an object that contains values or objects by name
    bool (*isHashtable)(_In_ void* self);
    // This node is an array that contains values or objects by array index
    bool (*isArray)(_In_ void* self);
    // Gets a value. Caller owns the value and must destroy it with stDestroy!
    bool (*get)(_In_ void* self, int32 idx, _In_opt_ strref name, _When_(return == true, _Out_) stvar* out, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Gets a pointer to a value. This points to the internal storage within the node
    // so it is only guaranteed to be valid while the read lock is held.
    _Ret_opt_valid_ stvar* (*ptr)(_In_ void* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Sets the given value
    bool (*set)(_In_ void* self, int32 idx, _In_opt_ strref name, stvar val, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Same as setValue but consumes the value
    // (consumes even on failure)
    bool (*setC)(_In_ void* self, int32 idx, _In_opt_ strref name, _Inout_ stvar* val, _Inout_ SSDLockState* _ssdCurrentLockState);
    // Removes a value
    bool (*remove)(_In_ void* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState);
    // How many values / objects does this node contain?
    int32 (*count)(_In_ void* self, _Inout_ SSDLockState* _ssdCurrentLockState);
    // IMPORTANT NOTE: The generic object iterator interface cannot take any parameters;
    // thus it always acquires a transient read lock and holds it until the iterator is
    // destroyed. The caller MUST NOT already have an SSDLock held.
    // If you want to use iterators inside a larger locked transaction or modify the tree,
    // use iterLocked() instead.
    _Ret_valid_ SSDIterator* (*iter)(_In_ void* self);
    SSDIterator* (*_iterLocked)(_In_ void* self, _Inout_ SSDLockState* _ssdCurrentLockState);
} SSDNode_ClassIf;
extern SSDNode_ClassIf SSDNode_ClassIf_tmpl;

typedef struct SSDIterator {
    union {
        SSDIterator_ClassIf* _;
        void* _is_SSDIterator;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    SSDNode* node;
    SSDLockState* lstate;
    SSDLockState transient_lock_state;
} SSDIterator;
extern ObjClassInfo SSDIterator_clsinfo;
#define SSDIterator(inst) ((SSDIterator*)(unused_noeval((inst) && &((inst)->_is_SSDIterator)), (inst)))
#define SSDIteratorNone ((SSDIterator*)NULL)

typedef struct SSDIterator_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SSDIterator_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SSDIterator_WeakRef;
#define SSDIterator_WeakRef(inst) ((SSDIterator_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SSDIterator_WeakRef)), (inst)))

_Ret_opt_valid_ ObjInst* SSDIterator_objInst(_In_ SSDIterator* self);
// ObjInst* ssditeratorObjInst(SSDIterator* self);
#define ssditeratorObjInst(self) SSDIterator_objInst(SSDIterator(self))

// bool ssditeratorIsHashtable(SSDIterator* self);
#define ssditeratorIsHashtable(self) (self)->_->isHashtable(SSDIterator(self))
// bool ssditeratorIsArray(SSDIterator* self);
#define ssditeratorIsArray(self) (self)->_->isArray(SSDIterator(self))
// bool ssditeratorValid(SSDIterator* self);
#define ssditeratorValid(self) (self)->_->valid(SSDIterator(self))
// bool ssditeratorNext(SSDIterator* self);
#define ssditeratorNext(self) (self)->_->next(SSDIterator(self))
// bool ssditeratorGet(SSDIterator* self, stvar* out);
#define ssditeratorGet(self, out) (self)->_->get(SSDIterator(self), out)
// stvar* ssditeratorPtr(SSDIterator* self);
#define ssditeratorPtr(self) (self)->_->ptr(SSDIterator(self))
// strref ssditeratorName(SSDIterator* self);
#define ssditeratorName(self) (self)->_->name(SSDIterator(self))
// int32 ssditeratorIdx(SSDIterator* self);
#define ssditeratorIdx(self) (self)->_->idx(SSDIterator(self))
// bool ssditeratorIterOut(SSDIterator* self, int32* idx, strref* name, stvar** val);
#define ssditeratorIterOut(self, idx, name, val) (self)->_->iterOut(SSDIterator(self), idx, name, val)

typedef struct SSDNode {
    union {
        SSDNode_ClassIf* _;
        void* _is_SSDNode;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    SSDTree* tree;
    int64 modified;        // The timestamp this node was last modified
} SSDNode;
extern ObjClassInfo SSDNode_clsinfo;
#define SSDNode(inst) ((SSDNode*)(unused_noeval((inst) && &((inst)->_is_SSDNode)), (inst)))
#define SSDNodeNone ((SSDNode*)NULL)

typedef struct SSDNode_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SSDNode_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SSDNode_WeakRef;
#define SSDNode_WeakRef(inst) ((SSDNode_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SSDNode_WeakRef)), (inst)))

void SSDNode_updateModified(_In_ SSDNode* self);
// void ssdnodeUpdateModified(SSDNode* self);
#define ssdnodeUpdateModified(self) SSDNode_updateModified(SSDNode(self))

// bool ssdnodeIsHashtable(SSDNode* self);
//
// This node is an object that contains values or objects by name
#define ssdnodeIsHashtable(self) (self)->_->isHashtable(SSDNode(self))
// bool ssdnodeIsArray(SSDNode* self);
//
// This node is an array that contains values or objects by array index
#define ssdnodeIsArray(self) (self)->_->isArray(SSDNode(self))
// bool ssdnodeGet(SSDNode* self, int32 idx, strref name, stvar* out, SSDLockState* _ssdCurrentLockState);
//
// Gets a value. Caller owns the value and must destroy it with stDestroy!
#define ssdnodeGet(self, idx, name, out, _ssdCurrentLockState) (self)->_->get(SSDNode(self), idx, name, out, _ssdCurrentLockState)
// stvar* ssdnodePtr(SSDNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Gets a pointer to a value. This points to the internal storage within the node
// so it is only guaranteed to be valid while the read lock is held.
#define ssdnodePtr(self, idx, name, _ssdCurrentLockState) (self)->_->ptr(SSDNode(self), idx, name, _ssdCurrentLockState)
// bool ssdnodeSet(SSDNode* self, int32 idx, strref name, stvar val, SSDLockState* _ssdCurrentLockState);
//
// Sets the given value
#define ssdnodeSet(self, idx, name, val, _ssdCurrentLockState) (self)->_->set(SSDNode(self), idx, name, val, _ssdCurrentLockState)
// bool ssdnodeSetC(SSDNode* self, int32 idx, strref name, stvar* val, SSDLockState* _ssdCurrentLockState);
//
// Same as setValue but consumes the value
// (consumes even on failure)
#define ssdnodeSetC(self, idx, name, val, _ssdCurrentLockState) (self)->_->setC(SSDNode(self), idx, name, val, _ssdCurrentLockState)
// bool ssdnodeRemove(SSDNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Removes a value
#define ssdnodeRemove(self, idx, name, _ssdCurrentLockState) (self)->_->remove(SSDNode(self), idx, name, _ssdCurrentLockState)
// int32 ssdnodeCount(SSDNode* self, SSDLockState* _ssdCurrentLockState);
//
// How many values / objects does this node contain?
#define ssdnodeCount(self, _ssdCurrentLockState) (self)->_->count(SSDNode(self), _ssdCurrentLockState)
// SSDIterator* ssdnodeIter(SSDNode* self);
//
// IMPORTANT NOTE: The generic object iterator interface cannot take any parameters;
// thus it always acquires a transient read lock and holds it until the iterator is
// destroyed. The caller MUST NOT already have an SSDLock held.
// If you want to use iterators inside a larger locked transaction or modify the tree,
// use iterLocked() instead.
#define ssdnodeIter(self) (self)->_->iter(SSDNode(self))
// SSDIterator* ssdnode_iterLocked(SSDNode* self, SSDLockState* _ssdCurrentLockState);
#define ssdnode_iterLocked(self, _ssdCurrentLockState) (self)->_->_iterLocked(SSDNode(self), _ssdCurrentLockState)

