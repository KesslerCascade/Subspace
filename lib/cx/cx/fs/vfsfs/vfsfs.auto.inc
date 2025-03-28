// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
VFSFS_ClassIf VFSFS_ClassIf_tmpl = {
    ._size = sizeof(VFSFS_ClassIf),
};

static VFSProvider _impl_VFSFS_VFSProvider = {
    ._size = sizeof(VFSProvider),
    ._implements = (ObjIface*)&VFSProvider_tmpl,
    .flags = (flags_t (*)(void*))VFSFS_flags,
    .open = (ObjInst* (*)(void*, strref, flags_t))VFSFS_open,
    .stat = (FSPathStat (*)(void*, strref, FSStat*))VFSFS_stat,
    .setTimes = (bool (*)(void*, strref, int64, int64))VFSFS_setTimes,
    .createDir = (bool (*)(void*, strref))VFSFS_createDir,
    .removeDir = (bool (*)(void*, strref))VFSFS_removeDir,
    .deleteFile = (bool (*)(void*, strref))VFSFS_deleteFile,
    .rename = (bool (*)(void*, strref, strref))VFSFS_rename,
    .getFSPath = (bool (*)(void*, string*, strref))VFSFS_getFSPath,
    .searchInit = (bool (*)(void*, FSSearchIter*, strref, strref, bool))VFSFS_searchInit,
    .searchValid = (bool (*)(void*, FSSearchIter*))VFSFS_searchValid,
    .searchNext = (bool (*)(void*, FSSearchIter*))VFSFS_searchNext,
    .searchFinish = (void (*)(void*, FSSearchIter*))VFSFS_searchFinish,
};

static VFSFS_ClassIf _impl_VFSFS_VFSFS_ClassIf = {
    ._size = sizeof(VFSFS_ClassIf),
    ._implements = (ObjIface*)&VFSFS_ClassIf_tmpl,
    .flags = (flags_t (*)(void*))VFSFS_flags,
    .open = (ObjInst* (*)(void*, strref, flags_t))VFSFS_open,
    .stat = (FSPathStat (*)(void*, strref, FSStat*))VFSFS_stat,
    .setTimes = (bool (*)(void*, strref, int64, int64))VFSFS_setTimes,
    .createDir = (bool (*)(void*, strref))VFSFS_createDir,
    .removeDir = (bool (*)(void*, strref))VFSFS_removeDir,
    .deleteFile = (bool (*)(void*, strref))VFSFS_deleteFile,
    .rename = (bool (*)(void*, strref, strref))VFSFS_rename,
    .getFSPath = (bool (*)(void*, string*, strref))VFSFS_getFSPath,
    .searchInit = (bool (*)(void*, FSSearchIter*, strref, strref, bool))VFSFS_searchInit,
    .searchValid = (bool (*)(void*, FSSearchIter*))VFSFS_searchValid,
    .searchNext = (bool (*)(void*, FSSearchIter*))VFSFS_searchNext,
    .searchFinish = (void (*)(void*, FSSearchIter*))VFSFS_searchFinish,
};

static ObjIface* _ifimpl_VFSFS[] = {
    (ObjIface*)&_impl_VFSFS_VFSProvider,
    (ObjIface*)&_impl_VFSFS_VFSFS_ClassIf,
    NULL
};

ObjClassInfo VFSFS_clsinfo = {
    .instsize = sizeof(VFSFS),
    .classif = (ObjIface*)&VFSFS_ClassIf_tmpl,
    .destroy = (void(*)(void*))VFSFS_destroy,
    .ifimpl = _ifimpl_VFSFS,
};

// ==================== Auto-generated section ends ======================
