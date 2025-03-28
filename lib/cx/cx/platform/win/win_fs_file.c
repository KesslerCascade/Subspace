#include "win_fs.h"
#include "cx/utils/compare.h"
#include "cx/platform/win.h"

// biggest I/O request the OS will let us do
// using a safe value for XP
#define MAX_TRANSFER_SIZE (16*1024*1024)

typedef struct FSFile {
    HANDLE h;
} FSFile;

_Use_decl_annotations_
FSFile *fsOpen(strref path, flags_t flags)
{
    FSFile *ret;
    DWORD access = 0;
    DWORD share = 0;
    DWORD disp = 0;

    if (flags & FS_Read)
        access |= GENERIC_READ;
    if (flags & FS_Write)
        access |= GENERIC_WRITE;

    if ((flags & FS_Create) && (flags & FS_Truncate))
        disp = CREATE_ALWAYS;
    else if (flags & FS_Create)
        disp = OPEN_ALWAYS;
    else if (flags & FS_Truncate)
        disp = TRUNCATE_EXISTING;
    else
        disp = OPEN_EXISTING;

    if (flags & FS_Lock)
        share = FILE_SHARE_READ;
    else
        share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

    HANDLE handle = CreateFileW(fsPathToNT(path), access, share, NULL, disp, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        winMapLastError();
        return NULL;
    }

    ret = xaAlloc(sizeof(FSFile));
    ret->h = handle;
    return ret;
}

_Use_decl_annotations_
bool fsClose(FSFile *file)
{
    int ret = true;
    if (!CloseHandle(file->h))
        ret = winMapLastError();
    xaFree(file);
    return ret;
}

_Use_decl_annotations_
bool fsRead(FSFile *file, void *buf, size_t sz, size_t *bytesread)
{
    DWORD didread = 0;

    if (sz < MAX_TRANSFER_SIZE) {
        // fast path, can do it in a single call
        if (!ReadFile(file->h, buf, (DWORD)sz, &didread, NULL)) {
            *bytesread = 0;
            return winMapLastError();
        }

        *bytesread = didread;
        return true;
    }

    // have to break it up into smaller chunks
    size_t actuallyread = 0;
    uint8 *bufp = (uint8*)buf;
    while (sz > 0) {
        if (!ReadFile(file->h, bufp, (DWORD)clamphigh(sz, MAX_TRANSFER_SIZE), &didread, NULL)) {
            *bytesread = 0;
            return winMapLastError();
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

_Use_decl_annotations_
bool fsWrite(FSFile *file, void *buf, size_t sz, size_t *byteswritten)
{
    DWORD didwrite = 0;

    if (sz < MAX_TRANSFER_SIZE) {
        // fast path, can do it in a single call
        if (!WriteFile(file->h, buf, (DWORD)sz, &didwrite, NULL)) {
            if (byteswritten)
                *byteswritten = 0;
            return winMapLastError();
        }

        if (byteswritten)
            *byteswritten = didwrite;
        return true;
    }

    // have to break it up into smaller chunks
    size_t actuallywrote = 0;
    uint8 *bufp = (uint8*)buf;
    while (sz > 0) {
        if (!WriteFile(file->h, bufp, (DWORD)clamphigh(sz, MAX_TRANSFER_SIZE), &didwrite, NULL)) {
            if (byteswritten)
                *byteswritten = 0;
            return winMapLastError();
        }

        bufp += didwrite;
        actuallywrote += didwrite;
        sz -= didwrite;
    }

    if (byteswritten)
        *byteswritten = actuallywrote;
    return true;
}

_Use_decl_annotations_
int64 fsTell(FSFile *file)
{
    LARGE_INTEGER zero = { 0 };
    LARGE_INTEGER out;

    if (!SetFilePointerEx(file->h, zero, &out, FILE_CURRENT)) {
        winMapLastError();
        return -1;
    }

    return out.QuadPart;
}

_Use_decl_annotations_
int64 fsSeek(FSFile *file, int64 off, FSSeekType seektype)
{
    LARGE_INTEGER move;
    LARGE_INTEGER out;
    DWORD method;

    move.QuadPart = off;
    switch (seektype) {
    case FS_Set:
        method = FILE_BEGIN;
        break;
    case FS_Cur:
        method = FILE_CURRENT;
        break;
    case FS_End:
        method = FILE_END;
        break;
    default:
        return -1;
    }

    if (!SetFilePointerEx(file->h, move, &out, method)) {
        winMapLastError();
        return -1;
    }

    return out.QuadPart;
}

_Use_decl_annotations_
bool fsFlush(FSFile *file)
{
    if (!FlushFileBuffers(file->h))
        return winMapLastError();

    return true;
}
