#include "vfs_private.h"
#include "cx/debug/error.h"
#include "cx/time/clock.h"
#include "vfsfs/vfsfs.h"
#include "vfsvfs/vfsvfs.h"

static void vfsUnmountAll(_Inout_ VFSDir *dir)
{
    foreach(hashtable, sdi, dir->subdirs) {
        vfsUnmountAll((VFSDir*)htiVal(ptr, sdi));
    }
    saClear(&dir->mounts);
}

_Use_decl_annotations_
void vfsDestroy(VFS **pvfs)
{
    if (!(pvfs && *pvfs))
        return;

    // Unmount all filesystems
    // This is to break a reference loop that can happen in a fairly common case
    // of mounting a VFS backed by a file that is in the same VFS as it's
    // being mounted to.

    VFS* vfs = *pvfs;
    rwlockAcquireWrite(&vfs->vfsdlock);
    foreach(hashtable, nsi, vfs->namespaces) {
        vfsUnmountAll((VFSDir*)htiVal(ptr, nsi));
    }
    vfsUnmountAll(vfs->root);
    htClear(&vfs->namespaces);
    htClear(&vfs->root->subdirs);
    htClear(&vfs->root->files);
    rwlockReleaseWrite(&vfs->vfsdlock);

    objRelease(pvfs);
}

_When_(!writelockheld, _Requires_shared_lock_held_(vfs->vfslock))
static _Ret_valid_ VFSDir *_vfsGetDirInternal(_Inout_ VFS *vfs, _Inout_ VFSDir *root, _In_reads_(plen) string *path, int32 plen, bool cache, uint64 now, bool writelockheld)
{
    root->touched = now;

    // if something in the path isn't cachable, the entire path becomes exempt
    if (!cache)
        root->cache = false;

    if (plen == 0)
        return root;

    VFSDir *child = 0;

    // empty path component means this is a root
    if (strEmpty(path[0]))
        child = root;
    else
        htFind(root->subdirs, string, path[0], ptr, &child);

    if (!child) {
        if (!writelockheld) {
            rwlockReleaseRead(&vfs->vfslock);
            rwlockAcquireWrite(&vfs->vfslock);
            // try again with the write lock held
            htFind(root->subdirs, string, path[0], ptr, &child);
        }
        if (!child) {
            child = _vfsDirCreate(vfs, root);
            child->cache = cache;
            strDup(&child->name, path[0]);
            htInsert(&root->subdirs, string, path[0], ptr, child);
        }
        if (!writelockheld) {
            rwlockDowngradeWrite(&vfs->vfslock);
        }
    }

    return _vfsGetDirInternal(vfs, child, &path[1], plen - 1, cache, now, writelockheld);
}

_Use_decl_annotations_
VFSDir *_vfsGetDir(VFS *vfs, strref path, bool isfile, bool cache, bool writelockheld)
{
    VFSDir *d, *ret = 0;
    string ns = 0;
    sa_string components;

    saInit(&components, string, 8, SA_Grow(Aggressive));
    pathDecompose(&ns, &components, path);

    if (strEmpty(ns)) {
        d = vfs->root;
    } else if (!htFind(vfs->namespaces, string, ns, ptr, &d)) {
        cxerr = CX_FileNotFound;
        goto out;
    }

    ret = _vfsGetDirInternal(vfs, d, components.a, saSize(components) - (isfile ? 1 : 0), cache, clockTimer(), writelockheld);

out:
    strDestroy(&ns);
    saDestroy(&components);
    return ret;
}

_Use_decl_annotations_
bool _vfsMountProvider(VFS *vfs, ObjInst *provider, strref path, flags_t flags)
{
    string ns = 0, rpath = 0;
    VFSProvider *provif;
    bool ret = false;

    // verify that this implements the right interface
    provif = objInstIf(provider, VFSProvider);
    if (!provif)
        return false;

    rwlockAcquireWrite(&vfs->vfsdlock);

    if (!pathIsAbsolute(path))
        goto out;           // must mount with an absolute path

    pathSplitNS(&ns, &rpath, path);
    strDestroy(&rpath);

    if (!strEmpty(ns) && !htHasKey(vfs->namespaces, string, ns)) {
        // namespace hasn't been added yet, create it now
        htInsert(&vfs->namespaces, string, ns, ptr, _vfsDirCreate(vfs, NULL));
    }

    VFSDir *dir = _vfsGetDir(vfs, path, false, false, true);
    if (!dir)
        goto out;

    // propagate certain flags from the VFS to all mounted providers
    if (vfs->flags & VFS_ReadOnly)
        flags |= VFS_ReadOnly;
    if (vfs->flags & VFS_NoCache)
        flags |= VFS_NoCache;

    VFSMount *nmount = vfsmountCreate(provider, flags | provif->flags(provider));
    saPushC(&dir->mounts, object, &nmount);
    _vfsInvalidateRecursive(vfs, dir, true);
    ret = true;

out:
    rwlockReleaseWrite(&vfs->vfsdlock);
    strDestroy(&ns);
    return ret;
}

