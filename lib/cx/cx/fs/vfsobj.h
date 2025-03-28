#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/thread/rwlock.h>

typedef struct VFSDir VFSDir;
typedef struct VFS VFS;
typedef struct VFS_WeakRef VFS_WeakRef;
typedef struct VFSMount VFSMount;
typedef struct VFSMount_WeakRef VFSMount_WeakRef;
saDeclarePtr(VFS);
saDeclarePtr(VFS_WeakRef);
saDeclarePtr(VFSMount);
saDeclarePtr(VFSMount_WeakRef);

typedef struct VFS {
    union {
        ObjIface* _;
        void* _is_VFS;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    VFSDir* root;        // root for namespaceless paths
    // namespaces are never case sensitive even if the paths are
    // hashtable of string/VFSDir
    hashtable namespaces;
    string curdir;
    RWLock vfslock;        // vfslock is for adding entires to the directory cache
    // vfsdlock is for operations such as mounting / unmounting / invalidating cache
    // that may destroy VFSDir entries and remove mounts
    RWLock vfsdlock;
    uint32 flags;
} VFS;
extern ObjClassInfo VFS_clsinfo;
#define VFS(inst) ((VFS*)(unused_noeval((inst) && &((inst)->_is_VFS)), (inst)))
#define VFSNone ((VFS*)NULL)

typedef struct VFS_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_VFS_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} VFS_WeakRef;
#define VFS_WeakRef(inst) ((VFS_WeakRef*)(unused_noeval((inst) && &((inst)->_is_VFS_WeakRef)), (inst)))

_objfactory_guaranteed VFS* VFS_create(uint32 flags);
// VFS* vfsCreate(uint32 flags);
//
// Create an empty VFS with nothing mounted
#define vfsCreate(flags) VFS_create(flags)

_objfactory_check VFS* VFS_createFromFS();
// VFS* vfsCreateFromFS();
//
// Create a VFS object configured to pass everything through to the
// underlying OS filesystem. The exact VFS namespace that is created
// is platform dependant.
#define vfsCreateFromFS() VFS_createFromFS()


typedef struct VFSMount {
    union {
        ObjIface* _;
        void* _is_VFSMount;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    ObjInst* provider;
    uint32 flags;
} VFSMount;
extern ObjClassInfo VFSMount_clsinfo;
#define VFSMount(inst) ((VFSMount*)(unused_noeval((inst) && &((inst)->_is_VFSMount)), (inst)))
#define VFSMountNone ((VFSMount*)NULL)

typedef struct VFSMount_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_VFSMount_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} VFSMount_WeakRef;
#define VFSMount_WeakRef(inst) ((VFSMount_WeakRef*)(unused_noeval((inst) && &((inst)->_is_VFSMount_WeakRef)), (inst)))

_objfactory_guaranteed VFSMount* VFSMount_create(ObjInst* provider, uint32 flags);
// VFSMount* vfsmountCreate(ObjInst* provider, uint32 flags);
#define vfsmountCreate(provider, flags) VFSMount_create(ObjInst(provider), flags)


