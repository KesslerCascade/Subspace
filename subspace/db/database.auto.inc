// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
Database_ClassIf Database_ClassIf_tmpl = {
    ._size = sizeof(Database_ClassIf),
};

static Database_ClassIf _impl_Database_Database_ClassIf = {
    ._size = sizeof(Database_ClassIf),
    ._implements = (ObjIface*)&Database_ClassIf_tmpl,
    .open = (bool (*)(void*))Database_open,
    .close = (void (*)(void*))Database_close,
};

static ObjIface* _ifimpl_Database[] = {
    (ObjIface*)&_impl_Database_Database_ClassIf,
    NULL
};

ObjClassInfo Database_clsinfo = {
    .instsize = sizeof(Database),
    .classif = (ObjIface*)&Database_ClassIf_tmpl,
    .destroy = (void(*)(void*))Database_destroy,
    .ifimpl = _ifimpl_Database,
};

// ==================== Auto-generated section ends ======================
