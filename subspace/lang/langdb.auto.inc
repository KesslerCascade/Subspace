// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
LanguageDB_ClassIf LanguageDB_ClassIf_tmpl = {
    ._size = sizeof(LanguageDB_ClassIf),
};

static LanguageDB_ClassIf _impl_LanguageDB_LanguageDB_ClassIf = {
    ._size = sizeof(LanguageDB_ClassIf),
    ._implements = (ObjIface*)&LanguageDB_ClassIf_tmpl,
    .load = (bool (*)(void*, StreamBuffer*, bool))LanguageDB_load,
    .get = (strref (*)(void*, strref))LanguageDB_get,
};

static ObjIface* _ifimpl_LanguageDB[] = {
    (ObjIface*)&_impl_LanguageDB_LanguageDB_ClassIf,
    NULL
};

ObjClassInfo LanguageDB_clsinfo = {
    .instsize = sizeof(LanguageDB),
    .classif = (ObjIface*)&LanguageDB_ClassIf_tmpl,
    .init = (bool(*)(void*))LanguageDB_init,
    .destroy = (void(*)(void*))LanguageDB_destroy,
    .ifimpl = _ifimpl_LanguageDB,
};

// ==================== Auto-generated section ends ======================
