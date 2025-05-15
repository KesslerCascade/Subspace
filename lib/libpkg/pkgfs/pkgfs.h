#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/fs/vfsprovider.h>
#include <cx/fs/vfsobj.h>
#include "pkgfile/pkgfile.h"

typedef struct VFSDir VFSDir;
typedef struct PkgFS PkgFS;
typedef struct PkgFS_WeakRef PkgFS_WeakRef;
saDeclarePtr(PkgFS);
saDeclarePtr(PkgFS_WeakRef);

typedef struct PkgFS_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // VFSProviderFlags enforced for this provider
    flags_t (*flags)(_In_ void* self);
    // returns an object that implements VFSFileProvider
    _Ret_opt_valid_ ObjInst* (*open)(_In_ void* self, _In_opt_ strref path, flags_t flags);
    FSPathStat (*stat)(_In_ void* self, _In_opt_ strref path, _When_(return != FS_Nonexistent, _Out_opt_) FSStat* stat);
    bool (*setTimes)(_In_ void* self, _In_opt_ strref path, int64 modified, int64 accessed);
    bool (*createDir)(_In_ void* self, _In_opt_ strref path);
    bool (*removeDir)(_In_ void* self, _In_opt_ strref path);
    bool (*deleteFile)(_In_ void* self, _In_opt_ strref path);
    bool (*rename)(_In_ void* self, _In_opt_ strref oldpath, _In_opt_ strref newpath);
    bool (*getFSPath)(_In_ void* self, _Inout_ string* out, _In_opt_ strref path);
    bool (*searchInit)(_In_ void* self, _Out_ FSSearchIter* iter, _In_opt_ strref path, _In_opt_ strref pattern, bool stat);
    bool (*searchValid)(_In_ void* self, _In_ FSSearchIter* iter);
    bool (*searchNext)(_In_ void* self, _Inout_ FSSearchIter* iter);
    void (*searchFinish)(_In_ void* self, _Inout_ FSSearchIter* iter);
} PkgFS_ClassIf;
extern PkgFS_ClassIf PkgFS_ClassIf_tmpl;

typedef struct PkgFS {
    union {
        PkgFS_ClassIf* _;
        void* _is_PkgFS;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    PkgFile* pkg;
} PkgFS;
extern ObjClassInfo PkgFS_clsinfo;
#define PkgFS(inst) ((PkgFS*)(unused_noeval((inst) && &((inst)->_is_PkgFS)), (inst)))
#define PkgFSNone ((PkgFS*)NULL)

typedef struct PkgFS_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_PkgFS_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} PkgFS_WeakRef;
#define PkgFS_WeakRef(inst) ((PkgFS_WeakRef*)(unused_noeval((inst) && &((inst)->_is_PkgFS_WeakRef)), (inst)))

_objfactory_guaranteed PkgFS* PkgFS_create(PkgFile* pkgf);
// PkgFS* pkgfsCreate(PkgFile* pkgf);
#define pkgfsCreate(pkgf) PkgFS_create(PkgFile(pkgf))

// flags_t pkgfsFlags(PkgFS* self);
//
// VFSProviderFlags enforced for this provider
#define pkgfsFlags(self) (self)->_->flags(PkgFS(self))
// ObjInst* pkgfsOpen(PkgFS* self, strref path, flags_t flags);
//
// returns an object that implements VFSFileProvider
#define pkgfsOpen(self, path, flags) (self)->_->open(PkgFS(self), path, flags)
// FSPathStat pkgfsStat(PkgFS* self, strref path, FSStat* stat);
#define pkgfsStat(self, path, stat) (self)->_->stat(PkgFS(self), path, stat)
// bool pkgfsSetTimes(PkgFS* self, strref path, int64 modified, int64 accessed);
#define pkgfsSetTimes(self, path, modified, accessed) (self)->_->setTimes(PkgFS(self), path, modified, accessed)
// bool pkgfsCreateDir(PkgFS* self, strref path);
#define pkgfsCreateDir(self, path) (self)->_->createDir(PkgFS(self), path)
// bool pkgfsRemoveDir(PkgFS* self, strref path);
#define pkgfsRemoveDir(self, path) (self)->_->removeDir(PkgFS(self), path)
// bool pkgfsDeleteFile(PkgFS* self, strref path);
#define pkgfsDeleteFile(self, path) (self)->_->deleteFile(PkgFS(self), path)
// bool pkgfsRename(PkgFS* self, strref oldpath, strref newpath);
#define pkgfsRename(self, oldpath, newpath) (self)->_->rename(PkgFS(self), oldpath, newpath)
// bool pkgfsGetFSPath(PkgFS* self, string* out, strref path);
#define pkgfsGetFSPath(self, out, path) (self)->_->getFSPath(PkgFS(self), out, path)
// bool pkgfsSearchInit(PkgFS* self, FSSearchIter* iter, strref path, strref pattern, bool stat);
#define pkgfsSearchInit(self, iter, path, pattern, stat) (self)->_->searchInit(PkgFS(self), iter, path, pattern, stat)
// bool pkgfsSearchValid(PkgFS* self, FSSearchIter* iter);
#define pkgfsSearchValid(self, iter) (self)->_->searchValid(PkgFS(self), iter)
// bool pkgfsSearchNext(PkgFS* self, FSSearchIter* iter);
#define pkgfsSearchNext(self, iter) (self)->_->searchNext(PkgFS(self), iter)
// void pkgfsSearchFinish(PkgFS* self, FSSearchIter* iter);
#define pkgfsSearchFinish(self, iter) (self)->_->searchFinish(PkgFS(self), iter)

