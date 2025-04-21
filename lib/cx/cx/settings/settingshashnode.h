#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/ssdtree/node/ssdhashnode.h>

typedef struct SettingsHashNode SettingsHashNode;
typedef struct SettingsHashNode_WeakRef SettingsHashNode_WeakRef;
saDeclarePtr(SettingsHashNode);
saDeclarePtr(SettingsHashNode_WeakRef);

typedef struct SettingsBind {
    stype type;                 // type of bound variable
    stgeneric *var;             // pointer to bound variable
    stgeneric cache;            // cached copy for change detection
    stgeneric def;              // default value

    bool userset;               // explicitly overridden, even if it's the default value
} SettingsBind;

typedef struct SettingsHashNode_ClassIf {
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
    bool (*bind)(_In_ void* self, _In_opt_ strref name, stype btyp, void* bvar, stgeneric bdef, SSDLockState* _ssdCurrentLockState);
    // check a single bound variable for changes
    void (*checkBound)(_In_ void* self, _In_opt_ strref name, SSDLockState* _ssdCurrentLockState);
    // check for bound variables that have changed
    void (*checkAll)(_In_ void* self, SSDLockState* _ssdCurrentLockState);
    void (*unbindAll)(_In_ void* self, SSDLockState* _ssdCurrentLockState);
} SettingsHashNode_ClassIf;
extern SettingsHashNode_ClassIf SettingsHashNode_ClassIf_tmpl;

typedef struct SettingsHashNode {
    union {
        SettingsHashNode_ClassIf* _;
        void* _is_SettingsHashNode;
        void* _is_SSDHashNode;
        void* _is_SSDNode;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    SSDTree* tree;
    int64 modified;        // The timestamp this node was last modified
    hashtable storage;
    hashtable binds;
} SettingsHashNode;
extern ObjClassInfo SettingsHashNode_clsinfo;
#define SettingsHashNode(inst) ((SettingsHashNode*)(unused_noeval((inst) && &((inst)->_is_SettingsHashNode)), (inst)))
#define SettingsHashNodeNone ((SettingsHashNode*)NULL)

typedef struct SettingsHashNode_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SettingsHashNode_WeakRef;
        void* _is_SSDHashNode_WeakRef;
        void* _is_SSDNode_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SettingsHashNode_WeakRef;
#define SettingsHashNode_WeakRef(inst) ((SettingsHashNode_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SettingsHashNode_WeakRef)), (inst)))

_objfactory_guaranteed SettingsHashNode* SettingsHashNode__create(SSDTree* tree);
// SettingsHashNode* settingshashnode_create(SSDTree* tree);
#define settingshashnode_create(tree) SettingsHashNode__create(SSDTree(tree))

// void settingshashnodeUpdateModified(SettingsHashNode* self);
#define settingshashnodeUpdateModified(self) SSDNode_updateModified(SSDNode(self))

// bool settingshashnodeIsHashtable(SettingsHashNode* self);
//
// This node is an object that contains values or objects by name
#define settingshashnodeIsHashtable(self) (self)->_->isHashtable(SettingsHashNode(self))
// bool settingshashnodeIsArray(SettingsHashNode* self);
//
// This node is an array that contains values or objects by array index
#define settingshashnodeIsArray(self) (self)->_->isArray(SettingsHashNode(self))
// bool settingshashnodeGet(SettingsHashNode* self, int32 idx, strref name, stvar* out, SSDLockState* _ssdCurrentLockState);
//
// Gets a value. Caller owns the value and must destroy it with stDestroy!
#define settingshashnodeGet(self, idx, name, out, _ssdCurrentLockState) (self)->_->get(SettingsHashNode(self), idx, name, out, _ssdCurrentLockState)
// stvar* settingshashnodePtr(SettingsHashNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Gets a pointer to a value. This points to the internal storage within the node
// so it is only guaranteed to be valid while the read lock is held.
#define settingshashnodePtr(self, idx, name, _ssdCurrentLockState) (self)->_->ptr(SettingsHashNode(self), idx, name, _ssdCurrentLockState)
// bool settingshashnodeSet(SettingsHashNode* self, int32 idx, strref name, stvar val, SSDLockState* _ssdCurrentLockState);
//
// Sets the given value
#define settingshashnodeSet(self, idx, name, val, _ssdCurrentLockState) (self)->_->set(SettingsHashNode(self), idx, name, val, _ssdCurrentLockState)
// bool settingshashnodeSetC(SettingsHashNode* self, int32 idx, strref name, stvar* val, SSDLockState* _ssdCurrentLockState);
//
// Same as setValue but consumes the value
// (consumes even on failure)
#define settingshashnodeSetC(self, idx, name, val, _ssdCurrentLockState) (self)->_->setC(SettingsHashNode(self), idx, name, val, _ssdCurrentLockState)
// bool settingshashnodeRemove(SettingsHashNode* self, int32 idx, strref name, SSDLockState* _ssdCurrentLockState);
//
// Removes a value
#define settingshashnodeRemove(self, idx, name, _ssdCurrentLockState) (self)->_->remove(SettingsHashNode(self), idx, name, _ssdCurrentLockState)
// int32 settingshashnodeCount(SettingsHashNode* self, SSDLockState* _ssdCurrentLockState);
//
// How many values / objects does this node contain?
#define settingshashnodeCount(self, _ssdCurrentLockState) (self)->_->count(SettingsHashNode(self), _ssdCurrentLockState)
// SSDIterator* settingshashnodeIter(SettingsHashNode* self);
//
// IMPORTANT NOTE: The generic object iterator interface cannot take any parameters;
// thus it always acquires a transient read lock and holds it until the iterator is
// destroyed. The caller MUST NOT already have an SSDLock held.
// If you want to use iterators inside a larger locked transaction or modify the tree,
// use iterLocked() instead.
#define settingshashnodeIter(self) (self)->_->iter(SettingsHashNode(self))
// SSDIterator* settingshashnode_iterLocked(SettingsHashNode* self, SSDLockState* _ssdCurrentLockState);
#define settingshashnode_iterLocked(self, _ssdCurrentLockState) (self)->_->_iterLocked(SettingsHashNode(self), _ssdCurrentLockState)
// bool settingshashnodeBind(SettingsHashNode* self, strref name, stype btyp, void* bvar, stgeneric bdef, SSDLockState* _ssdCurrentLockState);
#define settingshashnodeBind(self, name, btyp, bvar, bdef, _ssdCurrentLockState) (self)->_->bind(SettingsHashNode(self), name, btyp, bvar, bdef, _ssdCurrentLockState)
// void settingshashnodeCheckBound(SettingsHashNode* self, strref name, SSDLockState* _ssdCurrentLockState);
//
// check a single bound variable for changes
#define settingshashnodeCheckBound(self, name, _ssdCurrentLockState) (self)->_->checkBound(SettingsHashNode(self), name, _ssdCurrentLockState)
// void settingshashnodeCheckAll(SettingsHashNode* self, SSDLockState* _ssdCurrentLockState);
//
// check for bound variables that have changed
#define settingshashnodeCheckAll(self, _ssdCurrentLockState) (self)->_->checkAll(SettingsHashNode(self), _ssdCurrentLockState)
// void settingshashnodeUnbindAll(SettingsHashNode* self, SSDLockState* _ssdCurrentLockState);
#define settingshashnodeUnbindAll(self, _ssdCurrentLockState) (self)->_->unbindAll(SettingsHashNode(self), _ssdCurrentLockState)

