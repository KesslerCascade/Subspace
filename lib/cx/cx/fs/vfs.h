#pragma once

#include <cx/fs/file.h>
#include <cx/fs/fs.h>
#include <cx/fs/vfsprovider.h>
#include <cx/fs/vfsobj.h>

typedef struct VFS VFS;
typedef struct VFSFile VFSFile;

CX_C_BEGIN

// vfsCreate() is in vfsobj.h

_At_(*pvfs, _Pre_maybenull_ _Post_null_)
void vfsDestroy(VFS **pvfs);

bool vfsUnmount(_Inout_ VFS *vfs, _In_opt_ strref path);

bool _vfsMountProvider(_Inout_ VFS *vfs, _Inout_ ObjInst *provider, _In_opt_ strref path, flags_t flags);
#define vfsMountProvider(vfs, provider, path, ...) _vfsMountProvider(vfs, objInstBase(provider), path, opt_flags(__VA_ARGS__))

// Mounts the built-in OS filesystem provider to the given VFS
bool _vfsMountFS(_Inout_ VFS *vfs, _In_opt_ strref path, _In_opt_ strref fsroot, flags_t flags);
#define vfsMountFS(vfs, path, fsroot, ...) _vfsMountFS(vfs, path, fsroot, opt_flags(__VA_ARGS__))

// Mounts one VFS under another (loopback mode)
bool _vfsMountVFS(_Inout_ VFS *vfs, _In_opt_ strref path, _Inout_ VFS *vfs2, _In_opt_ strref vfs2root, flags_t flags);
#define vfsMountVFS(vfs, path, vfs2, vfs2root, ...) _vfsMountVFS(vfs, path, vfs2, vfs2root, opt_flags(__VA_ARGS__))

// Get / set current directory
void vfsCurDir(_Inout_ VFS *vfs, _Inout_ string *out);
bool vfsSetCurDir(_Inout_ VFS *vfs, _In_opt_ strref cur);
void vfsAbsolutePath(_Inout_ VFS *vfs, _Inout_ string *out, _In_opt_ strref path);

_Success_(return != FS_Nonexistent)
FSPathStat vfsStat(_Inout_ VFS *vfs, _In_opt_ strref path, _Out_opt_ FSStat *stat);

_meta_inline bool vfsExist(_Inout_ VFS *vfs, _In_opt_ strref path)
{
    return vfsStat(vfs, path, NULL) != FS_Nonexistent;
}
_meta_inline bool vfsIsDir(_Inout_ VFS *vfs, _In_opt_ strref path)
{
    return vfsStat(vfs, path, NULL) == FS_Directory;
}
_meta_inline bool vfsIsFile(_Inout_ VFS *vfs, _In_opt_ strref path)
{
    return vfsStat(vfs, path, NULL) == FS_File;
}

bool vfsSetTimes(_Inout_ VFS *vfs, _In_opt_ strref path, int64 modified, int64 accessed);

bool vfsCreateDir(_Inout_ VFS *vfs, _In_opt_ strref path);
bool vfsCreateAll(_Inout_ VFS *vfs, _In_opt_ strref path);
bool vfsRemoveDir(_Inout_ VFS *vfs, _In_opt_ strref path);
bool vfsDelete(_Inout_ VFS *vfs, _In_opt_ strref path);
bool vfsCopy(_Inout_ VFS *vfs, _In_opt_ strref from, _In_opt_ strref to);
bool vfsRename(_Inout_ VFS *vfs, _In_opt_ strref from, _In_opt_ strref to);

// Special case to get the actual underlying path from VFSFS, works *only* if the path
// in question is backed by a VFSFS provider.
bool vfsGetFSPath(_Inout_ string *out, _Inout_ VFS *vfs, _In_opt_ strref path);

typedef struct FSSearchIter FSSearchIter;
bool vfsSearchInit(_Out_ FSSearchIter *iter, _Inout_ VFS *vfs, _In_opt_ strref path,
                   _In_opt_ strref pattern, int typefilter, bool stat);
bool vfsSearchNext(_Inout_ FSSearchIter *iter);
void vfsSearchFinish(_Inout_ FSSearchIter *iter);
_meta_inline bool vfsSearchValid(_In_ FSSearchIter *iter) { return iter->_search; }

_Ret_opt_valid_ VFSFile *vfsOpen(_Inout_ VFS *vfs, _In_opt_ strref path, flags_t flags);
bool vfsClose(_Pre_opt_valid_ _Post_invalid_ VFSFile *file);
bool vfsRead(_Inout_ VFSFile *file, _Out_writes_bytes_to_(sz, *bytesread) void *buf, size_t sz, _Out_ _Deref_out_range_(0, sz) size_t *bytesread);
bool vfsWrite(_Inout_ VFSFile *file, _In_reads_bytes_(sz) void *buf, size_t sz, _Out_opt_ _Deref_out_range_(0, sz) size_t *byteswritten);
bool vfsWriteString(_Inout_ VFSFile *file, _In_opt_ strref str, _Out_opt_ size_t *byteswritten);
int64 vfsTell(_Inout_ VFSFile *file);
int64 vfsSeek(_Inout_ VFSFile *file, int64 off, FSSeekType seektype);

bool vfsFlush(_Inout_ VFSFile *file);

enum VFSFlags {
    // Flags that are valid for both VFS creation and VFS providers
    VFS_ReadOnly      = 0x00000001,     // write operations not allowed on this layer
    VFS_CaseSensitive = 0x00000002,     // filesystem or VFS is case-sensitive
    VFS_NoCache       = 0x00000004,     // do not populate file cache

    // Flags that are valid for VFS providers only (mount options)
    VFS_NewFiles      = 0x00000008,     // new file creations go to this VFS layer
    VFS_AlwaysCOW     = 0x00000010,     // write operations always create a copy in this layer,
                                        // even if a lower layer is writable
    VFS_Opaque        = 0x00000020,     // hides lower layers entirely
};

CX_C_END
