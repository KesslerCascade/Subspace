#include "cx/fs/fs_private.h"
#include "cx/fs/vfs.h"
#include "cx/platform/win.h"

bool _vfsAddPlatformSpecificMounts(VFS *vfs)
{
    DWORD ldrives = GetLogicalDrives();
    bool ret = true;
    string drive = 0;
    char drivestr[4];

    drivestr[1] = ':';
    drivestr[2] = '/';
    drivestr[3] = 0;
    for (char dletter = 'a'; dletter <= 'z'; dletter++) {
        if (ldrives & 1 << (dletter - 'a')) {
            drivestr[0] = dletter;
            strCopy(&drive, (string)drivestr);
            ret &= vfsMountFS(vfs, drive, drive);
        }
    }

    string curdir = 0;
    fsCurDir(&curdir);
    // mount current drive as root
    strSubStr(&drive, _fsCurDir, 0, 3);
    vfsMountFS(vfs, _S"/", drive);
    vfsSetCurDir(vfs, curdir);
    strDestroy(&curdir);
    strDestroy(&drive);

    return ret;
}

bool _vfsIsPlatformCaseSensitive()
{
    return false;
}
