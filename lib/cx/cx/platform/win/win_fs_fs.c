#include "win_fs.h"
#include "win_time.h"
#include "cx/string.h"
#include "cx/utils/lazyinit.h"
#include "cx/container/sarray.h"
#include "cx/thread/rwlock.h"
#include "cx/platform/win.h"

static LazyInitState fsCurDirInit;
RWLock _fsCurDirLock;
string _fsCurDir = 0;
string fsPlatformPathSepStr = _S"\\";

static void fsPathFromWinW(_Inout_ string *out, _In_z_ wchar_t *winpath)
{
    strFromUTF16(out, winpath, cstrLenw(winpath));
    pathFromPlatform(out, *out);
    pathNormalize(out);
}

static void initCurDir(void *data)
{
    rwlockInit(&_fsCurDirLock);

    DWORD sz = GetCurrentDirectoryW(0, NULL);
    wchar_t *p = xaAlloc(sz * sizeof(wchar_t));
    DWORD ret = GetCurrentDirectoryW(sz, p);
    if (ret != 0 && ret < sz)
        strFromUTF16(&_fsCurDir, p, ret);
    xaFree(p);

    pathFromPlatform(&_fsCurDir, _fsCurDir);
    pathNormalize(&_fsCurDir);
}

static bool fsIsUNC(_In_opt_ strref path)
{
    striter it;
    striBorrow(&it, path);
    char ch;

    if (striChar(&it, &ch) && ch == '\\' && striChar(&it, &ch) && ch == '\\')
        return true;

    return false;
}

_Use_decl_annotations_
wchar_t* fsPathToNT(strref path)
{
    string npath = 0, ntpath = 0;
    wchar_t *ret;
    pathMakeAbsolute(&npath, path);
    pathNormalize(&npath);
    pathToPlatform(&npath, npath);

    if (!fsIsUNC(npath)) {
        strConcat(&ntpath, _S"\\\\?\\", npath);
    } else {
        strSubStrI(&npath, 2, strEnd);
        strConcat(&ntpath, _S"\\\\?\\UNC\\", npath);
    }

    ret = strToUTF16S(ntpath);
    if(!ret) {
        ret = scratchGet(1);
        ret[0] = 0;
    }

    strDestroy(&npath);
    strDestroy(&ntpath);
    return ret;
}

_Use_decl_annotations_
void pathFromPlatform(string *out, strref platformpath)
{
    string ns = 0;
    string rpath = 0;
    string ret = 0;

    strDup(&rpath, platformpath);

    // first, convert all backslashes to forward slashes
    int32 idx = 0;
    while ((idx = strFind(rpath, idx, _S"\\")) != -1)
        strSetChar(&rpath, idx, '/');

    uint32 origlen = strLen(rpath);
    uint8 *buf = strBuffer(&rpath, 4);

    if (buf[0] == '/' && (buf[1] == '/' || buf[1] == '?') &&
        (buf[2] == '?' || buf[2] == '.') && buf[3] == '/') {
        // reserved prefix, can't use this!
        strSubStrI(&rpath, 4, origlen);
        buf = strBuffer(&rpath, 4);
    }

    if (buf[1] == ':' && buf[2] == '/') {
        // absolute path
        strSubStr(&ns, rpath, 0, 1);
        strSubStrI(&rpath, 2, origlen);
    } else if (buf[0] == '/' && buf[1] == '/') {
        // unc
        strDup(&ns, _S"unc");
        strSubStrI(&rpath, 1, origlen);
    } else if (buf[0] == '/') {
        // starts with a slash, but not a UNC path...
        // must be drive relative, so get the drive letter from curdir
        lazyInit(&fsCurDirInit, initCurDir, NULL);
        rwlockAcquireRead(&_fsCurDirLock);
        strSubStr(&ns, _fsCurDir, 0, 1);
        rwlockReleaseRead(&_fsCurDirLock);
        if (origlen < 4)
            strSetLen(&rpath, origlen);
    } else {
        if (origlen < 4)
            strSetLen(&rpath, origlen);
    }

    if (!strEmpty(ns)) {
        strLower(&ns);
        strNConcat(&ret, ns, fsNSSepStr, rpath);
    } else {
        strDup(&ret, rpath);
    }

    strDestroy(&ns);
    strDestroy(&rpath);
    strDestroy(out);
    *out = ret;
}

