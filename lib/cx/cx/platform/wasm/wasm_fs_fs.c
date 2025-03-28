#include "cx/time/time.h"
#include "cx/fs/fs_private.h"
#include "cx/string.h"
#include "cx/utils/lazyinit.h"
#include "cx/container/sarray.h"
#include "cx/thread/rwlock.h"
#include "cx/platform/unix.h"

#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

#ifdef _PLATFORM_FBSD
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include <fcntl.h>
#include <sys/stat.h>

static LazyInitState fsCurDirInit;
RWLock _fsCurDirLock;
string _fsCurDir = 0;
string fsPlatformPathSepStr = _S"/";

static void initCurDir(void *data)
{
    char *buf = 0, *pcur = 0;
    size_t bufsz = PATH_MAX / 2;

    rwlockInit(&_fsCurDirLock);

    // POSIX doesn't actually limit this to PATH_MAX, so expand as necessary
    do {
        xaFree(buf);
        bufsz *= 2;
        buf = xaAlloc(bufsz);

        pcur = getcwd(buf, bufsz);
    } while (!pcur && errno == ERANGE);

    if (!pcur) {
        // failed??? punt...
        xaFree(buf);
        strCopy(&_fsCurDir, _S"/");
        return;
    }

    pathFromPlatform(&_fsCurDir, (string)buf);
    pathNormalize(&_fsCurDir);
    xaFree(buf);
}

void pathFromPlatform(string *out, strref platformpath)
{
    // not really much to do here...
    strDup(out, platformpath);
}

void pathToPlatform(string *out, strref path)
{
    string ns = 0, rpath = 0;
    pathSplitNS(&ns, &rpath, path);
    // namespaces have no meaning in UNIX paths, so just
    // discard it and use the path part if there is one
    strDestroy(&ns);
    strDestroy(out);
    *out = rpath;
}

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

void fsCurDir(string *out)
{
    lazyInit(&fsCurDirInit, initCurDir, NULL);
    rwlockAcquireRead(&_fsCurDirLock);
    strDup(out, _fsCurDir);
    rwlockReleaseRead(&_fsCurDirLock);
}

bool fsSetCurDir(strref cur)
{
    if (strEmpty(cur))
        return false;

    string ncur = 0, ppath = 0;
    strDup(&ncur, cur);

    // be paranoid about what we set our current directory to
    pathNormalize(&ncur);
    pathMakeAbsolute(&ncur, ncur);

    // We don't need to actually change the OS's current directory since our own path
    // normalization always results in absolute paths for everything, but it helps
    // keep things from being confusing if subprocesses are created.
    pathToPlatform(&ppath, ncur);
    rwlockAcquireWrite(&_fsCurDirLock);
    if (chdir(strC(ppath)) != 0) {
        unixMapErrno();
        strDestroy(&ncur);
	strDestroy(&ppath);
        rwlockReleaseWrite(&_fsCurDirLock);
        return false;
    }

    strDestroy(&ppath);
    strDestroy(&_fsCurDir);
    _fsCurDir = ncur;
    rwlockReleaseWrite(&_fsCurDirLock);
    return true;
}

static void fsExeInit(void *data)
{
    string *exepath = (string*)data;

    size_t sz = 1024 / 2;
    char *buf = 0;
    ssize_t lsz;
    // loop until we're sure the buffer was big enough
    do {
        xaFree(buf);
        sz *= 2;
        buf = xaAlloc(sz);
        lsz = readlink("/proc/self/exe", buf, sz);
    } while (lsz != -1 && lsz == sz);
    if (lsz == -1) {
        // panic
        strCopy(exepath, _S"/");
    } else {
        buf[lsz] = 0;
        pathFromPlatform(exepath, (string)buf);
    }
    xaFree(buf);
    pathNormalize(exepath);
}

void fsExe(string *out)
{
    static LazyInitState execache;
    static string exepath;
    lazyInit(&execache, fsExeInit, &exepath);

    strDup(out, exepath);
}

void fsExeDir(string *out)
{
    fsExe(out);
    pathParent(out, *out);
}

FSPathStat fsStat(strref path, FSStat *fsstat)
{
    if (strEmpty(path))
        return FS_Nonexistent;

    struct stat sb;
    string ppath = 0;
    pathToPlatform(&ppath, path);
    if (stat(strC(ppath), &sb) != 0) {
        if (fsstat)
            memset(fsstat, 0, sizeof(FSStat));
	strDestroy(&ppath);
        return FS_Nonexistent;
    }
    strDestroy(&ppath);

    if (fsstat) {
        fsstat->size = sb.st_size;
        fsstat->accessed = timeFromAbsTimespec(&sb.st_atim);

        // unix ctime is inode change time, which is sometimes newer than mtime if the
        // metadata is touched
        fsstat->modified = max(timeFromAbsTimespec(&sb.st_mtim), timeFromAbsTimespec(&sb.st_ctim));

#ifdef _PLATFORM_FBSD
        fsstat->created = timeFromAbsTimespec(&sb.st_birthtim);
#else
        // Linux has no file creation timestamp
        fsstat->created = fsstat->modified;
#endif
    }
    if (S_ISDIR(sb.st_mode))
        return FS_Directory;
    return FS_File;
}

