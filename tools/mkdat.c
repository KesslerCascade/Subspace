#include <cx/container.h>
#include <cx/fs.h>
#include <cx/string.h>
#include <cx/sys.h>
#include <stdio.h>
#include "pkgfile/pkgfile.h"
#include "pkgfile/pkgstruct.h"

DEFINE_ENTRY_POINT

#define BSWAPU16(x) ((((x) & 0xff) << 8) | ((x) >> 8));
#define BSWAPU32(x) \
    ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | ((x) >> 24))

static void recurseFS(VFS* vfs, sa_PkgDirEnt* out, strref path, strref destpath)
{
    FSSearchIter iter;
    vfsSearchInit(&iter, vfs, path, NULL, 0, true);
    string subpath = 0;
    string subdest = 0;

    while (vfsSearchValid(&iter)) {
        pathJoin(&subdest, destpath, iter.name);

        if (iter.type == FS_File) {
            PkgDirEnt* nent = pkgdirentCreate(subdest, 0, iter.stat.size, iter.stat.size);
            saPushC(out, object, &nent);
        } else if (iter.type == FS_Directory) {
            pathJoin(&subpath, path, iter.name);

            recurseFS(vfs, out, subpath, subdest);
        }
        vfsSearchNext(&iter);
    }

    vfsSearchFinish(&iter);
    strDestroy(&subpath);
    strDestroy(&subdest);
}

static const uint8 pkg_magic[4] = { 'P', 'K', 'G', '\012' };
#define BLOCKSZ 65536

// we don't actually use this has function in pkgfs and instead use the cx hashtable, but for game
// compatibility go ahead and write it out
static inline uint32 pkgHash(strref path)
{
    uint32_t hash = 0;
    foreach (string, si, path) {
        for (uint32 i = 0; i < si.len; i++) {
            uint32_t c = si.bytes[i];
            if (c >= 'A' && c <= 'Z') {
                c += 0x20;
            }
            hash = hash << 27 | hash >> 5;
            hash ^= c;
        }
    }
    return hash;
}

static void makePkg(VFS* vfs, strref filename, strref path)
{
    PkgHeaderDisk hdr = { 0 };
    uint8* buf        = xaAlloc(BLOCKSZ);
    uint32 pos        = 0;
    size_t didwrite;

    VFSFile* out = vfsOpen(vfs, filename, FS_Overwrite);
    if (!out)
        return;

    sa_PkgDirEnt ents;
    sa_uint32 nameoff;
    saInit(&ents, object, 32);
    recurseFS(vfs, &ents, cmdArgs.a[1], _S"");

    uint32 nent   = saSize(ents);
    uint32 namesz = 0;
    uint16 hdrsz  = sizeof(hdr);
    uint16 entsz  = sizeof(PkgDirEntDisk);

    memcpy(hdr.magic, pkg_magic, 4);
    hdr.hdrsz = BSWAPU16(hdrsz);
    hdr.entsz = BSWAPU16(entsz);
    hdr.nent  = BSWAPU32(nent);

    // calculate nametable size
    saInit(&nameoff, uint32, nent);
    saSetSize(&nameoff, nent);
    pos = 0;
    for (int i = 0; i < nent; i++) {
        nameoff.a[i] = namesz;
        namesz += strLen(ents.a[i]->name) + 1;
    }
    hdr.namesz = BSWAPU32(namesz);

    // write out the file contents       
    vfsWrite(out, &hdr, sizeof(hdr), &didwrite);

    // skip ahead to name table
    vfsSeek(out, hdrsz + entsz * nent, FS_Set);
    for (int i = 0; i < saSize(ents); i++) {
        const char* buf = strC(ents.a[i]->name);
        vfsWrite(out, (void*)buf, strLen(ents.a[i]->name) + 1, &didwrite);
    }

    string srcname = 0;
    pos            = hdrsz + namesz + entsz * nent;
    vfsSeek(out, pos, FS_Set);

    for (int i = 0; i < saSize(ents); i++) {
        PkgDirEnt* ent = ents.a[i];
        ent->off       = pos;

        pathJoin(&srcname, path, ent->name);
        VFSFile* in = vfsOpen(vfs, srcname, FS_Read);
        if (in) {
            size_t didread = 0;
            uint32 left    = ent->size;

            while (left > 0) {
                if (!vfsRead(in, buf, min(left, BLOCKSZ), &didread) || didread == 0)
                    break;

                vfsWrite(out, buf, didread, &didwrite);

                left -= didread;
                pos += (uint32)didread;
            }

            vfsClose(in);
        }
    }

    // go back and write directory
    vfsSeek(out, hdrsz, FS_Set);
    for (int i = 0; i < saSize(ents); i++) {
        PkgDirEntDisk dd = { 0 };
        PkgDirEnt* ent   = ents.a[i];
        dd.nameidx_flags = BSWAPU32(nameoff.a[i]);
        dd.off           = BSWAPU32(ent->off);
        dd.sz            = BSWAPU32(ent->size);
        dd.csz           = BSWAPU32(ent->csize);
        uint32 hash      = pkgHash(ent->name);
        dd.hash          = BSWAPU32(hash);
        vfsWrite(out, &dd, sizeof(dd), &didwrite);
    }

    vfsClose(out);
    strDestroy(&srcname);
    saDestroy(&nameoff);
    saDestroy(&ents);
    xaFree(buf);
}

int entryPoint()
{
    // very very simple program to geneate a dat (pkg) file from a subdirectory, while stripping out
    // the first path entry

    VFS* vfs = vfsCreateFromFS();
    if (saSize(cmdArgs) != 2 || vfsStat(vfs, cmdArgs.a[1], NULL) != FS_Directory) {
        puts("Usage:");
        puts("    mkdat file.dat dirname");
    }

    makePkg(vfs, cmdArgs.a[0], cmdArgs.a[1]);

    return 0;
}