_Use_decl_annotations_
void pathToPlatform(string *out, strref path)
{
    string ns = 0, rpath = 0;
    string ret = 0;
    pathSplitNS(&ns, &rpath, path);
    strUpper(&ns);

    if (strEq(ns, _S"UNC")) {
        strNConcat(&ret, _S"/", rpath);
    } else if (!strEmpty(ns)) {
        strNConcat(&ret, ns, _S":", rpath);
    } else {
        strDup(&ret, rpath);
    }

    // finally, convert all forward slahes to backslashes
    int32 idx = 0;
    while ((idx = strFind(ret, idx, _S"/")) != -1)
        strSetChar(&ret, idx, '\\');

    strDestroy(&ns);
    strDestroy(&rpath);

    strDestroy(out);
    *out = ret;
}

_Use_decl_annotations_
bool pathMakeAbsolute(string *out, strref path)
{
    if (pathIsAbsolute(path)) {
        strDup(out, path);
        return true;
    }
    lazyInit(&fsCurDirInit, initCurDir, NULL);

    string tmp = 0;
    rwlockAcquireRead(&_fsCurDirLock);
    pathJoin(&tmp, _fsCurDir, path);
    rwlockReleaseRead(&_fsCurDirLock);
    strDestroy(out);
    *out = tmp;
    return true;
}

_Use_decl_annotations_
void fsCurDir(string *out)
{
    lazyInit(&fsCurDirInit, initCurDir, NULL);
    rwlockAcquireRead(&_fsCurDirLock);
    strDup(out, _fsCurDir);
    rwlockReleaseRead(&_fsCurDirLock);
}

_Use_decl_annotations_
bool fsSetCurDir(strref cur)
{
    if (strEmpty(cur))
        return false;

    string ncur = 0;
    strDup(&ncur, cur);

    // be paranoid about what we set our current directory to
    pathNormalize(&ncur);
    pathMakeAbsolute(&ncur, ncur);

    // We don't need to actually change the OS's current directory since our own path
    // normalization always results in absolute paths for everything, but it helps
    // keep things from being confusing if subprocesses are created.
    rwlockAcquireWrite(&_fsCurDirLock);
    if (!SetCurrentDirectoryW(fsPathToNT(ncur))) {
        winMapLastError();
        strDestroy(&ncur);
        rwlockReleaseWrite(&_fsCurDirLock);
        return false;
    }

    strDestroy(&_fsCurDir);
    _fsCurDir = ncur;
    rwlockReleaseWrite(&_fsCurDirLock);
    return true;
}

static void fsExeInit(void *data)
{
    string *exepath = (string*)data;

    // windows API is really stupid here, has no way to get size first
    wchar_t *buf = xaAlloc(32768 * sizeof(wchar_t));
    GetModuleFileNameW(NULL, buf, 32768);
    buf[32767] = 0;             // it also doesn't null terminate when it truncates...
    fsPathFromWinW(exepath, buf);
    xaFree(buf);
}

_Use_decl_annotations_
void fsExe(string *out)
{
    static LazyInitState execache;
    static string exepath = 0;
    lazyInit(&execache, fsExeInit, &exepath);

    strDup(out, exepath);
}

_Use_decl_annotations_
void fsExeDir(string *out)
{
    fsExe(out);
    pathParent(out, *out);
}

