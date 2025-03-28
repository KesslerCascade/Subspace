#pragma once

#include "vfs.h"
#include "fs_private.h"
#include "cx/container.h"
#include "cx/string.h"

typedef struct VFSFile {
    VFS *vfs;

    ObjInst *fileprov;      // VFSFileProvider
    VFSFileProvider *fileprovif;

    // for copy-on-write files
    ObjInst *cowprov;       // VFSProvider
    string cowpath;         // absolute path to COW file (for cache invalidation)
    string cowrpath;        // relative path for COW file for provider
} VFSFile;

typedef struct VFSDirEnt {
    string name;
    int type;
    FSStat stat;
} VFSDirEnt;
saDeclare(VFSDirEnt);

typedef struct VFSSearch {
    VFS *vfs;

    sa_VFSDirEnt ents;
    int32 idx;
} VFSSearch;

// object-like structures for VFS
// these use custom type ops instead of the object framework so that
// they can be tightly packed into arrays/hashtables

typedef struct VFSMount VFSMount;
typedef struct VFSCacheEnt {
    VFSMount *mount;        // which VFS mount this file belongs to
    string origpath;        // original path (relative to provider)
} VFSCacheEnt;
VFSCacheEnt *_vfsCacheEntCreate(VFSMount *m, strref opath);
extern STypeOps VFSCacheEnt_ops;

typedef struct VFSDir VFSDir;
typedef struct VFSDir {
    string name;
    VFSDir *parent;
    sa_VFSMount mounts;     // VFS providers mounted in this directory

    hashtable subdirs;      // hashtable of string/VFSDir*

    // CACHE
    hashtable files;        // hashtable of string/VFSCacheEnt*
    uint64 touched;         // timestamp of last use
    bool cache;             // only exists to cache directory entries, can be discarded
} VFSDir;
_Ret_valid_
VFSDir *_vfsDirCreate(_Inout_ VFS *vfs, _In_opt_ VFSDir *parent);
extern STypeOps VFSDir_ops;

// gets (and creates) path in VFS cache
// must be called with vfslock read (or write) lock held!
_Ret_valid_
_When_(!writelockheld, _Requires_shared_lock_held_(vfs->vfslock))
VFSDir *_vfsGetDir(_Inout_ VFS *vfs, _In_opt_ strref path, bool isfile, bool cache, bool writelockheld);
// gets a file from VFS cache if it exists
// must be called with vfslock read (or write) lock held!
_Ret_valid_
_When_(!writelockheld, _Requires_shared_lock_held_(vfs->vfslock))
VFSCacheEnt *_vfsGetFile(_Inout_ VFS *vfs, _In_opt_ strref path, bool writelockheld);
// finds a suitable provider for a particular file
enum VFS_FIND_PROVIDER_ENUM {
    VFS_FindWriteFile = 0x0100,
    VFS_FindCreate =    0x0200,
    VFS_FindDelete =    0x0400,
    VFS_FindCache =     0x1000,
};
_Ret_opt_valid_
VFSMount *_vfsFindMount(_Inout_ VFS *vfs, _Inout_ string *rpath, _In_opt_ strref path, _Out_opt_ VFSMount **cowmount, _Inout_opt_ string *cowrpath, flags_t flags);
void _vfsInvalidateCache(_Inout_ VFS *vfs, _In_opt_ strref path);
void _vfsInvalidateRecursive(_Inout_ VFS *vfs, _In_ VFSDir *dir, bool havelock);
void _vfsAbsPath(_Inout_ VFS *vfs, _Inout_ string *out, _In_opt_ strref path);

_Requires_shared_lock_held_(vfs->vfslock)
int _vfsFindCIHelper(_Inout_ VFS *vfs, _In_ VFSDir *vdir, _Inout_ string *out, _In_ sa_string components, _Inout_ VFSMount *mount, _Inout_ VFSProvider *provif);

bool _vfsAddPlatformSpecificMounts(_Inout_ VFS *vfs);
bool _vfsIsPlatformCaseSensitive();