_Use_decl_annotations_
bool vfsUnmount(VFS *vfs, strref path)
{
    string ns = 0, rpath = 0;
    bool ret = false;

    rwlockAcquireWrite(&vfs->vfsdlock);

    if (!pathIsAbsolute(path))
        goto out;           // must unmount with an absolute path

    pathSplitNS(&ns, &rpath, path);
    strDestroy(&rpath);

    VFSDir *dir = _vfsGetDir(vfs, path, false, true, true);
    if (!dir)
        goto out;

    vfsUnmountAll(dir);

    if (dir->parent) {
        // remove this dir from the tree; it'll be recached if a parent provider
        // still has it
        htRemove(&dir->parent->subdirs, string, dir->name);
    } else {
        // this is the root of something
        if (!strEmpty(ns)) {
            // it's a namespace, nuke it
            htRemove(&vfs->namespaces, string, ns);
        } else {
            // the root namespace should never be removed...
            // but invalidate the cache
            _vfsInvalidateRecursive(vfs, dir, true);
        }
    }
    ret = true;

out:
    rwlockReleaseWrite(&vfs->vfsdlock);
    strDestroy(&ns);
    return ret;
}

// Mounts the built-in OS filesystem provider to the given VFS
_Use_decl_annotations_
bool _vfsMountFS(VFS *vfs, strref path, strref fsroot, flags_t flags)
{
    VFSFS *fsprovider = vfsfsCreate(fsroot);
    if (!fsprovider)
        return false;

    bool ret = _vfsMountProvider(vfs, objInstBase(fsprovider), path, flags);
    objRelease(&fsprovider);
    return ret;
}

// Mounts one VFS underneath another
_Use_decl_annotations_
bool _vfsMountVFS(VFS *vfs, strref path, VFS *vfs2, strref vfs2root, flags_t flags)
{
    VFSVFS *vfsprovider = vfsvfsCreate(vfs2, vfs2root);
    if (!vfsprovider)
        return false;

    bool ret = _vfsMountProvider(vfs, objInstBase(vfsprovider), path, flags);
    objRelease(&vfsprovider);
    return ret;
}

_Use_decl_annotations_
VFSCacheEnt *_vfsGetFile(VFS *vfs, strref path, bool writelockheld)
{
    VFSDir *pdir = _vfsGetDir(vfs, path, true, true, writelockheld);
    VFSCacheEnt *ret = 0;
    string fname = 0;

    if (!pdir)
        return NULL;

    pathFilename(&fname, path);

    if (!htFind(pdir->files, string, fname, ptr, &ret))
        cxerr = CX_FileNotFound;

    strDestroy(&fname);
    return ret;
}

_Use_decl_annotations_
void _vfsInvalidateCache(VFS *vfs, strref path)
{
    string abspath = 0, fname = 0;

    rwlockAcquireWrite(&vfs->vfsdlock);
    _vfsAbsPath(vfs, &abspath, path);
    VFSDir *pdir = _vfsGetDir(vfs, abspath, true, true, true);

    if (!pdir) {
        rwlockReleaseWrite(&vfs->vfsdlock);
        strDestroy(&abspath);
        return;
    }

    pathFilename(&fname, abspath);
    htRemove(&pdir->files, string, fname);
    rwlockReleaseWrite(&vfs->vfsdlock);
    strDestroy(&fname);
    strDestroy(&abspath);
}

_Use_decl_annotations_
void _vfsInvalidateRecursive(VFS *vfs, VFSDir *dir, bool havelock)
{
    if (!havelock)
        rwlockAcquireWrite(&vfs->vfsdlock);

    if (dir->cache && dir->parent) {
        // can just remove the whole thing
        htRemove(&dir->parent->subdirs, string, dir->name);
    } else {
        htClear(&dir->files);
        foreach(hashtable, sdi, dir->subdirs) {
            _vfsInvalidateRecursive(vfs, (VFSDir*)htiVal(ptr, sdi), true);
        }
    }

    if (!havelock)
        rwlockReleaseWrite(&vfs->vfsdlock);
}

_Use_decl_annotations_
void _vfsAbsPath(VFS *vfs, string *out, strref path)
{
    if (pathIsAbsolute(path))
        strDup(out, path);
    else
        pathJoin(out, vfs->curdir, path);
}