_Use_decl_annotations_
FSPathStat fsStat(strref path, FSStat *stat)
{
    if (strEmpty(path))
        return FS_Nonexistent;

    if (!stat) {
        // fast path for if they don't care about times
        DWORD attr = GetFileAttributesW(fsPathToNT(path));
        // handle edge case where drive exists but is not ready
        if (attr == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_NOT_READY)
            return FS_Directory;

        if (attr == INVALID_FILE_ATTRIBUTES)
            return FS_Nonexistent;
        if (attr & FILE_ATTRIBUTE_DIRECTORY)
            return FS_Directory;
        return FS_File;
    }

    WIN32_FILE_ATTRIBUTE_DATA attrs;

    if (!GetFileAttributesExW(fsPathToNT(path), GetFileExInfoStandard, &attrs)) {
        memset(stat, 0, sizeof(FSStat));
        if (GetLastError() == ERROR_NOT_READY)
            return FS_Directory;
        return FS_Nonexistent;
    }

    stat->size = (uint64)attrs.nFileSizeHigh << 32 | attrs.nFileSizeLow;
    stat->accessed = timeFromFileTime(&attrs.ftLastAccessTime);
    stat->created = timeFromFileTime(&attrs.ftCreationTime);
    stat->modified = timeFromFileTime(&attrs.ftLastWriteTime);
    if (attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return FS_Directory;
    return FS_File;
}

_Use_decl_annotations_
bool fsCreateDir(strref path)
{
    if (strEmpty(path))
        return false;

    if (!CreateDirectoryW(fsPathToNT(path), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
        return winMapLastError();

    return true;
}

_Use_decl_annotations_
bool fsCreateAll(strref path)
{
    string parent = 0;
    pathParent(&parent, path);
    if (!strEmpty(parent) && !fsExist(parent))
        fsCreateAll(parent);
    strDestroy(&parent);

    return fsCreateDir(path);
}

_Use_decl_annotations_
bool fsRemoveDir(strref path)
{
    if (strEmpty(path))
        return false;

    if (!RemoveDirectoryW(fsPathToNT(path)))
        return winMapLastError();

    return true;
}

_Use_decl_annotations_
bool fsDelete(strref path)
{
    if (strEmpty(path))
        return false;

    if (!DeleteFileW(fsPathToNT(path)))
        return winMapLastError();

    return true;
}

_Use_decl_annotations_
bool fsRename(strref from, strref to)
{
    if (strEmpty(from) || strEmpty(to))
        return false;

    if (!MoveFileW(fsPathToNT(from), fsPathToNT(to)))
        return winMapLastError();

    return true;
}

typedef struct FSSearch {
    HANDLE h;
    WIN32_FIND_DATAW first;
} FSSearch;

_Use_decl_annotations_
bool fsSearchInit(FSSearchIter *iter, strref path, strref pattern, bool stat)
{
    string spath = 0;

    memset(iter, 0, sizeof(FSSearchIter));

    // stat is ignored for Windows since the API always returns file
    // size and timestamps

    FSSearch *search = xaAlloc(sizeof(FSSearch), XA_Zero);
    iter->_search = search;
    pathJoin(&spath, path, strEmpty(pattern) ? _S"*" : pattern);

    search->h = FindFirstFileW(fsPathToNT(spath), &search->first);
    strDestroy(&spath);

    if (search->h == INVALID_HANDLE_VALUE) {
        winMapLastError();
        xaRelease(&iter->_search);
        return false;
    }

    fsSearchNext(iter);
    return true;
}

_Use_decl_annotations_
bool fsSearchNext(FSSearchIter *iter)
{
    FSSearch *search = (FSSearch*)iter->_search;
    WIN32_FIND_DATAW data;
    if (!search)
        return false;

    do {
        // If we still have the first file from FindFirstFileW, use it
        if (search->first.dwFileAttributes != 0xFFFFFFFF) {
            memcpy(&data, &search->first, sizeof(WIN32_FIND_DATAW));
            search->first.dwFileAttributes = 0xFFFFFFFF;
        } else {
            if (!FindNextFileW(search->h, &data)) {
                fsSearchFinish(iter);
                return false;
            }
        }

        // loop until we have something we're actually interested in
    } while (cstrLenw(data.cFileName) == 0 ||
             !wcscmp(data.cFileName, L".") ||
             !wcscmp(data.cFileName, L".."));

    strFromUTF16(&iter->name, data.cFileName, cstrLenw(data.cFileName));
    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        iter->type = FS_Directory;
    else
        iter->type = FS_File;

    iter->stat.size = (uint64)data.nFileSizeHigh << 32 | data.nFileSizeLow;
    iter->stat.accessed = timeFromFileTime(&data.ftLastAccessTime);
    iter->stat.created = timeFromFileTime(&data.ftCreationTime);
    iter->stat.modified = timeFromFileTime(&data.ftLastWriteTime);

    return true;
}

_Use_decl_annotations_
void fsSearchFinish(FSSearchIter *iter)
{
    FSSearch *search = (FSSearch*)iter->_search;
    if (!search)
        return;

    strDestroy(&iter->name);
    FindClose(search->h);
    xaRelease(&iter->_search);
}

_Use_decl_annotations_
bool fsSetTimes(strref path, int64 modified, int64 accessed)
{
    FILETIME mtime, atime;
    if (modified >= 0 && !timeToFileTime(modified, &mtime))
        return false;

    if (accessed >= 0 && !timeToFileTime(accessed, &atime))
        return false;

    HANDLE fh = CreateFileW(fsPathToNT(path), FILE_WRITE_ATTRIBUTES,
                            FILE_SHARE_WRITE | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);
    if (!fh)
        return false;

    bool ret = SetFileTime(fh, NULL,
                           accessed >= 0 ? &atime : NULL,
                           modified >= 0 ? &mtime : NULL);

    CloseHandle(fh);
    return ret;
}
