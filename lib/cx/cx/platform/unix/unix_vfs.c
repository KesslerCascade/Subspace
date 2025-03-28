#include "cx/fs/fs_private.h"
#include "cx/fs/vfs.h"

bool _vfsAddPlatformSpecificMounts(VFS *vfs)
{
    bool ret = vfsMountFS(vfs, _S"/", _S"/", VFS_CaseSensitive);

    string curdir = 0;
    fsCurDir(&curdir);
    vfsSetCurDir(vfs, curdir);
    strDestroy(&curdir);
    return ret;
}

bool _vfsIsPlatformCaseSensitive()
{
    return true;
}
