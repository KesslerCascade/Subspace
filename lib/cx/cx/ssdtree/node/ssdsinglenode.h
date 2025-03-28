#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/ssdtree/node/ssdnode.h>

typedef struct SSDSingleNode SSDSingleNode;
typedef struct SSDSingleNode_WeakRef SSDSingleNode_WeakRef;
typedef struct SSDSingleIter SSDSingleIter;
typedef struct SSDSingleIter_WeakRef SSDSingleIter_WeakRef;
saDeclarePtr(SSDSingleNode);
saDeclarePtr(SSDSingleNode_WeakRef);
saDeclarePtr(SSDSingleIter);
saDeclarePtr(SSDSingleIter_WeakRef);

typedef struct SSDSingleNode_ClassIf {
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
} SSDSingleNode_ClassIf;
extern SSDSingleNode_ClassIf SSDSingleNode_ClassIf_tmpl;

typedef struct SSDSingleIter_ClassIf {
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
} SSDSingleIter_ClassIf;
extern SSDSingleIter_ClassIf SSDSingleIter_ClassIf_tmpl;

typedef struct SSDSingleNode {
    union {
        SSDSingleNode_ClassIf* _;
        void* _is_SSDSingleNode;
        void* _is_SSDNode;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    SSDTree* tree;
    int64 modified;        // The timestamp this node was last modified
    stvar storage;
} SSDSingleNode;
extern ObjClassInfo SSDSingleNode_clsinfo;
#define SSDSingleNode(inst) ((SSDSingleNode*)(unused_noeval((inst) && &((inst)->_is_SSDSingleNode)), (inst)))
#define SSDSingleNodeNone ((SSDSingleNode*)NULL)

typedef struct SSDSingleNode_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SSDSingleNode_WeakRef;
        void* _is_SSDNode_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SSDSingleNode_WeakRef;
#define SSDSingleNode_WeakRef(inst) ((SSDSingleNode_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SSDSingleNode_WeakRef)), (inst)))

_objfactory_guaranteed SSDSingleNode* SSDSingleNode__create(SSDTree* tree);
// SSDSingleNode* ssdsinglenode_create(SSDTree* tree);
#define ssdsinglenode_create(tree) SSDSingleNode__create(SSDTree(tree))

// void ssdsinglenodeUpdateModified(SSDSingleNode* self);
#define ssdsinglenodeUpdateModified(self) SSDNode_updateModified(SSDNode(self))

// bool ssdsinglenodeIsHashtable(SSDSingleNode* self);
//
// This node is an object that contains values or objects by name
#define ssdsinglenodeIsHashtable(self) (self)->_->isHashtable(SSDSingleNode(self))
// bool ssdsinglenodeIsArray(SSDSingleNode* self);
//
// This node is an array that contains values or objects by array index
#define ssdsinglenodeIsArray(self) (self)->_->isArray(SSDSingleNode(self))
// bool ssdsinglenodeGet(SSDSingleNode* self, int32 idx, strref name, stvar* out, SSDLockState* _ssdCurrentLockState);
//
// Gets a value. Caller owns the value and must destroy it with stDestroy!
#define ssdsinglenodeGet(self, idx, name, out, _ssdCurrentLockState) (self)->_->get(SSDSingleNode(self), idx, name, out, _ssdCurrentLockState)
// stvar* ssdsinglenodePtr(SSDSingleNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Gets a pointer to a value. This points to the internal storage within the node
// so it is only guaranteed to be valid while the read lock is held.
#define ssdsinglenodePtr(self, idx, name, _ssdCurrentLockState) (self)->_->ptr(SSDSingleNode(self), idx, name, _ssdCurrentLockState)
// bool ssdsinglenodeSet(SSDSingleNode* self, int32 idx, strref name, stvar val, SSDLockState* _ssdCurrentLockState);
//
// Sets the given value
#define ssdsinglenodeSet(self, idx, name, val, _ssdCurrentLockState) (self)->_->set(SSDSingleNode(self), idx, name, val, _ssdCurrentLockState)
// bool ssdsinglenodeSetC(SSDSingleNode* self, int32 idx, strref name, stvar* val, SSDLockState* _ssdCurrentLockState);
//
// Same as setValue but consumes the value
// (consumes even on failure)
#define ssdsinglenodeSetC(self, idx, name, val, _ssdCurrentLockState) (self)->_->setC(SSDSingleNode(self), idx, name, val, _ssdCurrentLockState)
// bool ssdsinglenodeRemove(SSDSingleNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Removes a value
#define ssdsinglenodeRemove(self, idx, name, _ssdCurrentLockState) (self)->_->remove(SSDSingleNode(self), idx, name, _ssdCurrentLockState)
// int32 ssdsinglenodeCount(SSDSingleNode* self, SSDLockState* _ssdCurrentLockState);
//
// How many values / objects does this node contain?
#define ssdsinglenodeCount(self, _ssdCurrentLockState) (self)->_->count(SSDSingleNode(self), _ssdCurrentLockState)
// SSDIterator* ssdsinglenodeIter(SSDSingleNode* self);
//
// IMPORTANT NOTE: The generic object iterator interface cannot take any parameters;
// thus it always acquires a transient read lock and holds it until the iterator is
// destroyed. The caller MUST NOT already have an SSDLock held.
// If you want to use iterators inside a larger locked transaction or modify the tree,
// use iterLocked() instead.
#define ssdsinglenodeIter(self) (self)->_->iter(SSDSingleNode(self))
// SSDIterator* ssdsinglenode_iterLocked(SSDSingleNode* self, SSDLockState* _ssdCurrentLockState);
#define ssdsinglenode_iterLocked(self, _ssdCurrentLockState) (self)->_->_iterLocked(SSDSingleNode(self), _ssdCurrentLockState)

typedef struct SSDSingleIter {
    union {
        SSDSingleIter_ClassIf* _;
        void* _is_SSDSingleIter;
        void* _is_SSDIterator;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    SSDNode* node;
    SSDLockState* lstate;
    SSDLockState transient_lock_state;
    bool done;
} SSDSingleIter;
extern ObjClassInfo SSDSingleIter_clsinfo;
#define SSDSingleIter(inst) ((SSDSingleIter*)(unused_noeval((inst) && &((inst)->_is_SSDSingleIter)), (inst)))
#define SSDSingleIterNone ((SSDSingleIter*)NULL)

typedef struct SSDSingleIter_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SSDSingleIter_WeakRef;
        void* _is_SSDIterator_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SSDSingleIter_WeakRef;
#define SSDSingleIter_WeakRef(inst) ((SSDSingleIter_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SSDSingleIter_WeakRef)), (inst)))

_objfactory_guaranteed SSDSingleIter* SSDSingleIter_create(SSDSingleNode* node, SSDLockState* lstate);
// SSDSingleIter* ssdsingleiterCreate(SSDSingleNode* node, SSDLockState* lstate);
#define ssdsingleiterCreate(node, lstate) SSDSingleIter_create(SSDSingleNode(node), lstate)

// ObjInst* ssdsingleiterObjInst(SSDSingleIter* self);
#define ssdsingleiterObjInst(self) SSDIterator_objInst(SSDIterator(self))

// bool ssdsingleiterIsHashtable(SSDSingleIter* self);
#define ssdsingleiterIsHashtable(self) (self)->_->isHashtable(SSDSingleIter(self))
// bool ssdsingleiterIsArray(SSDSingleIter* self);
#define ssdsingleiterIsArray(self) (self)->_->isArray(SSDSingleIter(self))
// bool ssdsingleiterValid(SSDSingleIter* self);
#define ssdsingleiterValid(self) (self)->_->valid(SSDSingleIter(self))
// bool ssdsingleiterNext(SSDSingleIter* self);
#define ssdsingleiterNext(self) (self)->_->next(SSDSingleIter(self))
// bool ssdsingleiterGet(SSDSingleIter* self, stvar* out);
#define ssdsingleiterGet(self, out) (self)->_->get(SSDSingleIter(self), out)
// stvar* ssdsingleiterPtr(SSDSingleIter* self);
#define ssdsingleiterPtr(self) (self)->_->ptr(SSDSingleIter(self))
// strref ssdsingleiterName(SSDSingleIter* self);
#define ssdsingleiterName(self) (self)->_->name(SSDSingleIter(self))
// int32 ssdsingleiterIdx(SSDSingleIter* self);
#define ssdsingleiterIdx(self) (self)->_->idx(SSDSingleIter(self))
// bool ssdsingleiterIterOut(SSDSingleIter* self, int32* idx, strref* name, stvar** val);
#define ssdsingleiterIterOut(self, idx, name, val) (self)->_->iterOut(SSDSingleIter(self), idx, name, val)

