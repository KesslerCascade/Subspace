#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/fs.h>
#include <cx/thread.h>
#include "pkgstruct.h"

typedef struct PkgDirEnt PkgDirEnt;
typedef struct PkgDirEnt_WeakRef PkgDirEnt_WeakRef;
typedef struct PkgFile PkgFile;
typedef struct PkgFile_WeakRef PkgFile_WeakRef;
saDeclarePtr(PkgDirEnt);
saDeclarePtr(PkgDirEnt_WeakRef);
saDeclarePtr(PkgFile);
saDeclarePtr(PkgFile_WeakRef);

typedef struct PkgFile_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*load)(_In_ void* self);
    void (*close)(_In_ void* self);
} PkgFile_ClassIf;
extern PkgFile_ClassIf PkgFile_ClassIf_tmpl;

typedef struct PkgDirEnt {
    union {
        ObjIface* _;
        void* _is_PkgDirEnt;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string name;
    uint32 off;
    uint32 csize;
    uint32 size;
    bool compressed;
} PkgDirEnt;
extern ObjClassInfo PkgDirEnt_clsinfo;
#define PkgDirEnt(inst) ((PkgDirEnt*)(unused_noeval((inst) && &((inst)->_is_PkgDirEnt)), (inst)))
#define PkgDirEntNone ((PkgDirEnt*)NULL)

typedef struct PkgDirEnt_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_PkgDirEnt_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} PkgDirEnt_WeakRef;
#define PkgDirEnt_WeakRef(inst) ((PkgDirEnt_WeakRef*)(unused_noeval((inst) && &((inst)->_is_PkgDirEnt_WeakRef)), (inst)))

_objfactory_guaranteed PkgDirEnt* PkgDirEnt_create(_In_opt_ strref name, uint32 off, uint32 csize, uint32 size);
// PkgDirEnt* pkgdirentCreate(strref name, uint32 off, uint32 csize, uint32 size);
#define pkgdirentCreate(name, off, csize, size) PkgDirEnt_create(name, off, csize, size)


typedef struct PkgFile {
    union {
        PkgFile_ClassIf* _;
        void* _is_PkgFile;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Mutex filelock;        // I/O lock
    VFSFile* pkgfile;
    hashtable files;
    hashtable dircache;
} PkgFile;
extern ObjClassInfo PkgFile_clsinfo;
#define PkgFile(inst) ((PkgFile*)(unused_noeval((inst) && &((inst)->_is_PkgFile)), (inst)))
#define PkgFileNone ((PkgFile*)NULL)

typedef struct PkgFile_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_PkgFile_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} PkgFile_WeakRef;
#define PkgFile_WeakRef(inst) ((PkgFile_WeakRef*)(unused_noeval((inst) && &((inst)->_is_PkgFile_WeakRef)), (inst)))

_objfactory_check PkgFile* PkgFile_open(VFSFile* f);
// PkgFile* pkgfileOpen(VFSFile* f);
//
// create for reading
#define pkgfileOpen(f) PkgFile_open(f)

// bool pkgfileLoad(PkgFile* self);
#define pkgfileLoad(self) (self)->_->load(PkgFile(self))
// void pkgfileClose(PkgFile* self);
#define pkgfileClose(self) (self)->_->close(PkgFile(self))

