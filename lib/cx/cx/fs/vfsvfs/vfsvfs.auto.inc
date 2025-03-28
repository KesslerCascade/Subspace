// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
VFSVFS_ClassIf VFSVFS_ClassIf_tmpl = {
    ._size = sizeof(VFSVFS_ClassIf),
};

static VFSProvider _impl_VFSVFS_VFSProvider = {
    ._size = sizeof(VFSProvider),
    ._implements = (ObjIface*)&VFSProvider_tmpl,
    .flags = (flags_t (*)(void*))VFSVFS_flags,
    .open = (ObjInst* (*)(void*, strref, flags_t))VFSVFS_open,
    .stat = (FSPathStat (*)(void*, strref, FSStat*))VFSVFS_stat,
    .setTimes = (bool (*)(void*, strref, int64, int64))VFSVFS_setTimes,
    .createDir = (bool (*)(void*, strref))VFSVFS_createDir,
    .removeDir = (bool (*)(void*, strref))VFSVFS_removeDir,
    .deleteFile = (bool (*)(void*, strref))VFSVFS_deleteFile,
    .rename = (bool (*)(void*, strref, strref))VFSVFS_rename,
    .getFSPath = (bool (*)(void*, string*, strref))VFSVFS_getFSPath,
    .searchInit = (bool (*)(void*, FSSearchIter*, strref, strref, bool))VFSVFS_searchInit,
    .searchValid = (bool (*)(void*, FSSearchIter*))VFSVFS_searchValid,
    .searchNext = (bool (*)(void*, FSSearchIter*))VFSVFS_searchNext,
    .searchFinish = (void (*)(void*, FSSearchIter*))VFSVFS_searchFinish,
};

static VFSVFS_ClassIf _impl_VFSVFS_VFSVFS_ClassIf = {
    ._size = sizeof(VFSVFS_ClassIf),
    ._implements = (ObjIface*)&VFSVFS_ClassIf_tmpl,
    .flags = (flags_t (*)(void*))VFSVFS_flags,
    .open = (ObjInst* (*)(void*, strref, flags_t))VFSVFS_open,
    .stat = (FSPathStat (*)(void*, strref, FSStat*))VFSVFS_stat,
    .setTimes = (bool (*)(void*, strref, int64, int64))VFSVFS_setTimes,
    .createDir = (bool (*)(void*, strref))VFSVFS_createDir,
    .removeDir = (bool (*)(void*, strref))VFSVFS_removeDir,
    .deleteFile = (bool (*)(void*, strref))VFSVFS_deleteFile,
    .rename = (bool (*)(void*, strref, strref))VFSVFS_rename,
    .getFSPath = (bool (*)(void*, string*, strref))VFSVFS_getFSPath,
    .searchInit = (bool (*)(void*, FSSearchIter*, strref, strref, bool))VFSVFS_searchInit,
    .searchValid = (bool (*)(void*, FSSearchIter*))VFSVFS_searchValid,
    .searchNext = (bool (*)(void*, FSSearchIter*))VFSVFS_searchNext,
    .searchFinish = (void (*)(void*, FSSearchIter*))VFSVFS_searchFinish,
};

static ObjIface* _ifimpl_VFSVFS[] = {
    (ObjIface*)&_impl_VFSVFS_VFSProvider,
    (ObjIface*)&_impl_VFSVFS_VFSVFS_ClassIf,
    NULL
};

ObjClassInfo VFSVFS_clsinfo = {
    .instsize = sizeof(VFSVFS),
    .classif = (ObjIface*)&VFSVFS_ClassIf_tmpl,
    .destroy = (void(*)(void*))VFSVFS_destroy,
    .ifimpl = _ifimpl_VFSVFS,
};

// ==================== Auto-generated section ends ======================
