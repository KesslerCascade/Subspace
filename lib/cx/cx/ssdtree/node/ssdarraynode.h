#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/ssdtree/node/ssdnode.h>

typedef struct SSDArrayNode SSDArrayNode;
typedef struct SSDArrayNode_WeakRef SSDArrayNode_WeakRef;
typedef struct SSDArrayIter SSDArrayIter;
typedef struct SSDArrayIter_WeakRef SSDArrayIter_WeakRef;
saDeclarePtr(SSDArrayNode);
saDeclarePtr(SSDArrayNode_WeakRef);
saDeclarePtr(SSDArrayIter);
saDeclarePtr(SSDArrayIter_WeakRef);

typedef struct SSDArrayNode_ClassIf {
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
    bool (*append)(_In_ void* self, stvar val, SSDLockState* _ssdCurrentLockState);
} SSDArrayNode_ClassIf;
extern SSDArrayNode_ClassIf SSDArrayNode_ClassIf_tmpl;

typedef struct SSDArrayIter_ClassIf {
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
} SSDArrayIter_ClassIf;
extern SSDArrayIter_ClassIf SSDArrayIter_ClassIf_tmpl;

typedef struct SSDArrayNode {
    union {
        SSDArrayNode_ClassIf* _;
        void* _is_SSDArrayNode;
        void* _is_SSDNode;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    SSDTree* tree;
    int64 modified;        // The timestamp this node was last modified
    sa_stvar storage;
} SSDArrayNode;
extern ObjClassInfo SSDArrayNode_clsinfo;
#define SSDArrayNode(inst) ((SSDArrayNode*)(unused_noeval((inst) && &((inst)->_is_SSDArrayNode)), (inst)))
#define SSDArrayNodeNone ((SSDArrayNode*)NULL)

typedef struct SSDArrayNode_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SSDArrayNode_WeakRef;
        void* _is_SSDNode_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SSDArrayNode_WeakRef;
#define SSDArrayNode_WeakRef(inst) ((SSDArrayNode_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SSDArrayNode_WeakRef)), (inst)))

_objfactory_guaranteed SSDArrayNode* SSDArrayNode__create(SSDTree* tree);
// SSDArrayNode* ssdarraynode_create(SSDTree* tree);
#define ssdarraynode_create(tree) SSDArrayNode__create(SSDTree(tree))

// void ssdarraynodeUpdateModified(SSDArrayNode* self);
#define ssdarraynodeUpdateModified(self) SSDNode_updateModified(SSDNode(self))

// bool ssdarraynodeIsHashtable(SSDArrayNode* self);
//
// This node is an object that contains values or objects by name
#define ssdarraynodeIsHashtable(self) (self)->_->isHashtable(SSDArrayNode(self))
// bool ssdarraynodeIsArray(SSDArrayNode* self);
//
// This node is an array that contains values or objects by array index
#define ssdarraynodeIsArray(self) (self)->_->isArray(SSDArrayNode(self))
// bool ssdarraynodeGet(SSDArrayNode* self, int32 idx, strref name, stvar* out, SSDLockState* _ssdCurrentLockState);
//
// Gets a value. Caller owns the value and must destroy it with stDestroy!
#define ssdarraynodeGet(self, idx, name, out, _ssdCurrentLockState) (self)->_->get(SSDArrayNode(self), idx, name, out, _ssdCurrentLockState)
// stvar* ssdarraynodePtr(SSDArrayNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Gets a pointer to a value. This points to the internal storage within the node
// so it is only guaranteed to be valid while the read lock is held.
#define ssdarraynodePtr(self, idx, name, _ssdCurrentLockState) (self)->_->ptr(SSDArrayNode(self), idx, name, _ssdCurrentLockState)
// bool ssdarraynodeSet(SSDArrayNode* self, int32 idx, strref name, stvar val, SSDLockState* _ssdCurrentLockState);
//
// Sets the given value
#define ssdarraynodeSet(self, idx, name, val, _ssdCurrentLockState) (self)->_->set(SSDArrayNode(self), idx, name, val, _ssdCurrentLockState)
// bool ssdarraynodeSetC(SSDArrayNode* self, int32 idx, strref name, stvar* val, SSDLockState* _ssdCurrentLockState);
//
// Same as setValue but consumes the value
// (consumes even on failure)
#define ssdarraynodeSetC(self, idx, name, val, _ssdCurrentLockState) (self)->_->setC(SSDArrayNode(self), idx, name, val, _ssdCurrentLockState)
// bool ssdarraynodeRemove(SSDArrayNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Removes a value
#define ssdarraynodeRemove(self, idx, name, _ssdCurrentLockState) (self)->_->remove(SSDArrayNode(self), idx, name, _ssdCurrentLockState)
// int32 ssdarraynodeCount(SSDArrayNode* self, SSDLockState* _ssdCurrentLockState);
//
// How many values / objects does this node contain?
#define ssdarraynodeCount(self, _ssdCurrentLockState) (self)->_->count(SSDArrayNode(self), _ssdCurrentLockState)
// SSDIterator* ssdarraynodeIter(SSDArrayNode* self);
//
// IMPORTANT NOTE: The generic object iterator interface cannot take any parameters;
// thus it always acquires a transient read lock and holds it until the iterator is
// destroyed. The caller MUST NOT already have an SSDLock held.
// If you want to use iterators inside a larger locked transaction or modify the tree,
// use iterLocked() instead.
#define ssdarraynodeIter(self) (self)->_->iter(SSDArrayNode(self))
// SSDIterator* ssdarraynode_iterLocked(SSDArrayNode* self, SSDLockState* _ssdCurrentLockState);
#define ssdarraynode_iterLocked(self, _ssdCurrentLockState) (self)->_->_iterLocked(SSDArrayNode(self), _ssdCurrentLockState)
// bool ssdarraynodeAppend(SSDArrayNode* self, stvar val, SSDLockState* _ssdCurrentLockState);
#define ssdarraynodeAppend(self, val, _ssdCurrentLockState) (self)->_->append(SSDArrayNode(self), val, _ssdCurrentLockState)

typedef struct SSDArrayIter {
    union {
        SSDArrayIter_ClassIf* _;
        void* _is_SSDArrayIter;
        void* _is_SSDIterator;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    SSDNode* node;
    SSDLockState* lstate;
    SSDLockState transient_lock_state;
    int32 idx;
    string lastName;
} SSDArrayIter;
extern ObjClassInfo SSDArrayIter_clsinfo;
#define SSDArrayIter(inst) ((SSDArrayIter*)(unused_noeval((inst) && &((inst)->_is_SSDArrayIter)), (inst)))
#define SSDArrayIterNone ((SSDArrayIter*)NULL)

typedef struct SSDArrayIter_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SSDArrayIter_WeakRef;
        void* _is_SSDIterator_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SSDArrayIter_WeakRef;
#define SSDArrayIter_WeakRef(inst) ((SSDArrayIter_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SSDArrayIter_WeakRef)), (inst)))

_objfactory_guaranteed SSDArrayIter* SSDArrayIter_create(SSDArrayNode* node, SSDLockState* lstate);
// SSDArrayIter* ssdarrayiterCreate(SSDArrayNode* node, SSDLockState* lstate);
#define ssdarrayiterCreate(node, lstate) SSDArrayIter_create(SSDArrayNode(node), lstate)

// ObjInst* ssdarrayiterObjInst(SSDArrayIter* self);
#define ssdarrayiterObjInst(self) SSDIterator_objInst(SSDIterator(self))

// bool ssdarrayiterIsHashtable(SSDArrayIter* self);
#define ssdarrayiterIsHashtable(self) (self)->_->isHashtable(SSDArrayIter(self))
// bool ssdarrayiterIsArray(SSDArrayIter* self);
#define ssdarrayiterIsArray(self) (self)->_->isArray(SSDArrayIter(self))
// bool ssdarrayiterValid(SSDArrayIter* self);
#define ssdarrayiterValid(self) (self)->_->valid(SSDArrayIter(self))
// bool ssdarrayiterNext(SSDArrayIter* self);
#define ssdarrayiterNext(self) (self)->_->next(SSDArrayIter(self))
// bool ssdarrayiterGet(SSDArrayIter* self, stvar* out);
#define ssdarrayiterGet(self, out) (self)->_->get(SSDArrayIter(self), out)
// stvar* ssdarrayiterPtr(SSDArrayIter* self);
#define ssdarrayiterPtr(self) (self)->_->ptr(SSDArrayIter(self))
// strref ssdarrayiterName(SSDArrayIter* self);
#define ssdarrayiterName(self) (self)->_->name(SSDArrayIter(self))
// int32 ssdarrayiterIdx(SSDArrayIter* self);
#define ssdarrayiterIdx(self) (self)->_->idx(SSDArrayIter(self))
// bool ssdarrayiterIterOut(SSDArrayIter* self, int32* idx, strref* name, stvar** val);
#define ssdarrayiterIterOut(self, idx, name, val) (self)->_->iterOut(SSDArrayIter(self), idx, name, val)