bool fsCreateDir(strref path)
{
    bool ret = true;
    if (strEmpty(path))
        return false;

    string ppath = 0;
    pathToPlatform(&ppath, path);
    if (mkdir(strC(ppath), 0755) == -1 && errno != EEXIST)
        ret = unixMapErrno();

    strDestroy(&ppath);
    return ret;
}

bool fsCreateAll(strref path)
{
    string parent = 0;
    pathParent(&parent, path);
    if (!strEmpty(parent) && !fsExist(parent))
        fsCreateAll(parent);
    strDestroy(&parent);

    return fsCreateDir(path);
}

bool fsRemoveDir(strref path)
{
    bool ret = true;
    if (strEmpty(path))
        return false;

    string ppath = 0;
    pathToPlatform(&ppath, path);
    if (rmdir(strC(ppath)) == -1)
        ret = unixMapErrno();

    strDestroy(&ppath);
    return ret;
}

bool fsDelete(strref path)
{
    bool ret = true;
    if (strEmpty(path))
        return false;

    string ppath = 0;
    pathToPlatform(&ppath, path);
    if (unlink(strC(ppath)) == -1)
        ret = unixMapErrno();

    strDestroy(&ppath);
    return ret;
}

bool fsRename(strref from, strref to)
{
    bool ret = true;
    if (strEmpty(from) || strEmpty(to))
        return false;

    string pfrom = 0, pto = 0;
    pathToPlatform(&pfrom, from);
    pathToPlatform(&pto, to);
    if (rename(strC(pfrom), strC(pto)) == -1)
        ret = unixMapErrno();

    strDestroy(&pfrom);
    strDestroy(&pto);
    return ret;
}

typedef struct FSSearch {
    DIR *d;
    string path;
    string pattern;
    bool stat;
} FSSearch;

bool fsSearchInit(FSSearchIter *iter, strref path, strref pattern, bool stat)
{
    string ppath = 0;
    pathToPlatform(&ppath, path);

    memset(iter, 0, sizeof(FSSearchIter));

    FSSearch *search = xaAlloc(sizeof(FSSearch), XA_Zero);
    iter->_search = search;
    search->d = opendir(strC(ppath));
    strDestroy(&ppath);

    if (!search->d) {
        unixMapErrno();
        xaRelease(&iter->_search);
        return false;
    }

    strDup(&search->path, path);
    strDup(&search->pattern, pattern);
    search->stat = stat;
    fsSearchNext(iter);
    return true;
}

bool fsSearchNext(FSSearchIter *iter)
{
    FSSearch *search = (FSSearch*)iter->_search;
    if (!search)
        return false;

    struct dirent *de;

    for(;;) {
        de = readdir(search->d);
        if (!de) {
	    fsSearchFinish(iter);
            return false;
	}

        if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..") &&
	    (strEmpty(search->pattern) || pathMatch((string)de->d_name, search->pattern, 0))) {
            strCopy(&iter->name, (string)de->d_name);
            if (de->d_type == DT_DIR)
                iter->type = FS_Directory;
            else
                iter->type = FS_File;

            if (search->stat) {
                string tpath = 0;
                pathJoin(&tpath, search->path, iter->name);
                fsStat(tpath, &iter->stat);
                strDestroy(&tpath);
            }

            return true;
        }
    }
}

void fsSearchFinish(FSSearchIter *iter)
{
    FSSearch *search = (FSSearch*)iter->_search;
    if (!search)
        return;

    strDestroy(&iter->name);
    closedir(search->d);
    strDestroy(&search->path);
    strDestroy(&search->pattern);
    xaRelease(&iter->_search);
}

bool fsSetTimes(strref path, int64 modified, int64 accessed)
{
    string ppath = 0;
    struct timespec times[2] = { 0 };
    bool ret = true;

    if (accessed >= 0)
	timeToAbsTimespec(&times[0], accessed);
    else
	times[0].tv_nsec = UTIME_OMIT;

    if (modified >= 0)
	timeToAbsTimespec(&times[1], modified);
    else
	times[1].tv_nsec = UTIME_OMIT;

    pathToPlatform(&ppath, path);
    if (utimensat(AT_FDCWD, strC(ppath), times, 0) == -1)
	ret = unixMapErrno();
    strDestroy(&ppath);

    return ret;
}
