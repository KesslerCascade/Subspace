#include "vfs_private.h"
#include <cx/debug/error.h>

static intptr dirEntCmp(stype st, stgeneric g1, stgeneric g2, uint32 flags)
{
    VFSDirEnt *ent1 = (VFSDirEnt*)g1.st_opaque;
    VFSDirEnt *ent2 = (VFSDirEnt*)g2.st_opaque;

    return strCmpi(ent1->name, ent2->name);
}

static intptr dirEntCmpCaseSensitive(stype st, stgeneric g1, stgeneric g2, uint32 flags)
{
    VFSDirEnt *ent1 = (VFSDirEnt*)g1.st_opaque;
    VFSDirEnt *ent2 = (VFSDirEnt*)g2.st_opaque;

    return strCmp(ent1->name, ent2->name);
}

static void dirEntCopy(stype st, stgeneric *gdest, stgeneric gsrc, uint32 flags)
{
    VFSDirEnt *ent = (VFSDirEnt*)gdest->st_opaque;
    VFSDirEnt *src = (VFSDirEnt*)gsrc.st_opaque;

    ent->name = 0;
    strDup(&ent->name, src->name);
    ent->type = src->type;
    ent->stat = src->stat;
}

static void dirEntDestroy(stype st, stgeneric *g, uint32 flags)
{
    VFSDirEnt *ent = (VFSDirEnt*)g->st_opaque;
    strDestroy(&ent->name);
}

static STypeOps VFSDirEnt_ops = {
    .cmp = dirEntCmp,
    .copy = dirEntCopy,
    .dtor = dirEntDestroy,
};

static STypeOps VFSDirEnt_ops_cs = {
    .cmp = dirEntCmpCaseSensitive,
    .copy = dirEntCopy,
    .dtor = dirEntDestroy,
};

