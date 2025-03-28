#include "vfs_private.h"
#include "cx/debug/error.h"
#include "vfsfs/vfsfs.h"

_Use_decl_annotations_
FSPathStat vfsStat(VFS *vfs, strref path, FSStat *stat)
{
    int ret = FS_Nonexistent;
    string rpath = 0;

    VFSMount *m = _vfsFindMount(vfs, &rpath, path, NULL, NULL, VFS_FindCache);
    if (!m) {
        ret = FS_Nonexistent;
        goto out;
    }

    VFSProvider *provif = objInstIf(m->provider, VFSProvider);
    if (provif)
        ret = provif->stat(m->provider, rpath, stat);
    else
        cxerr = CX_InvalidArgument;

    if (ret == FS_Nonexistent)
        _vfsInvalidateCache(vfs, path);

out:
    strDestroy(&rpath);
    objRelease(&m);
    return ret;
}

_Use_decl_annotations_
bool vfsSetTimes(VFS *vfs, strref path, int64 modified, int64 accessed)
{
    bool ret = false;
    string rpath = 0;

    VFSMount *m = _vfsFindMount(vfs, &rpath, path, NULL, NULL, VFS_FindCache);
    if (!m)
        goto out;

    VFSProvider *provif = objInstIf(m->provider, VFSProvider);
    if (provif)
        ret = provif->setTimes(m->provider, rpath, modified, accessed);
    else
        cxerr = CX_InvalidArgument;

out:
    strDestroy(&rpath);
    objRelease(&m);
    return ret;
}

_Use_decl_annotations_
bool vfsCreateDir(VFS *vfs, strref path)
{
    bool ret = false;
    string rpath = 0;

    VFSMount *m = _vfsFindMount(vfs, &rpath, path, NULL, NULL, VFS_FindCreate);
    if (!m) {
        cxerr = CX_FileNotFound;
        goto out;
    }

    // shouldn't happen, but double check
    if (m->flags & VFS_ReadOnly) {
        cxerr = CX_ReadOnly;
        goto out;
    }

    VFSProvider *provif = objInstIf(m->provider, VFSProvider);
    if (provif)
        ret = provif->createDir(m->provider, rpath);
    else
        cxerr = CX_InvalidArgument;

out:
    strDestroy(&rpath);
    objRelease(&m);
    return ret;
}

_Use_decl_annotations_
bool vfsCreateAll(VFS *vfs, strref path)
{
    string parent = 0;
    pathParent(&parent, path);
    if (!strEmpty(parent) && !vfsExist(vfs, parent))
        vfsCreateAll(vfs, parent);
    strDestroy(&parent);

    return vfsCreateDir(vfs, path);
}

_Use_decl_annotations_
bool vfsRemoveDir(VFS *vfs, strref path)
{
    bool ret = false;
    string rpath = 0;

    VFSMount *m = _vfsFindMount(vfs, &rpath, path, NULL, NULL, VFS_FindDelete);
    if (!m) {
        cxerr = CX_FileNotFound;
        goto out;
    }

    if (m->flags & VFS_ReadOnly) {
        cxerr = CX_ReadOnly;
        goto out;
    }

    VFSProvider *provif = objInstIf(m->provider, VFSProvider);
    if (provif)
        ret = provif->removeDir(m->provider, rpath);
    else
        cxerr = CX_InvalidArgument;

out:
    strDestroy(&rpath);
    objRelease(&m);
    return ret;
}

_Use_decl_annotations_
bool vfsDelete(VFS *vfs, strref path)
{
    bool ret = false;
    string rpath = 0;

    VFSMount *m = _vfsFindMount(vfs, &rpath, path, NULL, NULL, VFS_FindDelete);
    if (!m) {
        cxerr = CX_FileNotFound;
        goto out;
    }

    if (m->flags & VFS_ReadOnly) {
        cxerr = CX_ReadOnly;
        goto out;
    }

    VFSProvider *provif = objInstIf(m->provider, VFSProvider);
    if (provif)
        ret = provif->deleteFile(m->provider, rpath);
    else
        cxerr = CX_InvalidArgument;

    _vfsInvalidateCache(vfs, path);

out:
    strDestroy(&rpath);
    objRelease(&m);
    return ret;
}

#define COPYBLOCKSIZE 65536
_Use_decl_annotations_
bool vfsCopy(VFS *vfs, strref from, strref to)
{
    VFSFile *srcfile = 0, *dstfile = 0;
    bool ret = false;
    size_t bytes;

    uint8 *buf = xaAlloc(COPYBLOCKSIZE);
    srcfile = vfsOpen(vfs, from, FS_Read);
    if (!srcfile)
        goto out;
    dstfile = vfsOpen(vfs, to, FS_Overwrite);
    if (!dstfile)
        goto out;

    for (;;) {
        if (!vfsRead(srcfile, buf, COPYBLOCKSIZE, &bytes))
            goto out;
        if (bytes == 0)
            break;      // eof
        if (!vfsWrite(dstfile, buf, bytes, NULL))
            goto out;
    }

    ret = true;

out:
    vfsClose(srcfile);
    if (dstfile) {
        vfsClose(dstfile);
        if (!ret)
            vfsDelete(vfs, to);
    }
    xaFree(buf);
    return ret;
}

_Use_decl_annotations_
bool vfsRename(VFS *vfs, strref from, strref to)
{
    bool ret = false;
    string rpathfrom = 0, rpathto = 0;

    VFSMount *mfrom = _vfsFindMount(vfs, &rpathfrom, from, NULL, NULL, VFS_FindCache);
    VFSMount *mto = _vfsFindMount(vfs, &rpathto, to, NULL, NULL, VFS_FindWriteFile | VFS_FindCreate | VFS_FindCache);
    if (!(mfrom && mto)) {
        ret = FS_Nonexistent;
        goto out;
    }

    if (mto->flags & VFS_ReadOnly) {
        cxerr = CX_ReadOnly;
        goto out;
    }

    VFSProvider *provif = objInstIf(mto->provider, VFSProvider);
    if (!provif) {
        cxerr = CX_InvalidArgument;
        goto out;
    }

    if (provif->stat(mto->provider, rpathto, NULL) != FS_Nonexistent) {
        // destination already exists!
        cxerr = CX_AlreadyExists;
        goto out;
    }

    if (mfrom->provider == mto->provider) {
        // Same provider, so we can just rename it
        ret = provif->rename(mfrom->provider, rpathfrom, rpathto);
    } else {
        // Different providers, copy and delete original
        ret = vfsCopy(vfs, from, to);
        if (ret)
            vfsDelete(vfs, from);
    }

out:
    strDestroy(&rpathfrom);
    strDestroy(&rpathto);
    objRelease(&mfrom);
    objRelease(&mto);
    return ret;
}

_Use_decl_annotations_
bool vfsGetFSPath(string *out, VFS *vfs, strref path)
{
    bool ret = false;
    string rpath = 0;

    VFSMount *m = _vfsFindMount(vfs, &rpath, path, NULL, NULL, 0);
    if (!m) {
        cxerr = CX_FileNotFound;
        goto out;
    }

    VFSProvider *provif = objInstIf(m->provider, VFSProvider);
    if (provif)
        ret = provif->getFSPath(m->provider, out, rpath);
    else
        cxerr = CX_InvalidArgument;

out:
    strDestroy(&rpath);
    objRelease(&m);
    return ret;
}
