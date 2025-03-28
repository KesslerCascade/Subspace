#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/fs/vfsprovider.h>

typedef struct VFSFS VFSFS;
typedef struct VFSFS_WeakRef VFSFS_WeakRef;
saDeclarePtr(VFSFS);
saDeclarePtr(VFSFS_WeakRef);

typedef struct VFSFS_ClassIf {
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
} VFSFS_ClassIf;
extern VFSFS_ClassIf VFSFS_ClassIf_tmpl;

typedef struct VFSFS {
    union {
        VFSFS_ClassIf* _;
        void* _is_VFSFS;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string root;
} VFSFS;
extern ObjClassInfo VFSFS_clsinfo;
#define VFSFS(inst) ((VFSFS*)(unused_noeval((inst) && &((inst)->_is_VFSFS)), (inst)))
#define VFSFSNone ((VFSFS*)NULL)

typedef struct VFSFS_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_VFSFS_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} VFSFS_WeakRef;
#define VFSFS_WeakRef(inst) ((VFSFS_WeakRef*)(unused_noeval((inst) && &((inst)->_is_VFSFS_WeakRef)), (inst)))

_objfactory_check VFSFS* VFSFS_create(_In_opt_ strref rootpath);
// VFSFS* vfsfsCreate(strref rootpath);
#define vfsfsCreate(rootpath) VFSFS_create(rootpath)

// flags_t vfsfsFlags(VFSFS* self);
//
// VFSProviderFlags enforced for this provider
#define vfsfsFlags(self) (self)->_->flags(VFSFS(self))
// ObjInst* vfsfsOpen(VFSFS* self, strref path, flags_t flags);
//
// returns an object that implements VFSFileProvider
#define vfsfsOpen(self, path, flags) (self)->_->open(VFSFS(self), path, flags)
// FSPathStat vfsfsStat(VFSFS* self, strref path, FSStat* stat);
#define vfsfsStat(self, path, stat) (self)->_->stat(VFSFS(self), path, stat)
// bool vfsfsSetTimes(VFSFS* self, strref path, int64 modified, int64 accessed);
#define vfsfsSetTimes(self, path, modified, accessed) (self)->_->setTimes(VFSFS(self), path, modified, accessed)
// bool vfsfsCreateDir(VFSFS* self, strref path);
#define vfsfsCreateDir(self, path) (self)->_->createDir(VFSFS(self), path)
// bool vfsfsRemoveDir(VFSFS* self, strref path);
#define vfsfsRemoveDir(self, path) (self)->_->removeDir(VFSFS(self), path)
// bool vfsfsDeleteFile(VFSFS* self, strref path);
#define vfsfsDeleteFile(self, path) (self)->_->deleteFile(VFSFS(self), path)
// bool vfsfsRename(VFSFS* self, strref oldpath, strref newpath);
#define vfsfsRename(self, oldpath, newpath) (self)->_->rename(VFSFS(self), oldpath, newpath)
// bool vfsfsGetFSPath(VFSFS* self, string* out, strref path);
#define vfsfsGetFSPath(self, out, path) (self)->_->getFSPath(VFSFS(self), out, path)
// bool vfsfsSearchInit(VFSFS* self, FSSearchIter* iter, strref path, strref pattern, bool stat);
#define vfsfsSearchInit(self, iter, path, pattern, stat) (self)->_->searchInit(VFSFS(self), iter, path, pattern, stat)
// bool vfsfsSearchValid(VFSFS* self, FSSearchIter* iter);
#define vfsfsSearchValid(self, iter) (self)->_->searchValid(VFSFS(self), iter)
// bool vfsfsSearchNext(VFSFS* self, FSSearchIter* iter);
#define vfsfsSearchNext(self, iter) (self)->_->searchNext(VFSFS(self), iter)
// void vfsfsSearchFinish(VFSFS* self, FSSearchIter* iter);
#define vfsfsSearchFinish(self, iter) (self)->_->searchFinish(VFSFS(self), iter)

