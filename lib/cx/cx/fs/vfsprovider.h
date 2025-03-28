#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/fs/fs.h>
#include <cx/fs/file.h>


typedef struct VFSFileProvider {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*close)(_In_ void* self);
    bool (*read)(_In_ void* self, _Out_writes_bytes_to_(sz, *bytesread) void* buf, size_t sz, _Out_ _Deref_out_range_(0, sz) size_t* bytesread);
    bool (*write)(_In_ void* self, _In_reads_bytes_(sz) void* buf, size_t sz, _Out_opt_ _Deref_out_range_(0, sz) size_t* byteswritten);
    int64 (*tell)(_In_ void* self);
    int64 (*seek)(_In_ void* self, int64 off, FSSeekType seektype);
    bool (*flush)(_In_ void* self);
} VFSFileProvider;
extern VFSFileProvider VFSFileProvider_tmpl;

typedef struct VFSProvider {
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
} VFSProvider;
extern VFSProvider VFSProvider_tmpl;