_Use_decl_annotations_
void vfsAbsolutePath(VFS *vfs, string *out, strref path)
{
    _vfsAbsPath(vfs, out, path);
}

static int vfsFindCISub(_Inout_ VFSDir *vdir, _Inout_ string *out, _In_opt_ strref path,
        _In_reads_(target) string *components, int depth, int target,
        _Inout_ VFSMount *mount, _Inout_ VFSProvider *provif)
{
    int ret = FS_Nonexistent;
    string filepath = 0;
    VFSDir *cvdir = vdir;

    // walk backwards up vdir tree to current depth for caching
    for (int i = depth; i < target && cvdir; i++) {
        cvdir = cvdir->parent;
    }

    // get a directory listing from the current depth
    FSSearchIter dsiter;
    if (!provif->searchInit(mount->provider, &dsiter, path, NULL, false)) {
        provif->searchFinish(mount->provider, &dsiter);
        return ret;
    }

    do {
        pathJoin(&filepath, path, dsiter.name);

        // if we haven't found it yet (the loop continues to cache even after
        // we do), check to see if this entry matches what we're looking for
        // at the current depth
        if (ret == FS_Nonexistent && strEqi(dsiter.name, components[depth])) {
            if (depth == target) {
                // this is it!
                strDup(out, filepath);
                ret = dsiter.type;
            } else if (dsiter.type == FS_Directory) {
                // not at the target depth yet, so recurse into all matching
                // subdirectories (there may be more than one in a case
                // sensitive filesystem!)
                ret = vfsFindCISub(vdir, out, filepath, components,
                        depth + 1, target, mount, provif);
            }
        }

        if (cvdir && dsiter.type == FS_File && !(mount->flags & VFS_NoCache)) {
            // go ahead and add it to the cache while we're here
            VFSCacheEnt *newent = _vfsCacheEntCreate(mount, filepath);
            htInsertC(&cvdir->files, string, dsiter.name, ptr, &newent, HT_Ignore);
        }
    } while (provif->searchNext(mount->provider, &dsiter));
    provif->searchFinish(mount->provider, &dsiter);

    strDestroy(&filepath);
    return ret;
}

_Use_decl_annotations_
int _vfsFindCIHelper(VFS *vfs, VFSDir *vdir, string *out, sa_string components, VFSMount *mount, VFSProvider *provif)
{
    // This is ugly and slow. The hope is that once a given file is found, the
    // VFS cache helps take the edge off. All these dir searches help populate
    // the cache for neighboring files as well.

    if (saSize(components) == 0)
        return FS_Nonexistent;

    int ret;
    // grab write lock for this
    rwlockReleaseRead(&vfs->vfslock);
    rwlockAcquireWrite(&vfs->vfslock);

    ret = vfsFindCISub(vdir, out, NULL, components.a, 0, saSize(components) - 1, mount, provif);

    rwlockDowngradeWrite(&vfs->vfslock);
    return ret;
}

