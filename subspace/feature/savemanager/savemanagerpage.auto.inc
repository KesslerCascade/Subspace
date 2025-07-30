// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SaveManagerPage_ClassIf SaveManagerPage_ClassIf_tmpl = {
    ._size = sizeof(SaveManagerPage_ClassIf),
    ._parent = (ObjIface*)&SettingsPage_ClassIf_tmpl,
};

static SaveManagerPage_ClassIf _impl_SaveManagerPage_SaveManagerPage_ClassIf = {
    ._size = sizeof(SaveManagerPage_ClassIf),
    ._implements = (ObjIface*)&SaveManagerPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))SaveManagerPage_make,
    .update = (bool (*)(void*))SaveManagerPage_update,
};

static ObjIface* _ifimpl_SaveManagerPage[] = {
    (ObjIface*)&_impl_SaveManagerPage_SaveManagerPage_ClassIf,
    NULL
};

ObjClassInfo SaveManagerPage_clsinfo = {
    .instsize = sizeof(SaveManagerPage),
    .classif = (ObjIface*)&SaveManagerPage_ClassIf_tmpl,
    .parent = &SettingsPage_clsinfo,
    .ifimpl = _ifimpl_SaveManagerPage,
};

// ==================== Auto-generated section ends ======================
