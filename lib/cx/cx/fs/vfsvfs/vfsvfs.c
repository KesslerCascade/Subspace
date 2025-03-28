// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "vfsvfs.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "vfsvfsfile.h"
#include "cx/fs/path.h"

_objfactory_guaranteed VFSVFS* VFSVFS_create(VFS* vfs, _In_opt_ strref rootpath)
{
    VFSVFS *ret;
    ret = objInstCreate(VFSVFS);

    ret->vfs = objAcquire(vfs);
    strDup(&ret->root, rootpath);

    objInstInit(ret);
    return ret;
}

flags_t VFSVFS_flags(_In_ VFSVFS* self)
{
    return 0;
}

_Ret_opt_valid_ ObjInst* VFSVFS_open(_In_ VFSVFS* self, _In_opt_ strref path, flags_t flags)
{
    string vfspath = 0;
    pathJoin(&vfspath, self->root, path);

    VFSFile *file = vfsOpen(self->vfs, vfspath, flags);
    strDestroy(&vfspath);
    if (!file)
        return NULL;

    VFSVFSFile *fileprov = vfsvfsfileCreate(file);
    return objInstBase(fileprov);
}

FSPathStat VFSVFS_stat(_In_ VFSVFS* self, _In_opt_ strref path, _When_(return != FS_Nonexistent, _Out_opt_) FSStat* stat)
{
    string vfspath = 0;
    pathJoin(&vfspath, self->root, path);

    int ret = vfsStat(self->vfs, vfspath, stat);

    strDestroy(&vfspath);
    return ret;
}

bool VFSVFS_setTimes(_In_ VFSVFS* self, _In_opt_ strref path, int64 modified, int64 accessed)
{
    string vfspath = 0;
    pathJoin(&vfspath, self->root, path);

    bool ret = vfsSetTimes(self->vfs, vfspath, modified, accessed);

    strDestroy(&vfspath);
    return ret;
}

bool VFSVFS_createDir(_In_ VFSVFS* self, _In_opt_ strref path)
{
    string vfspath = 0;
    pathJoin(&vfspath, self->root, path);

    bool ret = vfsCreateDir(self->vfs, vfspath);

    strDestroy(&vfspath);
    return ret;
}

bool VFSVFS_removeDir(_In_ VFSVFS* self, _In_opt_ strref path)
{
    string vfspath = 0;
    pathJoin(&vfspath, self->root, path);

    bool ret = vfsRemoveDir(self->vfs, vfspath);

    strDestroy(&vfspath);
    return ret;
}

bool VFSVFS_deleteFile(_In_ VFSVFS* self, _In_opt_ strref path)
{
    string vfspath = 0;
    pathJoin(&vfspath, self->root, path);

    bool ret = vfsDelete(self->vfs, vfspath);

    strDestroy(&vfspath);
    return ret;
}

bool VFSVFS_rename(_In_ VFSVFS* self, _In_opt_ strref oldpath, _In_opt_ strref newpath)
{
    string vfsoldpath = 0, vfsnewpath = 0;
    pathJoin(&vfsoldpath, self->root, oldpath);
    pathJoin(&vfsnewpath, self->root, newpath);

    bool ret = vfsRename(self->vfs, vfsoldpath, vfsnewpath);

    strDestroy(&vfsoldpath);
    strDestroy(&vfsnewpath);
    return ret;
}

void VFSVFS_destroy(_In_ VFSVFS* self)
{
    // Autogen begins -----
    objRelease(&self->vfs);
    strDestroy(&self->root);
    // Autogen ends -------
}

bool VFSVFS_searchInit(_In_ VFSVFS* self, _Out_ FSSearchIter* iter, _In_opt_ strref path, _In_opt_ strref pattern, bool stat)
{
    string vfspath = 0;

    pathJoin(&vfspath, self->root, path);
    bool ret = vfsSearchInit(iter, self->vfs, vfspath, pattern, 0, stat);
    strDestroy(&vfspath);
    return ret;
}

bool VFSVFS_searchValid(_In_ VFSVFS* self, _In_ FSSearchIter* iter)
{
    return vfsSearchValid(iter);
}

bool VFSVFS_searchNext(_In_ VFSVFS* self, _Inout_ FSSearchIter* iter)
{
    return vfsSearchNext(iter);
}

void VFSVFS_searchFinish(_In_ VFSVFS* self, _Inout_ FSSearchIter* iter)
{
    vfsSearchFinish(iter);
}

bool VFSVFS_getFSPath(_In_ VFSVFS* self, _Inout_ string* out, _In_opt_ strref path)
{
    string vfspath = 0;
    pathJoin(&vfspath, self->root, path);

    bool ret = vfsGetFSPath(out, self->vfs, vfspath);

    strDestroy(&vfspath);
    return ret;
}

// Autogen begins -----
#include "vfsvfs.auto.inc"
// Autogen ends -------
