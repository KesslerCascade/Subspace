// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "pkgfile.h"
// clang-format on
// ==================== Auto-generated section ends ======================

static const uint8 pkg_magic[4] = { 'P', 'K', 'G', '\012' };

#define BSWAPU16(x) ((((x) & 0xff) << 8) | ((x) >> 8));
#define BSWAPU32(x) \
    ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | ((x) >> 24))

_objinit_guaranteed bool PkgFile_init(_In_ PkgFile* self)
{
    // Autogen begins -----
    mutexInit(&self->filelock);
    htInit(&self->files, string, object, 16, HT_CaseInsensitive);
    htInit(&self->dircache, string, sarray, 16, HT_CaseInsensitive);
    return true;
    // Autogen ends -------
}

_objfactory_check PkgFile* PkgFile_open(VFSFile* f)
{
    PkgFile* self;
    self = objInstCreate(PkgFile);

    self->pkgfile = f;

    objInstInit(self);

    if (!pkgfileLoad(self)) {
        objRelease(&self);
    }

    return self;
}

static bool pkgAddDCache(hashtable* dircache, string parentname, string name)
{
    bool found       = false;
    sa_ref none      = { 0 };
    htelem dcelem    = htInsert(dircache, string, parentname, sarray, none, HT_Ignore);
    sa_string* dcent = (sa_string*)hteValPtr(*dircache, sarray, dcelem);
    if (saValid(*dcent))
        found = true;
    else
        saInit(dcent, string, 16, SA_Sorted);
    saPush(dcent, string, name, SA_Unique);
    return found;
}

bool PkgFile_load(_In_ PkgFile* self)
{
    bool ret              = false;
    char* namebuf         = NULL;
    string dirname        = 0;
    string parentdir      = 0;
    string filename       = 0;
    sa_PkgDirEntDisk ents = saInitNone;
    vfsSeek(self->pkgfile, 0, FS_Set);

    PkgHeaderDisk hdr;
    size_t didread;
    if (!vfsRead(self->pkgfile, &hdr, sizeof(hdr), &didread) || didread != sizeof(hdr))
        goto out;

    hdr.hdrsz  = BSWAPU16(hdr.hdrsz);
    hdr.entsz  = BSWAPU16(hdr.entsz);
    hdr.nent   = BSWAPU32(hdr.nent);
    hdr.namesz = BSWAPU32(hdr.namesz);

    // sanity checks
    if (memcmp(hdr.magic, pkg_magic, sizeof(pkg_magic)) != 0 || hdr.hdrsz != sizeof(hdr) ||
        hdr.entsz != sizeof(PkgDirEntDisk))
        goto out;

    // read in name buffer
    namebuf = xaAlloc(hdr.namesz);
    vfsSeek(self->pkgfile, hdr.hdrsz + hdr.nent * hdr.entsz, FS_Set);
    if (!vfsRead(self->pkgfile, namebuf, hdr.namesz, &didread) || didread != hdr.namesz)
        goto out;

    saInit(&ents, opaque(PkgDirEntDisk), hdr.nent);
    saSetSize(&ents, hdr.nent);

    vfsSeek(self->pkgfile, hdr.hdrsz, FS_Set);
    if (!vfsRead(self->pkgfile, ents.a, hdr.nent * hdr.entsz, &didread) ||
        didread != hdr.nent * hdr.entsz)
        goto out;

    for (uint32 i = 0; i < hdr.nent; i++) {
        PkgDirEntDisk* de    = &ents.a[i];
        uint32 nameidx_flags = BSWAPU32(de->nameidx_flags);
        PkgDirEnt* nent      = pkgdirentCreate((strref)(&namebuf[nameidx_flags & 0xFFFFFF]),
                                          BSWAPU32(de->off),
                                          BSWAPU32(de->csz),
                                          BSWAPU32(de->sz));
        if (nameidx_flags & PKG_Compressed)
            nent->compressed = true;

        // precreate cache entries to prevent repeated scans of the directory
        if (!pathParent(&dirname, nent->name))
            strClear(&dirname);
        pathFilename(&filename, nent->name);
        bool hadparent = pkgAddDCache(&self->dircache, dirname, filename);

        // add directories to parents
        if (!hadparent && !strEmpty(dirname)) {
            while (pathParent(&parentdir, dirname)) {
                pathFilename(&filename, dirname);
                pkgAddDCache(&self->dircache, parentdir, filename);
                strDup(&dirname, parentdir);
            }
            pkgAddDCache(&self->dircache, NULL, dirname);   // add to root
        }

        htInsertC(&self->files, string, nent->name, object, &nent);
    }
    ret = true;

out:
    xaFree(namebuf);
    saDestroy(&ents);
    strDestroy(&parentdir);
    strDestroy(&dirname);
    strDestroy(&filename);
    return ret;
}

void PkgFile_close(_In_ PkgFile* self)
{
    withMutex (&self->filelock) {
        if (self->pkgfile)
            vfsClose(self->pkgfile);
        self->pkgfile = NULL;
    }
}

void PkgFile_destroy(_In_ PkgFile* self)
{
    pkgfileClose(self);
    // Autogen begins -----
    mutexDestroy(&self->filelock);
    htDestroy(&self->files);
    htDestroy(&self->dircache);
    // Autogen ends -------
}

_objfactory_guaranteed PkgDirEnt*
PkgDirEnt_create(_In_opt_ strref name, uint32 off, uint32 csize, uint32 size)
{
    PkgDirEnt* self;
    self = objInstCreate(PkgDirEnt);

    strDup(&self->name, name);
    self->off   = off;
    self->csize = csize;
    self->size  = size;

    objInstInit(self);
    return self;
}

void PkgDirEnt_destroy(_In_ PkgDirEnt* self)
{
    // Autogen begins -----
    strDestroy(&self->name);
    // Autogen ends -------
}

// Autogen begins -----
#include "pkgfile.auto.inc"
// Autogen ends -------
