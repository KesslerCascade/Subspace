// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
PkgFS_ClassIf PkgFS_ClassIf_tmpl = {
    ._size = sizeof(PkgFS_ClassIf),
};

static VFSProvider _impl_PkgFS_VFSProvider = {
    ._size = sizeof(VFSProvider),
    ._implements = (ObjIface*)&VFSProvider_tmpl,
    .flags = (flags_t (*)(void*))PkgFS_flags,
    .open = (ObjInst* (*)(void*, strref, flags_t))PkgFS_open,
    .stat = (FSPathStat (*)(void*, strref, FSStat*))PkgFS_stat,
    .setTimes = (bool (*)(void*, strref, int64, int64))PkgFS_setTimes,
    .createDir = (bool (*)(void*, strref))PkgFS_createDir,
    .removeDir = (bool (*)(void*, strref))PkgFS_removeDir,
    .deleteFile = (bool (*)(void*, strref))PkgFS_deleteFile,
    .rename = (bool (*)(void*, strref, strref))PkgFS_rename,
    .getFSPath = (bool (*)(void*, string*, strref))PkgFS_getFSPath,
    .searchInit = (bool (*)(void*, FSSearchIter*, strref, strref, bool))PkgFS_searchInit,
    .searchValid = (bool (*)(void*, FSSearchIter*))PkgFS_searchValid,
    .searchNext = (bool (*)(void*, FSSearchIter*))PkgFS_searchNext,
    .searchFinish = (void (*)(void*, FSSearchIter*))PkgFS_searchFinish,
};

static PkgFS_ClassIf _impl_PkgFS_PkgFS_ClassIf = {
    ._size = sizeof(PkgFS_ClassIf),
    ._implements = (ObjIface*)&PkgFS_ClassIf_tmpl,
    .flags = (flags_t (*)(void*))PkgFS_flags,
    .open = (ObjInst* (*)(void*, strref, flags_t))PkgFS_open,
    .stat = (FSPathStat (*)(void*, strref, FSStat*))PkgFS_stat,
    .setTimes = (bool (*)(void*, strref, int64, int64))PkgFS_setTimes,
    .createDir = (bool (*)(void*, strref))PkgFS_createDir,
    .removeDir = (bool (*)(void*, strref))PkgFS_removeDir,
    .deleteFile = (bool (*)(void*, strref))PkgFS_deleteFile,
    .rename = (bool (*)(void*, strref, strref))PkgFS_rename,
    .getFSPath = (bool (*)(void*, string*, strref))PkgFS_getFSPath,
    .searchInit = (bool (*)(void*, FSSearchIter*, strref, strref, bool))PkgFS_searchInit,
    .searchValid = (bool (*)(void*, FSSearchIter*))PkgFS_searchValid,
    .searchNext = (bool (*)(void*, FSSearchIter*))PkgFS_searchNext,
    .searchFinish = (void (*)(void*, FSSearchIter*))PkgFS_searchFinish,
};

static ObjIface* _ifimpl_PkgFS[] = {
    (ObjIface*)&_impl_PkgFS_VFSProvider,
    (ObjIface*)&_impl_PkgFS_PkgFS_ClassIf,
    NULL
};

ObjClassInfo PkgFS_clsinfo = {
    .instsize = sizeof(PkgFS),
    .classif = (ObjIface*)&PkgFS_ClassIf_tmpl,
    .destroy = (void(*)(void*))PkgFS_destroy,
    .ifimpl = _ifimpl_PkgFS,
};

// ==================== Auto-generated section ends ======================
