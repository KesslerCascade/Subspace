#include <cx/fs.h>
#include <cx/string.h>
#include <pkgfs/pkgfs.h>
#include "subspace.h"

bool subspaceCheckBaseDir(VFS* vfs, strref path)
{
    bool ret     = false;
    string tpath = 0;

    strConcat(&tpath, path, SETTINGS_FILENAME);
    if (vfsIsFile(vfs, tpath))
        ret = true;

    // Try some fallback options if that doesn't exist
    if (!ret) {
        strConcat(&tpath, path, DAT_FILENAME);
        if (vfsIsFile(vfs, tpath))
            ret = true;
    }

    if (!ret) {
        strConcat(&tpath, path, DB_FILENAME);
        if (vfsIsFile(vfs, tpath))
            ret = true;
    }

    if (!ret) {
        strConcat(&tpath, path, GAMEEXE_FILENAME);
        if (vfsIsFile(vfs, tpath))
            ret = true;
    }

    if (!ret) {
        strConcat(&tpath, path, DATADIR_FILENAME);
        if (vfsIsDir(vfs, tpath))
            ret = true;
    }

    if (!ret) {
        strConcat(&tpath, path, DEVMODE_FILENAME);
        if (vfsIsFile(vfs, tpath))
            ret = true;
    }

    strDestroy(&tpath);
    return ret;
}

void subspaceSetBaseDir(Subspace* ss, VFS* vfs, strref path)
{
    strDup(&ss->basedir, path);
    pathNormalize(&ss->basedir);

    string temp = 0;

    // check if the dev mode file exists
    strConcat(&temp, ss->basedir, DEVMODE_FILENAME);
    if (vfsIsFile(vfs, temp))
        ss->devmode = true;

    // find the appropriate subspacegame executible
    strConcat(&temp, ss->basedir, GAMEEXE_FILENAME);
    if (vfsIsFile(vfs, temp))
        strDup(&ss->gamepath, temp);

    if (ss->devmode) {
        strConcat(&temp, ss->basedir, GAMEEXE_DEV_FILENAME);
        if (vfsIsFile(vfs, temp))
            strDup(&ss->gamepath, temp);
    }

    strDestroy(&temp);
}

bool subspaceFindBaseDir(Subspace* ss, VFS* vfs)
{
    bool ret = false;

    // start with the exe path
    string exedir = 0, tpath = 0;
    fsExeDir(&exedir);
    strDup(&tpath, exedir);

    // search all the way up to the root
    do {
        if (subspaceCheckBaseDir(vfs, tpath)) {
            subspaceSetBaseDir(ss, vfs, tpath);
            ret = true;
            break;
        }
    } while (pathParent(&tpath, tpath));

    strDestroy(&exedir);
    strDestroy(&tpath);
    return ret;
}

bool subspaceMount(Subspace* ss)
{
    if (vfsIsFile(ss->fs, DAT_FILENAME)) {
        VFSFile* pkgfile = vfsOpen(ss->fs, DAT_FILENAME, FS_Read);
        if (pkgfile) {
            PkgFile* pkg = pkgfileOpen(pkgfile);
            if (pkg) {
                PkgFS* pkgfs = pkgfsCreate(pkg);
                vfsMountProvider(ss->fs, pkgfs, SSNS);
                objRelease(&pkg);
            }
        }
    }

    if (vfsIsDir(ss->fs, DATADIR_FILENAME)) {
        if (!vfsMountVFS(ss->fs, SSNS, ss->fs, DATADIR_FILENAME, VFS_ReadOnly)) {
            logFmt(Error,
                   _S"Failed to open data directory ${string}",
                   stvar(strref, DATADIR_FILENAME));
        }
    }
    return true;
}

void subspaceUnmount(Subspace* ss)
{
    vfsUnmount(ss->fs, SSNS);
}
