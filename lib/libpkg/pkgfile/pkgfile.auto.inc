// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
PkgFile_ClassIf PkgFile_ClassIf_tmpl = {
    ._size = sizeof(PkgFile_ClassIf),
};

static ObjIface* _ifimpl_PkgDirEnt[] = {
    NULL
};

ObjClassInfo PkgDirEnt_clsinfo = {
    .instsize = sizeof(PkgDirEnt),
    .destroy = (void(*)(void*))PkgDirEnt_destroy,
    .ifimpl = _ifimpl_PkgDirEnt,
};

static PkgFile_ClassIf _impl_PkgFile_PkgFile_ClassIf = {
    ._size = sizeof(PkgFile_ClassIf),
    ._implements = (ObjIface*)&PkgFile_ClassIf_tmpl,
    .load = (bool (*)(void*))PkgFile_load,
    .close = (void (*)(void*))PkgFile_close,
};

static ObjIface* _ifimpl_PkgFile[] = {
    (ObjIface*)&_impl_PkgFile_PkgFile_ClassIf,
    NULL
};

ObjClassInfo PkgFile_clsinfo = {
    .instsize = sizeof(PkgFile),
    .classif = (ObjIface*)&PkgFile_ClassIf_tmpl,
    .init = (bool(*)(void*))PkgFile_init,
    .destroy = (void(*)(void*))PkgFile_destroy,
    .ifimpl = _ifimpl_PkgFile,
};

// ==================== Auto-generated section ends ======================
