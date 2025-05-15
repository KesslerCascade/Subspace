// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "pkgfs.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "pkgfsfile.h"

_objfactory_guaranteed PkgFS* PkgFS_create(PkgFile* pkgf)
{
    PkgFS* self;
    self = objInstCreate(PkgFS);

    self->pkg = objAcquire(pkgf);

    objInstInit(self);

    return self;
}

flags_t PkgFS_flags(_In_ PkgFS* self)
{
    return VFS_ReadOnly;
}

_Ret_opt_valid_ ObjInst* PkgFS_open(_In_ PkgFS* self, _In_opt_ strref path, flags_t flags)
{
    PkgFSFile* ret = NULL;
    PkgDirEnt* ent;

    if (htFind(self->pkg->files, strref, path, object, &ent)) {
        ret = pkgfsfileCreate(self, ent);
        objRelease(&ent);
    }

    return ObjInst(ret);
}

FSPathStat PkgFS_stat(_In_ PkgFS* self, _In_opt_ strref path, _When_(return != FS_Nonexistent, _Out_opt_) FSStat* stat)
{
    if (stat)
        memset(stat, 0, sizeof(FSStat));

    PkgDirEnt* ent;
    if (htFind(self->pkg->files, strref, path, object, &ent)) {
        if (stat) {
            stat->size = ent->size;
        }
        objRelease(&ent);
        return FS_File;
    }

    if (htHasKey(self->pkg->dircache, strref, path))
        return FS_Directory;

    return FS_Nonexistent;
}

bool PkgFS_setTimes(_In_ PkgFS* self, _In_opt_ strref path, int64 modified, int64 accessed)
{
    return false;
}

bool PkgFS_createDir(_In_ PkgFS* self, _In_opt_ strref path)
{
    return false;
}

bool PkgFS_removeDir(_In_ PkgFS* self, _In_opt_ strref path)
{
    return false;
}

bool PkgFS_deleteFile(_In_ PkgFS* self, _In_opt_ strref path)
{
    return false;
}

bool PkgFS_rename(_In_ PkgFS* self, _In_opt_ strref oldpath, _In_opt_ strref newpath)
{
    return false;
}

bool PkgFS_getFSPath(_In_ PkgFS* self, _Inout_ string* out, _In_opt_ strref path)
{
    return false;
}

typedef struct PkgSearchState {
    string path;
    sa_string files;
    int32 idx;
} PkgSearchState;

bool PkgFS_searchValid(_In_ PkgFS* self, _In_ FSSearchIter* iter)
{
    return iter->_search;
}

void PkgFS_searchFinish(_In_ PkgFS* self, _Inout_ FSSearchIter* iter)
{
    PkgSearchState* search = iter->_search;
    if (!search)
        return;

    strDestroy(&search->path);
    saDestroy(&search->files);
    strDestroy(&iter->name);
    xaRelease(&iter->_search);
}

bool PkgFS_searchNext(_In_ PkgFS* self, _Inout_ FSSearchIter* iter)
{
    bool ret               = false;
    PkgSearchState* search = iter->_search;
    if (!search)
        return false;
    if (search->idx < 0 || search->idx >= saSize(search->files)) {
        PkgFS_searchFinish(self, iter);
        return false;
    }

    PkgFile* pkg = self->pkg;
    strref name  = search->files.a[search->idx];
    string path  = 0;
    pathJoin(&path, search->path, name);
    PkgDirEnt* ent;
    if (htFind(pkg->files, strref, name, object, &ent)) {
        iter->type = FS_File;
        strDup(&iter->name, name);
        iter->stat.size = ent->size;
        ret             = true;
        objRelease(&ent);
    } else if (htHasKey(pkg->dircache, strref, name)) {
        iter->type = FS_Directory;
        strDup(&iter->name, name);
        iter->stat.size = 0;
        ret             = true;
    }
    search->idx++;

    strDestroy(&path);
    return ret;
}

bool PkgFS_searchInit(_In_ PkgFS* self, _Out_ FSSearchIter* iter, _In_opt_ strref path,
                      _In_opt_ strref pattern, bool stat)
{
    sa_string namelist = saInitNone;

    memset(iter, 0, sizeof(FSSearchIter));

    // if there's no dircache entry, early out because the package doesn't have this dir
    htFind(self->pkg->dircache, strref, path, sarray, &namelist, HT_Borrow);

    if (!saValid(namelist))
        return false;

    PkgSearchState* search = xaAlloc(sizeof(PkgSearchState), XA_Zero);
    iter->_search          = search;

    strDup(&search->path, path);
    saInit(&search->files, string, 8);
    // scan namelist for matching filenames
    for (int32 i = 0; i < saSize(namelist); i++) {
        if (strEmpty(pattern) || pathMatch(namelist.a[i], pattern, PATH_CaseInsensitive))
            saPush(&search->files, string, namelist.a[i]);
    }

    PkgFS_searchNext(self, iter);
    return true;
}

void PkgFS_destroy(_In_ PkgFS* self)
{
    // Autogen begins -----
    objRelease(&self->pkg);
    // Autogen ends -------
}

// Autogen begins -----
#include "pkgfs.auto.inc"
// Autogen ends -------
