#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/thread.h>
#include <cx/fs/vfsobj.h>
#include <cx/ssdtree/ssdtreeobj.h>

typedef struct VFSDir VFSDir;
typedef struct SettingsTree SettingsTree;
typedef struct SettingsTree_WeakRef SettingsTree_WeakRef;
saDeclarePtr(SettingsTree);
saDeclarePtr(SettingsTree_WeakRef);

typedef struct SettingsTree_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    _objfactory_guaranteed SSDNode* (*createNode)(_In_ void* self, _In_range_(SSD_Create_None+1, SSD_Create_Count-1) SSDCreateType crtype);
} SettingsTree_ClassIf;
extern SettingsTree_ClassIf SettingsTree_ClassIf_tmpl;

typedef struct SettingsTree {
    union {
        SettingsTree_ClassIf* _;
        void* _is_SettingsTree;
        void* _is_SSDTree;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    RWLock lock;
    SSDTreeDebug dbg;
    uint32 flags;
    int64 modified;        // The most recent last-modified timestamp of any node in the tree
    SSDNodeFactory factories[SSD_Create_Count];        // Factory functions for if this tree wants to use derived node classes
    VFS* vfs;
    string filename;
    int64 interval;        // interval to flush to disk
    int64 check;        // timestamp of last background check
    int64 saved;        // timestamp the settings were last saved to disk
    bool checkbound;        // re-check all bound variables at next flush
} SettingsTree;
extern ObjClassInfo SettingsTree_clsinfo;
#define SettingsTree(inst) ((SettingsTree*)(unused_noeval((inst) && &((inst)->_is_SettingsTree)), (inst)))
#define SettingsTreeNone ((SettingsTree*)NULL)

typedef struct SettingsTree_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SettingsTree_WeakRef;
        void* _is_SSDTree_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SettingsTree_WeakRef;
#define SettingsTree_WeakRef(inst) ((SettingsTree_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SettingsTree_WeakRef)), (inst)))

_objfactory_guaranteed SettingsTree* SettingsTree_create();
// SettingsTree* settingstreeCreate();
#define settingstreeCreate() SettingsTree_create()

// SSDNode* settingstreeCreateNode(SettingsTree* self, SSDCreateType crtype);
#define settingstreeCreateNode(self, crtype) (self)->_->createNode(SettingsTree(self), crtype)

