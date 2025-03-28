// this is to prevent Linux in particular from being brain-damaged #smh
#define _FILE_OFFSET_BITS 64

#include "cx/fs/fs_private.h"
#include "cx/utils/compare.h"
#include "cx/platform/unix.h"
#include "cx/debug/error.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>

// biggest I/O request the OS will let us do
// BSD doesn't need this but Linux does
#define MAX_TRANSFER_SIZE (1024*1024*1024)

typedef struct FSFile {
    int fd;
    bool locked;
} FSFile;

FSFile *fsOpen(strref path, flags_t flags)
{
    FSFile *ret;
    int oflags = 0;
    string npath = 0;

    strDup(&npath, path);
    pathNormalize(&npath);
    pathToPlatform(&npath, npath);

    if ((flags & FS_Read) && (flags & FS_Write))
        oflags = O_RDWR;
    else if (flags & FS_Read)
        oflags = O_RDONLY;
    else if (flags & FS_Write)
        oflags = O_WRONLY;

    if (flags & FS_Create)
        oflags |= O_CREAT;
    if (flags & FS_Truncate)
        oflags |= O_TRUNC;

    int fd = open(strC(npath), oflags, 0644);
    if (fd < 0) {
        unixMapErrno();
	strDestroy(&npath);
        return NULL;
    }

    int lockop = 0;
    if (flags & FS_Lock)
        lockop = LOCK_EX;       // for locking the file we need exclusive access
    else if (flags & FS_Write)
        lockop = LOCK_SH;       // for writing, need a shared lock

    // do we need a lock
    if (lockop != 0) {
        if (flock(fd, lockop | LOCK_NB) == EWOULDBLOCK) {
            // could not get the lock
            cxerr = CX_AccessDenied;
            close(fd);
        }
        // other types of failures like locking not supported on the
        // filesystem are silently ignored; not a great way to handle it
        // but the alternative is to just not work at all in those cases
    }

    ret = xaAlloc(sizeof(FSFile));
    ret->fd = fd;
    ret->locked = (lockop != 0);
    strDestroy(&npath);
    return ret;
}

bool fsClose(FSFile *file)
{
    int ret = true;
    if (file->locked)
        flock(file->fd, LOCK_UN);
    if (!close(file->fd))
        ret = unixMapErrno();
    xaFree(file);
    return ret;
}

bool fsRead(FSFile *file, void *buf, size_t sz, size_t *bytesread)
{
    ssize_t didread = 0;

    if (sz < MAX_TRANSFER_SIZE) {
        // fast path, can do it in a single call
        didread = read(file->fd, buf, sz);
        if (didread < 0) {
            *bytesread = 0;
            return unixMapErrno();
        }

        *bytesread = (size_t)didread;
        return true;
    }

    // have to break it up into smaller chunks
    size_t actuallyread = 0;
    uint8 *bufp = (uint8*)buf;
    while (sz > 0) {
        didread = read(file->fd, bufp, clamphigh(sz, MAX_TRANSFER_SIZE));
        if (didread < 0) {
            *bytesread = 0;
            return unixMapErrno();
        }
        if (didread == 0)       // EOF
            break;

        bufp += didread;
        actuallyread += didread;
        sz -= didread;
    }

    *bytesread = actuallyread;
    return true;
}

bool fsWrite(FSFile *file, void *buf, size_t sz, size_t *byteswritten)
{
    ssize_t didwrite = 0;

    if (sz < MAX_TRANSFER_SIZE) {
        // fast path, can do it in a single call
        didwrite = write(file->fd, buf, sz);
        if (didwrite < 0) {
            if (byteswritten)
                *byteswritten = 0;
            return unixMapErrno();
        }

        if (byteswritten)
            *byteswritten = didwrite;
        return true;
    }

    // have to break it up into smaller chunks
    size_t actuallywrote = 0;
    uint8 *bufp = (uint8*)buf;
    while (sz > 0) {
        didwrite = write(file->fd, bufp, clamphigh(sz, MAX_TRANSFER_SIZE));
        if (didwrite < 0) {
            if (byteswritten)
                *byteswritten = 0;
            return unixMapErrno();
        }

        bufp += didwrite;
        actuallywrote += didwrite;
        sz -= didwrite;
    }

    if (byteswritten)
        *byteswritten = actuallywrote;
    return true;
}

int64 fsTell(FSFile *file)
{
    off_t off;
    off = lseek(file->fd, 0, SEEK_CUR);
    if (off < 0) {
        unixMapErrno();
        return -1;
    }

    return off;
}

int64 fsSeek(FSFile *file, int64 off, FSSeekType seektype)
{
    int method;
    off_t out;

    switch (seektype) {
    case FS_Set:
        method = SEEK_SET;
        break;
    case FS_Cur:
        method = SEEK_CUR;
        break;
    case FS_End:
        method = SEEK_END;
        break;
    default:
        return -1;
    }

    out = lseek(file->fd, (off_t)off, method);
    if (out < 0) {
        unixMapErrno();
        return -1;
    }

    return out;
}

bool fsFlush(FSFile *file)
{
    if (fsync(file->fd) == -1)
        return unixMapErrno();

    return true;
}