// This function does all the heavy lifting of the VFS system.
// It's a bit hard to follow as a result.
_Use_decl_annotations_
VFSMount *_vfsFindMount(VFS *vfs, string *rpath, strref path, VFSMount **cowmount, string *cowrpath, uint32 flags)
{
    VFSMount *ret = 0;
    string abspath = 0, curpath = 0;

    if (cowmount)
        *cowmount = NULL;

    if (!vfs)
        return NULL;

    bool flwrite = flags & VFS_FindWriteFile;
    bool fldelete = flags & VFS_FindDelete;
    bool flcreate = flags & VFS_FindCreate;
    bool flcache = flags & VFS_FindCache;

    // do as much work as possible with only the read locks held
    rwlockAcquireRead(&vfs->vfsdlock);
    rwlockAcquireRead(&vfs->vfslock);

    // see if we can get this from the file cache
    _vfsAbsPath(vfs, &abspath, path);
    if (flcache && !flcreate && !fldelete) {
        VFSCacheEnt *ent = _vfsGetFile(vfs, abspath, false);
        // only for simple case, i.e. no need to do COW or find a writable layer
        if (ent && (!flwrite || !(ent->mount->flags & VFS_ReadOnly))) {
            strDup(rpath, ent->origpath);
            strDestroy(&abspath);
            ret = ent->mount;
            objAcquire(ret);
            rwlockReleaseRead(&vfs->vfslock);
            rwlockReleaseRead(&vfs->vfsdlock);
            return ret;
        }
    }

    VFSDir *vfsdir = _vfsGetDir(vfs, abspath, true, true, false), *pdir = vfsdir;
    VFSMount *firstwritable = 0;
    string firstwpath = 0;
    string ns = 0;
    sa_string components;
    sa_string relcomp = saInitNone;

    saInit(&components, string, 8, SA_Grow(Aggressive));
    pathDecompose(&ns, &components, abspath);

    int32 relstart = saSize(components) - 1;
    while (pdir) {
        devAssert(relstart >= 0);
        saDestroy(&relcomp);
        saSlice(&relcomp, components, relstart, 0);
        strJoin(&curpath, relcomp, fsPathSepStr);

        // traverse list of registered providers backwards, as providers registered later
        // are "higher" on the stack
        for (int i = saSize(pdir->mounts) - 1; i >= 0; --i) {
            // save first writable provider we find
            if (!firstwritable && !(pdir->mounts.a[i]->flags & VFS_ReadOnly)) {
                firstwritable = pdir->mounts.a[i];
                strDup(&firstwpath, curpath);
            }

            if (cowmount && (pdir->mounts.a[i]->flags & VFS_AlwaysCOW)) {
                // this provider wants to get COW copies for any write
                *cowmount = objAcquire(pdir->mounts.a[i]);
                strDup(cowrpath, curpath);
                cowmount = NULL;        // don't let anything else set it
            }

            VFSProvider *provif = objInstIf(pdir->mounts.a[i]->provider, VFSProvider);
            if (!provif)
                continue;

            int stat;
            if (!(vfs->flags & VFS_CaseSensitive) && (pdir->mounts.a[i]->flags & VFS_CaseSensitive)) {
                // case-sensitive file system on insensitive VFS, find the real underlying path
                stat = _vfsFindCIHelper(vfs, vfsdir, &curpath, relcomp, pdir->mounts.a[i], provif);
            } else {
                // check if this exists
                // drop the lock here to avoid deadlocking on a loopback VFSVFS
                rwlockReleaseRead(&vfs->vfslock);
                stat = provif->stat(pdir->mounts.a[i]->provider, curpath, NULL);
                rwlockAcquireRead(&vfs->vfslock);
            }

            if (stat == FS_Directory) {
                // found a directory, don't cache this as a file
                ret = pdir->mounts.a[i];
                strDup(rpath, curpath);
                flcache = false;
                goto done;
            } else if (stat == FS_File) {
                // found an existing file
                ret = pdir->mounts.a[i];
                strDup(rpath, curpath);
                goto done;
            }

            // do we capture new files on this layer?
            if (flcreate && (pdir->mounts.a[i]->flags & VFS_NewFiles)) {
                ret = pdir->mounts.a[i];
                strDup(rpath, curpath);
                // do not exit, keep searching to see if it exists in a lower layer
            }

            // if this layer is opaque, the buck stops here
            if (pdir->mounts.a[i]->flags & VFS_Opaque)
                goto done;
        }

        relstart--;
        pdir = pdir->parent;
    }

done:
    rwlockReleaseRead(&vfs->vfslock);
    if (ret && flwrite && (ret->flags & VFS_ReadOnly) && cowmount) {
        // let the caller know they should COW to a writable provider
        *cowmount = objAcquire(firstwritable);
        strDup(cowrpath, firstwpath);
    }

    // didn't find a provider? if we're writing, go ahead and create a new file
    if (!ret && (flwrite || flcreate) && !fldelete) {
        ret = firstwritable;
        strDup(rpath, firstwpath);
    }

    if (!ret)
        cxerr = CX_FileNotFound;

    if (ret && flcache && !(ret->flags & VFS_NoCache)) {
        rwlockAcquireWrite(&vfs->vfslock);
        VFSCacheEnt *newent = _vfsCacheEntCreate(ret, *rpath);
        htInsertC(&vfsdir->files, string, components.a[saSize(components) - 1], ptr, &newent, HT_Ignore);
        rwlockReleaseWrite(&vfs->vfslock);
    }

    objAcquire(ret);

    rwlockReleaseRead(&vfs->vfsdlock);

    strDestroy(&ns);
    strDestroy(&abspath);
    strDestroy(&curpath);
    strDestroy(&firstwpath);
    saDestroy(&relcomp);
    saDestroy(&components);
    return ret;
}

_Use_decl_annotations_
void vfsCurDir(VFS *vfs, string *out)
{
    rwlockAcquireRead(&vfs->vfslock);
    strDup(out, vfs->curdir);
    rwlockReleaseRead(&vfs->vfslock);
}

_Use_decl_annotations_
bool vfsSetCurDir(VFS *vfs, strref cur)
{
    if (!pathIsAbsolute(cur))
        return false;

    rwlockAcquireWrite(&vfs->vfslock);
    strDup(&vfs->curdir, cur);
    rwlockReleaseWrite(&vfs->vfslock);
    return true;
}