_Use_decl_annotations_
bool vfsSearchInit(FSSearchIter *iter, VFS *vfs, strref path, strref pattern, int typefilter, bool stat)
{
    string abspath = 0, curpath = 0, filepath = 0;
    hashtable names;
    int32 idx;
    bool exists = false;

    cxerr = CX_Success;
    memset(iter, 0, sizeof(FSSearchIter));

    if ((vfs->flags & VFS_CaseSensitive))
        htInit(&names, string, intptr, 8, HT_RefKeys | HT_Grow(MaxSpeed));
    else
        htInit(&names, string, intptr, 8, HT_CaseInsensitive | HT_RefKeys | HT_Grow(MaxSpeed));

    // just hold the write lock for this since we'll be adding entries to the cache throughout
    rwlockAcquireRead(&vfs->vfsdlock);
    rwlockAcquireWrite(&vfs->vfslock);

    _vfsAbsPath(vfs, &abspath, path);

    VFSDir *vfsdir = _vfsGetDir(vfs, abspath, false, false, true), *pdir = vfsdir;
    string ns = 0;
    sa_string components;
    sa_string relcomp = saInitNone;

    if (!vfsdir) {
        cxerr = CX_InvalidArgument;
        rwlockReleaseWrite(&vfs->vfslock);
        rwlockReleaseRead(&vfs->vfsdlock);
        return false;
    }

    saInit(&components, string, 8, SA_Grow(Aggressive));
    pathDecompose(&ns, &components, abspath);

    VFSSearch *search = xaAlloc(sizeof(VFSSearch), XA_Zero);
    iter->_search = search;
    search->vfs = objAcquire(vfs);
    search->idx = 0;
    STypeOps direntops = (vfs->flags & VFS_CaseSensitive) ? VFSDirEnt_ops_cs : VFSDirEnt_ops;
    saInit(&search->ents, custom(opaque(VFSDirEnt), direntops), 16, SA_Grow(Aggressive));

    // add child mount points as subdirectories
    foreach(hashtable, sdi, vfsdir->subdirs) {
        VFSDir *sd = (VFSDir*)htiVal(ptr, sdi);
        if (saSize(sd->mounts) > 0) {
            VFSDirEnt ent = { 0 };
            strDup(&ent.name, sd->name);
            ent.type = FS_Directory;
            saPushC(&search->ents, opaque, &ent);
            htInsert(&names, string, sd->name, intptr, 1);
        }
    }

    // start at the target directory and recurse upwards to see if any providers know about
    // this directory
    int32 relstart = saSize(components);
    while (pdir) {
        devAssert(relstart >= 0);
        saDestroy(&relcomp);
        saSlice(&relcomp, components, relstart, 0);
        strJoin(&curpath, relcomp, fsPathSepStr);

        // traverse list of registered providers backwards, as providers registered later
        // are "higher" on the stack
        for (int i = saSize(pdir->mounts) - 1; i >= 0; --i) {
            ObjInst *provider = pdir->mounts.a[i]->provider;
            VFSProvider *provif = objInstIf(provider, VFSProvider);
            if (!provif)
                continue;

            if (!(vfs->flags & VFS_CaseSensitive) && (pdir->mounts.a[i]->flags & VFS_CaseSensitive)) {
                // case-sensitive file system on insensitive VFS, find the real underlying path
                _vfsFindCIHelper(vfs, vfsdir, &curpath, relcomp, pdir->mounts.a[i], provif);
            }

            // see if we can get a directory listing out of it
            FSSearchIter dsiter;
            if (!provif->searchInit(provider, &dsiter, curpath, pattern, stat))
                continue;

            // we did! so gather up all the files
            exists = true;
            while (provif->searchValid(provider, &dsiter)) {
                // have we seen this file already on a higher layer?
                if ((!typefilter || (dsiter.type & typefilter) == typefilter) &&
                    !htHasKey(names, string, dsiter.name)) {
                    // add to list and hash table of seen files
                    VFSDirEnt ent = {
                        .name = dsiter.name,        // borrowed ref!
                        .type = dsiter.type,
                        .stat = dsiter.stat
                    };
                    idx = saPush(&search->ents, opaque, ent);
                    htInsert(&names, string, search->ents.a[idx].name, intptr, 1);

                    if (dsiter.type == FS_File && !(pdir->mounts.a[i]->flags & VFS_NoCache)) {
                        // go ahead and add it to the cache while we're here
                        pathJoin(&filepath, curpath, dsiter.name);
                        VFSCacheEnt *newent = _vfsCacheEntCreate(pdir->mounts.a[i], filepath);
                        htInsertC(&vfsdir->files, string, dsiter.name, ptr, &newent, HT_Ignore);
                    }
                }
                provif->searchNext(provider, &dsiter);
            }
            provif->searchFinish(provider, &dsiter);

            // if this layer is opaque, the buck stops here
            if (pdir->mounts.a[i]->flags & VFS_Opaque)
                goto done;
        }

        relstart--;
        pdir = pdir->parent;
    }

done:
    rwlockReleaseWrite(&vfs->vfslock);
    rwlockReleaseRead(&vfs->vfsdlock);
    saSort(&search->ents, true);

    strDestroy(&ns);
    strDestroy(&abspath);
    strDestroy(&curpath);
    strDestroy(&filepath);
    saDestroy(&relcomp);
    saDestroy(&components);
    htDestroy(&names);

    // did the path exist somewhere in the VFS?
    if (exists) {
        vfsSearchNext(iter);
        return true;
    } else {
        vfsSearchFinish(iter);
        cxerr = CX_FileNotFound;
        return false;
    }
}

_Use_decl_annotations_
bool vfsSearchNext(FSSearchIter *iter)
{
    VFSSearch *search = (VFSSearch*)iter->_search;

    if (!search)
        return false;

    if (search->idx >= saSize(search->ents)) {
        vfsSearchFinish(iter);
        return false;
    }

    _Analysis_assume_(search->ents.a != NULL);      // because saSize returned > 0
    VFSDirEnt *ent = &search->ents.a[search->idx];
    strDup(&iter->name, ent->name);
    iter->type = ent->type;
    iter->stat = ent->stat;
    search->idx++;

    return true;
}

_Use_decl_annotations_
void vfsSearchFinish(FSSearchIter *iter)
{
    VFSSearch *search = (VFSSearch*)iter->_search;
    if (!search)
        return;

    strDestroy(&iter->name);

    saDestroy(&search->ents);
    objRelease(&search->vfs);
    xaRelease(&iter->_search);
}
