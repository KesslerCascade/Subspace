#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/fs/vfsprovider.h>
#include <cx/fs/vfsobj.h>

typedef struct VFSDir VFSDir;
typedef struct VFSVFS VFSVFS;
typedef struct VFSVFS_WeakRef VFSVFS_WeakRef;
saDeclarePtr(VFSVFS);
saDeclarePtr(VFSVFS_WeakRef);

typedef struct VFSVFS_ClassIf {
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
} VFSVFS_ClassIf;
extern VFSVFS_ClassIf VFSVFS_ClassIf_tmpl;

typedef struct VFSVFS {
    union {
        VFSVFS_ClassIf* _;
        void* _is_VFSVFS;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    VFS* vfs;
    string root;
} VFSVFS;
extern ObjClassInfo VFSVFS_clsinfo;
#define VFSVFS(inst) ((VFSVFS*)(unused_noeval((inst) && &((inst)->_is_VFSVFS)), (inst)))
#define VFSVFSNone ((VFSVFS*)NULL)

typedef struct VFSVFS_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_VFSVFS_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} VFSVFS_WeakRef;
#define VFSVFS_WeakRef(inst) ((VFSVFS_WeakRef*)(unused_noeval((inst) && &((inst)->_is_VFSVFS_WeakRef)), (inst)))

_objfactory_guaranteed VFSVFS* VFSVFS_create(VFS* vfs, _In_opt_ strref rootpath);
// VFSVFS* vfsvfsCreate(VFS* vfs, strref rootpath);
#define vfsvfsCreate(vfs, rootpath) VFSVFS_create(VFS(vfs), rootpath)

// flags_t vfsvfsFlags(VFSVFS* self);
//
// VFSProviderFlags enforced for this provider
#define vfsvfsFlags(self) (self)->_->flags(VFSVFS(self))
// ObjInst* vfsvfsOpen(VFSVFS* self, strref path, flags_t flags);
//
// returns an object that implements VFSFileProvider
#define vfsvfsOpen(self, path, flags) (self)->_->open(VFSVFS(self), path, flags)
// FSPathStat vfsvfsStat(VFSVFS* self, strref path, FSStat* stat);
#define vfsvfsStat(self, path, stat) (self)->_->stat(VFSVFS(self), path, stat)
// bool vfsvfsSetTimes(VFSVFS* self, strref path, int64 modified, int64 accessed);
#define vfsvfsSetTimes(self, path, modified, accessed) (self)->_->setTimes(VFSVFS(self), path, modified, accessed)
// bool vfsvfsCreateDir(VFSVFS* self, strref path);
#define vfsvfsCreateDir(self, path) (self)->_->createDir(VFSVFS(self), path)
// bool vfsvfsRemoveDir(VFSVFS* self, strref path);
#define vfsvfsRemoveDir(self, path) (self)->_->removeDir(VFSVFS(self), path)
// bool vfsvfsDeleteFile(VFSVFS* self, strref path);
#define vfsvfsDeleteFile(self, path) (self)->_->deleteFile(VFSVFS(self), path)
// bool vfsvfsRename(VFSVFS* self, strref oldpath, strref newpath);
#define vfsvfsRename(self, oldpath, newpath) (self)->_->rename(VFSVFS(self), oldpath, newpath)
// bool vfsvfsGetFSPath(VFSVFS* self, string* out, strref path);
#define vfsvfsGetFSPath(self, out, path) (self)->_->getFSPath(VFSVFS(self), out, path)
// bool vfsvfsSearchInit(VFSVFS* self, FSSearchIter* iter, strref path, strref pattern, bool stat);
#define vfsvfsSearchInit(self, iter, path, pattern, stat) (self)->_->searchInit(VFSVFS(self), iter, path, pattern, stat)
// bool vfsvfsSearchValid(VFSVFS* self, FSSearchIter* iter);
#define vfsvfsSearchValid(self, iter) (self)->_->searchValid(VFSVFS(self), iter)
// bool vfsvfsSearchNext(VFSVFS* self, FSSearchIter* iter);
#define vfsvfsSearchNext(self, iter) (self)->_->searchNext(VFSVFS(self), iter)
// void vfsvfsSearchFinish(VFSVFS* self, FSSearchIter* iter);
#define vfsvfsSearchFinish(self, iter) (self)->_->searchFinish(VFSVFS(self), iter)

